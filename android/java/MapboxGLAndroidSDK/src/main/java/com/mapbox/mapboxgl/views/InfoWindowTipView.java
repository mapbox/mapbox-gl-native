package com.mapbox.mapboxgl.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

public class InfoWindowTipView extends View {

    private Paint mPaint;
    private Path mPath;

    public InfoWindowTipView(Context context, AttributeSet attrs) {
        super(context, attrs);

        mPath = new Path();

        this.mPaint = new Paint();
        this.mPaint.setColor(Color.WHITE);
        this.mPaint.setAntiAlias(true);
        this.mPaint.setStrokeWidth(0.0f);
        this.mPaint.setStyle(Paint.Style.FILL);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        int height = getMeasuredHeight();
        int width = getMeasuredWidth();

        mPath.rewind();
        mPath.moveTo((width / 2) - height, 0);
        mPath.lineTo((width / 2) + height, 0);
        mPath.lineTo((width / 2), height);
        mPath.lineTo((width / 2) - height, 0);
        canvas.drawPath(mPath, this.mPaint);
    }
}
