#pragma once

#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

using FeatureStates = std::unordered_map<FeatureIdentifier, PropertyMap>;
using FeatureStatesMap = std::unordered_map<std::string, FeatureStates>;

struct FeatureStateChange {
  enum class ChangeType {
    Insert,
    Clear
  };
    
  ChangeType type;
  FeatureIdentifier id;
  std::string key;
  optional<Value> value;
  FeatureStateChange(ChangeType type_,
		     FeatureIdentifier&& id_,
		     std::string&& key_,
		     optional<Value> value_) :
    type(type_),
    id(std::move(id_)),
    key(std::move(key_)),
    value(value_) {}
};

}
