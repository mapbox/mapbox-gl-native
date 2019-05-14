package com.mapbox.mapboxsdk.annotations;

import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import androidx.annotation.NonNull;

@Deprecated
class Bubble extends Drawable {

  private RectF rect;
  private float arrowWidth;
  private float arrowHeight;
  private float arrowPosition;
  private float cornersRadius;
  @NonNull
  private Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
  private float strokeWidth;
  private Paint strokePaint;
  private Path strokePath;
  @NonNull
  private Path path = new Path();

  Bubble(@NonNull RectF rect, @NonNull ArrowDirection arrowDirection, float arrowWidth, float arrowHeight,
         float arrowPosition, float cornersRadius, int bubbleColor, float strokeWidth, int strokeColor) {
    this.rect = rect;
    this.arrowWidth = arrowWidth;
    this.arrowHeight = arrowHeight;
    this.arrowPosition = arrowPosition;
    this.cornersRadius = cornersRadius;
    paint.setColor(bubbleColor);
    this.strokeWidth = strokeWidth;

    if (strokeWidth > 0) {
      strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
      strokePaint.setColor(strokeColor);
      strokePath = new Path();
      initPath(arrowDirection, path, strokeWidth);
      initPath(arrowDirection, strokePath, 0);
    } else {
      initPath(arrowDirection, path, 0);
    }
  }

  @Override
  protected void onBoundsChange(Rect bounds) {
    super.onBoundsChange(bounds);
  }

  @Override
  public void draw(@NonNull Canvas canvas) {
    if (strokeWidth > 0) {
      canvas.drawPath(strokePath, strokePaint);
    }
    canvas.drawPath(path, paint);
  }

  @Override
  public int getOpacity() {
    return PixelFormat.TRANSLUCENT;
  }

  @Override
  public void setAlpha(int alpha) {
    paint.setAlpha(alpha);
  }

  @Override
  public void setColorFilter(ColorFilter cf) {
    paint.setColorFilter(cf);
  }

  @Override
  public int getIntrinsicWidth() {
    return (int) rect.width();
  }

  @Override
  public int getIntrinsicHeight() {
    return (int) rect.height();
  }

  private void initPath(@NonNull ArrowDirection arrowDirection, @NonNull Path path, float strokeWidth) {
    switch (arrowDirection.getValue()) {
      case ArrowDirection.LEFT:
        if (cornersRadius <= 0) {
          initLeftSquarePath(rect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > cornersRadius) {
          initLeftSquarePath(rect, path, strokeWidth);
          break;
        }

        initLeftRoundedPath(rect, path, strokeWidth);
        break;
      case ArrowDirection.TOP:
        if (cornersRadius <= 0) {
          initTopSquarePath(rect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > cornersRadius) {
          initTopSquarePath(rect, path, strokeWidth);
          break;
        }

        initTopRoundedPath(rect, path, strokeWidth);
        break;
      case ArrowDirection.RIGHT:
        if (cornersRadius <= 0) {
          initRightSquarePath(rect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > cornersRadius) {
          initRightSquarePath(rect, path, strokeWidth);
          break;
        }

        initRightRoundedPath(rect, path, strokeWidth);
        break;
      case ArrowDirection.BOTTOM:
        if (cornersRadius <= 0) {
          initBottomSquarePath(rect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > cornersRadius) {
          initBottomSquarePath(rect, path, strokeWidth);
          break;
        }

        initBottomRoundedPath(rect, path, strokeWidth);
        break;
    }
  }

  private void initLeftSquarePath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(arrowWidth + rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - strokeWidth, rect.top + strokeWidth);

    path.lineTo(rect.right - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + arrowWidth + strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + arrowWidth + strokeWidth, arrowHeight + arrowPosition - (strokeWidth / 2));
    path.lineTo(rect.left + strokeWidth + strokeWidth, arrowPosition + arrowHeight / 2);
    path.lineTo(rect.left + arrowWidth + strokeWidth, arrowPosition + (strokeWidth / 2));

    path.lineTo(rect.left + arrowWidth + strokeWidth, rect.top + strokeWidth);

    path.close();
  }

  private void initLeftRoundedPath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(arrowWidth + rect.left + cornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - cornersRadius - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - cornersRadius, rect.top + strokeWidth, rect.right - strokeWidth,
      cornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - strokeWidth, rect.bottom - cornersRadius - strokeWidth);
    path.arcTo(new RectF(rect.right - cornersRadius, rect.bottom - cornersRadius,
      rect.right - strokeWidth, rect.bottom - strokeWidth), 0, 90);

    path.lineTo(rect.left + arrowWidth + cornersRadius + strokeWidth, rect.bottom - strokeWidth);

    path.arcTo(new RectF(rect.left + arrowWidth + strokeWidth, rect.bottom - cornersRadius,
      cornersRadius + rect.left + arrowWidth, rect.bottom - strokeWidth), 90, 90);

    path.lineTo(rect.left + arrowWidth + strokeWidth, arrowHeight + arrowPosition - (strokeWidth / 2));

    path.lineTo(rect.left + strokeWidth + strokeWidth, arrowPosition + arrowHeight / 2);

    path.lineTo(rect.left + arrowWidth + strokeWidth, arrowPosition + (strokeWidth / 2));

    path.lineTo(rect.left + arrowWidth + strokeWidth, rect.top + cornersRadius + strokeWidth);

    path.arcTo(new RectF(rect.left + arrowWidth + strokeWidth, rect.top + strokeWidth, cornersRadius
      + rect.left + arrowWidth, cornersRadius + rect.top), 180, 90);

    path.close();
  }

  private void initTopSquarePath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + arrowPosition + strokeWidth, rect.top + arrowHeight + strokeWidth);

    path.lineTo(rect.left + arrowPosition + (strokeWidth / 2), rect.top + arrowHeight + strokeWidth);
    path.lineTo(rect.left + arrowWidth / 2 + arrowPosition, rect.top + strokeWidth + strokeWidth);
    path.lineTo(rect.left + arrowWidth + arrowPosition - (strokeWidth / 2), rect.top + arrowHeight + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.top + arrowHeight + strokeWidth);

    path.lineTo(rect.right - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.top + arrowHeight + strokeWidth);

    path.lineTo(rect.left + arrowPosition + strokeWidth, rect.top + arrowHeight + strokeWidth);

    path.close();
  }

  private void initTopRoundedPath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + Math.min(arrowPosition, cornersRadius) + strokeWidth, rect.top + arrowHeight
      + strokeWidth);
    path.lineTo(rect.left + arrowPosition + (strokeWidth / 2), rect.top + arrowHeight + strokeWidth);
    path.lineTo(rect.left + arrowWidth / 2 + arrowPosition, rect.top + strokeWidth + strokeWidth);
    path.lineTo(rect.left + arrowWidth + arrowPosition - (strokeWidth / 2), rect.top + arrowHeight + strokeWidth);
    path.lineTo(rect.right - cornersRadius - strokeWidth, rect.top + arrowHeight + strokeWidth);

    path.arcTo(new RectF(rect.right - cornersRadius,
        rect.top + arrowHeight + strokeWidth, rect.right - strokeWidth, cornersRadius + rect.top + arrowHeight),
      270, 90);
    path.lineTo(rect.right - strokeWidth, rect.bottom - cornersRadius - strokeWidth);

    path.arcTo(new RectF(rect.right - cornersRadius, rect.bottom - cornersRadius,
      rect.right - strokeWidth, rect.bottom - strokeWidth), 0, 90);
    path.lineTo(rect.left + cornersRadius + strokeWidth, rect.bottom - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - cornersRadius,
      cornersRadius + rect.left, rect.bottom - strokeWidth), 90, 90);

    path.lineTo(rect.left + strokeWidth, rect.top + arrowHeight + cornersRadius + strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + arrowHeight + strokeWidth, cornersRadius
      + rect.left, cornersRadius + rect.top + arrowHeight), 180, 90);

    path.close();
  }

  private void initRightSquarePath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - arrowWidth - strokeWidth, rect.top + strokeWidth);

    path.lineTo(rect.right - arrowWidth - strokeWidth, arrowPosition + (strokeWidth / 2));
    path.lineTo(rect.right - strokeWidth - strokeWidth, arrowPosition + arrowHeight / 2);
    path.lineTo(rect.right - arrowWidth - strokeWidth, arrowPosition + arrowHeight - (strokeWidth / 2));

    path.lineTo(rect.right - arrowWidth - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.bottom - strokeWidth);
    path.lineTo(rect.left + strokeWidth, rect.top + strokeWidth);

    path.close();
  }

  private void initRightRoundedPath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + cornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - cornersRadius - arrowWidth - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - cornersRadius - arrowWidth,
      rect.top + strokeWidth, rect.right - arrowWidth - strokeWidth, cornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - arrowWidth - strokeWidth, arrowPosition + (strokeWidth / 2));
    path.lineTo(rect.right - strokeWidth - strokeWidth, arrowPosition + arrowHeight / 2);
    path.lineTo(rect.right - arrowWidth - strokeWidth, arrowPosition + arrowHeight - (strokeWidth / 2));
    path.lineTo(rect.right - arrowWidth - strokeWidth, rect.bottom - cornersRadius - strokeWidth);

    path.arcTo(new RectF(rect.right - cornersRadius - arrowWidth, rect.bottom - cornersRadius,
      rect.right - arrowWidth - strokeWidth, rect.bottom - strokeWidth), 0, 90);
    path.lineTo(rect.left + arrowWidth + strokeWidth, rect.bottom - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - cornersRadius,
      cornersRadius + rect.left, rect.bottom - strokeWidth), 90, 90);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + strokeWidth, cornersRadius
      + rect.left, cornersRadius + rect.top), 180, 90);
    path.close();
  }

  private void initBottomSquarePath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.bottom - arrowHeight - strokeWidth);

    path.lineTo(rect.left + arrowWidth + arrowPosition - (strokeWidth / 2), rect.bottom - arrowHeight - strokeWidth);
    path.lineTo(rect.left + arrowPosition + arrowWidth / 2, rect.bottom - strokeWidth - strokeWidth);
    path.lineTo(rect.left + arrowPosition + (strokeWidth / 2), rect.bottom - arrowHeight - strokeWidth);
    path.lineTo(rect.left + arrowPosition + strokeWidth, rect.bottom - arrowHeight - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.bottom - arrowHeight - strokeWidth);
    path.lineTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.close();
  }

  private void initBottomRoundedPath(@NonNull RectF rect, @NonNull Path path, float strokeWidth) {
    path.moveTo(rect.left + cornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - cornersRadius - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - cornersRadius,
      rect.top + strokeWidth, rect.right - strokeWidth, cornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - strokeWidth, rect.bottom - arrowHeight - cornersRadius - strokeWidth);
    path.arcTo(new RectF(rect.right - cornersRadius, rect.bottom - cornersRadius - arrowHeight,
      rect.right - strokeWidth, rect.bottom - arrowHeight - strokeWidth), 0, 90);

    path.lineTo(rect.left + arrowWidth + arrowPosition - (strokeWidth / 2), rect.bottom - arrowHeight - strokeWidth);
    path.lineTo(rect.left + arrowPosition + arrowWidth / 2, rect.bottom - strokeWidth - strokeWidth);
    path.lineTo(rect.left + arrowPosition + (strokeWidth / 2), rect.bottom - arrowHeight - strokeWidth);
    path.lineTo(rect.left + Math.min(cornersRadius, arrowPosition) + strokeWidth, rect.bottom - arrowHeight
      - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - cornersRadius - arrowHeight,
      cornersRadius + rect.left, rect.bottom - arrowHeight - strokeWidth), 90, 90);
    path.lineTo(rect.left + strokeWidth, rect.top + cornersRadius + strokeWidth);
    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + strokeWidth, cornersRadius
      + rect.left, cornersRadius + rect.top), 180, 90);
    path.close();
  }
}