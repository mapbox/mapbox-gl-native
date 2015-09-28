package com.mapbox.mapboxgl.views.widget;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.Icon;
import android.net.Uri;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxgl.views.R;

public class LogoView extends ImageView {

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
        super.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
    }

    @Override
    public void setVisibility(int visibility) {
        super.setVisibility(View.VISIBLE);
    }

    @Override
    public void setImageDrawable(Drawable drawable) {
        if (LogoValidator.validate(getContext(), R.drawable.ic_logo_mapbox)) {
            super.setImageDrawable(drawable);
        } else {
            throw new RuntimeException("Not possible to change the MapboxLogo");
        }
    }

    @Override
    public void setImageIcon(Icon icon) {
        //ignore
    }

    @Override
    public void setImageURI(Uri uri) {
        //ignore
    }

    @Override
    public void setImageResource(int resId) {
        //ignore
    }

    private static class LogoValidator {

        private static final int BYTE_COUNT = 46800;

        public static boolean validate(final Context context, final int resourceId) {
            BitmapFactory.Options opts = new BitmapFactory.Options();
            opts.inScaled = false;
            Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, opts);
            int byteCount = bitmap.getByteCount();
            return byteCount == BYTE_COUNT;
        }
    }
}
