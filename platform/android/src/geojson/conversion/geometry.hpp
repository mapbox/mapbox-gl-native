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
     * static Point fromLngLat(double longitude,double latitude)
     */
    jni::jobject* operator()(const mapbox::geometry::point<T> &geometry) const {
       static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/Point")).release();
       static jni::jmethodID* fromLngLat = &jni::GetStaticMethodID(env, *javaClass, "fromLngLat", "(DD)Lcom/mapbox/geojson/Point;");

       return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromLngLat, geometry.x, geometry.y));
    }

    /**
     * static LineString fromLngLats(List<Point> points)
     */
    jni::jobject* operator()(const mapbox::geometry::line_string<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/LineString")).release();
        static jni::jmethodID* fromLngLats = &jni::GetStaticMethodID(env, *javaClass, "fromLngLats", "(Ljava/util/List;)Lcom/mapbox/geojson/LineString;");

        // Create
        jni::LocalObject<jni::jobject> listOfPoints = jni::NewLocalObject(env, toGeoJsonListOfPoints(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromLngLats, listOfPoints.get()));
    }

    /**
     * static MultiPoint fromLngLats(List<Point> points)
     */
    jni::jobject* operator()(const mapbox::geometry::multi_point<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/MultiPoint")).release();
        static jni::jmethodID* fromLngLats = &jni::GetStaticMethodID(env, *javaClass, "fromLngLats", "(Ljava/util/List;)Lcom/mapbox/geojson/MultiPoint;");

        // Create
        jni::LocalObject<jni::jobject> coordinates = jni::NewLocalObject(env, toGeoJsonListOfPoints(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromLngLats, coordinates.get()));
    }

    /**
     * static Polygon fromLngLats(List<List<Point>> coordinates)
     */
    jni::jobject* operator()(const mapbox::geometry::polygon<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/Polygon")).release();
        static jni::jmethodID* fromLngLats = &jni::GetStaticMethodID(env, *javaClass, "fromLngLats", "(Ljava/util/List;)Lcom/mapbox/geojson/Polygon;");

        // Create
        jni::LocalObject<jni::jobject> shape = jni::NewLocalObject(env, toShape<>(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromLngLats, shape.get()));
    }

    /**
     * static MultiLineString fromLngLats(List<List<Point>> points)
     */
    jni::jobject*  operator()(const mapbox::geometry::multi_line_string<T> &geometry) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/MultiLineString")).release();
        static jni::jmethodID* fromLngLats = &jni::GetStaticMethodID(env, *javaClass, "fromLngLats", "(Ljava/util/List;)Lcom/mapbox/geojson/MultiLineString;");

        // Create
        jni::LocalObject<jni::jobject> shape = jni::NewLocalObject(env, toShape<>(env, geometry));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromLngLats, shape.get()));
    }

    /**
     * MultiPolygon (double[][][][]) -> [[[D + Object array ==  [[[[D
     *
     * static MultiPolygon fromLngLats(List<List<List<Point>>> points)
     */
    jni::jobject*  operator()(const mapbox::geometry::multi_polygon<T> &geometry) const {
        // ArrayList
        static jni::jclass* arrayListClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/ArrayList")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *arrayListClass, "<init>", "(I)V");
        static jni::jmethodID* add = &jni::GetMethodID(env, *arrayListClass, "add", "(ILjava/lang/Object;)V");
        jni::jobject* arrayList = &jni::NewObject(env, *arrayListClass, *constructor, geometry.size());

        for(size_t i = 0; i < geometry.size(); i = i + 1) {
            jni::LocalObject<jni::jobject> shape = jni::NewLocalObject(env, toShape<>(env, geometry.at(i)));
            jni::CallMethod<void>(env, arrayList, *add, i, shape.get());
        }

        // Create the MultiPolygon
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/MultiPolygon")).release();
        static jni::jmethodID* fromGeometries = &jni::GetStaticMethodID(env, *javaClass, "fromLngLats", "(Ljava/util/List;)Lcom/mapbox/geojson/MultiPolygon;");
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromGeometries, arrayList));
    }

    /**
     * GeometryCollection
     */
    jni::jobject*  operator()(const mapbox::geometry::geometry_collection<T> &collection) const {
        static jni::jclass* geometryClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/Geometry")).release();
        jni::LocalObject<jni::jarray<jni::jobject>> jarray = jni::NewLocalObject(env, &jni::NewObjectArray(env, collection.size(), *geometryClass));

        for(size_t i = 0; i < collection.size(); i = i + 1) {
            auto& geometry = collection.at(i);
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mapbox::geometry::geometry<T>::visit(geometry, *this));
            jni::SetObjectArrayElement(env, *jarray, i, converted.get());
        }

        // Turn into array list and create the GeometryCollection
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/GeometryCollection")).release();
        static jni::jmethodID* fromGeometries = &jni::GetStaticMethodID(env, *javaClass, "fromGeometries", "(Ljava/util/List;)Lcom/mapbox/geojson/GeometryCollection;");

        jni::LocalObject<jni::jobject> list = jni::NewLocalObject(env, toArrayList<>(env, *jarray));
        return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromGeometries, list.get()));
    }

private:

    /**
     * vector<point<T>> -> List<Point>
   */
    static jni::jobject* toGeoJsonListOfPoints(JNIEnv& env, std::vector<mapbox::geometry::point<T>> points) {

        // ArrayList
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/ArrayList")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(I)V");
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(ILjava/lang/Object;)V");
        jni::jobject* arrayList = &jni::NewObject(env, *javaClass, *constructor, points.size());


        // Point
        static jni::jclass* pointJavaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/geojson/Point")).release();
        static jni::jmethodID* fromLngLat = &jni::GetStaticMethodID(env, *pointJavaClass, "fromLngLat", "(DD)Lcom/mapbox/geojson/Point;");

        for(size_t i = 0; i < points.size(); i = i + 1) {
            mapbox::geometry::point<T> point = points.at(i);
            jni::LocalObject<jni::jobject> pointObject =
                    jni::NewLocalObject(env, jni::CallStaticMethod<jni::jobject*>(env, *pointJavaClass, *fromLngLat, point.x, point.y));
            jni::CallMethod<void>(env, arrayList, *add, i, pointObject.get());
        }

        return arrayList;
    }

    /**
     *  geometry -> List<List<Point>>
     */
    template <class SHAPE>
    static jni::jobject* toShape(JNIEnv& env, SHAPE value) {

        // ArrayList
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/ArrayList")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(I)V");
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(ILjava/lang/Object;)V");
        jni::jobject* arrayList = &jni::NewObject(env, *javaClass, *constructor, value.size());


        for(size_t i = 0; i < value.size(); i = i + 1) {
            jni::LocalObject<jni::jobject> listOfPoints = jni::NewLocalObject(env, toGeoJsonListOfPoints(env, value.at(i)));
            jni::CallMethod<void>(env, arrayList, *add, i, listOfPoints.get());
        }

        return arrayList;
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
