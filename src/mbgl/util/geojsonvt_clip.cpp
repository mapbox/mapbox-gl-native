#include "geojsonvt_clip.hpp"

namespace mapbox { namespace util { namespace geojsonvt {

std::vector<ProjectedFeature> Clip::clip(const std::vector<ProjectedFeature> features, uint32_t scale, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double)) {

    k1 /= scale;
    k2 /= scale;

    std::vector<ProjectedFeature> clipped;

    for (size_t i = 0; i < features.size(); ++i) {

        const ProjectedFeature feature = features[i];
        const ProjectedGeometry geometry = feature.geometry;
        const ProjectedFeatureType type = feature.type;
        double min = 0;
        double max = 0;

        if (feature.minPoint) {
            min = (axis == 0 ? feature.minPoint.x : feature.minPoint.y);
            max = (axis == 0 ? feature.maxPoint.x : feature.maxPoint.y);

            if (min >= k1 && max <= k2) {
                clipped.push_back(feature);
                continue;
            } else if (min > k2 || max < k1) {
                continue;
            }
        }

        ProjectedGeometryContainer slices;

        if (type == ProjectedFeatureType::Point) {
            slices = clipPoints(geometry.get<ProjectedGeometryContainer>(), k1, k2, axis);
        } else {
            slices = clipGeometry(geometry.get<ProjectedGeometryContainer>(), k1, k2, axis, intersect, (type == ProjectedFeatureType::Polygon));
        }

        if (slices.members.size()) {
            clipped.push_back(ProjectedFeature(slices, type, features[i].tags));
        }
    }

    return std::move(clipped);
}

ProjectedGeometryContainer Clip::clipPoints(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis) {

    ProjectedGeometryContainer slice;

    for (size_t i = 0; i < geometry.members.size(); ++i) {
        ProjectedPoint *a = &(geometry.members[i].get<ProjectedPoint>());
        double ak = (axis == 0 ? a->x : a->y);

        if (ak >= k1 && ak <= k2) {
            slice.members.push_back(*a);
        }
    }

    return std::move(slice);
}

ProjectedGeometryContainer Clip::clipGeometry(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double), bool closed) {

    ProjectedGeometryContainer slices;

    for (size_t i = 0; i < geometry.members.size(); ++i) {

        double ak = 0;
        double bk = 0;
        ProjectedPoint b;
        const ProjectedGeometryContainer *points = &(geometry.members[i].get<ProjectedGeometryContainer>());
        const double area = points->area;
        const double dist = points->dist;
        const size_t len = points->members.size();
        ProjectedPoint a;

        ProjectedGeometryContainer slice;

        for (size_t j = 0; j < (len - 1); ++j) {
            a = (b ? b : points->members[j].get<ProjectedPoint>());
            b = points->members[j + 1].get<ProjectedPoint>();
            ak = (bk ? bk : (axis == 0 ? a.x : a.y));
            bk = (axis == 0 ? b.x : b.y);

            if (ak < k1) {
                if (bk > k2) {
                    slice.members.push_back(intersect(a, b, k1));
                    slice.members.push_back(intersect(a, b, k2));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk >= k1) {
                    slice.members.push_back(intersect(a, b, k1));
                }
            } else if (ak > k2) {
                if (bk < k1) {
                    slice.members.push_back(intersect(a, b, k2));
                    slice.members.push_back(intersect(a, b, k1));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk <= k2) {
                    slice.members.push_back(intersect(a, b, k2));
                }
            } else {
                slice.members.push_back(a);

                if (bk < k1) {
                    slice.members.push_back(intersect(a, b, k1));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk > k2) {
                    slice.members.push_back(intersect(a, b, k2));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                }
            }
        }

        a = points->members[len - 1].get<ProjectedPoint>();
        ak = (axis == 0 ? a.x : a.y);

        if (ak >= k1 && ak <= k2) {
            slice.members.push_back(a);
        }

        if (closed && slice.members.size()) {
            const ProjectedPoint *first = &(slice.members[0].get<ProjectedPoint>());
            const ProjectedPoint *last  = &(slice.members[slice.members.size() - 1].get<ProjectedPoint>());
            if (first != last) {
                slice.members.push_back(ProjectedPoint(first->x, first->y, first->z));
            }
        }

        newSlice(slices, slice, area, dist);
    }

    return std::move(slices);
}

ProjectedGeometryContainer Clip::newSlice(ProjectedGeometryContainer &slices, ProjectedGeometryContainer &slice, double area, double dist) {

    if (slice.members.size()) {
        slice.area = area;
        slice.dist = dist;
        slices.members.push_back(slice);
    }

    return ProjectedGeometryContainer();
}

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
