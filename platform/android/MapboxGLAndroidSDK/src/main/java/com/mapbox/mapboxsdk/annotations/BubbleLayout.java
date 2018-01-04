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
  private ArrowDirection arrowDirection;
  private float arrowWidth;
  private float arrowHeight;
  private float arrowPosition;
  private float cornersRadius;
  private Bubble bubble;
  private int bubbleColor;
  private float strokeWidth;
  private int strokeColor;

  /**
   * Creates an instance of bubble layout.
   *
   * @param context The context used to inflate this bubble layout
   */
  public BubbleLayout(Context context) {
    this(context, null, 0);
  }

  /**
   * Creates an instance of bubble layout.
   *
   * @param context The context used to inflate this bubble layout
   * @param attrs   The attribute set to initialise this bubble layout from
   */
  public BubbleLayout(Context context, AttributeSet attrs) {
    this(context, attrs, 0);
  }

  /**
   * Creates an instance of bubble layout.
   *
   * @param context      The context used to inflate this bubble layout
   * @param attrs        The attribute set to initialise this bubble layout from
   * @param defStyleAttr The default style to apply this bubble layout with
   */
  public BubbleLayout(Context context, AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);

    TypedArray a = getContext().obtainStyledAttributes(attrs, R.styleable.mapbox_BubbleLayout);
    @ArrowDirection.Value
    int location = a.getInt(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowDirection,
      ArrowDirection.LEFT);
    arrowDirection = new ArrowDirection(location);
    arrowWidth = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowWidth,
      convertDpToPixel(8, context));
    arrowHeight = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowHeight,
      convertDpToPixel(8, context));
    arrowPosition = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_arrowPosition,
      convertDpToPixel(12, context));
    cornersRadius = a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_cornersRadius, 0);
    bubbleColor = a.getColor(R.styleable.mapbox_BubbleLayout_mapbox_bl_bubbleColor, Color.WHITE);
    strokeWidth =
      a.getDimension(R.styleable.mapbox_BubbleLayout_mapbox_bl_strokeWidth, DEFAULT_STROKE_WIDTH);
    strokeColor = a.getColor(R.styleable.mapbox_BubbleLayout_mapbox_bl_strokeColor, Color.GRAY);

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
    if (bubble != null) {
      bubble.draw(canvas);
    }
    super.dispatchDraw(canvas);
  }

  static float convertDpToPixel(float dp, Context context) {
    DisplayMetrics metrics = context.getResources().getDisplayMetrics();
    return dp * (metrics.densityDpi / DisplayMetrics.DENSITY_DEFAULT);
  }

  /**
   * Get the arrow direction.
   *
   * @return the arrow direction
   */
  public ArrowDirection getArrowDirection() {
    return arrowDirection;
  }

  /**
   * Set the arrow direction.
   *
   * @param arrowDirection The direction of the arrow
   * @return this
   */
  public BubbleLayout setArrowDirection(ArrowDirection arrowDirection) {
    resetPadding();
    this.arrowDirection = arrowDirection;
    initPadding();
    return this;
  }

  /**
   * Get the arrow width.
   *
   * @return the width of the arrow
   */
  public float getArrowWidth() {
    return arrowWidth;
  }

  /**
   * Set the arrow width.
   *
   * @param arrowWidth The width of the arrow
   * @return this
   */
  public BubbleLayout setArrowWidth(float arrowWidth) {
    resetPadding();
    this.arrowWidth = arrowWidth;
    initPadding();
    return this;
  }

  /**
   * Get the arrow height
   *
   * @return the height of the arrow
   */
  public float getArrowHeight() {
    return arrowHeight;
  }

  /**
   * Set the arrow height.
   *
   * @param arrowHeight The height of the arrow
   * @return this
   */
  public BubbleLayout setArrowHeight(float arrowHeight) {
    resetPadding();
    this.arrowHeight = arrowHeight;
    initPadding();
    return this;
  }

  /**
   * Get the arrow position.
   *
   * @return the arrow position
   */
  public float getArrowPosition() {
    return arrowPosition;
  }

  /**
   * Get the arrow position.
   *
   * @param arrowPosition The arrow position
   * @return this
   */
  public BubbleLayout setArrowPosition(float arrowPosition) {
    resetPadding();
    this.arrowPosition = arrowPosition;
    initPadding();
    return this;
  }

  /**
   * Get the corner radius
   *
   * @return the corner radius
   */
  public float getCornersRadius() {
    return cornersRadius;
  }

  /**
   * Set the corner radius
   *
   * @param cornersRadius The corner radius
   * @return this
   */
  public BubbleLayout setCornersRadius(float cornersRadius) {
    this.cornersRadius = cornersRadius;
    requestLayout();
    return this;
  }

  /**
   * Get the bubble color.
   *
   * @return the bubble color
   */
  public int getBubbleColor() {
    return bubbleColor;
  }

  /**
   * Set the bubble color.
   *
   * @param bubbleColor The buble color
   * @return this
   */
  public BubbleLayout setBubbleColor(int bubbleColor) {
    this.bubbleColor = bubbleColor;
    requestLayout();
    return this;
  }

  /**
   * Get stroke width.
   *
   * @return the stroke width
   */
  public float getStrokeWidth() {
    return strokeWidth;
  }

  /**
   * Set the stroke width.
   *
   * @param strokeWidth The stroke width
   * @return this
   */
  public BubbleLayout setStrokeWidth(float strokeWidth) {
    resetPadding();
    this.strokeWidth = strokeWidth;
    initPadding();
    return this;
  }

  /**
   * Get the stroke color.
   *
   * @return the stroke color
   */
  public int getStrokeColor() {
    return strokeColor;
  }

  /**
   * Set the stroke color.
   *
   * @param strokeColor The stroke color
   * @return this
   */
  public BubbleLayout setStrokeColor(int strokeColor) {
    this.strokeColor = strokeColor;
    requestLayout();
    return this;
  }

  private void initPadding() {
    int paddingLeft = getPaddingLeft();
    int paddingRight = getPaddingRight();
    int paddingTop = getPaddingTop();
    int paddingBottom = getPaddingBottom();
    switch (arrowDirection.getValue()) {
      case ArrowDirection.LEFT:
        paddingLeft += arrowWidth;
        break;
      case ArrowDirection.RIGHT:
        paddingRight += arrowWidth;
        break;
      case ArrowDirection.TOP:
        paddingTop += arrowHeight;
        break;
      case ArrowDirection.BOTTOM:
        paddingBottom += arrowHeight;
        break;
    }
    if (strokeWidth > 0) {
      paddingLeft += strokeWidth;
      paddingRight += strokeWidth;
      paddingTop += strokeWidth;
      paddingBottom += strokeWidth;
    }
    setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  private void initDrawable(int left, int right, int top, int bottom) {
    if (right < left || bottom < top) {
      return;
    }

    RectF rectF = new RectF(left, top, right, bottom);
    bubble = new Bubble(rectF, arrowDirection, arrowWidth, arrowHeight, arrowPosition, cornersRadius,
      bubbleColor, strokeWidth, strokeColor);
  }

  private void resetPadding() {
    int paddingLeft = getPaddingLeft();
    int paddingRight = getPaddingRight();
    int paddingTop = getPaddingTop();
    int paddingBottom = getPaddingBottom();
    switch (arrowDirection.getValue()) {
      case ArrowDirection.LEFT:
        paddingLeft -= arrowWidth;
        break;
      case ArrowDirection.RIGHT:
        paddingRight -= arrowWidth;
        break;
      case ArrowDirection.TOP:
        paddingTop -= arrowHeight;
        break;
      case ArrowDirection.BOTTOM:
        paddingBottom -= arrowHeight;
        break;
    }
    if (strokeWidth > 0) {
      paddingLeft -= strokeWidth;
      paddingRight -= strokeWidth;
      paddingTop -= strokeWidth;
      paddingBottom -= strokeWidth;
    }
    setPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);
  }
}
