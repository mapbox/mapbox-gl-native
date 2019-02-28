package com.mapbox.mapboxsdk.maps

import android.graphics.Bitmap
import android.graphics.drawable.Drawable
import android.graphics.drawable.ShapeDrawable
import android.util.Pair
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import com.mapbox.mapboxsdk.utils.BitmapUtils
import io.mockk.mockk
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
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
    fun testWithLayers() {
        val layer1 = mockk<SymbolLayer>()
        val layer2 = mockk<SymbolLayer>()
        val builder = Style.Builder()
        builder.withLayers(layer1, layer2)
        assertEquals(layer1, builder.layers[0].layer)
        assertEquals(layer2, builder.layers[1].layer)
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
    fun testWithSources() {
        val source1 = mockk<GeoJsonSource>()
        val source2 = mockk<GeoJsonSource>()
        val builder = Style.Builder()
        builder.withSources(source1, source2)
        assertEquals(source1, builder.sources[0])
        assertEquals(source2, builder.sources[1])
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
    fun testWithImages() {
        val bitmap1 = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val bitmap2 = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val builder = Style.Builder()
        builder.withBitmapImages(Pair("id1", bitmap1), Pair("id2", bitmap2))
        assertEquals(bitmap1, builder.images[0].bitmap)
        assertEquals("id1", builder.images[0].id)
        assertEquals(false, builder.images[0].sdf)
        assertEquals(bitmap2, builder.images[1].bitmap)
        assertEquals("id2", builder.images[1].id)
        assertEquals(false, builder.images[1].sdf)
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
    fun testWithImageSdfs() {
        val bitmap1 = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val bitmap2 = Bitmap.createBitmap(1, 1, Bitmap.Config.ALPHA_8)
        val builder = Style.Builder()
        builder.withBitmapImages(true, Pair("id1", bitmap1), Pair("id2", bitmap2))
        assertEquals(bitmap1, builder.images[0].bitmap)
        assertEquals("id1", builder.images[0].id)
        assertEquals(true, builder.images[0].sdf)
        assertEquals(bitmap2, builder.images[1].bitmap)
        assertEquals("id2", builder.images[1].id)
        assertEquals(true, builder.images[1].sdf)
    }

    @Test
    fun testWithImageDrawable() {
        val drawable = ShapeDrawable()
        drawable.intrinsicWidth = 1
        drawable.intrinsicHeight = 1
        val builder = Style.Builder()
        builder.withImage("id", drawable)
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable)!!,
                builder.images[0].bitmap)
        )
        assertEquals("id", builder.images[0].id)
        assertEquals(false, builder.images[0].sdf)
    }

    @Test
    fun testWithImageDrawableSdf() {
        val drawable = ShapeDrawable()
        drawable.intrinsicWidth = 1
        drawable.intrinsicHeight = 1
        val builder = Style.Builder()
        builder.withImage("id", drawable, true)
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable)!!,
                builder.images[0].bitmap)
        )
        assertEquals("id", builder.images[0].id)
        assertEquals(true, builder.images[0].sdf)
    }

    @Test
    fun testWithImageDrawables() {
        val drawable1 = ShapeDrawable()
        drawable1.intrinsicWidth = 1
        drawable1.intrinsicHeight = 1
        val drawable2 = ShapeDrawable()
        drawable2.intrinsicWidth = 1
        drawable2.intrinsicHeight = 1
        val builder = Style.Builder()
        builder.withDrawableImages(Pair<String, Drawable>("id1", drawable1), Pair<String, Drawable>("id2", drawable2))
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable1)!!,
                builder.images[0].bitmap)
        )
        assertEquals("id1", builder.images[0].id)
        assertEquals(false, builder.images[0].sdf)
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable2)!!,
                builder.images[1].bitmap)
        )
        assertEquals("id2", builder.images[1].id)
        assertEquals(false, builder.images[1].sdf)
    }

    @Test
    fun testWithImageSdfDrawables() {
        val drawable1 = ShapeDrawable()
        drawable1.intrinsicWidth = 1
        drawable1.intrinsicHeight = 1
        val drawable2 = ShapeDrawable()
        drawable2.intrinsicWidth = 1
        drawable2.intrinsicHeight = 1
        val builder = Style.Builder()
        builder.withDrawableImages(true, Pair<String, Drawable>("id1", drawable1), Pair<String, Drawable>("id2", drawable2))
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable1)!!,
                builder.images[0].bitmap)
        )
        assertEquals("id1", builder.images[0].id)
        assertEquals(true, builder.images[0].sdf)
        assertTrue(BitmapUtils.equals(
                BitmapUtils.getBitmapFromDrawable(drawable2)!!,
                builder.images[1].bitmap)
        )
        assertEquals("id2", builder.images[1].id)
        assertEquals(true, builder.images[1].sdf)
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