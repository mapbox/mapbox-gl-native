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

  private Paint mPaint;
  private Path mPath;
  private int mLineWidth;

  public InfoWindowTipView(Context context, AttributeSet attrs) {
    super(context, attrs);

    mPath = new Path();
    mLineWidth = (int) context.getResources().getDimension(R.dimen.mapbox_infowindow_line_width);
    mPaint = new Paint();
    mPaint.setColor(Color.WHITE);
    mPaint.setAntiAlias(true);
    mPaint.setStrokeWidth(0.0f);
    mPaint.setStyle(Paint.Style.FILL);
  }

  @Override
  protected void onDraw(Canvas canvas) {
    super.onDraw(canvas);
    int height = getMeasuredHeight();
    int width = getMeasuredWidth();

    mPath.rewind();

    this.mPaint.setColor(Color.WHITE);
    this.mPaint.setAntiAlias(true);
    this.mPaint.setStrokeWidth(0.0f);
    this.mPaint.setStyle(Paint.Style.FILL);

    mPath.moveTo(0, 0);
    mPath.lineTo(width, 0);
    mPath.lineTo((width / 2), height);
    mPath.lineTo(0, 0);
    canvas.drawPath(mPath, this.mPaint);

    mPath.rewind();

    this.mPaint.setColor(Color.parseColor("#C2C2C2"));
    this.mPaint.setAntiAlias(true);
    this.mPaint.setStrokeWidth(mLineWidth);
    this.mPaint.setStyle(Paint.Style.STROKE);

    mPath.moveTo(0, 0);
    mPath.lineTo(width / 2, height);
    mPath.lineTo(width, 0);
    canvas.drawPath(mPath, this.mPaint);
  }
}
