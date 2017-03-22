#pragma once

#include "../../conversion/constant.hpp"
#include "../../conversion/collection.hpp"

#include <mapbox/geometry.hpp>
#include <jni/jni.hpp>
#include "../../jni/local_object.hpp"

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Turn mapbox::geometry type into Java GeoJson Geometries
 */
template <typename T>
class GeometryEvaluator {
public:

    jni::JNIEnv& env;

    /**
     * Point (double[])
     */
    jni::jobject* operator()(const mapbox::geometry::point<T> &geometry) const {
       static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/Point")).release();
       static jni::jmethodID* fromCoordinates = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([D)Lcom/mapbox/services/commons/geojson/Point;");

       // Create Point
       jni::LocalObject<jni::jarray<jni::jdouble>> position = jni::NewLocalObject(env, toGeoJsonPosition(env, geometry.x, geometry.y));
       return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromCoordinates, position.get()));
    }

    /**
     * LineString (double[][])
     */
    jni::jobject* operator()(const mapbox::geometry::line_string<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/LineString")).release();
        static jni::jmethodID* fromCoordinates = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([[D)Lcom/mapbox/services/commons/geojson/LineString;");

        // Create
        jni::LocalObject<jni::jarray<jni::jobject>> coordinates = jni::NewLocalObject(env, toGeoJsonCoordinates(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromCoordinates, coordinates.get()));
    }

    /**
     * MultiPoint (double[][])
     */
    jni::jobject* operator()(const mapbox::geometry::multi_point<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/MultiPoint")).release();
        static jni::jmethodID* fromCoordinates = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([[D)Lcom/mapbox/services/commons/geojson/MultiPoint;");

        // Create
        jni::LocalObject<jni::jarray<jni::jobject>> coordinates = jni::NewLocalObject(env, toGeoJsonCoordinates(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromCoordinates, coordinates.get()));
    }

    /**
     * Polygon (double[][][])
     */
    jni::jobject* operator()(const mapbox::geometry::polygon<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/Polygon")).release();
        static jni::jmethodID* fromCoordinates = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([[[D)Lcom/mapbox/services/commons/geojson/Polygon;");

        // Create
        jni::LocalObject<jni::jarray<jni::jobject>> shape = jni::NewLocalObject(env, toShape<>(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromCoordinates, shape.get()));
    }

    /**
     * MultiLineString (double[][][])
     */
    jni::jobject*  operator()(const mapbox::geometry::multi_line_string<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/MultiLineString")).release();
        static jni::jmethodID* fromCoordinates = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([[[D)Lcom/mapbox/services/commons/geojson/MultiLineString;");

        // Create
        jni::LocalObject<jni::jarray<jni::jobject>> shape = jni::NewLocalObject(env, toShape<>(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromCoordinates, shape.get()));
    }

    /**
     * MultiPolygon (double[][][][]) -> [[[D + Object array ==  [[[[D
     */
    jni::jobject*  operator()(const mapbox::geometry::multi_polygon<T> &geometry) const {
        static jni::jclass* listClass = jni::NewGlobalRef(env, &jni::FindClass(env, "[[[D")).release();
        jni::LocalObject<jni::jarray<jni::jobject>> jarray = jni::NewLocalObject(env, &jni::NewObjectArray(env, geometry.size(), *listClass));

        for(size_t i = 0; i < geometry.size(); i = i + 1) {
            jni::LocalObject<jni::jarray<jni::jobject>> shape = jni::NewLocalObject(env, toShape<>(env, geometry.at(i)));
            jni::SetObjectArrayElement(env, *jarray, i, shape.get());
        }

        // Create the MultiPolygon
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/MultiPolygon")).release();
        static jni::jmethodID* fromGeometries = &jni::GetStaticMethodID(env, *javaClass, "fromCoordinates", "([[[[D)Lcom/mapbox/services/commons/geojson/MultiPolygon;");
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromGeometries, jarray.get()));
    }

    /**
     * GeometryCollection
     */
    jni::jobject*  operator()(const mapbox::geometry::geometry_collection<T> &collection) const {
        static jni::jclass* geometryClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/Geometry")).release();
        jni::LocalObject<jni::jarray<jni::jobject>> jarray = jni::NewLocalObject(env, &jni::NewObjectArray(env, collection.size(), *geometryClass));

        for(size_t i = 0; i < collection.size(); i = i + 1) {
            auto& geometry = collection.at(i);
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mapbox::geometry::geometry<T>::visit(geometry, *this));
            jni::SetObjectArrayElement(env, *jarray, i, converted.get());
        }

        // Turn into array list and create the GeometryCollection
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/GeometryCollection")).release();
        static jni::jmethodID* fromGeometries = &jni::GetStaticMethodID(env, *javaClass, "fromGeometries", "(Ljava/util/List;)Lcom/mapbox/services/commons/geojson/GeometryCollection;");

        jni::LocalObject<jni::jobject> list = jni::NewLocalObject(env, toArrayList<>(env, *jarray));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromGeometries, list.get()));
    }

private:

    /**
     * x, y -> jarray<jdouble> ([x,y])
     */
    static jni::jarray<jni::jdouble>* toGeoJsonPosition(JNIEnv& env, double x, double y) {
        jni::jarray<jni::jdouble>& jarray = jni::NewArray<jni::jdouble>(env, 2);
        jni::jdouble array[] = {x, y};
        jni::SetArrayRegion(env, jarray, 0, 2, array);
        return &jarray;
    }

    /**
     * vector<point<T>> -> jarray<jobject> (double[][]) -> [D + Object array ==  [[D
     */
    static jni::jarray<jni::jobject>* toGeoJsonCoordinates(JNIEnv& env, std::vector<mapbox::geometry::point<T>> points) {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "[D")).release();
        jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, points.size(), *javaClass);

        for(size_t i = 0; i < points.size(); i = i + 1) {
            mapbox::geometry::point<T> point = points.at(i);
            jni::LocalObject<jni::jarray<jni::jdouble>> position = jni::NewLocalObject(env, toGeoJsonPosition(env, point.x, point.y));
            jni::SetObjectArrayElement(env, jarray, i, position.get());
        }

        return &jarray;
    }

    /**
     * polygon<T>
     * multi_line_string<T>
     *       -> jarray<jobject> (double[][][]) -> [[D + Object array ==  [[[D
     */
    template <class SHAPE>
    static jni::jarray<jni::jobject>* toShape(JNIEnv& env, SHAPE value) {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "[[D")).release();
        jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *javaClass);

        for(size_t i = 0; i < value.size(); i = i + 1) {
            jni::LocalObject<jni::jarray<jni::jobject>> coordinates = jni::NewLocalObject(env, toGeoJsonCoordinates(env, value.at(i)));
            jni::SetObjectArrayElement(env, jarray, i, coordinates.get());
        }

        return &jarray;
    }
};

/**
 * mapbox::geometry::geometry<T> -> Java GeoJson Geometry<>
 */
template <class T>
struct Converter<jni::jobject*, mapbox::geometry::geometry<T>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mapbox::geometry::geometry<T>& value) const {
       GeometryEvaluator<double> evaluator { env } ;
       jni::jobject* converted = mapbox::geometry::geometry<double>::visit(value, evaluator);
       return {converted};
    }
};


}
}
}
