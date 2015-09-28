package com.mapbox.mapboxgl.views.widget;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.Icon;
import android.net.Uri;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxgl.utils.ImageViewUtil;
import com.mapbox.mapboxgl.views.R;

public final class LogoView extends ImageView {

    private static final int MAPBOX_LOGO_SIZE = 46800;

    public LogoView(Context context) {
        super(context);
        initialize();
    }

    public LogoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize();
    }

    public LogoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    private void initialize() {
        setImageDrawable(ContextCompat.getDrawable(getContext(), R.drawable.ic_logo_mapbox));
        setContentDescription(getResources().getString(R.string.mapboxIconContentDescription));
        setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
    }

    @Override
    public  void setVisibility(int visibility) {
        super.setVisibility(View.VISIBLE);
    }

    @Override
    public void setImageDrawable(Drawable drawable) {
        if (ImageViewUtil.validateByteSize(getContext(), R.drawable.ic_logo_mapbox, MAPBOX_LOGO_SIZE)) {
            super.setImageDrawable(drawable);
        } else {
            throw new RuntimeException("Not possible to change the Logo of Mapbox");
        }
    }

    @Override
    public void setImageIcon(Icon icon) {
        //lockdown
    }

    @Override
    public void setImageURI(Uri uri) {
        //lockdown
    }

    @Override
    public void setImageResource(int resId) {
        //lockdown
    }
}
