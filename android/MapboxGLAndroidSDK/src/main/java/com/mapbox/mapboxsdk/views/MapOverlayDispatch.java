package com.mapbox.mapboxsdk.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

final class MapOverlayDispatch extends View {

    private List<Overlay> _overlayList = new ArrayList<>();
    private PointF _wgsCenter = new PointF();
    private RectF _wgsBounds = new RectF();

    public MapOverlayDispatch(Context context) {
        super(context);
    }

    public MapOverlayDispatch(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MapOverlayDispatch(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        for (int i = 0; i < _overlayList.size(); i++) {
            Overlay overlay = _overlayList.get(i);
            if (overlay.isEnabled()) {
                overlay.drawLayer(canvas, _wgsBounds, _wgsCenter);
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        for (int i = 0; i < _overlayList.size(); i++) {
            _overlayList.get(i).onTouchEvent(event);
        }
        return super.onTouchEvent(event);
    }

    public void addOverlay(Overlay overlay) {
        _overlayList.add(overlay);
    }

    public void clearOverlays() {
        _overlayList.clear();
    }

    public void removeOverlay(Overlay overlay) {
        _overlayList.remove(overlay);
    }

    public void invalidate(final RectF wgsBounds, final PointF wgsCenter) {
        _wgsCenter.set(wgsCenter);

        //left, top, right, bottom
        _wgsBounds.set(wgsBounds);

        super.invalidate();

    }
}
