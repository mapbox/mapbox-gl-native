package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.geometry.LatLng;

public final class MarkerOptions implements Parcelable {

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
        String spriteId = in.readString();
        Bitmap spriteBitmap = in.readParcelable(Bitmap.class.getClassLoader());
        Sprite icon = new Sprite(spriteId, spriteBitmap);
        icon(icon);
        title(in.readString());
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeParcelable(getPosition(), flags);
        out.writeString(getSnippet());
        out.writeString(getIcon().getId());
        out.writeParcelable(getIcon().getBitmap(), flags);
        out.writeString(getTitle());
    }

    private Marker marker;

    public MarkerOptions() {
        marker = new Marker();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Marker getMarker() {
        return marker;
    }

    public LatLng getPosition() {
        return marker.getPosition();
    }

    public String getSnippet() {
        return marker.getSnippet();
    }

    public String getTitle() {
        return marker.getTitle();
    }

    public Sprite getIcon() {
        return marker.getIcon();
    }

    public MarkerOptions position(LatLng position) {
        marker.setPosition(position);
        return this;
    }

    public MarkerOptions snippet(String snippet) {
        marker.setSnippet(snippet);
        return this;
    }

    public MarkerOptions icon(@Nullable Sprite icon) {
        marker.setIcon(icon);
        return this;
    }

    public MarkerOptions title(String title) {
        marker.setTitle(title);
        return this;
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
