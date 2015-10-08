package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.RelativeLayout;

import com.mapbox.mapboxsdk.R;

class InfoWindowView extends RelativeLayout {

    private InfoWindowTipView mTipView;

    public InfoWindowView(Context context) {
        this(context, null);
    }

    public InfoWindowView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public InfoWindowView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context);
    }

    private void initialize(Context context) {
        LayoutInflater.from(context).inflate(R.layout.infowindow_content, this);
        mTipView = (InfoWindowTipView) findViewById(R.id.infowindow_tipview);
    }

    void setTipViewMarginLeft(int marginLeft) {
        RelativeLayout.LayoutParams layoutParams = (RelativeLayout.LayoutParams) mTipView.getLayoutParams();
        layoutParams.leftMargin = marginLeft;
        // This is a bit of a hack but prevents an occasional 1 pixel gap between the InfoWindow and
        // the tip
        layoutParams.topMargin = -1;
    }
}
