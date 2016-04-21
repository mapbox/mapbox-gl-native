package com.mapbox.mapboxsdk.testapp.view;

import android.content.Context;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.testapp.R;

public class CustomMarkerView extends MarkerView {

    private TextView textView;
    private ImageView imageView;

    public CustomMarkerView(Context context) {
        super(context);
        init(context);
    }

    private void init(Context context) {
        LayoutInflater.from(context).inflate(R.layout.view_custom_marker, this);
        textView = (TextView) findViewById(R.id.textView);
        imageView = (ImageView) findViewById(R.id.imageView);
    }

    public void setText(@NonNull String text) {
        textView.setText(text);
    }

    public void setImage(@DrawableRes int drawableRes) {
        imageView.setImageResource(drawableRes);
    }
}
