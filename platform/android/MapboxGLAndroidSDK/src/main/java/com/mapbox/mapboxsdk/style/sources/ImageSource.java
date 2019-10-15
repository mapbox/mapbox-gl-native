package com.mapbox.mapboxsdk.style.sources;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import androidx.annotation.DrawableRes;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.geometry.LatLngQuad;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.net.URI;
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
  @Keep
  ImageSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create an ImageSource from coordinates and an image URL
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param url         remote json file
   * @deprecated use {@link ImageSource#ImageSource(String, LatLngQuad, URI)} instead
   */
  @Deprecated
  public ImageSource(String id, LatLngQuad coordinates, @NonNull URL url) {
    super();
    initialize(id, coordinates);
    setUrl(url);
  }

  /**
   * Create an ImageSource from coordinates and an image URI
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param uri         json file uri
   */
  public ImageSource(String id, LatLngQuad coordinates, @NonNull URI uri) {
    super();
    initialize(id, coordinates);
    setUri(uri);
  }

  /**
   * Create an ImageSource from coordinates and a bitmap image
   *
   * @param id          The source id
   * @param coordinates The Latitude and Longitude of the four corners of the image
   * @param bitmap      A Bitmap image
   */
  public ImageSource(String id, LatLngQuad coordinates, @NonNull Bitmap bitmap) {
    super();
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
    super();
    initialize(id, coordinates);
    setImage(resourceId);
  }

  /**
   * Updates the source image url
   *
   * @param url An Image url
   * @deprecated use {@link #setUri(URI)} instead
   */
  @Deprecated
  public void setUrl(@NonNull URL url) {
    setUrl(url.toExternalForm());
  }

  /**
   * Updates the source image url
   *
   * @param url An image url
   * @deprecated use {@link #setUri(String)} instead
   */
  @Deprecated
  public void setUrl(String url) {
    checkThread();
    nativeSetUrl(url);
  }

  /**
   * Updates the source image URI.
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param uri An Image uri
   */
  public void setUri(@NonNull URI uri) {
    checkThread();
    nativeSetUrl(uri.toString());
  }

  /**
   * Updates the source image URI.
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param uri An image uri
   */
  public void setUri(String uri) {
    checkThread();
    nativeSetUrl(uri);
  }

  /**
   * Updates the source image to a bitmap
   *
   * @param bitmap A Bitmap image
   */
  public void setImage(@NonNull android.graphics.Bitmap bitmap) {
    checkThread();
    nativeSetImage(bitmap);
  }

  /**
   * Updates the source image to a bitmap image resource
   *
   * @param resourceId The resource ID of a Bitmap image
   */
  public void setImage(@DrawableRes int resourceId) throws IllegalArgumentException {
    checkThread();
    Context context = Mapbox.getApplicationContext();
    Drawable drawable = BitmapUtils.getDrawableFromRes(context, resourceId);
    if (drawable instanceof BitmapDrawable) {
      BitmapDrawable bitmapDrawable = (BitmapDrawable) drawable;
      nativeSetImage(bitmapDrawable.getBitmap());
    } else {
      throw new IllegalArgumentException("Failed to decode image. The resource provided must be a Bitmap.");
    }
  }

  /**
   * @return The url or null
   * @deprecated use {@link #getUri()} instead
   */
  @Deprecated
  @Nullable
  public String getUrl() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * Get the source URI.
   *
   * @return The uri or null
   */
  @Nullable
  public String getUri() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * Updates the latitude and longitude of the four corners of the image
   *
   * @param latLngQuad latitude and longitude of the four corners of the image
   */
  public void setCoordinates(LatLngQuad latLngQuad) {
    checkThread();
    nativeSetCoordinates(latLngQuad);
  }

  @Keep
  protected native void initialize(String layerId, LatLngQuad payload);

  @Keep
  protected native void nativeSetUrl(String url);

  @NonNull
  @Keep
  protected native String nativeGetUrl();

  @Keep
  protected native void nativeSetImage(Bitmap bitmap);

  @Keep
  protected native void nativeSetCoordinates(LatLngQuad latLngQuad);

  @Override
  @Keep
  protected native void finalize() throws Throwable;
}
