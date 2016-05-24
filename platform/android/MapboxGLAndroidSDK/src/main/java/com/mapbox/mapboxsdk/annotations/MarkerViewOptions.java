package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.PointF;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class MarkerViewOptions extends BaseMarkerViewOptions<MarkerView, MarkerViewOptions> {

    private MarkerView marker;

    public MarkerViewOptions() {
        marker = new MarkerView();
    }

    protected MarkerViewOptions(Parcel in) {
        marker = new MarkerView();
        position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
        snippet(in.readString());
        title(in.readString());
        flat(in.readByte() != 0);
        centerOffset((PointF) in.readParcelable(PointF.class.getClassLoader()));
        infoWindowOffset((Point) in.readParcelable(Point.class.getClassLoader()));
        selectAnimatorResource(in.readInt());
        deselectAnimatorResource(in.readInt());
        rotation(in.readInt());
        visible(in.readByte() != 0);
        if (in.readByte() != 0) {
            // this means we have an icon
            String iconId = in.readString();
            Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
            Icon icon = new Icon(iconId, iconBitmap);
            icon(icon);
        }
    }

    @Override
    public MarkerViewOptions getThis() {
        return this;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeParcelable(getPosition(), flags);
        out.writeString(getSnippet());
        out.writeString(getTitle());
        out.writeByte((byte) (isFlat() ? 1 : 0));
        out.writeParcelable(getCenterOffset(), flags);
        out.writeParcelable(getInfoWindowOffset(), flags);
        out.writeInt(getSelectAnimRes());
        out.writeInt(getDeselectAnimRes());
        out.writeInt(getRotation());
        out.writeByte((byte) (isVisible() ? 1 : 0));
        Icon icon = getIcon();
        out.writeByte((byte) (icon != null ? 1 : 0));
        if (icon != null) {
            out.writeString(getIcon().getId());
            out.writeParcelable(getIcon().getBitmap(), flags);
        }
    }

    @Override
    public MarkerView getMarker() {
        marker.setPosition(position);
        marker.setSnippet(snippet);
        marker.setTitle(title);
        marker.setIcon(icon);
        marker.setFlat(flat);
        marker.setCenterOffset(centerOffset);
        marker.setInfoWindowOffset(infoWindowOffset);
        marker.setSelectAnimRes(selectAnimRes);
        marker.setDeselectAnimRes(deselectAnimRes);
        marker.setRotation(rotation);
        marker.setVisible(visible);
        return marker;
    }

    public static final Parcelable.Creator<MarkerViewOptions> CREATOR
            = new Parcelable.Creator<MarkerViewOptions>() {
        public MarkerViewOptions createFromParcel(Parcel in) {
            return new MarkerViewOptions(in);
        }

        public MarkerViewOptions[] newArray(int size) {
            return new MarkerViewOptions[size];
        }
    };

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        MarkerViewOptions that = (MarkerViewOptions) o;
        return marker != null ? marker.equals(that.marker) : that.marker == null;
    }

    @Override
    public int hashCode() {
        return marker != null ? marker.hashCode() : 0;
    }
}
