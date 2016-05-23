package com.mapbox.mapboxsdk.annotations;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.support.annotation.NonNull;
import android.view.View;

import java.util.HashMap;
import java.util.Map;

public class MarkerViewTransformer {

    private Map<MarkerView, View> mMarkerViewMap;

    public MarkerViewTransformer() {
        mMarkerViewMap = new HashMap<>();
    }

    /**
     * Animate the rotation of a marker view.
     *
     * @param markerView
     * @param rotation
     */
    public void animateRotation(@NonNull MarkerView markerView, float rotation) {
        final View convertView = mMarkerViewMap.get(markerView);
        if (convertView != null) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(convertView, View.ROTATION, convertView.getRotation(), rotation);
            rotateAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                }
            });
            rotateAnimator.start();
        }
    }

    public void animateAlpha(@NonNull MarkerView markerView, float alpha) {
        final View convertView = mMarkerViewMap.get(markerView);
        if (convertView != null) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(convertView, View.ALPHA, convertView.getAlpha(), alpha);
            rotateAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                }
            });
            rotateAnimator.start();
        }
    }

    /**
     * Set tilt to marker views in the viewport.
     *
     * @param tilt
     */
    public void setTilt(float tilt) {
        for (MarkerView markerView : mMarkerViewMap.keySet()) {
            if (markerView.isFlat()) {
                markerView.setTiltValue(tilt);
                mMarkerViewMap.get(markerView).setRotationX(tilt);
            }
        }
    }

    public Map<MarkerView, View> getMarkerViewMap() {
        return mMarkerViewMap;
    }

}
