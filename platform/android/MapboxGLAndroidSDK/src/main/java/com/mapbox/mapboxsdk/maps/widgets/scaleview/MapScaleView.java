package com.mapbox.mapboxsdk.maps.widgets.scaleview;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.Projection;


public class MapScaleView extends View {

    private final Paint paint;
    private final ViewConfig viewConfig;
    private final MapScaleModel mapScaleModel;

    private final float textHeight;
    private final float strokeWidth;
    private final float horizontalLineY;

    private Scale scale;

    public MapScaleView(Context context) {
        this(context, null);
    }

    public MapScaleView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MapScaleView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mapScaleModel = new MapScaleModel();

        viewConfig = new ViewConfig(context, attrs);

        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setTextSize(viewConfig.textSize);
        paint.setStrokeWidth(viewConfig.strokeWidth);
        paint.setColor(viewConfig.color);

        strokeWidth = viewConfig.strokeWidth;

        Rect textRect = new Rect();
        paint.getTextBounds("A", 0, 1, textRect);
        textHeight = textRect.height();

        horizontalLineY = textHeight + textHeight / 2;
    }

    public void update(Projection projection, CameraPosition cameraPosition) {
        scale = mapScaleModel.setProjection(projection, cameraPosition);
        invalidate();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = measureDimension(desiredWidth(), widthMeasureSpec);
        int height = measureDimension(desiredHeight(), heightMeasureSpec);

        mapScaleModel.setMaxWidth(width);

        setMeasuredDimension(width, height);
    }

    private int desiredWidth() {
        return viewConfig.desiredWidth;
    }

    private int desiredHeight() {
        return (int) (paint.getTextSize() * 1.5 + paint.getStrokeWidth());
    }

    private int measureDimension(int desiredSize, int measureSpec) {
        int mode = View.MeasureSpec.getMode(measureSpec);
        int size = View.MeasureSpec.getSize(measureSpec);

        if (mode == View.MeasureSpec.EXACTLY) {
            return size;
        } else if (mode == View.MeasureSpec.AT_MOST) {
            return Math.min(desiredSize, size);
        } else {
            return desiredSize;
        }
    }

    @Override
    public void onDraw(Canvas canvas) {
        canvas.save();

        drawView(canvas, paint);

        canvas.restore();
    }

    private void drawView(Canvas canvas, Paint paint) {
        if (scale == null) return;

        final float lineLength = scale.length();
        final String text = scale.text();

        canvas.drawText(text, 0, textHeight, paint);

        float verticalLineX = lineLength - strokeWidth / 2;

        canvas.drawLine(0, horizontalLineY, lineLength, horizontalLineY, paint);
        canvas.drawLine(verticalLineX, horizontalLineY, verticalLineX, textHeight, paint);
    }
}
