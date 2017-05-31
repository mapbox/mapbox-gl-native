package com.mapbox.mapboxsdk.storage;

import android.support.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Resource provides access to resource types.
 */
public final class Resource {
  // Note: Keep this in sync with include/mbgl/storage/resource.hpp

  /**
   * Resource type variants.
   */
  @IntDef( {UNKNOWN, STYLE, SOURCE, TILE, GLYPHS, SPRITE_IMAGE, SPRITE_JSON})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Kind {
  }

  /**
   * Unknown type
   */
  public static final int UNKNOWN = 0;

  /**
   * Style sheet JSON file
   */
  public static final int STYLE = 1;

  /**
   * TileJSON file as specified in https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sources
   */
  public static final int SOURCE = 2;

  /**
   * A vector or raster tile as described in the style sheet at
   * https://www.mapbox.com/mapbox-gl-js/style-spec/#sources
   */
  public static final int TILE = 3;

  /**
   * Signed distance field glyphs for text rendering. These are the URLs specified in the style
   * in https://www.mapbox.com/mapbox-gl-js/style-spec/#root-glyphs
   */
  public static final int GLYPHS = 4;

  /**
   * Image part of a sprite sheet. It is constructed of the prefix in
   *  https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sprite and a PNG file extension.
   */
  public static final int SPRITE_IMAGE = 5;

  /**
   * JSON part of a sprite sheet. It is constructed of the prefix in
   * https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sprite and a JSON file extension.
   */
  public static final int SPRITE_JSON = 6;
}
