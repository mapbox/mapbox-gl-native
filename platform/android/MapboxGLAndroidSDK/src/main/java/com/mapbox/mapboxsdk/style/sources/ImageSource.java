package com.mapbox.mapboxsdk.style.sources;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.geometry.LatLngQuad;

import java.net.URL;


/**
 * Image source, allows a georeferenced raster image to be shown on the map.
 * <p>
 * The georeferenced image scales and rotates as the user zooms and rotates the map.
 * The geographic location of the raster image content, supplied with `LatLngQuad`,
 * can be non-axis aligned.
 * </p>
 * * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-image">the style specification</a>
 */
@UiThread
public class ImageSource extends Source {

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  public ImageSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create an ImageSource from coordinates and an image URL
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param url         remote json file
   */
  public ImageSource(String id, LatLngQuad coordinates, URL url) {
    initialize(id, coordinates);
    setUrl(url);
  }

  /**
   * Create an ImageSource from coordinates and a bitmap image
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param bitmap      A Bitmap image
   */
  public ImageSource(String id, LatLngQuad coordinates, @NonNull android.graphics.Bitmap bitmap) {
    initialize(id, coordinates);
    setImage(bitmap);
  }

  /**
   * Create an ImageSource from coordinates and a bitmap image resource
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param resourceId  The resource ID of a Bitmap image
   */
  public ImageSource(String id, LatLngQuad coordinates, @DrawableRes int resourceId) {
    initialize(id, coordinates);
    setImage(resourceId);
  }

  /**
   * Updates the source image url
   *
   * @param url An Image url
   */
  public void setUrl(URL url) {
    setUrl(url.toExternalForm());
  }

  /**
   * Updates the source image url
   *
   * @param url An image url
   */
  public void setUrl(String url) {
    nativeSetUrl(url);
  }

  /**
   * Updates the source image to a bitmap
   *
   * @param bitmap A Bitmap image
   */
  public void setImage(@NonNull android.graphics.Bitmap bitmap) {
    nativeSetImage(bitmap);
  }

  /**
   * Updates the source image to a bitmap image resource
   *
   * @param resourceId The resource ID of a Bitmap image
   */
  public void setImage(@DrawableRes int resourceId) throws IllegalArgumentException {
    Context context = Mapbox.getApplicationContext();
    Drawable drawable = ContextCompat.getDrawable(context, resourceId);
    if (drawable instanceof BitmapDrawable) {
      BitmapDrawable bitmapDrawable = (BitmapDrawable) drawable;
      nativeSetImage(bitmapDrawable.getBitmap());
    } else {
      throw new IllegalArgumentException("Failed to decode image. The resource provided must be a Bitmap.");
    }
  }

  /**
   * @return The url or null
   */
  @Nullable
  public String getUrl() {
    return nativeGetUrl();
  }

  /**
   * Updates the latitude and longitude of the four corners of the image
   *
   * @param latLngQuad latitude and longitude of the four corners of the image
   */
  public void setCoordinates(LatLngQuad latLngQuad) {
    nativeSetCoordinates(latLngQuad);
  }

  protected native void initialize(String layerId, LatLngQuad payload);

  protected native void nativeSetUrl(String url);

  protected native String nativeGetUrl();

  protected native void nativeSetImage(Bitmap bitmap);

  protected native void nativeSetCoordinates(LatLngQuad latLngQuad);

  @Override
  protected native void finalize() throws Throwable;
}
