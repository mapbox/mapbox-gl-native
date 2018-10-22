package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.exceptions.InvalidMarkerPositionException;
import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * builder class for composing MarkerView objects.
 * <p>
 * Do not extend this class directly but extend {@link BaseMarkerViewOptions} instead.
 * </p>
 * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
 * SDK views to be used as a symbol see https://github.com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android/MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
 */
@Deprecated
public class MarkerViewOptions extends BaseMarkerViewOptions<MarkerView, MarkerViewOptions> {

  private MarkerView marker;

  /**
   * Defines default options for a MarkerView. Extend {@link BaseMarkerViewOptions} if you need
   * more customization.
   */
  public MarkerViewOptions() {
    marker = new MarkerView();
  }

  protected MarkerViewOptions(Parcel in) {
    marker = new MarkerView();
    position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
    snippet(in.readString());
    title(in.readString());
    flat(in.readByte() != 0);
    anchor(in.readFloat(), in.readFloat());
    infoWindowAnchor(in.readFloat(), in.readFloat());
    rotation(in.readFloat());
    visible(in.readByte() != 0);
    alpha(in.readFloat());
    if (in.readByte() != 0) {
      // this means we have an icon
      String iconId = in.readString();
      Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
      Icon icon = new Icon(iconId, iconBitmap);
      icon(icon);
    }
  }

  /**
   * Get the instance of the object for which this method was called.
   *
   * @return the object for which this method was called.
   */
  @Override
  public MarkerViewOptions getThis() {
    return this;
  }

  /**
   * Describe the kinds of special objects contained in this Parcelable's
   * marshalled representation.
   *
   * @return integer 0.
   */
  @Override
  public int describeContents() {
    return 0;
  }

  /**
   * Flatten this object in to a Parcel.
   *
   * @param out   The Parcel in which the object should be written.
   * @param flags Additional flags about how the object should be written. May be 0 or
   *              {@link #PARCELABLE_WRITE_RETURN_VALUE}.
   */
  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeParcelable(getPosition(), flags);
    out.writeString(getSnippet());
    out.writeString(getTitle());
    out.writeByte((byte) (isFlat() ? 1 : 0));
    out.writeFloat(getAnchorU());
    out.writeFloat(getAnchorV());
    out.writeFloat(getInfoWindowAnchorU());
    out.writeFloat(getInfoWindowAnchorV());
    out.writeFloat(getRotation());
    out.writeByte((byte) (isVisible() ? 1 : 0));
    out.writeFloat(alpha);
    Icon icon = getIcon();
    out.writeByte((byte) (icon != null ? 1 : 0));
    if (icon != null) {
      out.writeString(getIcon().getId());
      out.writeParcelable(getIcon().getBitmap(), flags);
    }
  }

  /**
   * Get the {@link MarkerView}.
   *
   * @return {@link MarkerView}.
   */
  @Override
  public MarkerView getMarker() {
    if (position == null) {
      throw new InvalidMarkerPositionException();
    }

    marker.setPosition(position);
    marker.setSnippet(snippet);
    marker.setTitle(title);
    marker.setIcon(icon);
    marker.setFlat(flat);
    marker.setAnchor(anchorU, anchorV);
    marker.setInfoWindowAnchor(infoWindowAnchorU, infoWindowAnchorV);
    marker.setRotation(rotation);
    marker.setVisible(visible);
    marker.setAlpha(alpha);
    return marker;
  }

  public static final Parcelable.Creator<MarkerViewOptions> CREATOR =
    new Parcelable.Creator<MarkerViewOptions>() {
      public MarkerViewOptions createFromParcel(Parcel in) {
        return new MarkerViewOptions(in);
      }

      public MarkerViewOptions[] newArray(int size) {
        return new MarkerViewOptions[size];
      }
    };

  /**
   * Compares this {@link MarkerViewOptions} object with another {@link MarkerViewOptions} and
   * determines if they match.
   *
   * @param object Another {@link MarkerViewOptions} to compare with this object.
   * @return True if the {@link MarkerViewOptions} being passed in matches this
   * {@link PolylineOptions} object. Else, false.
   */
  @Override
  public boolean equals(Object object) {
    if (this == object) {
      return true;
    }
    if (object == null || getClass() != object.getClass()) {
      return false;
    }
    MarkerViewOptions that = (MarkerViewOptions) object;
    return marker != null ? marker.equals(that.marker) : that.marker == null;
  }

  /**
   * Gives an integer which can be used as the bucket number for storing elements of the set/map.
   * This bucket number is the address of the element inside the set/map. There's no guarantee
   * that this hash value will be consistent between different Java implementations, or even
   * between different execution runs of the same program.
   *
   * @return integer value you can use for storing element.
   */
  @Override
  public int hashCode() {
    return marker != null ? marker.hashCode() : 0;
  }
}
