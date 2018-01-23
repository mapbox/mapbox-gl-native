// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.StringDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Paint/Layout properties for Layer
 */
public final class Property {

  // VISIBILITY: Whether this layer is displayed.

  /**
   * The layer is shown.
   */
  public static final String VISIBLE = "visible";
  /**
   * The layer is hidden.
   */
  public static final String NONE = "none";

  @StringDef({
        VISIBLE,
        NONE
  })
  @Retention(RetentionPolicy.SOURCE)
  public @interface VISIBILITY {}

  // LINE_CAP: The display of line endings.

  /**
   * A cap with a squared-off end which is drawn to the exact endpoint of the line.
   */
  public static final String LINE_CAP_BUTT = "butt";
  /**
   * A cap with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
   */
  public static final String LINE_CAP_ROUND = "round";
  /**
   * A cap with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
   */
  public static final String LINE_CAP_SQUARE = "square";

  /**
   * The display of line endings.
   */
  @StringDef({
      LINE_CAP_BUTT,
      LINE_CAP_ROUND,
      LINE_CAP_SQUARE,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface LINE_CAP {}

  // LINE_JOIN: The display of lines when joining.

  /**
   * A join with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
   */
  public static final String LINE_JOIN_BEVEL = "bevel";
  /**
   * A join with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
   */
  public static final String LINE_JOIN_ROUND = "round";
  /**
   * A join with a sharp, angled corner which is drawn with the outer sides beyond the endpoint of the path until they meet.
   */
  public static final String LINE_JOIN_MITER = "miter";

  /**
   * The display of lines when joining.
   */
  @StringDef({
      LINE_JOIN_BEVEL,
      LINE_JOIN_ROUND,
      LINE_JOIN_MITER,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface LINE_JOIN {}

  // SYMBOL_PLACEMENT: Label placement relative to its geometry.

  /**
   * The label is placed at the point where the geometry is located.
   */
  public static final String SYMBOL_PLACEMENT_POINT = "point";
  /**
   * The label is placed along the line of the geometry. Can only be used on LineString and Polygon geometries.
   */
  public static final String SYMBOL_PLACEMENT_LINE = "line";

  /**
   * Label placement relative to its geometry.
   */
  @StringDef({
      SYMBOL_PLACEMENT_POINT,
      SYMBOL_PLACEMENT_LINE,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface SYMBOL_PLACEMENT {}

  // ICON_ROTATION_ALIGNMENT: In combination with `symbol-placement`, determines the rotation behavior of icons.

  /**
   * When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_POINT}, aligns icons east-west. When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_LINE}, aligns icon x-axes with the line.
   */
  public static final String ICON_ROTATION_ALIGNMENT_MAP = "map";
  /**
   * Produces icons whose x-axes are aligned with the x-axis of the viewport, regardless of the value of {@link SYMBOL_PLACEMENT}.
   */
  public static final String ICON_ROTATION_ALIGNMENT_VIEWPORT = "viewport";
  /**
   * When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_POINT}, this is equivalent to {@link Property#ICON_ROTATION_ALIGNMENT_VIEWPORT}. When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_LINE}, this is equivalent to {@link Property#ICON_ROTATION_ALIGNMENT_MAP}.
   */
  public static final String ICON_ROTATION_ALIGNMENT_AUTO = "auto";

  /**
   * In combination with `symbol-placement`, determines the rotation behavior of icons.
   */
  @StringDef({
      ICON_ROTATION_ALIGNMENT_MAP,
      ICON_ROTATION_ALIGNMENT_VIEWPORT,
      ICON_ROTATION_ALIGNMENT_AUTO,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ICON_ROTATION_ALIGNMENT {}

  // ICON_TEXT_FIT: Scales the icon to fit around the associated text.

  /**
   * The icon is displayed at its intrinsic aspect ratio.
   */
  public static final String ICON_TEXT_FIT_NONE = "none";
  /**
   * The icon is scaled in the x-dimension to fit the width of the text.
   */
  public static final String ICON_TEXT_FIT_WIDTH = "width";
  /**
   * The icon is scaled in the y-dimension to fit the height of the text.
   */
  public static final String ICON_TEXT_FIT_HEIGHT = "height";
  /**
   * The icon is scaled in both x- and y-dimensions.
   */
  public static final String ICON_TEXT_FIT_BOTH = "both";

  /**
   * Scales the icon to fit around the associated text.
   */
  @StringDef({
      ICON_TEXT_FIT_NONE,
      ICON_TEXT_FIT_WIDTH,
      ICON_TEXT_FIT_HEIGHT,
      ICON_TEXT_FIT_BOTH,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ICON_TEXT_FIT {}

  // ICON_ANCHOR: Part of the icon placed closest to the anchor.

  /**
   * The center of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_CENTER = "center";
  /**
   * The left side of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_LEFT = "left";
  /**
   * The right side of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_RIGHT = "right";
  /**
   * The top of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_TOP = "top";
  /**
   * The bottom of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_BOTTOM = "bottom";
  /**
   * The top left corner of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_TOP_LEFT = "top-left";
  /**
   * The top right corner of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_TOP_RIGHT = "top-right";
  /**
   * The bottom left corner of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_BOTTOM_LEFT = "bottom-left";
  /**
   * The bottom right corner of the icon is placed closest to the anchor.
   */
  public static final String ICON_ANCHOR_BOTTOM_RIGHT = "bottom-right";

  /**
   * Part of the icon placed closest to the anchor.
   */
  @StringDef({
      ICON_ANCHOR_CENTER,
      ICON_ANCHOR_LEFT,
      ICON_ANCHOR_RIGHT,
      ICON_ANCHOR_TOP,
      ICON_ANCHOR_BOTTOM,
      ICON_ANCHOR_TOP_LEFT,
      ICON_ANCHOR_TOP_RIGHT,
      ICON_ANCHOR_BOTTOM_LEFT,
      ICON_ANCHOR_BOTTOM_RIGHT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ICON_ANCHOR {}

  // ICON_PITCH_ALIGNMENT: Orientation of icon when map is pitched.

  /**
   * The icon is aligned to the plane of the map.
   */
  public static final String ICON_PITCH_ALIGNMENT_MAP = "map";
  /**
   * The icon is aligned to the plane of the viewport.
   */
  public static final String ICON_PITCH_ALIGNMENT_VIEWPORT = "viewport";
  /**
   * Automatically matches the value of {@link ICON_ROTATION_ALIGNMENT}.
   */
  public static final String ICON_PITCH_ALIGNMENT_AUTO = "auto";

  /**
   * Orientation of icon when map is pitched.
   */
  @StringDef({
      ICON_PITCH_ALIGNMENT_MAP,
      ICON_PITCH_ALIGNMENT_VIEWPORT,
      ICON_PITCH_ALIGNMENT_AUTO,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ICON_PITCH_ALIGNMENT {}

  // TEXT_PITCH_ALIGNMENT: Orientation of text when map is pitched.

  /**
   * The text is aligned to the plane of the map.
   */
  public static final String TEXT_PITCH_ALIGNMENT_MAP = "map";
  /**
   * The text is aligned to the plane of the viewport.
   */
  public static final String TEXT_PITCH_ALIGNMENT_VIEWPORT = "viewport";
  /**
   * Automatically matches the value of {@link TEXT_ROTATION_ALIGNMENT}.
   */
  public static final String TEXT_PITCH_ALIGNMENT_AUTO = "auto";

  /**
   * Orientation of text when map is pitched.
   */
  @StringDef({
      TEXT_PITCH_ALIGNMENT_MAP,
      TEXT_PITCH_ALIGNMENT_VIEWPORT,
      TEXT_PITCH_ALIGNMENT_AUTO,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_PITCH_ALIGNMENT {}

  // TEXT_ROTATION_ALIGNMENT: In combination with `symbol-placement`, determines the rotation behavior of the individual glyphs forming the text.

  /**
   * When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_POINT}, aligns text east-west. When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_LINE}, aligns text x-axes with the line.
   */
  public static final String TEXT_ROTATION_ALIGNMENT_MAP = "map";
  /**
   * Produces glyphs whose x-axes are aligned with the x-axis of the viewport, regardless of the value of {@link SYMBOL_PLACEMENT}.
   */
  public static final String TEXT_ROTATION_ALIGNMENT_VIEWPORT = "viewport";
  /**
   * When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_POINT}, this is equivalent to {@link Property#TEXT_ROTATION_ALIGNMENT_VIEWPORT}. When {@link SYMBOL_PLACEMENT} is set to {@link Property#SYMBOL_PLACEMENT_LINE}, this is equivalent to {@link Property#TEXT_ROTATION_ALIGNMENT_MAP}.
   */
  public static final String TEXT_ROTATION_ALIGNMENT_AUTO = "auto";

  /**
   * In combination with `symbol-placement`, determines the rotation behavior of the individual glyphs forming the text.
   */
  @StringDef({
      TEXT_ROTATION_ALIGNMENT_MAP,
      TEXT_ROTATION_ALIGNMENT_VIEWPORT,
      TEXT_ROTATION_ALIGNMENT_AUTO,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_ROTATION_ALIGNMENT {}

  // TEXT_JUSTIFY: Text justification options.

  /**
   * The text is aligned to the left.
   */
  public static final String TEXT_JUSTIFY_LEFT = "left";
  /**
   * The text is centered.
   */
  public static final String TEXT_JUSTIFY_CENTER = "center";
  /**
   * The text is aligned to the right.
   */
  public static final String TEXT_JUSTIFY_RIGHT = "right";

  /**
   * Text justification options.
   */
  @StringDef({
      TEXT_JUSTIFY_LEFT,
      TEXT_JUSTIFY_CENTER,
      TEXT_JUSTIFY_RIGHT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_JUSTIFY {}

  // TEXT_ANCHOR: Part of the text placed closest to the anchor.

  /**
   * The center of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_CENTER = "center";
  /**
   * The left side of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_LEFT = "left";
  /**
   * The right side of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_RIGHT = "right";
  /**
   * The top of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_TOP = "top";
  /**
   * The bottom of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_BOTTOM = "bottom";
  /**
   * The top left corner of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_TOP_LEFT = "top-left";
  /**
   * The top right corner of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_TOP_RIGHT = "top-right";
  /**
   * The bottom left corner of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_BOTTOM_LEFT = "bottom-left";
  /**
   * The bottom right corner of the text is placed closest to the anchor.
   */
  public static final String TEXT_ANCHOR_BOTTOM_RIGHT = "bottom-right";

  /**
   * Part of the text placed closest to the anchor.
   */
  @StringDef({
      TEXT_ANCHOR_CENTER,
      TEXT_ANCHOR_LEFT,
      TEXT_ANCHOR_RIGHT,
      TEXT_ANCHOR_TOP,
      TEXT_ANCHOR_BOTTOM,
      TEXT_ANCHOR_TOP_LEFT,
      TEXT_ANCHOR_TOP_RIGHT,
      TEXT_ANCHOR_BOTTOM_LEFT,
      TEXT_ANCHOR_BOTTOM_RIGHT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_ANCHOR {}

  // TEXT_TRANSFORM: Specifies how to capitalize text, similar to the CSS `text-transform` property.

  /**
   * The text is not altered.
   */
  public static final String TEXT_TRANSFORM_NONE = "none";
  /**
   * Forces all letters to be displayed in uppercase.
   */
  public static final String TEXT_TRANSFORM_UPPERCASE = "uppercase";
  /**
   * Forces all letters to be displayed in lowercase.
   */
  public static final String TEXT_TRANSFORM_LOWERCASE = "lowercase";

  /**
   * Specifies how to capitalize text, similar to the CSS `text-transform` property.
   */
  @StringDef({
      TEXT_TRANSFORM_NONE,
      TEXT_TRANSFORM_UPPERCASE,
      TEXT_TRANSFORM_LOWERCASE,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_TRANSFORM {}

  // FILL_TRANSLATE_ANCHOR: Controls the frame of reference for `fill-translate`.

  /**
   * The fill is translated relative to the map.
   */
  public static final String FILL_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * The fill is translated relative to the viewport.
   */
  public static final String FILL_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `fill-translate`.
   */
  @StringDef({
      FILL_TRANSLATE_ANCHOR_MAP,
      FILL_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface FILL_TRANSLATE_ANCHOR {}

  // LINE_TRANSLATE_ANCHOR: Controls the frame of reference for `line-translate`.

  /**
   * The line is translated relative to the map.
   */
  public static final String LINE_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * The line is translated relative to the viewport.
   */
  public static final String LINE_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `line-translate`.
   */
  @StringDef({
      LINE_TRANSLATE_ANCHOR_MAP,
      LINE_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface LINE_TRANSLATE_ANCHOR {}

  // ICON_TRANSLATE_ANCHOR: Controls the frame of reference for `icon-translate`.

  /**
   * Icons are translated relative to the map.
   */
  public static final String ICON_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * Icons are translated relative to the viewport.
   */
  public static final String ICON_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `icon-translate`.
   */
  @StringDef({
      ICON_TRANSLATE_ANCHOR_MAP,
      ICON_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ICON_TRANSLATE_ANCHOR {}

  // TEXT_TRANSLATE_ANCHOR: Controls the frame of reference for `text-translate`.

  /**
   * The text is translated relative to the map.
   */
  public static final String TEXT_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * The text is translated relative to the viewport.
   */
  public static final String TEXT_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `text-translate`.
   */
  @StringDef({
      TEXT_TRANSLATE_ANCHOR_MAP,
      TEXT_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface TEXT_TRANSLATE_ANCHOR {}

  // CIRCLE_TRANSLATE_ANCHOR: Controls the frame of reference for `circle-translate`.

  /**
   * The circle is translated relative to the map.
   */
  public static final String CIRCLE_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * The circle is translated relative to the viewport.
   */
  public static final String CIRCLE_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `circle-translate`.
   */
  @StringDef({
      CIRCLE_TRANSLATE_ANCHOR_MAP,
      CIRCLE_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface CIRCLE_TRANSLATE_ANCHOR {}

  // CIRCLE_PITCH_SCALE: Controls the scaling behavior of the circle when the map is pitched.

  /**
   * Circles are scaled according to their apparent distance to the camera.
   */
  public static final String CIRCLE_PITCH_SCALE_MAP = "map";
  /**
   * Circles are not scaled.
   */
  public static final String CIRCLE_PITCH_SCALE_VIEWPORT = "viewport";

  /**
   * Controls the scaling behavior of the circle when the map is pitched.
   */
  @StringDef({
      CIRCLE_PITCH_SCALE_MAP,
      CIRCLE_PITCH_SCALE_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface CIRCLE_PITCH_SCALE {}

  // CIRCLE_PITCH_ALIGNMENT: Orientation of circle when map is pitched.

  /**
   * The circle is aligned to the plane of the map.
   */
  public static final String CIRCLE_PITCH_ALIGNMENT_MAP = "map";
  /**
   * The circle is aligned to the plane of the viewport.
   */
  public static final String CIRCLE_PITCH_ALIGNMENT_VIEWPORT = "viewport";

  /**
   * Orientation of circle when map is pitched.
   */
  @StringDef({
      CIRCLE_PITCH_ALIGNMENT_MAP,
      CIRCLE_PITCH_ALIGNMENT_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface CIRCLE_PITCH_ALIGNMENT {}

  // FILL_EXTRUSION_TRANSLATE_ANCHOR: Controls the frame of reference for `fill-extrusion-translate`.

  /**
   * The fill extrusion is translated relative to the map.
   */
  public static final String FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP = "map";
  /**
   * The fill extrusion is translated relative to the viewport.
   */
  public static final String FILL_EXTRUSION_TRANSLATE_ANCHOR_VIEWPORT = "viewport";

  /**
   * Controls the frame of reference for `fill-extrusion-translate`.
   */
  @StringDef({
      FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP,
      FILL_EXTRUSION_TRANSLATE_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface FILL_EXTRUSION_TRANSLATE_ANCHOR {}

  // HILLSHADE_ILLUMINATION_ANCHOR: Direction of light source when map is rotated.

  /**
   * The hillshade illumination is relative to the north direction.
   */
  public static final String HILLSHADE_ILLUMINATION_ANCHOR_MAP = "map";
  /**
   * The hillshade illumination is relative to the top of the viewport.
   */
  public static final String HILLSHADE_ILLUMINATION_ANCHOR_VIEWPORT = "viewport";

  /**
   * Direction of light source when map is rotated.
   */
  @StringDef({
      HILLSHADE_ILLUMINATION_ANCHOR_MAP,
      HILLSHADE_ILLUMINATION_ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface HILLSHADE_ILLUMINATION_ANCHOR {}

  // ANCHOR: Whether extruded geometries are lit relative to the map or viewport.

  /**
   * The position of the light source is aligned to the rotation of the map.
   */
  public static final String ANCHOR_MAP = "map";
  /**
   * The position of the light source is aligned to the rotation of the viewport.
   */
  public static final String ANCHOR_VIEWPORT = "viewport";

  /**
   * Whether extruded geometries are lit relative to the map or viewport.
   */
  @StringDef({
      ANCHOR_MAP,
      ANCHOR_VIEWPORT,
    })
  @Retention(RetentionPolicy.SOURCE)
  public @interface ANCHOR {}


  private Property() {
  }
}
