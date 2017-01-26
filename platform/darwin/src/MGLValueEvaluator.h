#import <Foundation/Foundation.h>

#import <mbgl/util/geometry.hpp>

/**
 Recursively transforms a C++ type into the corresponding Foundation type.
 */
class ValueEvaluator {
public:
    id operator()(const mbgl::NullValue &) const {
        return [NSNull null];
    }

    id operator()(const bool &value) const {
        return value ? @YES : @NO;
    }

    id operator()(const uint64_t &value) const {
        return @(value);
    }

    id operator()(const int64_t &value) const {
        return @(value);
    }

    id operator()(const double &value) const {
        return @(value);
    }

    id operator()(const std::string &value) const {
        return @(value.c_str());
    }

    id operator()(const std::vector<mbgl::Value> &values) const {
        NSMutableArray *objects = [NSMutableArray arrayWithCapacity:values.size()];
        for (const auto &v : values) {
            [objects addObject:mbgl::Value::visit(v, *this)];
        }
        return objects;
    }

    id operator()(const std::unordered_map<std::string, mbgl::Value> &items) const {
        NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:items.size()];
        for (auto &item : items) {
            attributes[@(item.first.c_str())] = mbgl::Value::visit(item.second, *this);
        }
        return attributes;
    }
};
