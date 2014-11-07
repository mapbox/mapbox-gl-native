#ifndef MBGL_UTIL_SINGLETON_HPP
#define MBGL_UTIL_SINGLETON_HPP

// stl
#include <stdexcept> // std::runtime_error
#include <cstdlib> // std::atexit
#include <new> // operator new
#define SINGLETON_THREADSAFE
#ifdef SINGLETON_THREADSAFE
#include <mutex>
#endif

namespace mbgl { namespace util {

template <typename T>
class CreateUsingNew
{
public:
    static T* create()
    {
        return new T;
    }
    static void destroy(T* obj)
    {
        delete obj;
    }
};

template <typename T>
class CreateStatic
{
private:
    union MaxAlign
    {
        char t_[sizeof(T)];
        short int shortInt_;
        int int_;
        long int longInt_;
        float float_;
        double double_;
        long double longDouble_;
        struct Test;
        int Test::* pMember_;
        int (Test::*pMemberFn_)(int);
    };

public:

    static T* create()
    {
        static MaxAlign staticMemory;
        return new(&staticMemory) T;
    }
    static void destroy(volatile T* obj)
    {
        obj->~T();
    }
};

template <typename T,
          template <typename U> class CreatePolicy=CreateStatic> class singleton
{
    friend class CreatePolicy<T>;
    static T* pInstance_;
    static bool destroyed_;
    singleton(const singleton &rhs);
    singleton& operator=(const singleton&);

    static void onDeadReference()
    {
        throw std::runtime_error("dead reference!");
    }

    static void DestroySingleton()
    {
        CreatePolicy<T>::destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }

protected:

#ifdef SINGLETON_THREADSAFE
        static std::mutex mutex_;
#endif
        singleton() {}
    public:
        static T& instance()
        {
            if (! pInstance_)
            {
#ifdef SINGLETON_THREADSAFE
                std::unique_lock<std::mutex> lock(mutex_);
#endif
                if (! pInstance_)
                {
                    if (destroyed_)
                    {
                        destroyed_ = false;
                        onDeadReference();
                    }
                    else
                    {
                        pInstance_ = CreatePolicy<T>::create();
                        // register destruction
                        std::atexit(&DestroySingleton);
                    }
                }
            }
            return *pInstance_;
        }
};

#ifdef SINGLETON_THREADSAFE
    template <typename T,
              template <typename U> class CreatePolicy> std::mutex singleton<T,CreatePolicy>::mutex_;
#endif

    template <typename T,
              template <typename U> class CreatePolicy> T* singleton<T,CreatePolicy>::pInstance_=0;
    template <typename T,
              template <typename U> class CreatePolicy> bool singleton<T,CreatePolicy>::destroyed_=false;

}}

#endif //
