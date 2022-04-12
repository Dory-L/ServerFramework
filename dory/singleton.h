#pragma once

namespace dory {

template<class T, class X = void, int N = 0>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

template <class T, class X = void, int N = 0>
class SingletonPtr {
    static std::shared_ptr<T> GetIncetance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}