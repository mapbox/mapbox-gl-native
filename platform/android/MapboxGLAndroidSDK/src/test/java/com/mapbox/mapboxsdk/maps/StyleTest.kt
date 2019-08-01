package com.mapbox.mapboxsdk.maps

import android.graphics.Bitmap
import android.graphics.drawable.ShapeDrawable
import com.mapbox.mapboxsdk.constants.MapboxConstants
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import io.mockk.*
import org.junit.Assert
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class StyleTest {

    private lateinit var mapboxMap: MapboxMap

    private lateinit var nativeMapView: NativeMap

    @Before
    fun setup() {
        nativeMapView = mockk(relaxed = true)
        mapboxMap = MapboxMap(nativeMapView, null, null, null, null, null, null)
        every { nativeMapView.isDestroyed } returns false
        mapboxMap.injectLocationComponent(spyk())
    }

    @Test
    fun testFromUrl() {
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
    }

    @Test
    fun testFromJson() {
        val builder = Style.Builder().fromJson("{}")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = "{}" }
    }

    @Test
    fun testEmptyBuilder() {
        val builder = Style.Builder()
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = Style.EMPTY_JSON }
    }

    @Test
    fun testWithLayer() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayer(layer)
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, MapboxConstants.LAYER_ID_ANNOTATIONS) }
    }

    @Test
    fun testWithLayerAbove() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerAbove(layer, "id")
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addLayerAbove(layer, "id") }
    }

    @Test
    fun testWithLayerBelow() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerBelow(layer, "id")
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "id") }
    }

    @Test
    fun testWithLayerAt() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerAt(layer, 1)
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addLayerAt(layer, 1) }
    }

    @Test
    fun testWithSource() {
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addSource(source) }
    }

    @Test
    fun testWithTransitionOptions() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder().withTransition(transitionOptions)
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.transitionOptions = transitionOptions }
    }

    @Test
    fun testWithFromLoadingSource() {
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addSource(source) }
    }

    @Test
    fun testWithFromLoadingLayer() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayer(layer)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, MapboxConstants.LAYER_ID_ANNOTATIONS) }
    }

    @Test
    fun testWithFromLoadingLayerAt() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerAt(layer, 1)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerAt(layer, 1) }
    }

    @Test
    fun testWithFromLoadingLayerBelow() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerBelow(layer, "below")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "below") }
    }

    @Test
    fun testWithFromLoadingLayerAbove() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerBelow(layer, "below")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "below") }
    }

    @Test
    fun testWithFromLoadingTransitionOptions() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withTransition(transitionOptions)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.transitionOptions = transitionOptions }
    }

    @Test
    fun testFromCallback() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS)
        mapboxMap.setStyle(builder, callback)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testWithCallback() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder, callback)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addSource(source) }
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncWith() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder)
        mapboxMap.onFinishLoadingStyle()
        verify(exactly = 1) { nativeMapView.addSource(source) }
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncFrom() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromJson("{}")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = "{}" }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncWithFrom() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addSource(source) }
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetNullStyle() {
        Assert.assertNull(mapboxMap.style)
    }

    @Test
    fun testGetNullWhileLoading() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withTransition(transitionOptions)
        mapboxMap.setStyle(builder)
        Assert.assertNull(mapboxMap.style)
        mapboxMap.notifyStyleLoaded()
        Assert.assertNotNull(mapboxMap.style)
    }

    @Test
    fun testNotReinvokeSameListener() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromJson("{}")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = "{}" }
        mapboxMap.notifyStyleLoaded()
        mapboxMap.setStyle(Style.MAPBOX_STREETS)
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test(expected = IllegalStateException::class)
    fun testIllegalStateExceptionWithStyleReload() {
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()
        val style = mapboxMap.style
        mapboxMap.setStyle(Style.Builder().fromUrl(Style.DARK))
        style!!.addLayer(mockk<SymbolLayer>())
    }

    @Test
    fun testAddImage() {
        val bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888)
        val builder = Style.Builder().fromUrl(Style.SATELLITE).withImage("id", bitmap)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.SATELLITE }
        verify(exactly = 0) { nativeMapView.addImages(any()) }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addImages(any()) }
    }

    @Test
    fun testAddDrawable() {
        val drawable = ShapeDrawable()
        drawable.intrinsicHeight = 10
        drawable.intrinsicWidth = 10
        val builder = Style.Builder().fromUrl(Style.SATELLITE).withImage("id", drawable)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUri = Style.SATELLITE }
        verify(exactly = 0) { nativeMapView.addImages(any()) }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addImages(any()) }
    }

    @Test
    fun testSourceSkippedIfAdditionFails() {
        val source1 = mockk<GeoJsonSource>(relaxed = true)
        every { source1.id } returns "source1"
        val source2 = mockk<GeoJsonSource>(relaxed = true)
        every { source2.id } returns "source1" // same ID

        val builder = Style.Builder().withSource(source1)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()

        every { nativeMapView.addSource(any()) } throws CannotAddSourceException("Duplicate ID")

        try {
            mapboxMap.style!!.addSource(source2)
        } catch (ex: Exception) {
            Assert.assertEquals("Source that failed to be added shouldn't be cached", source1, mapboxMap.style!!.getSource("source1"))
        }
    }

    @Test
    fun testLayerSkippedIfAdditionFails() {
        val layer1 = mockk<SymbolLayer>(relaxed = true)
        every { layer1.id } returns "layer1"
        val layer2 = mockk<SymbolLayer>(relaxed = true)
        every { layer2.id } returns "layer1" // same ID

        val builder = Style.Builder().withLayer(layer1)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()

        every { nativeMapView.addLayer(any()) } throws CannotAddLayerException("Duplicate ID")

        try {
            mapboxMap.style!!.addLayer(layer2)
        } catch (ex: Exception) {
            Assert.assertEquals("Layer that failed to be added shouldn't be cached", layer1, mapboxMap.style!!.getLayer("layer1"))
        }
    }

    @Test
    fun testLayerSkippedIfAdditionBelowFails() {
        val layer1 = mockk<SymbolLayer>(relaxed = true)
        every { layer1.id } returns "layer1"
        val layer2 = mockk<SymbolLayer>(relaxed = true)
        every { layer2.id } returns "layer1" // same ID

        val builder = Style.Builder().withLayer(layer1)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()

        every { nativeMapView.addLayerBelow(any(), "") } throws CannotAddLayerException("Duplicate ID")

        try {
            mapboxMap.style!!.addLayerBelow(layer2, "")
        } catch (ex: Exception) {
            Assert.assertEquals("Layer that failed to be added shouldn't be cached", layer1, mapboxMap.style!!.getLayer("layer1"))
        }
    }

    @Test
    fun testLayerSkippedIfAdditionAboveFails() {
        val layer1 = mockk<SymbolLayer>(relaxed = true)
        every { layer1.id } returns "layer1"
        val layer2 = mockk<SymbolLayer>(relaxed = true)
        every { layer2.id } returns "layer1" // same ID

        val builder = Style.Builder().withLayer(layer1)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()

        every { nativeMapView.addLayerAbove(any(), "") } throws CannotAddLayerException("Duplicate ID")

        try {
            mapboxMap.style!!.addLayerAbove(layer2, "")
        } catch (ex: Exception) {
            Assert.assertEquals("Layer that failed to be added shouldn't be cached", layer1, mapboxMap.style!!.getLayer("layer1"))
        }
    }

    @Test
    fun testLayerSkippedIfAdditionAtFails() {
        val layer1 = mockk<SymbolLayer>(relaxed = true)
        every { layer1.id } returns "layer1"
        val layer2 = mockk<SymbolLayer>(relaxed = true)
        every { layer2.id } returns "layer1" // same ID

        val builder = Style.Builder().withLayer(layer1)
        mapboxMap.setStyle(builder)
        mapboxMap.notifyStyleLoaded()

        every { nativeMapView.addLayerAt(any(), 5) } throws CannotAddLayerException("Duplicate ID")

        try {
            mapboxMap.style!!.addLayerAt(layer2, 5)
        } catch (ex: Exception) {
            Assert.assertEquals("Layer that failed to be added shouldn't be cached", layer1, mapboxMap.style!!.getLayer("layer1"))
        }
    }

  @Test
  fun testClearRemovesSourcesFirst() {
    val source1 = mockk<GeoJsonSource>(relaxed = true)
    every { source1.id } returns "source1"
    val layer1 = mockk<SymbolLayer>(relaxed = true)
    every { layer1.id } returns "layer1"

    val builder = Style.Builder().withLayer(layer1).withSource(source1)
    mapboxMap.setStyle(builder)
    mapboxMap.notifyStyleLoaded()
    mapboxMap.setStyle(Style.MAPBOX_STREETS)

    verifyOrder {
      nativeMapView.removeLayer(layer1)
      nativeMapView.removeSource(source1)
    }
  }
}