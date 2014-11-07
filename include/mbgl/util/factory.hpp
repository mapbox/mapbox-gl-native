#ifndef MBGL_UTIL_FACTORY_HPP
#define MBGL_UTIL_FACTORY_HPP

#include "singleton.hpp"

// stl
#include <stdexcept>
#include <map>

namespace mbgl { namespace util {

template
<
typename product_type,
typename key_type,
typename ...Args >
class factory : public singleton<factory <product_type,
                                          key_type,
                                          Args...> >
{
private:
    using product_creator = product_type* (*) (Args...);
    using product_map = std::map<key_type,product_creator>;
    product_map map_;
public:

    bool register_product(key_type const& key, product_creator creator)
    {
        return map_.insert(typename product_map::value_type(key,creator)).second;
    }

    bool unregister_product(const key_type& key)
    {
        return map_.erase(key)==1;
    }

    product_type* create_object(key_type const& key, Args...args)
    {
        typename product_map::const_iterator pos=map_.find(key);
        if (pos!=map_.end())
        {
            return (pos->second)(args...);
        }
        return 0;
    }
};

}}

#endif
