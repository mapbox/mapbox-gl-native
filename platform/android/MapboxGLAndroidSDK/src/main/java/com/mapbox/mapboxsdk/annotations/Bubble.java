package com.mapbox.mapboxsdk.annotations;

import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;

class Bubble extends Drawable {

  private RectF mRect;
  private Path mPath = new Path();
  private Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
  private Path mStrokePath;
  private Paint mStrokePaint;
  private float mArrowWidth;
  private float mCornersRadius;
  private float mArrowHeight;
  private float mArrowPosition;
  private float mStrokeWidth;

  public Bubble(RectF rect, float arrowWidth, float cornersRadius, float arrowHeight, float arrowPosition, float
    strokeWidth, int strokeColor, int bubbleColor, ArrowDirection arrowDirection) {
    this.mRect = rect;

    this.mArrowWidth = arrowWidth;
    this.mCornersRadius = cornersRadius;
    this.mArrowHeight = arrowHeight;
    this.mArrowPosition = arrowPosition;
    this.mStrokeWidth = strokeWidth;

    mPaint.setColor(bubbleColor);

    if (strokeWidth > 0) {
      mStrokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
      mStrokePaint.setColor(strokeColor);
      mStrokePath = new Path();

      initPath(arrowDirection, mPath, strokeWidth);
      initPath(arrowDirection, mStrokePath, 0);
    } else {
      initPath(arrowDirection, mPath, 0);
    }
  }

  @Override
  protected void onBoundsChange(Rect bounds) {
    super.onBoundsChange(bounds);
  }

  @Override
  public void draw(Canvas canvas) {
    if (mStrokeWidth > 0) {
      canvas.drawPath(mStrokePath, mStrokePaint);
    }
    canvas.drawPath(mPath, mPaint);
  }

  @Override
  public int getOpacity() {
    return PixelFormat.TRANSLUCENT;
  }

  @Override
  public void setAlpha(int alpha) {
    mPaint.setAlpha(alpha);
  }

  @Override
  public void setColorFilter(ColorFilter cf) {
    mPaint.setColorFilter(cf);
  }

  @Override
  public int getIntrinsicWidth() {
    return (int) mRect.width();
  }

  @Override
  public int getIntrinsicHeight() {
    return (int) mRect.height();
  }

  private void initPath(ArrowDirection arrowDirection, Path path, float strokeWidth) {
    switch (arrowDirection) {
      case LEFT:
        if (mCornersRadius <= 0) {
          initLeftSquarePath(mRect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > mCornersRadius) {
          initLeftSquarePath(mRect, path, strokeWidth);
          break;
        }

        initLeftRoundedPath(mRect, path, strokeWidth);
        break;
      case TOP:
        if (mCornersRadius <= 0) {
          initTopSquarePath(mRect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > mCornersRadius) {
          initTopSquarePath(mRect, path, strokeWidth);
          break;
        }

        initTopRoundedPath(mRect, path, strokeWidth);

        break;
      case RIGHT:
        if (mCornersRadius <= 0) {
          initRightSquarePath(mRect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > mCornersRadius) {
          initRightSquarePath(mRect, path, strokeWidth);
          break;
        }

        initRightRoundedPath(mRect, path, strokeWidth);

        break;
      case BOTTOM:
        if (mCornersRadius <= 0) {
          initBottomSquarePath(mRect, path, strokeWidth);
          break;
        }

        if (strokeWidth > 0 && strokeWidth > mCornersRadius) {
          initBottomSquarePath(mRect, path, strokeWidth);
          break;
        }

        initBottomRoundedPath(mRect, path, strokeWidth);
        break;
    }
  }

  private void initLeftRoundedPath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(mArrowWidth + rect.left + mCornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - mCornersRadius - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - mCornersRadius, rect.top + strokeWidth, rect.right - strokeWidth,
      mCornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - strokeWidth, rect.bottom - mCornersRadius - strokeWidth);
    path.arcTo(new RectF(rect.right - mCornersRadius, rect.bottom - mCornersRadius,
      rect.right - strokeWidth, rect.bottom - strokeWidth), 0, 90);


    path.lineTo(rect.left + mArrowWidth + mCornersRadius + strokeWidth, rect.bottom - strokeWidth);


    path.arcTo(new RectF(rect.left + mArrowWidth + strokeWidth, rect.bottom - mCornersRadius,
      mCornersRadius + rect.left + mArrowWidth, rect.bottom - strokeWidth), 90, 90);

    path.lineTo(rect.left + mArrowWidth + strokeWidth, mArrowHeight + mArrowPosition - (strokeWidth / 2));

    path.lineTo(rect.left + strokeWidth + strokeWidth, mArrowPosition + mArrowHeight / 2);


    path.lineTo(rect.left + mArrowWidth + strokeWidth, mArrowPosition + (strokeWidth / 2));

    path.lineTo(rect.left + mArrowWidth + strokeWidth, rect.top + mCornersRadius + strokeWidth);

    path.arcTo(new RectF(rect.left + mArrowWidth + strokeWidth, rect.top + strokeWidth, mCornersRadius
      + rect.left + mArrowWidth, mCornersRadius + rect.top), 180, 90);

    path.close();
  }

  private void initLeftSquarePath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(mArrowWidth + rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - strokeWidth, rect.top + strokeWidth);

    path.lineTo(rect.right - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + mArrowWidth + strokeWidth, rect.bottom - strokeWidth);


    path.lineTo(rect.left + mArrowWidth + strokeWidth, mArrowHeight + mArrowPosition - (strokeWidth / 2));
    path.lineTo(rect.left + strokeWidth + strokeWidth, mArrowPosition + mArrowHeight / 2);
    path.lineTo(rect.left + mArrowWidth + strokeWidth, mArrowPosition + (strokeWidth / 2));

    path.lineTo(rect.left + mArrowWidth + strokeWidth, rect.top + strokeWidth);


    path.close();
  }


  private void initTopRoundedPath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + Math.min(mArrowPosition, mCornersRadius) + strokeWidth, rect.top + mArrowHeight
      + strokeWidth);
    path.lineTo(rect.left + mArrowPosition + (strokeWidth / 2), rect.top + mArrowHeight + strokeWidth);
    path.lineTo(rect.left + mArrowWidth / 2 + mArrowPosition, rect.top + strokeWidth + strokeWidth);
    path.lineTo(rect.left + mArrowWidth + mArrowPosition - (strokeWidth / 2), rect.top + mArrowHeight + strokeWidth);
    path.lineTo(rect.right - mCornersRadius - strokeWidth, rect.top + mArrowHeight + strokeWidth);

    path.arcTo(new RectF(rect.right - mCornersRadius,
        rect.top + mArrowHeight + strokeWidth, rect.right - strokeWidth, mCornersRadius + rect.top + mArrowHeight),
      270, 90);
    path.lineTo(rect.right - strokeWidth, rect.bottom - mCornersRadius - strokeWidth);

    path.arcTo(new RectF(rect.right - mCornersRadius, rect.bottom - mCornersRadius,
      rect.right - strokeWidth, rect.bottom - strokeWidth), 0, 90);
    path.lineTo(rect.left + mCornersRadius + strokeWidth, rect.bottom - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - mCornersRadius,
      mCornersRadius + rect.left, rect.bottom - strokeWidth), 90, 90);

    path.lineTo(rect.left + strokeWidth, rect.top + mArrowHeight + mCornersRadius + strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + mArrowHeight + strokeWidth, mCornersRadius
      + rect.left, mCornersRadius + rect.top + mArrowHeight), 180, 90);

    path.close();
  }

  private void initTopSquarePath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + mArrowPosition + strokeWidth, rect.top + mArrowHeight + strokeWidth);

    path.lineTo(rect.left + mArrowPosition + (strokeWidth / 2), rect.top + mArrowHeight + strokeWidth);
    path.lineTo(rect.left + mArrowWidth / 2 + mArrowPosition, rect.top + strokeWidth + strokeWidth);
    path.lineTo(rect.left + mArrowWidth + mArrowPosition - (strokeWidth / 2), rect.top + mArrowHeight + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.top + mArrowHeight + strokeWidth);

    path.lineTo(rect.right - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.bottom - strokeWidth);


    path.lineTo(rect.left + strokeWidth, rect.top + mArrowHeight + strokeWidth);

    path.lineTo(rect.left + mArrowPosition + strokeWidth, rect.top + mArrowHeight + strokeWidth);


    path.close();
  }


  private void initRightRoundedPath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + mCornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - mCornersRadius - mArrowWidth - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - mCornersRadius - mArrowWidth,
      rect.top + strokeWidth, rect.right - mArrowWidth - strokeWidth, mCornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - mArrowWidth - strokeWidth, mArrowPosition + (strokeWidth / 2));
    path.lineTo(rect.right - strokeWidth - strokeWidth, mArrowPosition + mArrowHeight / 2);
    path.lineTo(rect.right - mArrowWidth - strokeWidth, mArrowPosition + mArrowHeight - (strokeWidth / 2));
    path.lineTo(rect.right - mArrowWidth - strokeWidth, rect.bottom - mCornersRadius - strokeWidth);

    path.arcTo(new RectF(rect.right - mCornersRadius - mArrowWidth, rect.bottom - mCornersRadius,
      rect.right - mArrowWidth - strokeWidth, rect.bottom - strokeWidth), 0, 90);
    path.lineTo(rect.left + mArrowWidth + strokeWidth, rect.bottom - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - mCornersRadius,
      mCornersRadius + rect.left, rect.bottom - strokeWidth), 90, 90);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + strokeWidth, mCornersRadius
      + rect.left, mCornersRadius + rect.top), 180, 90);
    path.close();
  }

  private void initRightSquarePath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - mArrowWidth - strokeWidth, rect.top + strokeWidth);

    path.lineTo(rect.right - mArrowWidth - strokeWidth, mArrowPosition + (strokeWidth / 2));
    path.lineTo(rect.right - strokeWidth - strokeWidth, mArrowPosition + mArrowHeight / 2);
    path.lineTo(rect.right - mArrowWidth - strokeWidth, mArrowPosition + mArrowHeight - (strokeWidth / 2));

    path.lineTo(rect.right - mArrowWidth - strokeWidth, rect.bottom - strokeWidth);

    path.lineTo(rect.left + strokeWidth, rect.bottom - strokeWidth);
    path.lineTo(rect.left + strokeWidth, rect.top + strokeWidth);

    path.close();
  }


  private void initBottomRoundedPath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + mCornersRadius + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.width() - mCornersRadius - strokeWidth, rect.top + strokeWidth);
    path.arcTo(new RectF(rect.right - mCornersRadius,
      rect.top + strokeWidth, rect.right - strokeWidth, mCornersRadius + rect.top), 270, 90);

    path.lineTo(rect.right - strokeWidth, rect.bottom - mArrowHeight - mCornersRadius - strokeWidth);
    path.arcTo(new RectF(rect.right - mCornersRadius, rect.bottom - mCornersRadius - mArrowHeight,
      rect.right - strokeWidth, rect.bottom - mArrowHeight - strokeWidth), 0, 90);

    path.lineTo(rect.left + mArrowWidth + mArrowPosition - (strokeWidth / 2), rect.bottom - mArrowHeight - strokeWidth);
    path.lineTo(rect.left + mArrowPosition + mArrowWidth / 2, rect.bottom - strokeWidth - strokeWidth);
    path.lineTo(rect.left + mArrowPosition + (strokeWidth / 2), rect.bottom - mArrowHeight - strokeWidth);
    path.lineTo(rect.left + Math.min(mCornersRadius, mArrowPosition) + strokeWidth, rect.bottom - mArrowHeight
      - strokeWidth);

    path.arcTo(new RectF(rect.left + strokeWidth, rect.bottom - mCornersRadius - mArrowHeight,
      mCornersRadius + rect.left, rect.bottom - mArrowHeight - strokeWidth), 90, 90);
    path.lineTo(rect.left + strokeWidth, rect.top + mCornersRadius + strokeWidth);
    path.arcTo(new RectF(rect.left + strokeWidth, rect.top + strokeWidth, mCornersRadius
      + rect.left, mCornersRadius + rect.top), 180, 90);
    path.close();
  }

  private void initBottomSquarePath(RectF rect, Path path, float strokeWidth) {
    path.moveTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.top + strokeWidth);
    path.lineTo(rect.right - strokeWidth, rect.bottom - mArrowHeight - strokeWidth);


    path.lineTo(rect.left + mArrowWidth + mArrowPosition - (strokeWidth / 2), rect.bottom - mArrowHeight - strokeWidth);
    path.lineTo(rect.left + mArrowPosition + mArrowWidth / 2, rect.bottom - strokeWidth - strokeWidth);
    path.lineTo(rect.left + mArrowPosition + (strokeWidth / 2), rect.bottom - mArrowHeight - strokeWidth);
    path.lineTo(rect.left + mArrowPosition + strokeWidth, rect.bottom - mArrowHeight - strokeWidth);


    path.lineTo(rect.left + strokeWidth, rect.bottom - mArrowHeight - strokeWidth);
    path.lineTo(rect.left + strokeWidth, rect.top + strokeWidth);
    path.close();
  }
}