package com.mapbox.mapboxsdk.maps;

import android.graphics.RectF;

import com.mapbox.mapboxsdk.annotations.Annotation;

import java.util.List;

interface ShapeAnnotations {

  List<Annotation> obtainAllIn(RectF rectF);

}
