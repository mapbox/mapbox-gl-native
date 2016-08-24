package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

import com.mapbox.mapboxsdk.R;

final class InfoWindowTipView extends View {

    private Paint paint;
    private Path path;
    private int lineWidth;

    public InfoWindowTipView(Context context, AttributeSet attrs) {
        super(context, attrs);

        path = new Path();
        lineWidth = (int) context.getResources().getDimension(R.dimen.infowindow_line_width);
        paint = new Paint();
        paint.setColor(Color.WHITE);
        paint.setAntiAlias(true);
        paint.setStrokeWidth(0.0f);
        paint.setStyle(Paint.Style.FILL);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int height = getMeasuredHeight();
        int width = getMeasuredWidth();

        path.rewind();

        this.paint.setColor(Color.WHITE);
        this.paint.setAntiAlias(true);
        this.paint.setStrokeWidth(0.0f);
        this.paint.setStyle(Paint.Style.FILL);

        path.moveTo(0, 0);
        path.lineTo(width, 0);
        path.lineTo((width / 2), height);
        path.lineTo(0, 0);
        canvas.drawPath(path, this.paint);

        path.rewind();

        this.paint.setColor(Color.parseColor("#C2C2C2"));
        this.paint.setAntiAlias(true);
        this.paint.setStrokeWidth(lineWidth);
        this.paint.setStyle(Paint.Style.STROKE);

        path.moveTo(0, 0);
        path.lineTo(width / 2, height);
        path.lineTo(width, 0);
        canvas.drawPath(path, this.paint);
    }
}
