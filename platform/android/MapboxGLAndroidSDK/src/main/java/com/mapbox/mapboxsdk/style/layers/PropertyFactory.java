// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.annotation.SuppressLint;
import android.support.annotation.ColorInt;

import com.mapbox.mapboxsdk.style.functions.Function;
import com.mapbox.mapboxsdk.style.functions.CameraFunction;

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
  public static PropertyValue<String> visibility(@Property.VISIBILITY String value) {
    return new LayoutPropertyValue<>("visibility", value);
  }

  /**
   * Set the property visibility.
   *
   * @param <T> the function input type
   * @param function the visibility function
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> visibility(Function<T, String> function) {
    return new LayoutPropertyValue<>("visibility", function);
  }

  /**
   * Whether or not the fill should be antialiased.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> fillAntialias(Boolean value) {
    return new PaintPropertyValue<>("fill-antialias", value);
  }


  /**
   * Whether or not the fill should be antialiased.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> fillAntialias(CameraFunction<Z, Boolean> function) {
    return new PaintPropertyValue<>("fill-antialias", function);
  }

  /**
   * The opacity of the entire fill layer. In contrast to the {@link PropertyFactory#fillColor}, this value will also affect the 1px stroke around the fill, if the stroke is used.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> fillOpacity(Float value) {
    return new PaintPropertyValue<>("fill-opacity", value);
  }


  /**
   * The opacity of the entire fill layer. In contrast to the {@link PropertyFactory#fillColor}, this value will also affect the 1px stroke around the fill, if the stroke is used.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> fillOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("fill-opacity", function);
  }

  /**
   * The color of the filled part of this layer. This color can be specified as `rgba` with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> fillColor(@ColorInt int value) {
    return new PaintPropertyValue<>("fill-color", colorToRgbaString(value));
  }

  /**
   * The color of the filled part of this layer. This color can be specified as `rgba` with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillColor(String value) {
    return new PaintPropertyValue<>("fill-color", value);
  }


  /**
   * The color of the filled part of this layer. This color can be specified as `rgba` with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> fillColor(Function<T, String> function) {
    return new PaintPropertyValue<>("fill-color", function);
  }

  /**
   * The outline color of the fill. Matches the value of {@link PropertyFactory#fillColor} if unspecified.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> fillOutlineColor(@ColorInt int value) {
    return new PaintPropertyValue<>("fill-outline-color", colorToRgbaString(value));
  }

  /**
   * The outline color of the fill. Matches the value of {@link PropertyFactory#fillColor} if unspecified.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillOutlineColor(String value) {
    return new PaintPropertyValue<>("fill-outline-color", value);
  }


  /**
   * The outline color of the fill. Matches the value of {@link PropertyFactory#fillColor} if unspecified.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> fillOutlineColor(Function<T, String> function) {
    return new PaintPropertyValue<>("fill-outline-color", function);
  }

  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> fillTranslate(Float[] value) {
    return new PaintPropertyValue<>("fill-translate", value);
  }


  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> fillTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("fill-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillTranslateAnchor(@Property.FILL_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("fill-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> fillTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("fill-translate-anchor", function);
  }

  /**
   * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillPattern(String value) {
    return new PaintPropertyValue<>("fill-pattern", value);
  }


  /**
   * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> fillPattern(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("fill-pattern", function);
  }

  /**
   * The opacity at which the line will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineOpacity(Float value) {
    return new PaintPropertyValue<>("line-opacity", value);
  }


  /**
   * The opacity at which the line will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> lineOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("line-opacity", function);
  }

  /**
   * The color with which the line will be drawn.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> lineColor(@ColorInt int value) {
    return new PaintPropertyValue<>("line-color", colorToRgbaString(value));
  }

  /**
   * The color with which the line will be drawn.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> lineColor(String value) {
    return new PaintPropertyValue<>("line-color", value);
  }


  /**
   * The color with which the line will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> lineColor(Function<T, String> function) {
    return new PaintPropertyValue<>("line-color", function);
  }

  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> lineTranslate(Float[] value) {
    return new PaintPropertyValue<>("line-translate", value);
  }


  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> lineTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("line-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> lineTranslateAnchor(@Property.LINE_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("line-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> lineTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("line-translate-anchor", function);
  }

  /**
   * Stroke thickness.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineWidth(Float value) {
    return new PaintPropertyValue<>("line-width", value);
  }


  /**
   * Stroke thickness.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> lineWidth(Function<T, Float> function) {
    return new PaintPropertyValue<>("line-width", function);
  }

  /**
   * Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineGapWidth(Float value) {
    return new PaintPropertyValue<>("line-gap-width", value);
  }


  /**
   * Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> lineGapWidth(Function<T, Float> function) {
    return new PaintPropertyValue<>("line-gap-width", function);
  }

  /**
   * The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineOffset(Float value) {
    return new PaintPropertyValue<>("line-offset", value);
  }


  /**
   * The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> lineOffset(Function<T, Float> function) {
    return new PaintPropertyValue<>("line-offset", function);
  }

  /**
   * Blur applied to the line, in density-independent pixels.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineBlur(Float value) {
    return new PaintPropertyValue<>("line-blur", value);
  }


  /**
   * Blur applied to the line, in density-independent pixels.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> lineBlur(Function<T, Float> function) {
    return new PaintPropertyValue<>("line-blur", function);
  }

  /**
   * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to density-independent pixels, multiply the length by the current line width.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> lineDasharray(Float[] value) {
    return new PaintPropertyValue<>("line-dasharray", value);
  }


  /**
   * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to density-independent pixels, multiply the length by the current line width.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> lineDasharray(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("line-dasharray", function);
  }

  /**
   * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> linePattern(String value) {
    return new PaintPropertyValue<>("line-pattern", value);
  }


  /**
   * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> linePattern(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("line-pattern", function);
  }

  /**
   * The opacity at which the icon will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconOpacity(Float value) {
    return new PaintPropertyValue<>("icon-opacity", value);
  }


  /**
   * The opacity at which the icon will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> iconOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("icon-opacity", function);
  }

  /**
   * The color of the icon. This can only be used with sdf icons.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> iconColor(@ColorInt int value) {
    return new PaintPropertyValue<>("icon-color", colorToRgbaString(value));
  }

  /**
   * The color of the icon. This can only be used with sdf icons.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconColor(String value) {
    return new PaintPropertyValue<>("icon-color", value);
  }


  /**
   * The color of the icon. This can only be used with sdf icons.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> iconColor(Function<T, String> function) {
    return new PaintPropertyValue<>("icon-color", function);
  }

  /**
   * The color of the icon's halo. Icon halos can only be used with SDF icons.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> iconHaloColor(@ColorInt int value) {
    return new PaintPropertyValue<>("icon-halo-color", colorToRgbaString(value));
  }

  /**
   * The color of the icon's halo. Icon halos can only be used with SDF icons.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconHaloColor(String value) {
    return new PaintPropertyValue<>("icon-halo-color", value);
  }


  /**
   * The color of the icon's halo. Icon halos can only be used with SDF icons.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> iconHaloColor(Function<T, String> function) {
    return new PaintPropertyValue<>("icon-halo-color", function);
  }

  /**
   * Distance of halo to the icon outline.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconHaloWidth(Float value) {
    return new PaintPropertyValue<>("icon-halo-width", value);
  }


  /**
   * Distance of halo to the icon outline.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> iconHaloWidth(Function<T, Float> function) {
    return new PaintPropertyValue<>("icon-halo-width", function);
  }

  /**
   * Fade out the halo towards the outside.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconHaloBlur(Float value) {
    return new PaintPropertyValue<>("icon-halo-blur", value);
  }


  /**
   * Fade out the halo towards the outside.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> iconHaloBlur(Function<T, Float> function) {
    return new PaintPropertyValue<>("icon-halo-blur", function);
  }

  /**
   * Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> iconTranslate(Float[] value) {
    return new PaintPropertyValue<>("icon-translate", value);
  }


  /**
   * Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> iconTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("icon-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconTranslateAnchor(@Property.ICON_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("icon-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> iconTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("icon-translate-anchor", function);
  }

  /**
   * The opacity at which the text will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textOpacity(Float value) {
    return new PaintPropertyValue<>("text-opacity", value);
  }


  /**
   * The opacity at which the text will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> textOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("text-opacity", function);
  }

  /**
   * The color with which the text will be drawn.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> textColor(@ColorInt int value) {
    return new PaintPropertyValue<>("text-color", colorToRgbaString(value));
  }

  /**
   * The color with which the text will be drawn.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textColor(String value) {
    return new PaintPropertyValue<>("text-color", value);
  }


  /**
   * The color with which the text will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textColor(Function<T, String> function) {
    return new PaintPropertyValue<>("text-color", function);
  }

  /**
   * The color of the text's halo, which helps it stand out from backgrounds.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> textHaloColor(@ColorInt int value) {
    return new PaintPropertyValue<>("text-halo-color", colorToRgbaString(value));
  }

  /**
   * The color of the text's halo, which helps it stand out from backgrounds.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textHaloColor(String value) {
    return new PaintPropertyValue<>("text-halo-color", value);
  }


  /**
   * The color of the text's halo, which helps it stand out from backgrounds.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textHaloColor(Function<T, String> function) {
    return new PaintPropertyValue<>("text-halo-color", function);
  }

  /**
   * Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textHaloWidth(Float value) {
    return new PaintPropertyValue<>("text-halo-width", value);
  }


  /**
   * Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> textHaloWidth(Function<T, Float> function) {
    return new PaintPropertyValue<>("text-halo-width", function);
  }

  /**
   * The halo's fadeout distance towards the outside.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textHaloBlur(Float value) {
    return new PaintPropertyValue<>("text-halo-blur", value);
  }


  /**
   * The halo's fadeout distance towards the outside.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> textHaloBlur(Function<T, Float> function) {
    return new PaintPropertyValue<>("text-halo-blur", function);
  }

  /**
   * Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> textTranslate(Float[] value) {
    return new PaintPropertyValue<>("text-translate", value);
  }


  /**
   * Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> textTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("text-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textTranslateAnchor(@Property.TEXT_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("text-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> textTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("text-translate-anchor", function);
  }

  /**
   * Circle radius.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> circleRadius(Float value) {
    return new PaintPropertyValue<>("circle-radius", value);
  }


  /**
   * Circle radius.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> circleRadius(Function<T, Float> function) {
    return new PaintPropertyValue<>("circle-radius", function);
  }

  /**
   * The fill color of the circle.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> circleColor(@ColorInt int value) {
    return new PaintPropertyValue<>("circle-color", colorToRgbaString(value));
  }

  /**
   * The fill color of the circle.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circleColor(String value) {
    return new PaintPropertyValue<>("circle-color", value);
  }


  /**
   * The fill color of the circle.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> circleColor(Function<T, String> function) {
    return new PaintPropertyValue<>("circle-color", function);
  }

  /**
   * Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> circleBlur(Float value) {
    return new PaintPropertyValue<>("circle-blur", value);
  }


  /**
   * Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> circleBlur(Function<T, Float> function) {
    return new PaintPropertyValue<>("circle-blur", function);
  }

  /**
   * The opacity at which the circle will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> circleOpacity(Float value) {
    return new PaintPropertyValue<>("circle-opacity", value);
  }


  /**
   * The opacity at which the circle will be drawn.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> circleOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("circle-opacity", function);
  }

  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> circleTranslate(Float[] value) {
    return new PaintPropertyValue<>("circle-translate", value);
  }


  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> circleTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("circle-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circleTranslateAnchor(@Property.CIRCLE_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("circle-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> circleTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("circle-translate-anchor", function);
  }

  /**
   * Controls the scaling behavior of the circle when the map is pitched.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circlePitchScale(@Property.CIRCLE_PITCH_SCALE String value) {
    return new PaintPropertyValue<>("circle-pitch-scale", value);
  }


  /**
   * Controls the scaling behavior of the circle when the map is pitched.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> circlePitchScale(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("circle-pitch-scale", function);
  }

  /**
   * Orientation of circle when map is pitched.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circlePitchAlignment(@Property.CIRCLE_PITCH_ALIGNMENT String value) {
    return new PaintPropertyValue<>("circle-pitch-alignment", value);
  }


  /**
   * Orientation of circle when map is pitched.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> circlePitchAlignment(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("circle-pitch-alignment", function);
  }

  /**
   * The width of the circle's stroke. Strokes are placed outside of the {@link PropertyFactory#circleRadius}.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> circleStrokeWidth(Float value) {
    return new PaintPropertyValue<>("circle-stroke-width", value);
  }


  /**
   * The width of the circle's stroke. Strokes are placed outside of the {@link PropertyFactory#circleRadius}.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> circleStrokeWidth(Function<T, Float> function) {
    return new PaintPropertyValue<>("circle-stroke-width", function);
  }

  /**
   * The stroke color of the circle.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> circleStrokeColor(@ColorInt int value) {
    return new PaintPropertyValue<>("circle-stroke-color", colorToRgbaString(value));
  }

  /**
   * The stroke color of the circle.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circleStrokeColor(String value) {
    return new PaintPropertyValue<>("circle-stroke-color", value);
  }


  /**
   * The stroke color of the circle.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> circleStrokeColor(Function<T, String> function) {
    return new PaintPropertyValue<>("circle-stroke-color", function);
  }

  /**
   * The opacity of the circle's stroke.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> circleStrokeOpacity(Float value) {
    return new PaintPropertyValue<>("circle-stroke-opacity", value);
  }


  /**
   * The opacity of the circle's stroke.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> circleStrokeOpacity(Function<T, Float> function) {
    return new PaintPropertyValue<>("circle-stroke-opacity", function);
  }

  /**
   * The opacity of the entire fill extrusion layer. This is rendered on a per-layer, not per-feature, basis, and data-driven styling is not available.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> fillExtrusionOpacity(Float value) {
    return new PaintPropertyValue<>("fill-extrusion-opacity", value);
  }


  /**
   * The opacity of the entire fill extrusion layer. This is rendered on a per-layer, not per-feature, basis, and data-driven styling is not available.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> fillExtrusionOpacity(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("fill-extrusion-opacity", function);
  }

  /**
   * The base color of the extruded fill. The extrusion's surfaces will be shaded differently based on this color in combination with the root `light` settings. If this color is specified as `rgba` with an alpha component, the alpha component will be ignored; use {@link PropertyFactory#fillExtrusionOpacity} to set layer opacity.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> fillExtrusionColor(@ColorInt int value) {
    return new PaintPropertyValue<>("fill-extrusion-color", colorToRgbaString(value));
  }

  /**
   * The base color of the extruded fill. The extrusion's surfaces will be shaded differently based on this color in combination with the root `light` settings. If this color is specified as `rgba` with an alpha component, the alpha component will be ignored; use {@link PropertyFactory#fillExtrusionOpacity} to set layer opacity.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillExtrusionColor(String value) {
    return new PaintPropertyValue<>("fill-extrusion-color", value);
  }


  /**
   * The base color of the extruded fill. The extrusion's surfaces will be shaded differently based on this color in combination with the root `light` settings. If this color is specified as `rgba` with an alpha component, the alpha component will be ignored; use {@link PropertyFactory#fillExtrusionOpacity} to set layer opacity.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> fillExtrusionColor(Function<T, String> function) {
    return new PaintPropertyValue<>("fill-extrusion-color", function);
  }

  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up (on the flat plane), respectively.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> fillExtrusionTranslate(Float[] value) {
    return new PaintPropertyValue<>("fill-extrusion-translate", value);
  }


  /**
   * The geometry's offset. Values are [x, y] where negatives indicate left and up (on the flat plane), respectively.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> fillExtrusionTranslate(CameraFunction<Z, Float[]> function) {
    return new PaintPropertyValue<>("fill-extrusion-translate", function);
  }

  /**
   * Controls the translation reference point.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillExtrusionTranslateAnchor(@Property.FILL_EXTRUSION_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("fill-extrusion-translate-anchor", value);
  }


  /**
   * Controls the translation reference point.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> fillExtrusionTranslateAnchor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("fill-extrusion-translate-anchor", function);
  }

  /**
   * Name of image in sprite to use for drawing images on extruded fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillExtrusionPattern(String value) {
    return new PaintPropertyValue<>("fill-extrusion-pattern", value);
  }


  /**
   * Name of image in sprite to use for drawing images on extruded fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> fillExtrusionPattern(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("fill-extrusion-pattern", function);
  }

  /**
   * The height with which to extrude this layer.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> fillExtrusionHeight(Float value) {
    return new PaintPropertyValue<>("fill-extrusion-height", value);
  }


  /**
   * The height with which to extrude this layer.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> fillExtrusionHeight(Function<T, Float> function) {
    return new PaintPropertyValue<>("fill-extrusion-height", function);
  }

  /**
   * The height with which to extrude the base of this layer. Must be less than or equal to {@link PropertyFactory#fillExtrusionHeight}.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> fillExtrusionBase(Float value) {
    return new PaintPropertyValue<>("fill-extrusion-base", value);
  }


  /**
   * The height with which to extrude the base of this layer. Must be less than or equal to {@link PropertyFactory#fillExtrusionHeight}.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> fillExtrusionBase(Function<T, Float> function) {
    return new PaintPropertyValue<>("fill-extrusion-base", function);
  }

  /**
   * The opacity at which the image will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterOpacity(Float value) {
    return new PaintPropertyValue<>("raster-opacity", value);
  }


  /**
   * The opacity at which the image will be drawn.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterOpacity(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-opacity", function);
  }

  /**
   * Rotates hues around the color wheel.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterHueRotate(Float value) {
    return new PaintPropertyValue<>("raster-hue-rotate", value);
  }


  /**
   * Rotates hues around the color wheel.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterHueRotate(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-hue-rotate", function);
  }

  /**
   * Increase or reduce the brightness of the image. The value is the minimum brightness.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterBrightnessMin(Float value) {
    return new PaintPropertyValue<>("raster-brightness-min", value);
  }


  /**
   * Increase or reduce the brightness of the image. The value is the minimum brightness.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterBrightnessMin(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-brightness-min", function);
  }

  /**
   * Increase or reduce the brightness of the image. The value is the maximum brightness.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterBrightnessMax(Float value) {
    return new PaintPropertyValue<>("raster-brightness-max", value);
  }


  /**
   * Increase or reduce the brightness of the image. The value is the maximum brightness.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterBrightnessMax(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-brightness-max", function);
  }

  /**
   * Increase or reduce the saturation of the image.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterSaturation(Float value) {
    return new PaintPropertyValue<>("raster-saturation", value);
  }


  /**
   * Increase or reduce the saturation of the image.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterSaturation(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-saturation", function);
  }

  /**
   * Increase or reduce the contrast of the image.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterContrast(Float value) {
    return new PaintPropertyValue<>("raster-contrast", value);
  }


  /**
   * Increase or reduce the contrast of the image.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterContrast(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-contrast", function);
  }

  /**
   * Fade duration when a new tile is added.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> rasterFadeDuration(Float value) {
    return new PaintPropertyValue<>("raster-fade-duration", value);
  }


  /**
   * Fade duration when a new tile is added.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> rasterFadeDuration(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("raster-fade-duration", function);
  }

  /**
   * The color with which the background will be drawn.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> backgroundColor(@ColorInt int value) {
    return new PaintPropertyValue<>("background-color", colorToRgbaString(value));
  }

  /**
   * The color with which the background will be drawn.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> backgroundColor(String value) {
    return new PaintPropertyValue<>("background-color", value);
  }


  /**
   * The color with which the background will be drawn.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> backgroundColor(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("background-color", function);
  }

  /**
   * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> backgroundPattern(String value) {
    return new PaintPropertyValue<>("background-pattern", value);
  }


  /**
   * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> backgroundPattern(CameraFunction<Z, String> function) {
    return new PaintPropertyValue<>("background-pattern", function);
  }

  /**
   * The opacity at which the background will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> backgroundOpacity(Float value) {
    return new PaintPropertyValue<>("background-opacity", value);
  }


  /**
   * The opacity at which the background will be drawn.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> backgroundOpacity(CameraFunction<Z, Float> function) {
    return new PaintPropertyValue<>("background-opacity", function);
  }

  /**
   * The display of line endings.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> lineCap(@Property.LINE_CAP String value) {
    return new LayoutPropertyValue<>("line-cap", value);
  }



  /**
   * The display of line endings.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> lineCap(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("line-cap", function);
  }

  /**
   * The display of lines when joining.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> lineJoin(@Property.LINE_JOIN String value) {
    return new LayoutPropertyValue<>("line-join", value);
  }



  /**
   * The display of lines when joining.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> lineJoin(Function<T, String> function) {
    return new LayoutPropertyValue<>("line-join", function);
  }

  /**
   * Used to automatically convert miter joins to bevel joins for sharp angles.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineMiterLimit(Float value) {
    return new LayoutPropertyValue<>("line-miter-limit", value);
  }



  /**
   * Used to automatically convert miter joins to bevel joins for sharp angles.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> lineMiterLimit(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("line-miter-limit", function);
  }

  /**
   * Used to automatically convert round joins to miter joins for shallow angles.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> lineRoundLimit(Float value) {
    return new LayoutPropertyValue<>("line-round-limit", value);
  }



  /**
   * Used to automatically convert round joins to miter joins for shallow angles.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> lineRoundLimit(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("line-round-limit", function);
  }

  /**
   * Label placement relative to its geometry.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> symbolPlacement(@Property.SYMBOL_PLACEMENT String value) {
    return new LayoutPropertyValue<>("symbol-placement", value);
  }



  /**
   * Label placement relative to its geometry.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> symbolPlacement(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("symbol-placement", function);
  }

  /**
   * Distance between two symbol anchors.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> symbolSpacing(Float value) {
    return new LayoutPropertyValue<>("symbol-spacing", value);
  }



  /**
   * Distance between two symbol anchors.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> symbolSpacing(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("symbol-spacing", function);
  }

  /**
   * If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> symbolAvoidEdges(Boolean value) {
    return new LayoutPropertyValue<>("symbol-avoid-edges", value);
  }



  /**
   * If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> symbolAvoidEdges(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("symbol-avoid-edges", function);
  }

  /**
   * If true, the icon will be visible even if it collides with other previously drawn symbols.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> iconAllowOverlap(Boolean value) {
    return new LayoutPropertyValue<>("icon-allow-overlap", value);
  }



  /**
   * If true, the icon will be visible even if it collides with other previously drawn symbols.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> iconAllowOverlap(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("icon-allow-overlap", function);
  }

  /**
   * If true, other symbols can be visible even if they collide with the icon.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> iconIgnorePlacement(Boolean value) {
    return new LayoutPropertyValue<>("icon-ignore-placement", value);
  }



  /**
   * If true, other symbols can be visible even if they collide with the icon.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> iconIgnorePlacement(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("icon-ignore-placement", function);
  }

  /**
   * If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> iconOptional(Boolean value) {
    return new LayoutPropertyValue<>("icon-optional", value);
  }



  /**
   * If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> iconOptional(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("icon-optional", function);
  }

  /**
   * In combination with {@link Property.SYMBOL_PLACEMENT}, determines the rotation behavior of icons.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconRotationAlignment(@Property.ICON_ROTATION_ALIGNMENT String value) {
    return new LayoutPropertyValue<>("icon-rotation-alignment", value);
  }



  /**
   * In combination with {@link Property.SYMBOL_PLACEMENT}, determines the rotation behavior of icons.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> iconRotationAlignment(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("icon-rotation-alignment", function);
  }

  /**
   * Scales the original size of the icon by the provided factor. The new pixel size of the image will be the original pixel size multiplied by {@link PropertyFactory#iconSize}. 1 is the original size; 3 triples the size of the image.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconSize(Float value) {
    return new LayoutPropertyValue<>("icon-size", value);
  }



  /**
   * Scales the original size of the icon by the provided factor. The new pixel size of the image will be the original pixel size multiplied by {@link PropertyFactory#iconSize}. 1 is the original size; 3 triples the size of the image.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> iconSize(Function<T, Float> function) {
    return new LayoutPropertyValue<>("icon-size", function);
  }

  /**
   * Scales the icon to fit around the associated text.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconTextFit(@Property.ICON_TEXT_FIT String value) {
    return new LayoutPropertyValue<>("icon-text-fit", value);
  }



  /**
   * Scales the icon to fit around the associated text.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> iconTextFit(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("icon-text-fit", function);
  }

  /**
   * Size of the additional area added to dimensions determined by {@link Property.ICON_TEXT_FIT}, in clockwise order: top, right, bottom, left.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> iconTextFitPadding(Float[] value) {
    return new LayoutPropertyValue<>("icon-text-fit-padding", value);
  }



  /**
   * Size of the additional area added to dimensions determined by {@link Property.ICON_TEXT_FIT}, in clockwise order: top, right, bottom, left.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float[]>> iconTextFitPadding(CameraFunction<Z, Float[]> function) {
    return new LayoutPropertyValue<>("icon-text-fit-padding", function);
  }

  /**
   * Name of image in sprite to use for drawing an image background. A string with {tokens} replaced, referencing the data property to pull from.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconImage(String value) {
    return new LayoutPropertyValue<>("icon-image", value);
  }



  /**
   * Name of image in sprite to use for drawing an image background. A string with {tokens} replaced, referencing the data property to pull from.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> iconImage(Function<T, String> function) {
    return new LayoutPropertyValue<>("icon-image", function);
  }

  /**
   * Rotates the icon clockwise.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconRotate(Float value) {
    return new LayoutPropertyValue<>("icon-rotate", value);
  }



  /**
   * Rotates the icon clockwise.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> iconRotate(Function<T, Float> function) {
    return new LayoutPropertyValue<>("icon-rotate", function);
  }

  /**
   * Size of the additional area around the icon bounding box used for detecting symbol collisions.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> iconPadding(Float value) {
    return new LayoutPropertyValue<>("icon-padding", value);
  }



  /**
   * Size of the additional area around the icon bounding box used for detecting symbol collisions.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> iconPadding(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("icon-padding", function);
  }

  /**
   * If true, the icon may be flipped to prevent it from being rendered upside-down.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> iconKeepUpright(Boolean value) {
    return new LayoutPropertyValue<>("icon-keep-upright", value);
  }



  /**
   * If true, the icon may be flipped to prevent it from being rendered upside-down.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> iconKeepUpright(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("icon-keep-upright", function);
  }

  /**
   * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up. When combined with {@link PropertyFactory#iconRotate} the offset will be as if the rotated direction was up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> iconOffset(Float[] value) {
    return new LayoutPropertyValue<>("icon-offset", value);
  }



  /**
   * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up. When combined with {@link PropertyFactory#iconRotate} the offset will be as if the rotated direction was up.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <T> PropertyValue<Function<T, Float[]>> iconOffset(Function<T, Float[]> function) {
    return new LayoutPropertyValue<>("icon-offset", function);
  }

  /**
   * Part of the icon placed closest to the anchor.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconAnchor(@Property.ICON_ANCHOR String value) {
    return new LayoutPropertyValue<>("icon-anchor", value);
  }



  /**
   * Part of the icon placed closest to the anchor.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> iconAnchor(Function<T, String> function) {
    return new LayoutPropertyValue<>("icon-anchor", function);
  }

  /**
   * Orientation of icon when map is pitched.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconPitchAlignment(@Property.ICON_PITCH_ALIGNMENT String value) {
    return new LayoutPropertyValue<>("icon-pitch-alignment", value);
  }



  /**
   * Orientation of icon when map is pitched.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> iconPitchAlignment(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("icon-pitch-alignment", function);
  }

  /**
   * Orientation of text when map is pitched.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textPitchAlignment(@Property.TEXT_PITCH_ALIGNMENT String value) {
    return new LayoutPropertyValue<>("text-pitch-alignment", value);
  }



  /**
   * Orientation of text when map is pitched.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> textPitchAlignment(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("text-pitch-alignment", function);
  }

  /**
   * In combination with {@link Property.SYMBOL_PLACEMENT}, determines the rotation behavior of the individual glyphs forming the text.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textRotationAlignment(@Property.TEXT_ROTATION_ALIGNMENT String value) {
    return new LayoutPropertyValue<>("text-rotation-alignment", value);
  }



  /**
   * In combination with {@link Property.SYMBOL_PLACEMENT}, determines the rotation behavior of the individual glyphs forming the text.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String
   * @return property wrapper around a String function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String>> textRotationAlignment(CameraFunction<Z, String> function) {
    return new LayoutPropertyValue<>("text-rotation-alignment", function);
  }

  /**
   * Value to use for a text label. Feature properties are specified using tokens like {field_name}.  (Token replacement is only supported for literal {@link PropertyFactory#textField} values--not for property functions.)
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textField(String value) {
    return new LayoutPropertyValue<>("text-field", value);
  }



  /**
   * Value to use for a text label. Feature properties are specified using tokens like {field_name}.  (Token replacement is only supported for literal {@link PropertyFactory#textField} values--not for property functions.)
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textField(Function<T, String> function) {
    return new LayoutPropertyValue<>("text-field", function);
  }

  /**
   * Font stack to use for displaying text.
   *
   * @param value a String[] value
   * @return property wrapper around String[]
   */
  public static PropertyValue<String[]> textFont(String[] value) {
    return new LayoutPropertyValue<>("text-font", value);
  }



  /**
   * Font stack to use for displaying text.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for String[]
   * @return property wrapper around a String[] function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, String[]>> textFont(CameraFunction<Z, String[]> function) {
    return new LayoutPropertyValue<>("text-font", function);
  }

  /**
   * Font size.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textSize(Float value) {
    return new LayoutPropertyValue<>("text-size", value);
  }



  /**
   * Font size.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> textSize(Function<T, Float> function) {
    return new LayoutPropertyValue<>("text-size", function);
  }

  /**
   * The maximum line width for text wrapping.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textMaxWidth(Float value) {
    return new LayoutPropertyValue<>("text-max-width", value);
  }



  /**
   * The maximum line width for text wrapping.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> textMaxWidth(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("text-max-width", function);
  }

  /**
   * Text leading value for multi-line text.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textLineHeight(Float value) {
    return new LayoutPropertyValue<>("text-line-height", value);
  }



  /**
   * Text leading value for multi-line text.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> textLineHeight(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("text-line-height", function);
  }

  /**
   * Text tracking amount.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textLetterSpacing(Float value) {
    return new LayoutPropertyValue<>("text-letter-spacing", value);
  }



  /**
   * Text tracking amount.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> textLetterSpacing(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("text-letter-spacing", function);
  }

  /**
   * Text justification options.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textJustify(@Property.TEXT_JUSTIFY String value) {
    return new LayoutPropertyValue<>("text-justify", value);
  }



  /**
   * Text justification options.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textJustify(Function<T, String> function) {
    return new LayoutPropertyValue<>("text-justify", function);
  }

  /**
   * Part of the text placed closest to the anchor.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textAnchor(@Property.TEXT_ANCHOR String value) {
    return new LayoutPropertyValue<>("text-anchor", value);
  }



  /**
   * Part of the text placed closest to the anchor.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textAnchor(Function<T, String> function) {
    return new LayoutPropertyValue<>("text-anchor", function);
  }

  /**
   * Maximum angle change between adjacent characters.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textMaxAngle(Float value) {
    return new LayoutPropertyValue<>("text-max-angle", value);
  }



  /**
   * Maximum angle change between adjacent characters.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> textMaxAngle(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("text-max-angle", function);
  }

  /**
   * Rotates the text clockwise.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textRotate(Float value) {
    return new LayoutPropertyValue<>("text-rotate", value);
  }



  /**
   * Rotates the text clockwise.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float
   * @return property wrapper around a Float function
   */
  public static <T> PropertyValue<Function<T, Float>> textRotate(Function<T, Float> function) {
    return new LayoutPropertyValue<>("text-rotate", function);
  }

  /**
   * Size of the additional area around the text bounding box used for detecting symbol collisions.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> textPadding(Float value) {
    return new LayoutPropertyValue<>("text-padding", value);
  }



  /**
   * Size of the additional area around the text bounding box used for detecting symbol collisions.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Float
   * @return property wrapper around a Float function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Float>> textPadding(CameraFunction<Z, Float> function) {
    return new LayoutPropertyValue<>("text-padding", function);
  }

  /**
   * If true, the text may be flipped vertically to prevent it from being rendered upside-down.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> textKeepUpright(Boolean value) {
    return new LayoutPropertyValue<>("text-keep-upright", value);
  }



  /**
   * If true, the text may be flipped vertically to prevent it from being rendered upside-down.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> textKeepUpright(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("text-keep-upright", function);
  }

  /**
   * Specifies how to capitalize text, similar to the CSS {@link PropertyFactory#textTransform} property.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textTransform(@Property.TEXT_TRANSFORM String value) {
    return new LayoutPropertyValue<>("text-transform", value);
  }



  /**
   * Specifies how to capitalize text, similar to the CSS {@link PropertyFactory#textTransform} property.
   *
   * @param <T> the function input type
   * @param function a wrapper function for String
   * @return property wrapper around a String function
   */
  public static <T> PropertyValue<Function<T, String>> textTransform(Function<T, String> function) {
    return new LayoutPropertyValue<>("text-transform", function);
  }

  /**
   * Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> textOffset(Float[] value) {
    return new LayoutPropertyValue<>("text-offset", value);
  }



  /**
   * Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
   *
   * @param <T> the function input type
   * @param function a wrapper function for Float[]
   * @return property wrapper around a Float[] function
   */
  public static <T> PropertyValue<Function<T, Float[]>> textOffset(Function<T, Float[]> function) {
    return new LayoutPropertyValue<>("text-offset", function);
  }

  /**
   * If true, the text will be visible even if it collides with other previously drawn symbols.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> textAllowOverlap(Boolean value) {
    return new LayoutPropertyValue<>("text-allow-overlap", value);
  }



  /**
   * If true, the text will be visible even if it collides with other previously drawn symbols.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> textAllowOverlap(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("text-allow-overlap", function);
  }

  /**
   * If true, other symbols can be visible even if they collide with the text.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> textIgnorePlacement(Boolean value) {
    return new LayoutPropertyValue<>("text-ignore-placement", value);
  }



  /**
   * If true, other symbols can be visible even if they collide with the text.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> textIgnorePlacement(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("text-ignore-placement", function);
  }

  /**
   * If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
   *
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Boolean> textOptional(Boolean value) {
    return new LayoutPropertyValue<>("text-optional", value);
  }



  /**
   * If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
   *
   * @param <Z> the zoom parameter type
   * @param function a wrapper {@link CameraFunction} for Boolean
   * @return property wrapper around a Boolean function
   */
  public static <Z extends Number> PropertyValue<CameraFunction<Z, Boolean>> textOptional(CameraFunction<Z, Boolean> function) {
    return new LayoutPropertyValue<>("text-optional", function);
  }

  @SuppressLint("DefaultLocale")
  public static String colorToRgbaString(@ColorInt int value) {
    return String.format("rgba(%d, %d, %d, %d)", (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF, (value >> 24) & 0xFF);
  }

}
