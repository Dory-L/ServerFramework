# ServerFramework
## mutex

使用RAII机制封装信号量sem\_t, pthread\_mutex\_t, pthread\_rwlock\_t, pthread\_spinlock\_t, std::atomic\_flag，c++11中的互斥量没有读写分离的方式，因此选用pthread的互斥量和信号量，大并发的情况下，谢少读多的情况比较多，读写分离能提高性能。

- 和log整合
    LogAppender的setFormatter和getFormatter需要加锁，因为复杂数据类型的线程安全会导致内存错误，getLevel和setLevel不需要，因为enum是基本数据类型，线程安全只是导致值错误，streamstring线程都不安全，用到的地方都要加锁，LogFormatter 构造出来之后就不再改变，所以不需要加锁
- 加锁性能降低，需优化。
    方案：使用自旋锁，等待的时候只是在cpu上空跑，不会陷入内核态
    测试：使用pthread\_mutex\_t，24 m/s的写入速度;
    使用pthread\_spin\_t，31 m/s的写入速度;
    使用atomic_flag, 35 m/s的写入速度; //跟spinlock原理差不多，但spinlock不会陷入内核态会好一点，因此最后还是选择spinlock
- 和config整合
    配置写少读多，采用读写锁
    Config新添加一个访问者模式的方法，用来外部访问ConfigVarMap的元素。

* * *

## thread

每个**线程**可以设置线程名称，可以获取当前线程号，在Thread的构造函数设置信号量等待，在 线程回调函数中调用用户回调函数之前post，这样可以使线程按照构造顺序调用用户的回调函数。

* * *

## fiber

协程比线程更轻量级，数量可以是线程的成千上万倍，切换快，操作权在用户手中。
栈大小默认128K，可自行修改配置文件。

基于ucontext_t实现
制定macro便于调式

```
只能由main_fiber创建协程和回收子协程，虽然不灵活，但是好控制
FIber::GetThis()获取当前协程，如果没有创建一个，主协程通过new创建子协程。
YieldToHold/YieldToReady 让出子协程的执行时，并唤醒主协程
Thread->main_fiber <---------> sub_fiber
			↑
			|
			↓
		sub_fiber
```

**协程调度模块**

```cpp
		 1-N	    1-M
scheduler --> thread --> fiber
1、线程池，分配一组线程
2、协程调度器，将协程指定到相应的线程上去执行

N : M

m_threads
<function<void()>, fiber, threadid> m_fibers

schedule(func/fiber)

start()
stop() //不是立即退出，等在调度器的所有任务完成之后退出
run()

1、设置当前线程的scheduler
2、设置当前线程的run、fiber
3、协程调度循环while(true)
	1、协程消息队列里面是否有任务
	2、无任务执行、执行idle
```

**定时器模块**

```
Timer -> addTimer() -->cancel()
获取当前的定时器触发离现在的时间差
返回当前需要触发的定时器

基于epoll的定时器，精确到毫秒级
```

**io管理模块**

```
IOManager (epoll) ---> scheduler
	|
	|
	↓
  idle (epoll_wait)

	信号量
PutMessage(msg,) 信号量+1,single()
message_queue
	|
	|----Thread
	|----Thread
		wait()信号量-1,RecvMessage(msg,)

异步IO，等待数据返回。epoll_wait

epoll_create, epoll_ctl, epoll_wait
```

**调度模型**

```
				[Fiber]			   	 [Timer]
				   ^ N					^
				   |					|
				   | 1					|
				[Thread]		  [TimerManager]
				   ^ M 					^
				   |					|
				   | 1					|
			   [Scheduler]←----- [IOManager(epoll)]
```

## hook

hook了以下函数

> sleep, usleep, nanosleep
>
> socket, close
>
> connect 
>
> accept, read, readv, recv, recvfrom, recvmsg, write, send, sendto, sendmsg
>
> fcntl, ioctl
>
> getsockopt, setsockopt

* sleep, usleep, nanosleep

  iom->addTimer()；//将当前fiber加入定时器

  yieldToHold(); //切入后台，等到超时唤醒

* socket

  创建的fd交给FdManager管理；

* close

  从FdManager中删除此socket;

  关闭socket;

* connect

  ```cpp
  if (connect成功) (
  	返回成功;
  )
  if (被阻塞) {
  	启用定时器;
  	iom->addEvent(fd, (event)WRITE); //将当前fiber加入调度器（写事件）
  	yieldToHold();//切换到后台
  	被唤醒; //超时或者connect成功事件发生
  	if (超时) {
  		返回失败；
  	} else {
  		返回成功；
  	}
  }
  ```

  * accept, read, readv, recv, recvfrom, recvmsg, write, send, sendto, sendmsg

    ```cpp
    if (fd不是socket || 用户设置非阻塞) {
    	return originFun();
    }
    if (被阻塞) {
    	设置定时器；
    	将当前fiber添加到调度器；
    	切换到后台;
    	被唤醒; //超时或者对应的事件；
    	if (超时) {
    		返回错误;
    	} else {
    		重新进行一次io；
    	}
    }
    return 结果;
    ```

    * fcntl, ioctl

      拿到nonblock信息

    * setsockopt

      设置FdCtx的超时信息；
