package com.mapbox.mapboxsdk.maps

import android.support.test.InstrumentationRegistry
import android.support.test.runner.AndroidJUnit4
import com.mapbox.mapboxsdk.AppCenter
import com.mapbox.mapboxsdk.style.layers.Layer
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
abstract class BaseLayerTest : AppCenter(){
    private lateinit var nativeMapView: NativeMap

    companion object {
        const val WIDTH = 500
        const val HEIGHT = WIDTH
    }

    fun before() {
        val context = InstrumentationRegistry.getContext()
        nativeMapView = NativeMapView(context, false, null, null, NativeMapViewTest.DummyRenderer(context))
        nativeMapView.resizeView(WIDTH, HEIGHT)
    }

    fun setupLayer(layer: Layer) {
        nativeMapView.addLayer(layer)
    }
}