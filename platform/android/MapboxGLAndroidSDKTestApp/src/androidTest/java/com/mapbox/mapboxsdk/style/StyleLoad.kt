package com.mapbox.mapboxsdk.style

import android.support.test.espresso.Espresso
import android.support.test.espresso.Espresso.onView
import android.support.test.espresso.UiController
import android.support.test.espresso.matcher.ViewMatchers.isRoot
import com.mapbox.geojson.Feature
import com.mapbox.geojson.FeatureCollection
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_POOL_LIMIT
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource.THREAD_PREFIX
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationLandscape
import com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationPortrait
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import com.mapbox.mapboxsdk.testapp.activity.style.GridSourceActivity
import org.junit.Assert
import org.junit.Ignore
import org.junit.Test

class CustomGeometrySourceTest : BaseActivityTest() {

    override fun getActivityClass(): Class<*> = SimpleMapActivity::class.java

    @Test
    fun sourceUpdateAfterStyleChange() {
        validateTestSetup()
        invoke(mapboxMap) { uiController: UiController, mapboxMap: MapboxMap ->
            val source = GeoJsonSource(
                    "source-id",
                    FeatureCollection.fromFeature(
                            Feature.fromGeometry(
                                    Point.fromLngLat(0.0, 0.0)
                            )
                    )
            )
            mapboxMap.addSource(source)

            mapboxMap.setStyle(Style.DARK)

            uiController.loopMainThreadForAtLeast(300)

            source.setGeoJson(
                    FeatureCollection.fromFeature(
                            Feature.fromGeometry(
                                    Point.fromLngLat(1.0, 1.0)
                            )
                    )
            )
        }
    }

    @Test
    fun layerUpdateAfterStyleChange() {
        validateTestSetup()
        invoke(mapboxMap) { uiController: UiController, mapboxMap: MapboxMap ->
            val source = GeoJsonSource(
                    "source-id",
                    FeatureCollection.fromFeature(
                            Feature.fromGeometry(
                                    Point.fromLngLat(0.0, 0.0)
                            )
                    )
            )
            mapboxMap.addSource(source)
            val layer = SymbolLayer("test", "source-id")
            mapboxMap.addLayerAbove(layer,"background")

            mapboxMap.setStyle(Style.DARK)
            layer.setProperties(iconImage("bus-15"))
            uiController.loopMainThreadForAtLeast(300)
            layer.setProperties(iconImage("bus-15"))
        }
    }

}