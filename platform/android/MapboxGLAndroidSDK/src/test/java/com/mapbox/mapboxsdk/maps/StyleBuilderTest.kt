package com.mapbox.mapboxsdk.maps

import android.graphics.Bitmap
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import io.mockk.mockk
import org.junit.Assert.assertEquals
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class StyleBuilderTest {

    @Test
    fun testFromUrl() {
        val expected = Style.MAPBOX_STREETS
        val builder = Style.Builder()
        builder.fromUrl(expected)
        assertEquals(expected, builder.url)
    }

    @Test
    fun testFromJson() {
        val expected = "{}"
        val builder = Style.Builder()
        builder.fromJson(expected)
        assertEquals(expected, builder.json)
    }

    @Test
    fun testWithLayer() {
        val layer = mockk<SymbolLayer>()
        val builder = Style.Builder()
        builder.withLayer(layer)
        assertEquals(layer, builder.layers[0].layer)
    }

    @Test
    fun testWithLayerAt() {
        val expectedIndex = 5
        val layer = mockk<SymbolLayer>()
        val builder = Style.Builder()
        builder.withLayerAt(layer, expectedIndex)
        assertEquals(layer, builder.layers[0].layer)
        assertEquals(expectedIndex, (builder.layers[0] as Style.Builder.LayerAtWrapper).index)
    }

    @Test
    fun testWithLayerAbove() {
        val expectedAbove = "above"
        val layer = mockk<SymbolLayer>()
        val builder = Style.Builder()
        builder.withLayerAbove(layer, expectedAbove)
        assertEquals(layer, builder.layers[0].layer)
        assertEquals(expectedAbove, (builder.layers[0] as Style.Builder.LayerAboveWrapper).aboveLayer)
    }

    @Test
    fun testWithLayerBelow() {
        val expectedBelow = "above"
        val layer = mockk<SymbolLayer>()
        val builder = Style.Builder()
        builder.withLayerBelow(layer, expectedBelow)
        assertEquals(layer, builder.layers[0].layer)
        assertEquals(expectedBelow, (builder.layers[0] as Style.Builder.LayerBelowWrapper).belowLayer)
    }

    @Test
    fun testWithSource() {
        val source = mockk<GeoJsonSource>()
        val builder = Style.Builder()
        builder.withSource(source)
        assertEquals(source, builder.sources[0])
    }

    @Test
    fun testWithImage() {
        val bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val builder = Style.Builder()
        builder.withImage("id", bitmap)
        assertEquals(bitmap, builder.images[0].bitmap)
        assertEquals("id", builder.images[0].id)
        assertEquals(false, builder.images[0].sdf)
    }

    @Test
    fun testWithImageSdf() {
        val bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val builder = Style.Builder()
        builder.withImage("id", bitmap, true)
        assertEquals(bitmap, builder.images[0].bitmap)
        assertEquals("id", builder.images[0].id)
        assertEquals(true, builder.images[0].sdf)
    }

    @Test
    fun testWithTransitionOptions() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder()
        builder.withTransition(transitionOptions)
        assertEquals(100, builder.transitionOptions.duration)
        assertEquals(200, builder.transitionOptions.delay)
    }
}