package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.geometry.LatLng;


/**
 * <p>
 * Builder for composing {@link com.mapbox.mapboxsdk.annotations.Marker} objects.
 * </p>
 * <p/>
 * <h3>Example</h3>
 * <p/>
 * <pre>
 * mMapView.addMarker(new MarkerOptions()
 *   .title("Intersection")
 *   .snippet("H St NW with 15th St NW")
 *   .position(new LatLng(38.9002073, -77.03364419)));
 * </pre>
 */
public final class MarkerOptions extends BaseMarkerOptions<Marker, MarkerOptions> implements Parcelable {

    public static final Parcelable.Creator<MarkerOptions> CREATOR
            = new Parcelable.Creator<MarkerOptions>() {
        public MarkerOptions createFromParcel(Parcel in) {
            return new MarkerOptions(in);
        }

        public MarkerOptions[] newArray(int size) {
            return new MarkerOptions[size];
        }
    };

    private MarkerOptions(Parcel in) {
        marker = new Marker();
        position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
        snippet(in.readString());

        if(in.readByte()!=0){
            // this means we have an icon
            String iconId = in.readString();
            Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
            Icon icon = new Icon(iconId, iconBitmap);
            icon(icon);
        }
        title(in.readString());
    }

    @Override
    public MarkerOptions getThis() {
        return this;
    }

    @Override
    public int describeContents() {
        return hashCode();
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeParcelable(getPosition(), flags);
        out.writeString(getSnippet());
        Icon icon = getIcon();
        out.writeByte((byte) (icon != null ? 1 : 0));
        if (icon != null) {
            out.writeString(getIcon().getId());
            out.writeParcelable(getIcon().getBitmap(), flags);
        }
        out.writeString(getTitle());
    }

    private Marker marker;

    public MarkerOptions() {
        marker = new Marker();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @return Marker The build marker
     */
    public Marker getMarker() {
        marker.setPosition(position);
        marker.setSnippet(snippet);
        marker.setTitle(title);
        marker.setIcon(icon);
        return marker;
    }

    public LatLng getPosition() {
        return position;
    }

    public String getSnippet() {
        return snippet;
    }

    public String getTitle() {
        return title;
    }

    public Icon getIcon() {
        return icon;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        MarkerOptions marker = (MarkerOptions) o;

        if (getPosition() != null ? !getPosition().equals(marker.getPosition()) : marker.getPosition() != null)
            return false;
        if (getSnippet() != null ? !getSnippet().equals(marker.getSnippet()) : marker.getSnippet() != null)
            return false;
        if (getIcon() != null ? !getIcon().equals(marker.getIcon()) : marker.getIcon() != null)
            return false;
        return !(getTitle() != null ? !getTitle().equals(marker.getTitle()) : marker.getTitle() != null);
    }

    @Override
    public int hashCode() {
        int result = 1;
        result = 31 * result + (getPosition() != null ? getPosition().hashCode() : 0);
        result = 31 * result + (getSnippet() != null ? getSnippet().hashCode() : 0);
        result = 31 * result + (getIcon() != null ? getIcon().hashCode() : 0);
        result = 31 * result + (getTitle() != null ? getTitle().hashCode() : 0);
        return result;
    }
}
