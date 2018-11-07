package com.mapbox.mapboxsdk.style

import android.support.test.espresso.UiController
import com.mapbox.geojson.Feature
import com.mapbox.geojson.FeatureCollection
import com.mapbox.geojson.Point
import com.mapbox.mapboxsdk.constants.Style
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest
import com.mapbox.mapboxsdk.testapp.activity.maplayout.SimpleMapActivity
import org.junit.Test

class CustomGeometrySourceTest : BaseActivityTest() {

    override fun getActivityClass(): Class<*> = SimpleMapActivity::class.java

    @Test
    fun sourceUpdateAfterStyleChange() {
        validateTestSetup()
        invoke(mapboxMap) { uiController: UiController, mapboxMap: MapboxMap ->
            val style = mapboxMap.style

            val source = GeoJsonSource(
                    "source-id",
                    FeatureCollection.fromFeature(
                            Feature.fromGeometry(
                                    Point.fromLngLat(0.0, 0.0)
                            )
                    )
            )
            style.addSource(source)

            style.loadStyle(Style.DARK)

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
            val style = mapboxMap.style

            val source = GeoJsonSource(
                    "source-id",
                    FeatureCollection.fromFeature(
                            Feature.fromGeometry(
                                    Point.fromLngLat(0.0, 0.0)
                            )
                    )
            )
            style.addSource(source)

            val layer = SymbolLayer("test", "source-id")
            style.addLayer(layer)

            style.loadStyle(Style.DARK)

            layer.setProperties(iconImage("bus-15"))

            uiController.loopMainThreadForAtLeast(300)

            layer.setProperties(iconImage("bus-15"))
        }
    }

}