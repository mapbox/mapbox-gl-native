#include <mbgl/style/expression/distance.hpp>

#include <mapbox/cheap_ruler.hpp>
#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/geometry_util.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/document.h>

#include <algorithm>
#include <deque>
#include <limits>
#include <queue>
#include <tuple>

namespace mbgl {
namespace {

const std::size_t MinPointsSize = 100;
const std::size_t MinLinePointsSize = 50;
const double InvalidDistance = std::numeric_limits<double>::quiet_NaN();
const double InfiniteDistance = std::numeric_limits<double>::infinity();
const mapbox::cheap_ruler::CheapRuler::Unit UnitInMeters = mapbox::cheap_ruler::CheapRuler::Unit::Meters;

// Inclusive index range for multipoint or linestring container
using IndexRange = std::pair<std::size_t, std::size_t>;

std::size_t getRangeSize(const IndexRange& range) {
    return range.second - range.first + 1;
}

std::pair<mbgl::optional<IndexRange>, mbgl::optional<IndexRange>> splitRange(const IndexRange& range, bool isLine) {
    auto size = getRangeSize(range);
    if (isLine) {
        if (size == 2) {
            return std::make_pair(range, nullopt);
        }
        auto size1 = size / 2;
        IndexRange range1(range.first, range.first + size1);
        IndexRange range2(range.first + size1, range.second);
        return std::make_pair(std::move(range1), std::move(range2));
    } else {
        if (size == 1) {
            return std::make_pair(range, nullopt);
        }
        auto size1 = size / 2 - 1;
        IndexRange range1(range.first, range.first + size1);
        IndexRange range2(range.first + size1 + 1, range.second);
        return std::make_pair(std::move(range1), std::move(range2));
    }
}

using DistanceBBox = GeometryBBox<double>;

DistanceBBox getBBox(const mapbox::geometry::multi_point<double>& points, const IndexRange& range) {
    assert(range.second >= range.first && range.second < points.size());
    DistanceBBox bbox = DefaultDistanceBBox;
    for (std::size_t i = range.first; i <= range.second; ++i) {
        updateBBox(bbox, points[i]);
    }
    return bbox;
}

DistanceBBox getBBox(const mapbox::geometry::line_string<double>& line, const IndexRange& range) {
    assert(range.second >= range.first && range.second < line.size());
    DistanceBBox bbox = DefaultDistanceBBox;
    for (std::size_t i = range.first; i <= range.second; ++i) {
        updateBBox(bbox, line[i]);
    }
    return bbox;
}

DistanceBBox getBBox(const mapbox::geometry::polygon<double>& polygon) {
    DistanceBBox bbox = DefaultDistanceBBox;
    for (const auto& ring : polygon) {
        for (const auto& p : ring) {
            updateBBox(bbox, p);
        }
    }
    return bbox;
}

// Calculate the distance between two bounding boxes.
// Calculate the delta in x and y direction, and use two fake points {0.0, 0.0} and {dx, dy} to calculate the distance.
// Distance will be 0.0 if bounding box are overlapping.
double bboxToBBoxDistance(const DistanceBBox& bbox1,
                          const DistanceBBox& bbox2,
                          mapbox::cheap_ruler::CheapRuler& ruler) {
    double dx = 0.0;
    double dy = 0.0;
    // bbox1 in left side
    if (bbox1[2] < bbox2[0]) {
        dx = bbox2[0] - bbox1[2];
    }
    // bbox1 in right side
    if (bbox1[0] > bbox2[2]) {
        dx = bbox1[0] - bbox2[2];
    }
    // bbox1 in above side
    if (bbox1[1] > bbox2[3]) {
        dy = bbox1[1] - bbox2[3];
    }
    // bbox1 in down side
    if (bbox1[3] < bbox2[1]) {
        dy = bbox2[1] - bbox1[3];
    }
    return ruler.distance(mapbox::geometry::point<double>{0.0, 0.0}, mapbox::geometry::point<double>{dx, dy});
}

double pointToLineDistance(const mapbox::geometry::point<double>& point,
                           const mapbox::geometry::line_string<double>& line,
                           mapbox::cheap_ruler::CheapRuler& ruler) {
    const auto nearestPoint = std::get<0>(ruler.pointOnLine(line, point));
    return ruler.distance(point, nearestPoint);
}

// precondition is two segments are not intersecting with each other
double segmentToSegmentDistance(const mapbox::geometry::point<double>& p1,
                                const mapbox::geometry::point<double>& p2,
                                const mapbox::geometry::point<double>& q1,
                                const mapbox::geometry::point<double>& q2,
                                mapbox::cheap_ruler::CheapRuler& ruler) {
    auto dist1 = std::min(pointToLineDistance(p1, mapbox::geometry::line_string<double>{q1, q2}, ruler),
                          pointToLineDistance(p2, mapbox::geometry::line_string<double>{q1, q2}, ruler));
    auto dist2 = std::min(pointToLineDistance(q1, mapbox::geometry::line_string<double>{p1, p2}, ruler),
                          pointToLineDistance(q2, mapbox::geometry::line_string<double>{p1, p2}, ruler));
    auto dist = std::min(dist1, dist2);
    return dist;
}

double lineToLineDistance(const mapbox::geometry::line_string<double>& line1,
                          IndexRange& range1,
                          const mapbox::geometry::line_string<double>& line2,
                          IndexRange& range2,
                          mapbox::cheap_ruler::CheapRuler& ruler) {
    bool rangeSafe = (range1.second >= range1.first && range1.second < line1.size()) &&
                     (range2.second >= range2.first && range2.second < line2.size());
    if (!rangeSafe) {
        mbgl::Log::Error(mbgl::Event::General, "index is out of range");
        return InvalidDistance;
    }
    double dist = InfiniteDistance;
    for (std::size_t i = range1.first; i < range1.second; ++i) {
        const auto& p1 = line1[i];
        const auto& p2 = line1[i + 1];
        for (std::size_t j = range2.first; j < range2.second; ++j) {
            const auto& q1 = line2[j];
            const auto& q2 = line2[j + 1];
            if (segmentIntersectSegment(p1, p2, q1, q2)) return 0.0;
            dist = std::min(dist, segmentToSegmentDistance(p1, p2, q1, q2, ruler));
        }
    }
    return dist;
}

double pointsToPointsDistance(const mapbox::geometry::multi_point<double>& points1,
                              IndexRange& range1,
                              const mapbox::geometry::multi_point<double>& points2,
                              IndexRange& range2,
                              mapbox::cheap_ruler::CheapRuler& ruler) {
    bool rangeSafe = (range1.second >= range1.first && range1.second < points1.size()) &&
                     (range2.second >= range2.first && range2.second < points2.size());
    if (!rangeSafe) {
        mbgl::Log::Error(mbgl::Event::General, "index is out of range");
        return InvalidDistance;
    }
    double dist = InfiniteDistance;
    for (std::size_t i = range1.first; i <= range1.second; ++i) {
        for (std::size_t j = range2.first; j <= range2.second; ++j) {
            dist = std::min(dist, ruler.distance(points1[i], points2[j]));
            if (dist == 0.0) return dist;
        }
    }
    return dist;
}

double pointToPolygonDistance(const mapbox::geometry::point<double>& point,
                              const mapbox::geometry::polygon<double>& polygon,
                              mapbox::cheap_ruler::CheapRuler& ruler) {
    if (pointWithinPolygon(point, polygon, true)) return 0.0;
    double dist = InfiniteDistance;
    for (const auto& ring : polygon) {
        const auto nearestPoint = std::get<0>(ruler.pointOnLine(ring, point));
        dist = std::min(dist, ruler.distance(point, nearestPoint));
        if (dist == 0.0) return dist;
    }
    return dist;
}

double lineToPolygonDistance(const mapbox::geometry::line_string<double>& line,
                             const IndexRange& range,
                             const mapbox::geometry::polygon<double>& polygon,
                             mapbox::cheap_ruler::CheapRuler& ruler) {
    bool rangeSafe = (range.second >= range.first && range.second < line.size());
    if (!rangeSafe) {
        mbgl::Log::Error(mbgl::Event::General, "index is out of range");
        return InvalidDistance;
    }
    for (std::size_t i = range.first; i <= range.second; ++i) {
        if (pointWithinPolygon(line[i], polygon, true)) return 0.0;
    }

    double dist = InfiniteDistance;
    for (std::size_t i = range.first; i < range.second; ++i) {
        const auto& p1 = line[i];
        const auto& p2 = line[i + 1];
        for (const auto& ring : polygon) {
            for (std::size_t j = 0; j < ring.size() - 2; ++j) {
                const auto& q1 = ring[j];
                const auto& q2 = ring[j + 1];
                if (segmentIntersectSegment(p1, p2, q1, q2)) return 0.0;
                dist = std::min(dist, segmentToSegmentDistance(p1, p2, q1, q2, ruler));
            }
        }
    }
    return dist;
}

double polygonToPolygonDistance(const mapbox::geometry::polygon<double>& polygon1,
                                const mapbox::geometry::polygon<double>& polygon2,
                                mapbox::cheap_ruler::CheapRuler& ruler,
                                double currentMiniDist = InfiniteDistance) {
    const auto bbox1 = getBBox(polygon1);
    const auto bbox2 = getBBox(polygon2);
    if (currentMiniDist != InfiniteDistance && bboxToBBoxDistance(bbox1, bbox2, ruler) >= currentMiniDist)
        return currentMiniDist;

    const auto polygonIntersect = [](const mapbox::geometry::polygon<double>& poly1,
                                     const mapbox::geometry::polygon<double>& poly2) {
        for (const auto& ring : poly1) {
            for (std::size_t i = 0; i <= ring.size() - 2; ++i) {
                if (pointWithinPolygon(ring[i], poly2, true)) return true;
            }
        }
        return false;
    };
    if (boxWithinBox(bbox1, bbox2)) {
        if (polygonIntersect(polygon1, polygon2)) return 0.0;
    } else if (polygonIntersect(polygon2, polygon1))
        return 0.0;

    double dist = InfiniteDistance;
    for (const auto& ring1 : polygon1) {
        for (std::size_t i = 0; i < ring1.size() - 2; ++i) {
            const auto& p1 = ring1[i];
            const auto& p2 = ring1[i + 1];
            for (const auto& ring2 : polygon2) {
                for (std::size_t j = 0; j < ring2.size() - 2; ++j) {
                    const auto& q1 = ring2[j];
                    const auto& q2 = ring2[j + 1];
                    if (segmentIntersectSegment(p1, p2, q1, q2)) return 0.0;
                    dist = std::min(dist, segmentToSegmentDistance(p1, p2, q1, q2, ruler));
                }
            }
        }
    }
    return dist;
}

// <distance, range1, range2>
using DistPair = std::tuple<double, IndexRange, IndexRange>;
struct Comparator {
    bool operator()(DistPair& left, DistPair& right) { return std::get<0>(left) < std::get<0>(right); }
};
// The priority queue will ensure the top element would always be the pair that has the biggest distance
using DistQueue = std::priority_queue<DistPair, std::deque<DistPair>, Comparator>;

// Divide and conqure, the time complexity is O(n*lgn), faster than Brute force O(n*n)
// Most of the time, use index for in-place processing.

double lineToPolygonDistance(const mapbox::geometry::line_string<double>& line,
                             const mapbox::geometry::polygon<double>& polygon,
                             mapbox::cheap_ruler::CheapRuler& ruler,
                             double currentMiniDist = InfiniteDistance) {
    auto miniDist = std::min(ruler.distance(line[0], polygon[0][0]), currentMiniDist);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, line.size() - 1), IndexRange(0, 0)));

    const auto polyBBox = getBBox(polygon);
    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& range = std::get<1>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(range) <= MinLinePointsSize) {
            auto tempDist = lineToPolygonDistance(line, range, polygon, ruler);
            if (std::isnan(tempDist)) return tempDist;
            miniDist = std::min(miniDist, tempDist);
            if (miniDist == 0.0) return 0.0;
        } else {
            auto newRangesA = splitRange(range, true /*isLine*/);
            const auto updateQueue =
                [&distQueue, &miniDist, &ruler, &line, &polyBBox](mbgl::optional<IndexRange>& rangeA) {
                    if (!rangeA) return;
                    auto tempDist = bboxToBBoxDistance(getBBox(line, *rangeA), polyBBox, ruler);
                    // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                    // The pair with biggest distance will be at the top
                    if (tempDist <= miniDist)
                        distQueue.push(std::make_tuple(tempDist, std::move(*rangeA), IndexRange(0, 0)));
                };
            updateQueue(newRangesA.first);
            updateQueue(newRangesA.second);
        }
    }
    return miniDist;
}

double pointsToPolygonDistance(const mapbox::geometry::multi_point<double>& points,
                               const mapbox::geometry::polygon<double>& polygon,
                               mapbox::cheap_ruler::CheapRuler& ruler,
                               double currentMiniDist = InfiniteDistance) {
    auto miniDist = std::min(ruler.distance(points[0], polygon[0][0]), currentMiniDist);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, points.size() - 1), IndexRange(0, 0)));

    const auto polyBBox = getBBox(polygon);
    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& range = std::get<1>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(range) <= MinLinePointsSize) {
            for (std::size_t i = range.first; i <= range.second; ++i) {
                auto tempDist = pointToPolygonDistance(points[i], polygon, ruler);
                miniDist = std::min(miniDist, tempDist);
                if (miniDist == 0.0) return 0.0;
            }
        } else {
            auto newRangesA = splitRange(range, false /*isLine*/);
            const auto updateQueue =
                [&distQueue, &miniDist, &ruler, &points, &polyBBox](mbgl::optional<IndexRange>& rangeA) {
                    if (!rangeA) return;
                    auto tempDist = bboxToBBoxDistance(getBBox(points, *rangeA), polyBBox, ruler);
                    // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                    // The pair with biggest distance will be at the top
                    if (tempDist <= miniDist)
                        distQueue.push(std::make_tuple(tempDist, std::move(*rangeA), IndexRange(0, 0)));
                };
            updateQueue(newRangesA.first);
            updateQueue(newRangesA.second);
        }
    }
    return miniDist;
}

double lineToLineDistance(const mapbox::geometry::line_string<double>& line1,
                          const mapbox::geometry::multi_point<double>& line2,
                          mapbox::cheap_ruler::CheapRuler& ruler,
                          double currentMiniDist = InfiniteDistance) {
    auto miniDist = std::min(ruler.distance(line1[0], line2[0]), currentMiniDist);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, line1.size() - 1), IndexRange(0, line2.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinLinePointsSize && getRangeSize(rangeB) <= MinLinePointsSize) {
            auto tempDist = lineToLineDistance(line1, rangeA, line2, rangeB, ruler);
            if (std::isnan(tempDist)) return tempDist;
            miniDist = std::min(miniDist, tempDist);
            if (miniDist == 0.0) return 0.0;
        } else {
            auto newRangesA = splitRange(rangeA, true /*isLine*/);
            auto newRangesB = splitRange(rangeB, true /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &line1, &line2](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(line1, *range1), getBBox(line2, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

double pointsToPointsDistance(const mapbox::geometry::multi_point<double>& pointSet1,
                              const mapbox::geometry::multi_point<double>& pointSet2,
                              mapbox::cheap_ruler::CheapRuler& ruler) {
    auto miniDist = ruler.distance(pointSet1[0], pointSet2[0]);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, pointSet1.size() - 1), IndexRange(0, pointSet2.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) {
            continue;
        }
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinPointsSize && getRangeSize(rangeB) <= MinPointsSize) {
            auto tempDist = pointsToPointsDistance(pointSet1, rangeA, pointSet2, rangeB, ruler);
            if (std::isnan(tempDist)) return tempDist;
            miniDist = std::min(miniDist, tempDist);
            if (miniDist == 0.0) return 0.0;
        } else {
            auto newRangesA = splitRange(rangeA, false /*isLine*/);
            auto newRangesB = splitRange(rangeB, false /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &pointSet1, &pointSet2](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(pointSet1, *range1), getBBox(pointSet2, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

double pointsToLineDistance(const mapbox::geometry::multi_point<double>& points,
                            const mapbox::geometry::line_string<double>& line,
                            mapbox::cheap_ruler::CheapRuler& ruler) {
    auto miniDist = ruler.distance(points[0], line[0]);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, points.size() - 1), IndexRange(0, line.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinPointsSize && getRangeSize(rangeB) <= MinLinePointsSize) {
            bool rangeSafe = (rangeA.second >= rangeA.first && rangeA.second < points.size()) &&
                             (rangeB.second >= rangeB.first && rangeB.second < line.size());
            if (!rangeSafe) {
                mbgl::Log::Error(mbgl::Event::General, "index is out of range");
                return InvalidDistance;
            }
            auto subLine =
                mapbox::geometry::multi_point<double>(line.begin() + rangeB.first, line.begin() + rangeB.second + 1);
            for (std::size_t i = rangeA.first; i <= rangeA.second; ++i) {
                miniDist = std::min(miniDist, pointToLineDistance(points[i], subLine, ruler));
                if (miniDist == 0.0) return 0.0;
            }
        } else {
            auto newRangesA = splitRange(rangeA, false /*isLine*/);
            auto newRangesB = splitRange(rangeB, true /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &points, &line](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(points, *range1), getBBox(line, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

double pointsToLinesDistance(const mapbox::geometry::multi_point<double>& points,
                             const mapbox::geometry::multi_line_string<double>& lines,
                             mapbox::cheap_ruler::CheapRuler& ruler) {
    double dist = InfiniteDistance;
    for (const auto& line : lines) {
        dist = std::min(dist, pointsToLineDistance(points, line, ruler));
        if (dist == 0.0) return 0.0;
    }
    return dist;
}

double lineToLinesDistance(const mapbox::geometry::line_string<double>& line,
                           const mapbox::geometry::multi_line_string<double>& lines,
                           mapbox::cheap_ruler::CheapRuler& ruler) {
    double dist = InfiniteDistance;
    for (const auto& l : lines) {
        dist = std::min(dist, lineToLineDistance(line, l, ruler, dist));
        if (dist == 0.0) return 0.0;
    }
    return dist;
}

double pointsToGeometryDistance(const mapbox::geometry::multi_point<double>& points,
                                const Feature::geometry_type& geoSet) {
    mapbox::cheap_ruler::CheapRuler ruler(points.front().y, UnitInMeters);
    return geoSet.match(
        [&points, &ruler](const mapbox::geometry::point<double>& p) {
            return pointsToPointsDistance(mapbox::geometry::multi_point<double>{p}, points, ruler);
        },
        [&points, &ruler](const mapbox::geometry::multi_point<double>& points1) {
            return pointsToPointsDistance(points, points1, ruler);
        },
        [&points, &ruler](const mapbox::geometry::line_string<double>& line) {
            return pointsToLineDistance(points, line, ruler);
        },
        [&points, &ruler](const mapbox::geometry::multi_line_string<double>& lines) {
            return pointsToLinesDistance(points, lines, ruler);
        },
        [&points, &ruler](const mapbox::geometry::polygon<double>& polygon) -> double {
            return pointsToPolygonDistance(points, polygon, ruler);
        },
        [&points, &ruler](const mapbox::geometry::multi_polygon<double>& polygons) -> double {
            double dist = InfiniteDistance;
            for (const auto& polygon : polygons) {
                auto tempDist = pointsToPolygonDistance(points, polygon, ruler, dist);
                if (std::isnan(tempDist)) return tempDist;
                dist = std::min(dist, tempDist);
                if (dist == 0.0 || std::isnan(dist)) return dist;
            }
            return dist;
        },
        [](const auto&) { return InvalidDistance; });
}

double lineToGeometryDistance(const mapbox::geometry::line_string<double>& line, const Feature::geometry_type& geoSet) {
    assert(!line.empty());
    mapbox::cheap_ruler::CheapRuler ruler(line.front().y, UnitInMeters);
    return geoSet.match(
        [&line, &ruler](const mapbox::geometry::point<double>& p) {
            return pointsToLineDistance(mapbox::geometry::multi_point<double>{p}, line, ruler);
        },
        [&line, &ruler](const mapbox::geometry::multi_point<double>& points) {
            return pointsToLineDistance(points, line, ruler);
        },
        [&line, &ruler](const mapbox::geometry::line_string<double>& line1) {
            return lineToLineDistance(line, line1, ruler);
        },
        [&line, &ruler](const mapbox::geometry::multi_line_string<double>& lines) {
            return lineToLinesDistance(line, lines, ruler);
        },
        [&line, &ruler](const mapbox::geometry::polygon<double>& polygon) -> double {
            return lineToPolygonDistance(line, polygon, ruler);
        },
        [&line, &ruler](const mapbox::geometry::multi_polygon<double>& polygons) -> double {
            double dist = InfiniteDistance;
            for (const auto& polygon : polygons) {
                auto tempDist = lineToPolygonDistance(line, polygon, ruler, dist);
                if (std::isnan(tempDist)) return tempDist;
                dist = std::min(dist, tempDist);
                if (dist == 0.0 || std::isnan(dist)) return dist;
            }
            return dist;
        },
        [](const auto&) { return InvalidDistance; });
}

double polygonToGeometryDistance(const mapbox::geometry::polygon<double>& polygon,
                                 const Feature::geometry_type& geoSet) {
    assert(!polygon.empty());
    mapbox::cheap_ruler::CheapRuler ruler(polygon.front().front().y, UnitInMeters);
    return geoSet.match(
        [&polygon, &ruler](const mapbox::geometry::point<double>& p) {
            return pointToPolygonDistance(p, polygon, ruler);
        },
        [&polygon, &ruler](const mapbox::geometry::multi_point<double>& points) {
            return pointsToPolygonDistance(points, polygon, ruler);
        },
        [&polygon, &ruler](const mapbox::geometry::line_string<double>& line) {
            return lineToPolygonDistance(line, polygon, ruler);
        },
        [&polygon, &ruler](const mapbox::geometry::multi_line_string<double>& lines) {
            double dist = InfiniteDistance;
            for (const auto& line : lines) {
                auto tempDist = lineToPolygonDistance(line, polygon, ruler);
                dist = std::min(dist, tempDist);
                if (dist == 0.0 || std::isnan(dist)) return dist;
            }
            return dist;
        },
        [&polygon, &ruler](const mapbox::geometry::polygon<double>& polygon1) {
            return polygonToPolygonDistance(polygon, polygon1, ruler);
        },
        [&polygon, &ruler](const mapbox::geometry::multi_polygon<double>& polygons) {
            double dist = InfiniteDistance;
            for (const auto& polygon1 : polygons) {
                auto tempDist = polygonToPolygonDistance(polygon, polygon1, ruler, dist);
                dist = std::min(dist, tempDist);
                if (dist == 0.0 || std::isnan(dist)) return dist;
            }
            return dist;
        },
        [](const auto&) { return InvalidDistance; });
}

double calculateDistance(const GeometryTileFeature& feature,
                         const CanonicalTileID& canonical,
                         const Feature::geometry_type& geoSet) {
    return convertGeometry(feature, canonical)
        .match(
            [&geoSet](const mapbox::geometry::point<double>& point) -> double {
                return pointsToGeometryDistance(mapbox::geometry::multi_point<double>{point}, geoSet);
            },
            [&geoSet](const mapbox::geometry::multi_point<double>& points) -> double {
                return pointsToGeometryDistance(points, geoSet);
            },
            [&geoSet](const mapbox::geometry::line_string<double>& line) -> double {
                return lineToGeometryDistance(line, geoSet);
            },
            [&geoSet](const mapbox::geometry::multi_line_string<double>& lines) -> double {
                double dist = InfiniteDistance;
                for (const auto& line : lines) {
                    auto tempDist = lineToGeometryDistance(line, geoSet);
                    if (std::isnan(tempDist)) return tempDist;
                    dist = std::min(dist, tempDist);
                    if (dist == 0.0 || std::isnan(dist)) return dist;
                }
                return dist;
            },
            [&geoSet](const mapbox::geometry::polygon<double>& polygon) -> double {
                return polygonToGeometryDistance(polygon, geoSet);
            },
            [&geoSet](const mapbox::geometry::multi_polygon<double>& polygons) -> double {
                double dist = InfiniteDistance;
                for (const auto& polygon : polygons) {
                    auto tempDist = polygonToGeometryDistance(polygon, geoSet);
                    if (std::isnan(tempDist)) return tempDist;
                    dist = std::min(dist, tempDist);
                    if (dist == 0.0 || std::isnan(dist)) return dist;
                }
                return dist;
            },
            [](const auto&) -> double { return InvalidDistance; });
}

optional<GeoJSON> parseValue(const style::conversion::Convertible& value, style::expression::ParsingContext& ctx) {
    if (isArray(value)) {
        // object value, quoted with ["distance", GeoJSONObj]
        auto length = arrayLength(value);
        if (length != 2) {
            ctx.error("'distance' expression requires one argument, but found " +
                      util::toString(arrayLength(value) - 1) + " instead.");
            return nullopt;
        }

        // Parse geometry info
        const auto& argument = arrayMember(value, 1);
        if (isObject(argument)) {
            style::conversion::Error error;
            auto geojson = toGeoJSON(argument, error);
            if (geojson && error.message.empty()) {
                return *geojson;
            }
            ctx.error(error.message);
        }
    }
    ctx.error("'distance' expression needs to be an array with format [\"distance\", GeoJSONObj].");
    return nullopt;
}

optional<Feature::geometry_type> getGeometry(const Feature& feature, mbgl::style::expression::ParsingContext& ctx) {
    const auto type = apply_visitor(ToFeatureType(), feature.geometry);
    if (type != FeatureType::Unknown) {
        return feature.geometry;
    }
    ctx.error(
        "'distance' expression requires valid geojson object with valid geometry type: Point, LineString or Polygon.");
    return nullopt;
}
} // namespace

namespace style {
namespace expression {

Distance::Distance(GeoJSON geojson, Feature::geometry_type geometries_)
    : Expression(Kind::Distance, type::Number), geoJSONSource(std::move(geojson)), geometries(std::move(geometries_)) {}

Distance::~Distance() = default;

using namespace mbgl::style::conversion;

EvaluationResult Distance::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return EvaluationError{"distance expression requirs valid feature and canonical information."};
    }
    auto geometryType = params.feature->getType();
    if (geometryType != FeatureType::Unknown) {
        auto distance = calculateDistance(*params.feature, *params.canonical, geometries);
        if (!std::isnan(distance)) {
            assert(distance >= 0.0);
            return distance;
        }
    }
    return EvaluationError{"distance expression currently only evaluates Point/LineString/Polygon geometries."};
}

ParseResult Distance::parse(const Convertible& value, ParsingContext& ctx) {
    auto parsedValue = parseValue(value, ctx);
    if (!parsedValue) {
        return ParseResult();
    }

    return parsedValue->match(
        [&parsedValue, &ctx](const mapbox::geometry::geometry<double>& geometrySet) {
            if (auto ret = getGeometry(mbgl::Feature(geometrySet), ctx)) {
                return ParseResult(std::make_unique<Distance>(*parsedValue, std::move(*ret)));
            }
            return ParseResult();
        },
        [&parsedValue, &ctx](const mapbox::feature::feature<double>& feature) {
            if (auto ret = getGeometry(mbgl::Feature(feature), ctx)) {
                return ParseResult(std::make_unique<Distance>(*parsedValue, std::move(*ret)));
            }
            return ParseResult();
        },
        [&parsedValue, &ctx](const mapbox::feature::feature_collection<double>& features) {
            for (const auto& feature : features) {
                if (auto ret = getGeometry(mbgl::Feature(feature), ctx)) {
                    return ParseResult(std::make_unique<Distance>(*parsedValue, std::move(*ret)));
                }
            }
            return ParseResult();
        },
        [&ctx](const auto&) {
            ctx.error("'distance' expression requires valid geojson that contains LineString/Point geometries.");
            return ParseResult();
        });

    return ParseResult();
}

mbgl::Value convertValue(const mapbox::geojson::rapidjson_value& v) {
    if (v.IsNumber()) {
        if (v.IsInt64()) return std::int64_t(v.GetInt64());
        if (v.IsUint64()) return std::uint64_t(v.GetUint64());
        return v.GetDouble();
    }
    if (v.IsBool()) {
        return v.GetBool();
    }
    if (v.IsString()) {
        return std::string(v.GetString());
    }
    if (v.IsArray()) {
        std::vector<mbgl::Value> result;
        result.reserve(v.Size());
        for (const auto& m : v.GetArray()) {
            result.push_back(convertValue(m));
        }
        return result;
    }
    if (v.IsObject()) {
        std::unordered_map<std::string, mbgl::Value> result;
        for (const auto& m : v.GetObject()) {
            result.emplace(m.name.GetString(), convertValue(m.value));
        }
        return result;
    }
    // Ignore other types as valid geojson only contains above types.
    return Null;
}

mbgl::Value Distance::serialize() const {
    std::unordered_map<std::string, mbgl::Value> serialized;
    rapidjson::CrtAllocator allocator;
    const mapbox::geojson::rapidjson_value value = mapbox::geojson::convert(geoJSONSource, allocator);
    if (value.IsObject()) {
        for (const auto& m : value.GetObject()) {
            serialized.emplace(m.name.GetString(), convertValue(m.value));
        }
    } else {
        mbgl::Log::Error(mbgl::Event::General,
                         "Failed to serialize 'distance' expression, converted rapidJSON is not an object");
    }
    return std::vector<mbgl::Value>{{getOperator(), serialized}};
}

bool Distance::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Distance) {
        auto rhs = static_cast<const Distance*>(&e);
        return geoJSONSource == rhs->geoJSONSource && geometries == rhs->geometries;
    }
    return false;
}

std::vector<optional<Value>> Distance::possibleOutputs() const {
    return {nullopt};
}

std::string Distance::getOperator() const {
    return "distance";
}

} // namespace expression
} // namespace style
} // namespace mbgl
