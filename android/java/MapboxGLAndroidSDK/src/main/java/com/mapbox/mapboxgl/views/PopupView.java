package com.mapbox.mapboxgl.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.widget.FrameLayout;

/**
 * Created by erf on 8/9/15.
 */
public class PopupView extends FrameLayout {

    private int x;
    private int y;

    public PopupView(Context context) {
        super(context);
        init();
    }

    public PopupView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public PopupView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init(){
    }

    public void pos(int x, int y) {
        this.x = x;
        this.y = y;
        invalidate();
        requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);

        /*
        int w = getWidth();
        int h = getHeight();

        FrameLayout.LayoutParams layoutParams = new LayoutParams(w, h);
        layoutParams.setMargins(x, y, x+w, y+h);
        setLayoutParams(layoutParams);
        */

        // TODO requires API level 11, but i had an issue with updating position on the code above
        setTranslationX(x);
        setTranslationY(y);
    }
}
