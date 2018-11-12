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
  std::string sourceLayer;
  FeatureIdentifier id;
  std::string key;
  optional<Value> value;
  FeatureStateChange(ChangeType type_,
             const std::string& sourceLayer_,
             const FeatureIdentifier& id_,
		     const std::string& key_,
		     optional<Value> value_) :
    type(type_),
    sourceLayer(sourceLayer_),
    id(std::move(id_)),
    key(std::move(key_)),
    value(std::move(value_)) {}
    
  FeatureStateChange(ChangeType type_,
             const FeatureIdentifier& id_,
             const std::string& key_,
             optional<Value> value_) :
    type(type_),
    id(std::move(id_)),
    key(std::move(key_)),
    value(std::move(value_)) {}

};

using FeatureStateChangeSet = std::vector<FeatureStateChange>;

} // namespace mbgl
