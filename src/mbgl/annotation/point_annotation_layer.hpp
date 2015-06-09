#ifndef MBGL_ANNOTATION_POINT_ANNOTATION_LAYER
#define MBGL_ANNOTATION_POINT_ANNOTATION_LAYER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/annotation.hpp>
#include <mbgl/map/tile.hpp>

#include <string>
#include <vector>

namespace mbgl {

// Stores a tree of AnnotationPoints. Creates Bucket objects for a particular tile ID.
class PointAnnotationLayer : private util::noncopyable {
    class Tree;

public:
    PointAnnotationLayer();
    ~PointAnnotationLayer();

    void add(const std::shared_ptr<const Annotation>&);
    void add(const std::vector<std::shared_ptr<const Annotation>>&);

    void remove(const std::shared_ptr<const Annotation>&);
    void remove(const std::vector<std::shared_ptr<const Annotation>>&);

    std::vector<std::shared_ptr<const Annotation>> getInBounds(const LatLngBounds&) const;

private:

    const std::unique_ptr<Tree> tree;
};

} // end namespace mbgl

#endif
