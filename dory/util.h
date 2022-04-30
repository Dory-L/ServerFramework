#pragma once
#include <thread>
#include <stdint.h>
#include <vector>
#include <string>

namespace dory {
//线程号
pid_t GetThreadId();
//协程号
uint32_t GetFiberId();

//输出函数调用栈，skip-越过的层数
void Backtrace(std::vector<std::string>& bt, int size, int skip = 2);

//prefix自定义前缀
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

} // namespace dory
