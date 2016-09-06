// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import android.annotation.SuppressLint;
import android.support.annotation.ColorInt;

/**
 * Constructs paint/layout properties for Layers
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers">Layer style documentation</a>
 */
public class PropertyFactory {

    /**
     * Set the property visibility.
     *
     * @param value the visibility value
     * @return property wrapper around visibility
     */
    public static Property<String> visibility(@Property.VISIBILITY String value) {
        return new LayoutProperty<>("visibility", value);
    }

    /**
     * Set the property visibility.
     *
     * @param function the visibility function
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> visibility(Function<String> function) {
        return new LayoutProperty<>("visibility", function);
    }

    /**
     * Whether or not the fill should be antialiased.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> fillAntialias(Boolean value) {
        return new PaintProperty<>("fill-antialias", value);
    }

    /**
     * Whether or not the fill should be antialiased.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> fillAntialias(Function<Boolean> function) {
        return new PaintProperty<>("fill-antialias", function);
    }

    /**
     * The opacity of the entire fill layer. In contrast to the fill-color, this value will also affect the 1px stroke around the fill, if the stroke is used.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> fillOpacity(Float value) {
        return new PaintProperty<>("fill-opacity", value);
    }

    /**
     * The opacity of the entire fill layer. In contrast to the fill-color, this value will also affect the 1px stroke around the fill, if the stroke is used.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> fillOpacity(Function<Float> function) {
        return new PaintProperty<>("fill-opacity", function);
    }

    /**
     * The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> fillColor(@ColorInt int value) {
        return new PaintProperty<>("fill-color", colorToRgbaString(value));
    }

    /**
     * The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> fillColor(String value) {
        return new PaintProperty<>("fill-color", value);
    }

    /**
     * The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> fillColor(Function<String> function) {
        return new PaintProperty<>("fill-color", function);
    }

    /**
     * The outline color of the fill. Matches the value of `fill-color` if unspecified.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> fillOutlineColor(@ColorInt int value) {
        return new PaintProperty<>("fill-outline-color", colorToRgbaString(value));
    }

    /**
     * The outline color of the fill. Matches the value of `fill-color` if unspecified.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> fillOutlineColor(String value) {
        return new PaintProperty<>("fill-outline-color", value);
    }

    /**
     * The outline color of the fill. Matches the value of `fill-color` if unspecified.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> fillOutlineColor(Function<String> function) {
        return new PaintProperty<>("fill-outline-color", function);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> fillTranslate(Float[] value) {
        return new PaintProperty<>("fill-translate", value);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> fillTranslate(Function<Float[]> function) {
        return new PaintProperty<>("fill-translate", function);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> fillTranslateAnchor(@Property.FILL_TRANSLATE_ANCHOR String value) {
        return new PaintProperty<>("fill-translate-anchor", value);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> fillTranslateAnchor(Function<String> function) {
        return new PaintProperty<>("fill-translate-anchor", function);
    }

    /**
     * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> fillPattern(String value) {
        return new PaintProperty<>("fill-pattern", value);
    }

    /**
     * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> fillPattern(Function<String> function) {
        return new PaintProperty<>("fill-pattern", function);
    }

    /**
     * The opacity at which the line will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineOpacity(Float value) {
        return new PaintProperty<>("line-opacity", value);
    }

    /**
     * The opacity at which the line will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineOpacity(Function<Float> function) {
        return new PaintProperty<>("line-opacity", function);
    }

    /**
     * The color with which the line will be drawn.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> lineColor(@ColorInt int value) {
        return new PaintProperty<>("line-color", colorToRgbaString(value));
    }

    /**
     * The color with which the line will be drawn.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> lineColor(String value) {
        return new PaintProperty<>("line-color", value);
    }

    /**
     * The color with which the line will be drawn.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> lineColor(Function<String> function) {
        return new PaintProperty<>("line-color", function);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> lineTranslate(Float[] value) {
        return new PaintProperty<>("line-translate", value);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> lineTranslate(Function<Float[]> function) {
        return new PaintProperty<>("line-translate", function);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> lineTranslateAnchor(@Property.LINE_TRANSLATE_ANCHOR String value) {
        return new PaintProperty<>("line-translate-anchor", value);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> lineTranslateAnchor(Function<String> function) {
        return new PaintProperty<>("line-translate-anchor", function);
    }

    /**
     * Stroke thickness.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineWidth(Float value) {
        return new PaintProperty<>("line-width", value);
    }

    /**
     * Stroke thickness.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineWidth(Function<Float> function) {
        return new PaintProperty<>("line-width", function);
    }

    /**
     * Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineGapWidth(Float value) {
        return new PaintProperty<>("line-gap-width", value);
    }

    /**
     * Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineGapWidth(Function<Float> function) {
        return new PaintProperty<>("line-gap-width", function);
    }

    /**
     * The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineOffset(Float value) {
        return new PaintProperty<>("line-offset", value);
    }

    /**
     * The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineOffset(Function<Float> function) {
        return new PaintProperty<>("line-offset", function);
    }

    /**
     * Blur applied to the line, in pixels.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineBlur(Float value) {
        return new PaintProperty<>("line-blur", value);
    }

    /**
     * Blur applied to the line, in pixels.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineBlur(Function<Float> function) {
        return new PaintProperty<>("line-blur", function);
    }

    /**
     * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to pixels, multiply the length by the current line width.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> lineDasharray(Float[] value) {
        return new PaintProperty<>("line-dasharray", value);
    }

    /**
     * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to pixels, multiply the length by the current line width.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> lineDasharray(Function<Float[]> function) {
        return new PaintProperty<>("line-dasharray", function);
    }

    /**
     * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> linePattern(String value) {
        return new PaintProperty<>("line-pattern", value);
    }

    /**
     * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> linePattern(Function<String> function) {
        return new PaintProperty<>("line-pattern", function);
    }

    /**
     * The opacity at which the icon will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconOpacity(Float value) {
        return new PaintProperty<>("icon-opacity", value);
    }

    /**
     * The opacity at which the icon will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconOpacity(Function<Float> function) {
        return new PaintProperty<>("icon-opacity", function);
    }

    /**
     * The color of the icon. This can only be used with sdf icons.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> iconColor(@ColorInt int value) {
        return new PaintProperty<>("icon-color", colorToRgbaString(value));
    }

    /**
     * The color of the icon. This can only be used with sdf icons.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconColor(String value) {
        return new PaintProperty<>("icon-color", value);
    }

    /**
     * The color of the icon. This can only be used with sdf icons.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconColor(Function<String> function) {
        return new PaintProperty<>("icon-color", function);
    }

    /**
     * The color of the icon's halo. Icon halos can only be used with sdf icons.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> iconHaloColor(@ColorInt int value) {
        return new PaintProperty<>("icon-halo-color", colorToRgbaString(value));
    }

    /**
     * The color of the icon's halo. Icon halos can only be used with sdf icons.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconHaloColor(String value) {
        return new PaintProperty<>("icon-halo-color", value);
    }

    /**
     * The color of the icon's halo. Icon halos can only be used with sdf icons.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconHaloColor(Function<String> function) {
        return new PaintProperty<>("icon-halo-color", function);
    }

    /**
     * Distance of halo to the icon outline.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconHaloWidth(Float value) {
        return new PaintProperty<>("icon-halo-width", value);
    }

    /**
     * Distance of halo to the icon outline.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconHaloWidth(Function<Float> function) {
        return new PaintProperty<>("icon-halo-width", function);
    }

    /**
     * Fade out the halo towards the outside.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconHaloBlur(Float value) {
        return new PaintProperty<>("icon-halo-blur", value);
    }

    /**
     * Fade out the halo towards the outside.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconHaloBlur(Function<Float> function) {
        return new PaintProperty<>("icon-halo-blur", function);
    }

    /**
     * Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> iconTranslate(Float[] value) {
        return new PaintProperty<>("icon-translate", value);
    }

    /**
     * Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> iconTranslate(Function<Float[]> function) {
        return new PaintProperty<>("icon-translate", function);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen).
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconTranslateAnchor(@Property.ICON_TRANSLATE_ANCHOR String value) {
        return new PaintProperty<>("icon-translate-anchor", value);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen).
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconTranslateAnchor(Function<String> function) {
        return new PaintProperty<>("icon-translate-anchor", function);
    }

    /**
     * The opacity at which the text will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textOpacity(Float value) {
        return new PaintProperty<>("text-opacity", value);
    }

    /**
     * The opacity at which the text will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textOpacity(Function<Float> function) {
        return new PaintProperty<>("text-opacity", function);
    }

    /**
     * The color with which the text will be drawn.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> textColor(@ColorInt int value) {
        return new PaintProperty<>("text-color", colorToRgbaString(value));
    }

    /**
     * The color with which the text will be drawn.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textColor(String value) {
        return new PaintProperty<>("text-color", value);
    }

    /**
     * The color with which the text will be drawn.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textColor(Function<String> function) {
        return new PaintProperty<>("text-color", function);
    }

    /**
     * The color of the text's halo, which helps it stand out from backgrounds.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> textHaloColor(@ColorInt int value) {
        return new PaintProperty<>("text-halo-color", colorToRgbaString(value));
    }

    /**
     * The color of the text's halo, which helps it stand out from backgrounds.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textHaloColor(String value) {
        return new PaintProperty<>("text-halo-color", value);
    }

    /**
     * The color of the text's halo, which helps it stand out from backgrounds.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textHaloColor(Function<String> function) {
        return new PaintProperty<>("text-halo-color", function);
    }

    /**
     * Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textHaloWidth(Float value) {
        return new PaintProperty<>("text-halo-width", value);
    }

    /**
     * Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textHaloWidth(Function<Float> function) {
        return new PaintProperty<>("text-halo-width", function);
    }

    /**
     * The halo's fadeout distance towards the outside.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textHaloBlur(Float value) {
        return new PaintProperty<>("text-halo-blur", value);
    }

    /**
     * The halo's fadeout distance towards the outside.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textHaloBlur(Function<Float> function) {
        return new PaintProperty<>("text-halo-blur", function);
    }

    /**
     * Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> textTranslate(Float[] value) {
        return new PaintProperty<>("text-translate", value);
    }

    /**
     * Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> textTranslate(Function<Float[]> function) {
        return new PaintProperty<>("text-translate", function);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen).
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textTranslateAnchor(@Property.TEXT_TRANSLATE_ANCHOR String value) {
        return new PaintProperty<>("text-translate-anchor", value);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen).
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textTranslateAnchor(Function<String> function) {
        return new PaintProperty<>("text-translate-anchor", function);
    }

    /**
     * Circle radius.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> circleRadius(Float value) {
        return new PaintProperty<>("circle-radius", value);
    }

    /**
     * Circle radius.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> circleRadius(Function<Float> function) {
        return new PaintProperty<>("circle-radius", function);
    }

    /**
     * The color of the circle.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> circleColor(@ColorInt int value) {
        return new PaintProperty<>("circle-color", colorToRgbaString(value));
    }

    /**
     * The color of the circle.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> circleColor(String value) {
        return new PaintProperty<>("circle-color", value);
    }

    /**
     * The color of the circle.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> circleColor(Function<String> function) {
        return new PaintProperty<>("circle-color", function);
    }

    /**
     * Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> circleBlur(Float value) {
        return new PaintProperty<>("circle-blur", value);
    }

    /**
     * Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> circleBlur(Function<Float> function) {
        return new PaintProperty<>("circle-blur", function);
    }

    /**
     * The opacity at which the circle will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> circleOpacity(Float value) {
        return new PaintProperty<>("circle-opacity", value);
    }

    /**
     * The opacity at which the circle will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> circleOpacity(Function<Float> function) {
        return new PaintProperty<>("circle-opacity", function);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> circleTranslate(Float[] value) {
        return new PaintProperty<>("circle-translate", value);
    }

    /**
     * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> circleTranslate(Function<Float[]> function) {
        return new PaintProperty<>("circle-translate", function);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> circleTranslateAnchor(@Property.CIRCLE_TRANSLATE_ANCHOR String value) {
        return new PaintProperty<>("circle-translate-anchor", value);
    }

    /**
     * Control whether the translation is relative to the map (north) or viewport (screen)
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> circleTranslateAnchor(Function<String> function) {
        return new PaintProperty<>("circle-translate-anchor", function);
    }

    /**
     * Controls the scaling behavior of the circle when the map is pitched. The value `map` scales circles according to their apparent distance to the camera. The value `viewport` results in no pitch-related scaling.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> circlePitchScale(@Property.CIRCLE_PITCH_SCALE String value) {
        return new PaintProperty<>("circle-pitch-scale", value);
    }

    /**
     * Controls the scaling behavior of the circle when the map is pitched. The value `map` scales circles according to their apparent distance to the camera. The value `viewport` results in no pitch-related scaling.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> circlePitchScale(Function<String> function) {
        return new PaintProperty<>("circle-pitch-scale", function);
    }

    /**
     * The opacity at which the image will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterOpacity(Float value) {
        return new PaintProperty<>("raster-opacity", value);
    }

    /**
     * The opacity at which the image will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterOpacity(Function<Float> function) {
        return new PaintProperty<>("raster-opacity", function);
    }

    /**
     * Rotates hues around the color wheel.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterHueRotate(Float value) {
        return new PaintProperty<>("raster-hue-rotate", value);
    }

    /**
     * Rotates hues around the color wheel.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterHueRotate(Function<Float> function) {
        return new PaintProperty<>("raster-hue-rotate", function);
    }

    /**
     * Increase or reduce the brightness of the image. The value is the minimum brightness.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterBrightnessMin(Float value) {
        return new PaintProperty<>("raster-brightness-min", value);
    }

    /**
     * Increase or reduce the brightness of the image. The value is the minimum brightness.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterBrightnessMin(Function<Float> function) {
        return new PaintProperty<>("raster-brightness-min", function);
    }

    /**
     * Increase or reduce the brightness of the image. The value is the maximum brightness.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterBrightnessMax(Float value) {
        return new PaintProperty<>("raster-brightness-max", value);
    }

    /**
     * Increase or reduce the brightness of the image. The value is the maximum brightness.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterBrightnessMax(Function<Float> function) {
        return new PaintProperty<>("raster-brightness-max", function);
    }

    /**
     * Increase or reduce the saturation of the image.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterSaturation(Float value) {
        return new PaintProperty<>("raster-saturation", value);
    }

    /**
     * Increase or reduce the saturation of the image.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterSaturation(Function<Float> function) {
        return new PaintProperty<>("raster-saturation", function);
    }

    /**
     * Increase or reduce the contrast of the image.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterContrast(Float value) {
        return new PaintProperty<>("raster-contrast", value);
    }

    /**
     * Increase or reduce the contrast of the image.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterContrast(Function<Float> function) {
        return new PaintProperty<>("raster-contrast", function);
    }

    /**
     * Fade duration when a new tile is added.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> rasterFadeDuration(Float value) {
        return new PaintProperty<>("raster-fade-duration", value);
    }

    /**
     * Fade duration when a new tile is added.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> rasterFadeDuration(Function<Float> function) {
        return new PaintProperty<>("raster-fade-duration", function);
    }

    /**
     * The color with which the background will be drawn.
     *
     * @param value a int color value
     * @return property wrapper around String color
     */
    public static Property<String> backgroundColor(@ColorInt int value) {
        return new PaintProperty<>("background-color", colorToRgbaString(value));
    }

    /**
     * The color with which the background will be drawn.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> backgroundColor(String value) {
        return new PaintProperty<>("background-color", value);
    }

    /**
     * The color with which the background will be drawn.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> backgroundColor(Function<String> function) {
        return new PaintProperty<>("background-color", function);
    }

    /**
     * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> backgroundPattern(String value) {
        return new PaintProperty<>("background-pattern", value);
    }

    /**
     * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> backgroundPattern(Function<String> function) {
        return new PaintProperty<>("background-pattern", function);
    }

    /**
     * The opacity at which the background will be drawn.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> backgroundOpacity(Float value) {
        return new PaintProperty<>("background-opacity", value);
    }

    /**
     * The opacity at which the background will be drawn.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> backgroundOpacity(Function<Float> function) {
        return new PaintProperty<>("background-opacity", function);
    }

    /**
     * The display of line endings.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> lineCap(@Property.LINE_CAP String value) {
        return new LayoutProperty<>("line-cap", value);
    }

    /**
     * The display of line endings.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> lineCap(Function<String> function) {
        return new LayoutProperty<>("line-cap", function);
    }

    /**
     * The display of lines when joining.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> lineJoin(@Property.LINE_JOIN String value) {
        return new LayoutProperty<>("line-join", value);
    }

    /**
     * The display of lines when joining.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> lineJoin(Function<String> function) {
        return new LayoutProperty<>("line-join", function);
    }

    /**
     * Used to automatically convert miter joins to bevel joins for sharp angles.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineMiterLimit(Float value) {
        return new LayoutProperty<>("line-miter-limit", value);
    }

    /**
     * Used to automatically convert miter joins to bevel joins for sharp angles.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineMiterLimit(Function<Float> function) {
        return new LayoutProperty<>("line-miter-limit", function);
    }

    /**
     * Used to automatically convert round joins to miter joins for shallow angles.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> lineRoundLimit(Float value) {
        return new LayoutProperty<>("line-round-limit", value);
    }

    /**
     * Used to automatically convert round joins to miter joins for shallow angles.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> lineRoundLimit(Function<Float> function) {
        return new LayoutProperty<>("line-round-limit", function);
    }

    /**
     * Label placement relative to its geometry. `line` can only be used on LineStrings and Polygons.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> symbolPlacement(@Property.SYMBOL_PLACEMENT String value) {
        return new LayoutProperty<>("symbol-placement", value);
    }

    /**
     * Label placement relative to its geometry. `line` can only be used on LineStrings and Polygons.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> symbolPlacement(Function<String> function) {
        return new LayoutProperty<>("symbol-placement", function);
    }

    /**
     * Distance between two symbol anchors.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> symbolSpacing(Float value) {
        return new LayoutProperty<>("symbol-spacing", value);
    }

    /**
     * Distance between two symbol anchors.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> symbolSpacing(Function<Float> function) {
        return new LayoutProperty<>("symbol-spacing", function);
    }

    /**
     * If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> symbolAvoidEdges(Boolean value) {
        return new LayoutProperty<>("symbol-avoid-edges", value);
    }

    /**
     * If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> symbolAvoidEdges(Function<Boolean> function) {
        return new LayoutProperty<>("symbol-avoid-edges", function);
    }

    /**
     * If true, the icon will be visible even if it collides with other previously drawn symbols.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> iconAllowOverlap(Boolean value) {
        return new LayoutProperty<>("icon-allow-overlap", value);
    }

    /**
     * If true, the icon will be visible even if it collides with other previously drawn symbols.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> iconAllowOverlap(Function<Boolean> function) {
        return new LayoutProperty<>("icon-allow-overlap", function);
    }

    /**
     * If true, other symbols can be visible even if they collide with the icon.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> iconIgnorePlacement(Boolean value) {
        return new LayoutProperty<>("icon-ignore-placement", value);
    }

    /**
     * If true, other symbols can be visible even if they collide with the icon.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> iconIgnorePlacement(Function<Boolean> function) {
        return new LayoutProperty<>("icon-ignore-placement", function);
    }

    /**
     * If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> iconOptional(Boolean value) {
        return new LayoutProperty<>("icon-optional", value);
    }

    /**
     * If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> iconOptional(Function<Boolean> function) {
        return new LayoutProperty<>("icon-optional", function);
    }

    /**
     * Orientation of icon when map is rotated. Aligns icon to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Selecting `auto` defaults to `map` for line placement and `viewport` for symbol placement
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconRotationAlignment(@Property.ICON_ROTATION_ALIGNMENT String value) {
        return new LayoutProperty<>("icon-rotation-alignment", value);
    }

    /**
     * Orientation of icon when map is rotated. Aligns icon to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Selecting `auto` defaults to `map` for line placement and `viewport` for symbol placement
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconRotationAlignment(Function<String> function) {
        return new LayoutProperty<>("icon-rotation-alignment", function);
    }

    /**
     * Scale factor for icon. 1 is original size, 3 triples the size.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconSize(Float value) {
        return new LayoutProperty<>("icon-size", value);
    }

    /**
     * Scale factor for icon. 1 is original size, 3 triples the size.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconSize(Function<Float> function) {
        return new LayoutProperty<>("icon-size", function);
    }

    /**
     * Position and scale an icon by the its corresponding text.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconTextFit(@Property.ICON_TEXT_FIT String value) {
        return new LayoutProperty<>("icon-text-fit", value);
    }

    /**
     * Position and scale an icon by the its corresponding text.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconTextFit(Function<String> function) {
        return new LayoutProperty<>("icon-text-fit", function);
    }

    /**
     * Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> iconTextFitPadding(Float[] value) {
        return new LayoutProperty<>("icon-text-fit-padding", value);
    }

    /**
     * Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> iconTextFitPadding(Function<Float[]> function) {
        return new LayoutProperty<>("icon-text-fit-padding", function);
    }

    /**
     * A string with {tokens} replaced, referencing the data property to pull from.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> iconImage(String value) {
        return new LayoutProperty<>("icon-image", value);
    }

    /**
     * A string with {tokens} replaced, referencing the data property to pull from.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> iconImage(Function<String> function) {
        return new LayoutProperty<>("icon-image", function);
    }

    /**
     * Rotates the icon clockwise.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconRotate(Float value) {
        return new LayoutProperty<>("icon-rotate", value);
    }

    /**
     * Rotates the icon clockwise.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconRotate(Function<Float> function) {
        return new LayoutProperty<>("icon-rotate", function);
    }

    /**
     * Size of the additional area around the icon bounding box used for detecting symbol collisions.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> iconPadding(Float value) {
        return new LayoutProperty<>("icon-padding", value);
    }

    /**
     * Size of the additional area around the icon bounding box used for detecting symbol collisions.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> iconPadding(Function<Float> function) {
        return new LayoutProperty<>("icon-padding", function);
    }

    /**
     * If true, the icon may be flipped to prevent it from being rendered upside-down.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> iconKeepUpright(Boolean value) {
        return new LayoutProperty<>("icon-keep-upright", value);
    }

    /**
     * If true, the icon may be flipped to prevent it from being rendered upside-down.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> iconKeepUpright(Function<Boolean> function) {
        return new LayoutProperty<>("icon-keep-upright", function);
    }

    /**
     * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> iconOffset(Float[] value) {
        return new LayoutProperty<>("icon-offset", value);
    }

    /**
     * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> iconOffset(Function<Float[]> function) {
        return new LayoutProperty<>("icon-offset", function);
    }

    /**
     * Orientation of text when map is pitched. Aligns text to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Matches `text-rotation-alignment` if `auto`.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textPitchAlignment(@Property.TEXT_PITCH_ALIGNMENT String value) {
        return new LayoutProperty<>("text-pitch-alignment", value);
    }

    /**
     * Orientation of text when map is pitched. Aligns text to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Matches `text-rotation-alignment` if `auto`.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textPitchAlignment(Function<String> function) {
        return new LayoutProperty<>("text-pitch-alignment", function);
    }

    /**
     * Orientation of text when map is rotated. Aligns text to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Selecting `auto` defaults to the plane of the `map` for line placement and the `viewport` for symbol placement.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textRotationAlignment(@Property.TEXT_ROTATION_ALIGNMENT String value) {
        return new LayoutProperty<>("text-rotation-alignment", value);
    }

    /**
     * Orientation of text when map is rotated. Aligns text to the plane of the viewport when set to `viewport` or the plane of the map when set to `map`. Selecting `auto` defaults to the plane of the `map` for line placement and the `viewport` for symbol placement.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textRotationAlignment(Function<String> function) {
        return new LayoutProperty<>("text-rotation-alignment", function);
    }

    /**
     * Value to use for a text label. Feature properties are specified using tokens like {field_name}.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textField(String value) {
        return new LayoutProperty<>("text-field", value);
    }

    /**
     * Value to use for a text label. Feature properties are specified using tokens like {field_name}.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textField(Function<String> function) {
        return new LayoutProperty<>("text-field", function);
    }

    /**
     * Font stack to use for displaying text.
     *
     * @param value a String[] value
     * @return property wrapper around String[]
     */
    public static Property<String[]> textFont(String[] value) {
        return new LayoutProperty<>("text-font", value);
    }

    /**
     * Font stack to use for displaying text.
     *
     * @param function a wrapper function for String[]
     * @return property wrapper around a String[] function
     */
    public static Property<Function<String[]>> textFont(Function<String[]> function) {
        return new LayoutProperty<>("text-font", function);
    }

    /**
     * Font size.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textSize(Float value) {
        return new LayoutProperty<>("text-size", value);
    }

    /**
     * Font size.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textSize(Function<Float> function) {
        return new LayoutProperty<>("text-size", function);
    }

    /**
     * The maximum line width for text wrapping.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textMaxWidth(Float value) {
        return new LayoutProperty<>("text-max-width", value);
    }

    /**
     * The maximum line width for text wrapping.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textMaxWidth(Function<Float> function) {
        return new LayoutProperty<>("text-max-width", function);
    }

    /**
     * Text leading value for multi-line text.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textLineHeight(Float value) {
        return new LayoutProperty<>("text-line-height", value);
    }

    /**
     * Text leading value for multi-line text.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textLineHeight(Function<Float> function) {
        return new LayoutProperty<>("text-line-height", function);
    }

    /**
     * Text tracking amount.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textLetterSpacing(Float value) {
        return new LayoutProperty<>("text-letter-spacing", value);
    }

    /**
     * Text tracking amount.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textLetterSpacing(Function<Float> function) {
        return new LayoutProperty<>("text-letter-spacing", function);
    }

    /**
     * Text justification options.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textJustify(@Property.TEXT_JUSTIFY String value) {
        return new LayoutProperty<>("text-justify", value);
    }

    /**
     * Text justification options.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textJustify(Function<String> function) {
        return new LayoutProperty<>("text-justify", function);
    }

    /**
     * Part of the text placed closest to the anchor.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textAnchor(@Property.TEXT_ANCHOR String value) {
        return new LayoutProperty<>("text-anchor", value);
    }

    /**
     * Part of the text placed closest to the anchor.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textAnchor(Function<String> function) {
        return new LayoutProperty<>("text-anchor", function);
    }

    /**
     * Maximum angle change between adjacent characters.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textMaxAngle(Float value) {
        return new LayoutProperty<>("text-max-angle", value);
    }

    /**
     * Maximum angle change between adjacent characters.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textMaxAngle(Function<Float> function) {
        return new LayoutProperty<>("text-max-angle", function);
    }

    /**
     * Rotates the text clockwise.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textRotate(Float value) {
        return new LayoutProperty<>("text-rotate", value);
    }

    /**
     * Rotates the text clockwise.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textRotate(Function<Float> function) {
        return new LayoutProperty<>("text-rotate", function);
    }

    /**
     * Size of the additional area around the text bounding box used for detecting symbol collisions.
     *
     * @param value a Float value
     * @return property wrapper around Float
     */
    public static Property<Float> textPadding(Float value) {
        return new LayoutProperty<>("text-padding", value);
    }

    /**
     * Size of the additional area around the text bounding box used for detecting symbol collisions.
     *
     * @param function a wrapper function for Float
     * @return property wrapper around a Float function
     */
    public static Property<Function<Float>> textPadding(Function<Float> function) {
        return new LayoutProperty<>("text-padding", function);
    }

    /**
     * If true, the text may be flipped vertically to prevent it from being rendered upside-down.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> textKeepUpright(Boolean value) {
        return new LayoutProperty<>("text-keep-upright", value);
    }

    /**
     * If true, the text may be flipped vertically to prevent it from being rendered upside-down.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> textKeepUpright(Function<Boolean> function) {
        return new LayoutProperty<>("text-keep-upright", function);
    }

    /**
     * Specifies how to capitalize text, similar to the CSS `text-transform` property.
     *
     * @param value a String value
     * @return property wrapper around String
     */
    public static Property<String> textTransform(@Property.TEXT_TRANSFORM String value) {
        return new LayoutProperty<>("text-transform", value);
    }

    /**
     * Specifies how to capitalize text, similar to the CSS `text-transform` property.
     *
     * @param function a wrapper function for String
     * @return property wrapper around a String function
     */
    public static Property<Function<String>> textTransform(Function<String> function) {
        return new LayoutProperty<>("text-transform", function);
    }

    /**
     * Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param value a Float[] value
     * @return property wrapper around Float[]
     */
    public static Property<Float[]> textOffset(Float[] value) {
        return new LayoutProperty<>("text-offset", value);
    }

    /**
     * Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
     *
     * @param function a wrapper function for Float[]
     * @return property wrapper around a Float[] function
     */
    public static Property<Function<Float[]>> textOffset(Function<Float[]> function) {
        return new LayoutProperty<>("text-offset", function);
    }

    /**
     * If true, the text will be visible even if it collides with other previously drawn symbols.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> textAllowOverlap(Boolean value) {
        return new LayoutProperty<>("text-allow-overlap", value);
    }

    /**
     * If true, the text will be visible even if it collides with other previously drawn symbols.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> textAllowOverlap(Function<Boolean> function) {
        return new LayoutProperty<>("text-allow-overlap", function);
    }

    /**
     * If true, other symbols can be visible even if they collide with the text.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> textIgnorePlacement(Boolean value) {
        return new LayoutProperty<>("text-ignore-placement", value);
    }

    /**
     * If true, other symbols can be visible even if they collide with the text.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> textIgnorePlacement(Function<Boolean> function) {
        return new LayoutProperty<>("text-ignore-placement", function);
    }

    /**
     * If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
     *
     * @param value a Boolean value
     * @return property wrapper around Boolean
     */
    public static Property<Boolean> textOptional(Boolean value) {
        return new LayoutProperty<>("text-optional", value);
    }

    /**
     * If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
     *
     * @param function a wrapper function for Boolean
     * @return property wrapper around a Boolean function
     */
    public static Property<Function<Boolean>> textOptional(Function<Boolean> function) {
        return new LayoutProperty<>("text-optional", function);
    }

    @SuppressLint("DefaultLocale")
    static String colorToRgbaString(@ColorInt int value) {
        return String.format("rgba(%d, %d, %d, %d)", (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF, (value >> 24) & 0xFF);
    }

}
