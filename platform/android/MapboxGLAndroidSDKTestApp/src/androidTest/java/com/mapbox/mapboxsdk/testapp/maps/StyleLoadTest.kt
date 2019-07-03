package com.mapbox.mapboxsdk.testapp.maps

import android.support.test.espresso.UiController
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.maps.MapboxMap
import com.mapbox.mapboxsdk.maps.Style
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest
import com.mapbox.mapboxsdk.testapp.utils.TestingAsyncUtils
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class StyleLoadTest : EspressoTest() {

    @Test
    fun updateSourceAfterStyleLoad() {
        validateTestSetup()
        MapboxMapAction.invoke(mapboxMap) { uiController: UiController, mapboxMap: MapboxMap ->
            val source = GeoJsonSource("id")
            val layer = SymbolLayer("id", "id")
            mapboxMap.setStyle(Style.Builder().withSource(source).withLayer(layer))
            TestingAsyncUtils.waitForLayer(uiController, mapView)
            mapboxMap.setStyle(Style.Builder().fromUrl(Style.MAPBOX_STREETS))
            TestingAsyncUtils.waitForLayer(uiController, mapView)
            source.setGeoJson("{}")
        }
    }
}