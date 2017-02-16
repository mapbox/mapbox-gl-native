package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.widget.LinearLayout;

import com.mapbox.mapboxsdk.R;

/**
 * Bubble View for Android with custom stroke width and color, arrow size, position and direction.
 */
public class BubbleLayout extends LinearLayout {

  public static final float DEFAULT_STROKE_WIDTH = -1;

  private ArrowDirection mArrowDirection;
  private Bubble mBubble;

  private float mArrowWidth;
  private float mCornersRadius;
  private float mArrowHeight;
  private float mArrowPosition;
  private int mBubbleColor;
  private float mStrokeWidth;
  private int mStrokeColor;

  public BubbleLayout(Context context) {
    this(context, null, 0);
  }

  public BubbleLayout(Context context, AttributeSet attrs) {
    this(context, attrs, 0);
  }

  public BubbleLayout(Context context, AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);

    TypedArray a = getContext().obtainStyledAttributes(attrs, R.styleable.mapbox_BubbleLayout);
    mArrowWidth = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowWidth,
      convertDpToPixel(8, context));
    mArrowHeight = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowHeight,
      convertDpToPixel(8, context));
    mCornersRadius = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_cornersRadius, 0);
    mArrowPosition = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowPosition,
      convertDpToPixel(12, context));
    mBubbleColor = a.getColor(R.styleable.mapbox_BubbleLayout_mapbox_bl_bubbleColor, Color.WHITE);

    mStrokeWidth =
      a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_strokeWidth, DEFAULT_STROKE_WIDTH);
    mStrokeColor = a.getColor(R.styleable.mapbox_BubbleLayout_mapbox_bl_strokeColor, Color.GRAY);

    int location = a.getInt(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowDirection, ArrowDirection.LEFT.getValue());
    mArrowDirection = ArrowDirection.fromInt(location);

    a.recycle();
    initPadding();
  }

  @Override
  protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
    super.onLayout(changed, left, top, right, bottom);
    initDrawable(0, getWidth(), 0, getHeight());
  }

  @Override
  protected void dispatchDraw(Canvas canvas) {
    if (mBubble != null) {
      mBubble.draw(canvas);
    }
    super.dispatchDraw(canvas);
  }

  private void initDrawable(int left, int right, int top, int bottom) {
    if (right < left || bottom < top) {
      return;
    }

    RectF rectF = new RectF(left, top, right, bottom);
    mBubble = new Bubble(rectF, mArrowWidth, mCornersRadius, mArrowHeight, mArrowPosition,
      mStrokeWidth, mStrokeColor, mBubbleColor, mArrowDirection);
  }

  private void initPadding() {
    int paddingLeft = getPaddingLeft();
    int paddingRight = getPaddingRight();
    int paddingTop = getPaddingTop();
    int paddingBottom = getPaddingBottom();
    switch (mArrowDirection) {
      case LEFT:
        paddingLeft += mArrowWidth;
        break;
      case RIGHT:
        paddingRight += mArrowWidth;
        break;
      case TOP:
        paddingTop += mArrowHeight;
        break;
      case BOTTOM:
        paddingBottom += mArrowHeight;
        break;
    }
    if (mStrokeWidth > 0) {
      paddingLeft += mStrokeWidth;
      paddingRight += mStrokeWidth;
      paddingTop += mStrokeWidth;
      paddingBottom += mStrokeWidth;
    }
    setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  private void resetPadding() {
    int paddingLeft = getPaddingLeft();
    int paddingRight = getPaddingRight();
    int paddingTop = getPaddingTop();
    int paddingBottom = getPaddingBottom();
    switch (mArrowDirection) {
      case LEFT:
        paddingLeft -= mArrowWidth;
        break;
      case RIGHT:
        paddingRight -= mArrowWidth;
        break;
      case TOP:
        paddingTop -= mArrowHeight;
        break;
      case BOTTOM:
        paddingBottom -= mArrowHeight;
        break;
    }
    if (mStrokeWidth > 0) {
      paddingLeft -= mStrokeWidth;
      paddingRight -= mStrokeWidth;
      paddingTop -= mStrokeWidth;
      paddingBottom -= mStrokeWidth;
    }
    setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  static float convertDpToPixel(float dp, Context context) {
    DisplayMetrics metrics = context.getResources().getDisplayMetrics();
    return dp * (metrics.densityDpi / DisplayMetrics.DENSITY_DEFAULT);
  }

  public BubbleLayout setArrowDirection(ArrowDirection arrowDirection) {
    resetPadding();
    mArrowDirection = arrowDirection;
    initPadding();
    return this;
  }

  public BubbleLayout setArrowWidth(float arrowWidth) {
    resetPadding();
    mArrowWidth = arrowWidth;
    initPadding();
    return this;
  }

  public BubbleLayout setCornersRadius(float cornersRadius) {
    mCornersRadius = cornersRadius;
    requestLayout();
    return this;
  }

  public BubbleLayout setArrowHeight(float arrowHeight) {
    resetPadding();
    mArrowHeight = arrowHeight;
    initPadding();
    return this;
  }

  public BubbleLayout setArrowPosition(float arrowPosition) {
    resetPadding();
    mArrowPosition = arrowPosition;
    initPadding();
    return this;
  }

  public BubbleLayout setBubbleColor(int bubbleColor) {
    mBubbleColor = bubbleColor;
    requestLayout();
    return this;
  }

  public BubbleLayout setStrokeWidth(float strokeWidth) {
    resetPadding();
    mStrokeWidth = strokeWidth;
    initPadding();
    return this;
  }

  public BubbleLayout setStrokeColor(int strokeColor) {
    mStrokeColor = strokeColor;
    requestLayout();
    return this;
  }

  public ArrowDirection getArrowDirection() {
    return mArrowDirection;
  }

  public float getArrowWidth() {
    return mArrowWidth;
  }

  public float getCornersRadius() {
    return mCornersRadius;
  }

  public float getArrowHeight() {
    return mArrowHeight;
  }

  public float getArrowPosition() {
    return mArrowPosition;
  }

  public int getBubbleColor() {
    return mBubbleColor;
  }

  public float getStrokeWidth() {
    return mStrokeWidth;
  }

  public int getStrokeColor() {
    return mStrokeColor;
  }
}
