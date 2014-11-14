#ifndef MBGL_UTIL_FACTORY_HPP
#define MBGL_UTIL_FACTORY_HPP

// stl
#include <stdexcept>
#include <mutex>
#include <map>

namespace mbgl { namespace util {

template
<
typename product_type,
typename key_type,
typename ...Args >
class factory
{
public:
    using product_creator = product_type* (*) (Args...);
    using product_map = std::map<key_type,product_creator>;
    static product_map map_;
    static std::mutex mutex_;
public:

    static bool register_product(key_type const& key, product_creator creator)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return map_.insert(typename product_map::value_type(key,creator)).second;
    }

    static bool unregister_product(const key_type& key)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return map_.erase(key)==1;
    }

    static product_type* create_object(key_type const& key, Args...args)
    {
        typename product_map::const_iterator pos=map_.find(key);
        if (pos!=map_.end())
        {
            return (pos->second)(args...);
        }
        return 0;
    }
};

template <typename product_type, typename key_type, typename ...Args>
typename factory<product_type, key_type, Args...>::product_map factory<product_type, key_type, Args...>::map_;
template <typename product_type, typename key_type, typename ...Args>
typename std::mutex factory<product_type, key_type, Args...>::mutex_;

}}

#endif
