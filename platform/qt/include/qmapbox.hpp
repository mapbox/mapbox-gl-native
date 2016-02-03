#ifndef QMAPBOX_H
#define QMAPBOX_H

#include <QList>
#include <QPair>
#include <QString>

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

namespace QMapbox {

typedef QPair<double, double> Coordinate;
typedef QList<Coordinate> Coordinates;
typedef QList<Coordinates> CoordinateSegments;

typedef QPair<Coordinate, double> CoordinateZoom;

typedef quint32 AnnotationID;
typedef QList<AnnotationID> AnnotationIDs;

typedef QPair<Coordinate, QString> PointAnnotation;
typedef QList<PointAnnotation> PointAnnotations;

// FIXME: We need to add support for custom style properties
typedef QPair<CoordinateSegments, QString> ShapeAnnotation;
typedef QList<ShapeAnnotation> ShapeAnnotations;

}

#endif // QMAPBOX_H
