// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;

import com.mapbox.mapboxsdk.style.expressions.Expression;

import java.util.Locale;

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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillAntialias(Expression expression) {
    return new PaintPropertyValue<>("fill-antialias", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillOpacity(Expression expression) {
    return new PaintPropertyValue<>("fill-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillColor(Expression expression) {
    return new PaintPropertyValue<>("fill-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillOutlineColor(Expression expression) {
    return new PaintPropertyValue<>("fill-outline-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillTranslate(Expression expression) {
    return new PaintPropertyValue<>("fill-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#fillTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillTranslateAnchor(@Property.FILL_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("fill-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#fillTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("fill-translate-anchor", expression);
  }

  /**
   * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillPattern(String value) {
    return new PaintPropertyValue<>("fill-pattern", value);
  }

  /**
   * Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillPattern(Expression expression) {
    return new PaintPropertyValue<>("fill-pattern", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineOpacity(Expression expression) {
    return new PaintPropertyValue<>("line-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineColor(Expression expression) {
    return new PaintPropertyValue<>("line-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineTranslate(Expression expression) {
    return new PaintPropertyValue<>("line-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#lineTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> lineTranslateAnchor(@Property.LINE_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("line-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#lineTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("line-translate-anchor", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineWidth(Expression expression) {
    return new PaintPropertyValue<>("line-width", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineGapWidth(Expression expression) {
    return new PaintPropertyValue<>("line-gap-width", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineOffset(Expression expression) {
    return new PaintPropertyValue<>("line-offset", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineBlur(Expression expression) {
    return new PaintPropertyValue<>("line-blur", expression);
  }

  /**
   * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to density-independent pixels, multiply the length by the current line width. Note that GeoJSON sources with `lineMetrics: true` specified won't render dashed lines to the expected scale. Also note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> lineDasharray(Float[] value) {
    return new PaintPropertyValue<>("line-dasharray", value);
  }

  /**
   * Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to density-independent pixels, multiply the length by the current line width. Note that GeoJSON sources with `lineMetrics: true` specified won't render dashed lines to the expected scale. Also note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> lineDasharray(Expression expression) {
    return new PaintPropertyValue<>("line-dasharray", expression);
  }

  /**
   * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> linePattern(String value) {
    return new PaintPropertyValue<>("line-pattern", value);
  }

  /**
   * Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> linePattern(Expression expression) {
    return new PaintPropertyValue<>("line-pattern", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconOpacity(Expression expression) {
    return new PaintPropertyValue<>("icon-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconColor(Expression expression) {
    return new PaintPropertyValue<>("icon-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconHaloColor(Expression expression) {
    return new PaintPropertyValue<>("icon-halo-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconHaloWidth(Expression expression) {
    return new PaintPropertyValue<>("icon-halo-width", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconHaloBlur(Expression expression) {
    return new PaintPropertyValue<>("icon-halo-blur", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconTranslate(Expression expression) {
    return new PaintPropertyValue<>("icon-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#iconTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconTranslateAnchor(@Property.ICON_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("icon-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#iconTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> iconTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("icon-translate-anchor", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textOpacity(Expression expression) {
    return new PaintPropertyValue<>("text-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textColor(Expression expression) {
    return new PaintPropertyValue<>("text-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textHaloColor(Expression expression) {
    return new PaintPropertyValue<>("text-halo-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textHaloWidth(Expression expression) {
    return new PaintPropertyValue<>("text-halo-width", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textHaloBlur(Expression expression) {
    return new PaintPropertyValue<>("text-halo-blur", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textTranslate(Expression expression) {
    return new PaintPropertyValue<>("text-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#textTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textTranslateAnchor(@Property.TEXT_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("text-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#textTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> textTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("text-translate-anchor", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleRadius(Expression expression) {
    return new PaintPropertyValue<>("circle-radius", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleColor(Expression expression) {
    return new PaintPropertyValue<>("circle-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleBlur(Expression expression) {
    return new PaintPropertyValue<>("circle-blur", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleOpacity(Expression expression) {
    return new PaintPropertyValue<>("circle-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleTranslate(Expression expression) {
    return new PaintPropertyValue<>("circle-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#circleTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> circleTranslateAnchor(@Property.CIRCLE_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("circle-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#circleTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("circle-translate-anchor", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circlePitchScale(Expression expression) {
    return new PaintPropertyValue<>("circle-pitch-scale", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circlePitchAlignment(Expression expression) {
    return new PaintPropertyValue<>("circle-pitch-alignment", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleStrokeWidth(Expression expression) {
    return new PaintPropertyValue<>("circle-stroke-width", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleStrokeColor(Expression expression) {
    return new PaintPropertyValue<>("circle-stroke-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> circleStrokeOpacity(Expression expression) {
    return new PaintPropertyValue<>("circle-stroke-opacity", expression);
  }

  /**
   * Radius of influence of one heatmap point in density-independent pixels. Increasing the value makes the heatmap smoother, but less detailed.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> heatmapRadius(Float value) {
    return new PaintPropertyValue<>("heatmap-radius", value);
  }

  /**
   * Radius of influence of one heatmap point in density-independent pixels. Increasing the value makes the heatmap smoother, but less detailed.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> heatmapRadius(Expression expression) {
    return new PaintPropertyValue<>("heatmap-radius", expression);
  }

  /**
   * A measure of how much an individual point contributes to the heatmap. A value of 10 would be equivalent to having 10 points of weight 1 in the same spot. Especially useful when combined with clustering.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> heatmapWeight(Float value) {
    return new PaintPropertyValue<>("heatmap-weight", value);
  }

  /**
   * A measure of how much an individual point contributes to the heatmap. A value of 10 would be equivalent to having 10 points of weight 1 in the same spot. Especially useful when combined with clustering.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> heatmapWeight(Expression expression) {
    return new PaintPropertyValue<>("heatmap-weight", expression);
  }

  /**
   * Similar to {@link PropertyFactory#heatmapWeight} but controls the intensity of the heatmap globally. Primarily used for adjusting the heatmap based on zoom level.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> heatmapIntensity(Float value) {
    return new PaintPropertyValue<>("heatmap-intensity", value);
  }

  /**
   * Similar to {@link PropertyFactory#heatmapWeight} but controls the intensity of the heatmap globally. Primarily used for adjusting the heatmap based on zoom level.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> heatmapIntensity(Expression expression) {
    return new PaintPropertyValue<>("heatmap-intensity", expression);
  }

  /**
   * Defines the color of each pixel based on its density value in a heatmap.  Should be an expression that uses `["heatmap-density"]` as input.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> heatmapColor(@ColorInt int value) {
    return new PaintPropertyValue<>("heatmap-color", colorToRgbaString(value));
  }

  /**
   * Defines the color of each pixel based on its density value in a heatmap.  Should be an expression that uses `["heatmap-density"]` as input.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> heatmapColor(String value) {
    return new PaintPropertyValue<>("heatmap-color", value);
  }

  /**
   * Defines the color of each pixel based on its density value in a heatmap.  Should be an expression that uses `["heatmap-density"]` as input.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> heatmapColor(Expression expression) {
    return new PaintPropertyValue<>("heatmap-color", expression);
  }

  /**
   * The global opacity at which the heatmap layer will be drawn.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> heatmapOpacity(Float value) {
    return new PaintPropertyValue<>("heatmap-opacity", value);
  }

  /**
   * The global opacity at which the heatmap layer will be drawn.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> heatmapOpacity(Expression expression) {
    return new PaintPropertyValue<>("heatmap-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionOpacity(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionColor(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionTranslate(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-translate", expression);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#fillExtrusionTranslate}.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillExtrusionTranslateAnchor(@Property.FILL_EXTRUSION_TRANSLATE_ANCHOR String value) {
    return new PaintPropertyValue<>("fill-extrusion-translate-anchor", value);
  }

  /**
   * Controls the frame of reference for {@link PropertyFactory#fillExtrusionTranslate}.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionTranslateAnchor(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-translate-anchor", expression);
  }

  /**
   * Name of image in sprite to use for drawing images on extruded fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> fillExtrusionPattern(String value) {
    return new PaintPropertyValue<>("fill-extrusion-pattern", value);
  }

  /**
   * Name of image in sprite to use for drawing images on extruded fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionPattern(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-pattern", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionHeight(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-height", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> fillExtrusionBase(Expression expression) {
    return new PaintPropertyValue<>("fill-extrusion-base", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterOpacity(Expression expression) {
    return new PaintPropertyValue<>("raster-opacity", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterHueRotate(Expression expression) {
    return new PaintPropertyValue<>("raster-hue-rotate", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterBrightnessMin(Expression expression) {
    return new PaintPropertyValue<>("raster-brightness-min", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterBrightnessMax(Expression expression) {
    return new PaintPropertyValue<>("raster-brightness-max", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterSaturation(Expression expression) {
    return new PaintPropertyValue<>("raster-saturation", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterContrast(Expression expression) {
    return new PaintPropertyValue<>("raster-contrast", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> rasterFadeDuration(Expression expression) {
    return new PaintPropertyValue<>("raster-fade-duration", expression);
  }

  /**
   * The direction of the light source used to generate the hillshading with 0 as the top of the viewport if {@link Property.HILLSHADE_ILLUMINATION_ANCHOR} is set to `viewport` and due north if {@link Property.HILLSHADE_ILLUMINATION_ANCHOR} is set to `map`.
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> hillshadeIlluminationDirection(Float value) {
    return new PaintPropertyValue<>("hillshade-illumination-direction", value);
  }

  /**
   * The direction of the light source used to generate the hillshading with 0 as the top of the viewport if {@link Property.HILLSHADE_ILLUMINATION_ANCHOR} is set to `viewport` and due north if {@link Property.HILLSHADE_ILLUMINATION_ANCHOR} is set to `map`.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeIlluminationDirection(Expression expression) {
    return new PaintPropertyValue<>("hillshade-illumination-direction", expression);
  }

  /**
   * Direction of light source when map is rotated.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> hillshadeIlluminationAnchor(@Property.HILLSHADE_ILLUMINATION_ANCHOR String value) {
    return new PaintPropertyValue<>("hillshade-illumination-anchor", value);
  }

  /**
   * Direction of light source when map is rotated.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeIlluminationAnchor(Expression expression) {
    return new PaintPropertyValue<>("hillshade-illumination-anchor", expression);
  }

  /**
   * Intensity of the hillshade
   *
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Float> hillshadeExaggeration(Float value) {
    return new PaintPropertyValue<>("hillshade-exaggeration", value);
  }

  /**
   * Intensity of the hillshade
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeExaggeration(Expression expression) {
    return new PaintPropertyValue<>("hillshade-exaggeration", expression);
  }

  /**
   * The shading color of areas that face away from the light source.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> hillshadeShadowColor(@ColorInt int value) {
    return new PaintPropertyValue<>("hillshade-shadow-color", colorToRgbaString(value));
  }

  /**
   * The shading color of areas that face away from the light source.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> hillshadeShadowColor(String value) {
    return new PaintPropertyValue<>("hillshade-shadow-color", value);
  }

  /**
   * The shading color of areas that face away from the light source.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeShadowColor(Expression expression) {
    return new PaintPropertyValue<>("hillshade-shadow-color", expression);
  }

  /**
   * The shading color of areas that faces towards the light source.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> hillshadeHighlightColor(@ColorInt int value) {
    return new PaintPropertyValue<>("hillshade-highlight-color", colorToRgbaString(value));
  }

  /**
   * The shading color of areas that faces towards the light source.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> hillshadeHighlightColor(String value) {
    return new PaintPropertyValue<>("hillshade-highlight-color", value);
  }

  /**
   * The shading color of areas that faces towards the light source.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeHighlightColor(Expression expression) {
    return new PaintPropertyValue<>("hillshade-highlight-color", expression);
  }

  /**
   * The shading color used to accentuate rugged terrain like sharp cliffs and gorges.
   *
   * @param value a int color value
   * @return property wrapper around String color
   */
  public static PropertyValue<String> hillshadeAccentColor(@ColorInt int value) {
    return new PaintPropertyValue<>("hillshade-accent-color", colorToRgbaString(value));
  }

  /**
   * The shading color used to accentuate rugged terrain like sharp cliffs and gorges.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> hillshadeAccentColor(String value) {
    return new PaintPropertyValue<>("hillshade-accent-color", value);
  }

  /**
   * The shading color used to accentuate rugged terrain like sharp cliffs and gorges.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> hillshadeAccentColor(Expression expression) {
    return new PaintPropertyValue<>("hillshade-accent-color", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> backgroundColor(Expression expression) {
    return new PaintPropertyValue<>("background-color", expression);
  }

  /**
   * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> backgroundPattern(String value) {
    return new PaintPropertyValue<>("background-pattern", value);
  }

  /**
   * Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512). Note that zoom-dependent expressions will be evaluated only at integer zoom levels.
   *
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> backgroundPattern(Expression expression) {
    return new PaintPropertyValue<>("background-pattern", expression);
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
   * @param expression an expression statement
   * @return property wrapper around an expression statement
   */
  public static PropertyValue<Expression> backgroundOpacity(Expression expression) {
    return new PaintPropertyValue<>("background-opacity", expression);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> lineCap(Expression value) {
    return new LayoutPropertyValue<>("line-cap", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> lineJoin(Expression value) {
    return new LayoutPropertyValue<>("line-join", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> lineMiterLimit(Expression value) {
    return new LayoutPropertyValue<>("line-miter-limit", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> lineRoundLimit(Expression value) {
    return new LayoutPropertyValue<>("line-round-limit", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> symbolPlacement(Expression value) {
    return new LayoutPropertyValue<>("symbol-placement", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> symbolSpacing(Expression value) {
    return new LayoutPropertyValue<>("symbol-spacing", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> symbolAvoidEdges(Expression value) {
    return new LayoutPropertyValue<>("symbol-avoid-edges", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> iconAllowOverlap(Expression value) {
    return new LayoutPropertyValue<>("icon-allow-overlap", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> iconIgnorePlacement(Expression value) {
    return new LayoutPropertyValue<>("icon-ignore-placement", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> iconOptional(Expression value) {
    return new LayoutPropertyValue<>("icon-optional", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> iconRotationAlignment(Expression value) {
    return new LayoutPropertyValue<>("icon-rotation-alignment", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> iconSize(Expression value) {
    return new LayoutPropertyValue<>("icon-size", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> iconTextFit(Expression value) {
    return new LayoutPropertyValue<>("icon-text-fit", value);
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
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Expression> iconTextFitPadding(Expression value) {
    return new LayoutPropertyValue<>("icon-text-fit-padding", value);
  }

  /**
   * Name of image in sprite to use for drawing an image background.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> iconImage(String value) {
    return new LayoutPropertyValue<>("icon-image", value);
  }

  /**
   * Name of image in sprite to use for drawing an image background.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> iconImage(Expression value) {
    return new LayoutPropertyValue<>("icon-image", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> iconRotate(Expression value) {
    return new LayoutPropertyValue<>("icon-rotate", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> iconPadding(Expression value) {
    return new LayoutPropertyValue<>("icon-padding", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> iconKeepUpright(Expression value) {
    return new LayoutPropertyValue<>("icon-keep-upright", value);
  }

  /**
   * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up. Each component is multiplied by the value of {@link PropertyFactory#iconSize} to obtain the final offset in density-independent pixels. When combined with {@link PropertyFactory#iconRotate} the offset will be as if the rotated direction was up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Float[]> iconOffset(Float[] value) {
    return new LayoutPropertyValue<>("icon-offset", value);
  }

  /**
   * Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up. Each component is multiplied by the value of {@link PropertyFactory#iconSize} to obtain the final offset in density-independent pixels. When combined with {@link PropertyFactory#iconRotate} the offset will be as if the rotated direction was up.
   *
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Expression> iconOffset(Expression value) {
    return new LayoutPropertyValue<>("icon-offset", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> iconAnchor(Expression value) {
    return new LayoutPropertyValue<>("icon-anchor", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> iconPitchAlignment(Expression value) {
    return new LayoutPropertyValue<>("icon-pitch-alignment", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textPitchAlignment(Expression value) {
    return new LayoutPropertyValue<>("text-pitch-alignment", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textRotationAlignment(Expression value) {
    return new LayoutPropertyValue<>("text-rotation-alignment", value);
  }

  /**
   * Value to use for a text label.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<String> textField(String value) {
    return new LayoutPropertyValue<>("text-field", value);
  }

  /**
   * Value to use for a text label.
   *
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textField(Expression value) {
    return new LayoutPropertyValue<>("text-field", value);
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
   * @param value a String[] value
   * @return property wrapper around String[]
   */
  public static PropertyValue<Expression> textFont(Expression value) {
    return new LayoutPropertyValue<>("text-font", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textSize(Expression value) {
    return new LayoutPropertyValue<>("text-size", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textMaxWidth(Expression value) {
    return new LayoutPropertyValue<>("text-max-width", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textLineHeight(Expression value) {
    return new LayoutPropertyValue<>("text-line-height", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textLetterSpacing(Expression value) {
    return new LayoutPropertyValue<>("text-letter-spacing", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textJustify(Expression value) {
    return new LayoutPropertyValue<>("text-justify", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textAnchor(Expression value) {
    return new LayoutPropertyValue<>("text-anchor", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textMaxAngle(Expression value) {
    return new LayoutPropertyValue<>("text-max-angle", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textRotate(Expression value) {
    return new LayoutPropertyValue<>("text-rotate", value);
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
   * @param value a Float value
   * @return property wrapper around Float
   */
  public static PropertyValue<Expression> textPadding(Expression value) {
    return new LayoutPropertyValue<>("text-padding", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> textKeepUpright(Expression value) {
    return new LayoutPropertyValue<>("text-keep-upright", value);
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
   * @param value a String value
   * @return property wrapper around String
   */
  public static PropertyValue<Expression> textTransform(Expression value) {
    return new LayoutPropertyValue<>("text-transform", value);
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
   * @param value a Float[] value
   * @return property wrapper around Float[]
   */
  public static PropertyValue<Expression> textOffset(Expression value) {
    return new LayoutPropertyValue<>("text-offset", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> textAllowOverlap(Expression value) {
    return new LayoutPropertyValue<>("text-allow-overlap", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> textIgnorePlacement(Expression value) {
    return new LayoutPropertyValue<>("text-ignore-placement", value);
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
   * @param value a Boolean value
   * @return property wrapper around Boolean
   */
  public static PropertyValue<Expression> textOptional(Expression value) {
    return new LayoutPropertyValue<>("text-optional", value);
  }

  public static String colorToRgbaString(@ColorInt int value) {
    return String.format(Locale.US,"rgba(%d, %d, %d, %d)",
      (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF, (value >> 24) & 0xFF);
  }

}
