package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.graphics.PointF;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.AnimatorRes;
import android.support.annotation.NonNull;

public class MarkerViewSettings implements Parcelable {

    private PointF centerOffset;
    private Point infoWindowOffset;
    private boolean flat;

    private int animEnterRes;
    private int animSelectRes;
    private int animDeselectRes;
    private int animExitRes;

    MarkerViewSettings(PointF centerOffset, Point infoWindowOffset, boolean flat, int animEnterRes, int animSelectRes, int animDeselectRes, int animExitRes) {
        this.centerOffset = centerOffset;
        this.infoWindowOffset = infoWindowOffset;
        this.flat = flat;
        this.animEnterRes = animEnterRes;
        this.animSelectRes = animSelectRes;
        this.animDeselectRes = animDeselectRes;
        this.animExitRes = animExitRes;
    }

    MarkerViewSettings(Parcel parcel) {
        this.centerOffset = parcel.readParcelable(PointF.class.getClassLoader());
        this.infoWindowOffset = parcel.readParcelable(Point.class.getClassLoader());
        this.flat = parcel.readByte() != 0;
        this.animEnterRes = parcel.readInt();
        this.animSelectRes = parcel.readInt();
        this.animDeselectRes = parcel.readInt();
        this.animExitRes = parcel.readInt();
    }

    @NonNull
    public PointF getCenterOffset() {
        return centerOffset;
    }

    @NonNull
    public Point getInfoWindowOffset() {
        return infoWindowOffset;
    }

    public boolean isFlat() {
        return flat;
    }

    @AnimatorRes
    public int getAnimEnterRes() {
        return animEnterRes;
    }

    @AnimatorRes
    public int getAnimSelectRes() {
        return animSelectRes;
    }

    @AnimatorRes
    public int getAnimDeselectRes() {
        return animDeselectRes;
    }

    @AnimatorRes
    public int getAnimExitRes() {
        return animExitRes;
    }

    public static final Parcelable.Creator<MarkerViewSettings> CREATOR
            = new Parcelable.Creator<MarkerViewSettings>() {
        public MarkerViewSettings createFromParcel(Parcel in) {
            return new MarkerViewSettings(in);
        }

        public MarkerViewSettings[] newArray(int size) {
            return new MarkerViewSettings[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(centerOffset, flags);
        dest.writeParcelable(infoWindowOffset, flags);
        dest.writeByte((byte) (flat ? 1 : 0));
        dest.writeInt(animEnterRes);
        dest.writeInt(animSelectRes);
        dest.writeInt(animDeselectRes);
        dest.writeInt(animExitRes);
    }

    /**
     * Builder for composing LatLngBounds objects.
     */
    public static final class Builder {

        private PointF centerOffset;
        private Point infoWindowOffset;
        private boolean flat;

        private int animEnterRes;
        private int animSelectRes;
        private int animDeselectRes;
        private int animExitRes;

        public Builder() {
            centerOffset = new PointF();
            infoWindowOffset = new Point();
        }

        public Builder centerOffset(float dx, float dy) {
            centerOffset.offset(dx, dy);
            return this;
        }

        public Builder infoWindowOffset(int dx, int dy) {
            infoWindowOffset.offset(dx, -dy);
            return this;
        }

        public Builder flat(boolean isFlat) {
            flat = isFlat;
            return this;
        }

        public Builder animEnterRes(@AnimatorRes int animatorRes) {
            animEnterRes = animatorRes;
            return this;
        }

        public Builder animSelectRes(@AnimatorRes int animatorRes) {
            animSelectRes = animatorRes;
            return this;
        }

        public Builder animDeselectRes(@AnimatorRes int animatorRes) {
            animDeselectRes = animatorRes;
            return this;
        }

        public Builder animExitRes(@AnimatorRes int animatorRes) {
            animExitRes = animatorRes;
            return this;
        }

        public MarkerViewSettings build() {
            return new MarkerViewSettings(centerOffset, infoWindowOffset, flat, animEnterRes, animSelectRes, animDeselectRes, animExitRes);
        }
    }
}
