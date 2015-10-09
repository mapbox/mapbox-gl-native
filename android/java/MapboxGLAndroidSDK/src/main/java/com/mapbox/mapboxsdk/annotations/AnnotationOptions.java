package com.mapbox.mapboxsdk.annotations;

public abstract class AnnotationOptions {

    protected Annotation annotation;

    public AnnotationOptions() {}

    public AnnotationOptions alpha(float alpha) {
        annotation.alpha = alpha;
        return this;
    }

    public float getAlpha() {
        return annotation.alpha;
    }

    public boolean isVisible() {
        return annotation.visible;
    }

}
