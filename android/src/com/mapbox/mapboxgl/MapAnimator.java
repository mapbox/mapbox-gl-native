package com.mapbox.mapboxgl;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.TypeEvaluator;
import android.view.animation.LinearInterpolator;
import android.widget.Scroller;

// This class runs all the MapView animations
public class MapAnimator {
	
	// The time in ms for the animation durations
	// Used by the zoom controls
	public final static long ZOOM_DURATION = 300;
	public final static long PAN_DURATION = 300;

	// This controls the map display
	private MapController mController;
	
	// These objects actually run the animations
	private ObjectAnimator mZoomAnimator;
	private AnimatorSet mPaAnimatorSet, mPanAndZoomAnimatorSet;
	
	// This class holds the special sauce fling algorithm
	private Scroller mFlingScroller;
	private double mFlingLastX, mFlingLastY;
	
	// We need a reference to the map view to call invalidate on it
	private MapView mMapView;

	// The constructor
	public MapAnimator(MapView mapView, MapController controller) {
		mMapView = mapView;
		mController = controller;
		
		// This class holds the special sauce fling algorithm
		mFlingScroller = new Scroller(mapView.getContext());
		mFlingLastX = 0.0;
		mFlingLastY = 0.0;
	}
	
	//
	// Zoom animation functions
	//
	
	// Performs a smooth animation to the specified scale
	public void zoomTo(double mapInverseScale) {
		endAll(); // Stop any ongoing animations
		
		mZoomAnimator = ObjectAnimator.ofObject(mController, "mapInverseScale", new DoubleEvaluator(), mController.getMapInverseScale() * mapInverseScale);
		mZoomAnimator.setInterpolator(new LinearInterpolator()); // Linear interpolator looks better when holding down zoom button
		mZoomAnimator.setDuration(ZOOM_DURATION);
		mZoomAnimator.addListener(new MapAnimatorListener()); // Used to ensure we clean up
		mZoomAnimator.start();
	}
	
	// Handles the single button push zoom in and zoom out
	public void zoom(boolean zoomIn) {
		if (zoomIn) {
			// Zooming in so double the scale
			zoomTo(2.0);
		} else {
			// Zooming out so halve the scale
			zoomTo(0.5);
		}
	}
	
	// Cancels the zoom leaving it where it got up to
	public void cancelZoom() {
		if (mZoomAnimator != null) {
			mZoomAnimator.cancel();
			mZoomAnimator = null;
		}
	}
	
	// Ends the zoom immediately by going to the final scale
	public void endZoom() {
		if (mZoomAnimator != null) {
			mZoomAnimator.end();
			mZoomAnimator = null;
		}
	}
	
	//
	// Pan animation functions
	//
	
	// Smoothly animates to the given position
	public void panTo(double mapCentreX, double mapCentreY) {
		endAll(); // Stop any ongoing animations
		
		// Animate the X coordinate
		ObjectAnimator xAnimator = ObjectAnimator.ofObject(mController, "mapCentreX", new DoubleEvaluator(), mapCentreX);
		xAnimator.setInterpolator(new LinearInterpolator());
		xAnimator.setDuration(PAN_DURATION);

		// Animate the Y coordinate
		ObjectAnimator yAnimator = ObjectAnimator.ofObject(mController, "mapCentreY", new DoubleEvaluator(), mapCentreY);
		yAnimator.setInterpolator(new LinearInterpolator());
		yAnimator.setDuration(PAN_DURATION);
		
		// Combine the X and Y animations into a simultaneous animation
		mPaAnimatorSet = new AnimatorSet();
		mPaAnimatorSet.addListener(new MapAnimatorListener());
		mPaAnimatorSet.play(xAnimator).with(yAnimator);
		
		mPaAnimatorSet.start();
	}

	// Cancels the pan leaving it where it got up to
	public void cancelPan() {
		if (mPaAnimatorSet != null) {
			mPaAnimatorSet.cancel();
			mPaAnimatorSet = null;
		}
	}
	
	// Ends the zoom immediately by going to the final position
	public void endPan() {
		if (mPaAnimatorSet != null) {
			mPaAnimatorSet.end();
			mPaAnimatorSet = null;
		}
	}

	//
	// Pan and zoom animation functions
	//
	
	// Combines the pan and zoom animations into a single animation
	public void panAndZoomTo(double mapCentreX, double mapCentreY, double mapInverseScale) {
		endAll(); // Stop any ongoing animations
		
		// Animate the X coordinate
		ObjectAnimator xAnimator = ObjectAnimator.ofObject(mController, "mapCentreX", new DoubleEvaluator(), mapCentreX);
		xAnimator.setInterpolator(new LinearInterpolator());
		xAnimator.setDuration(PAN_DURATION);

		// Animate the Y coordinate
		ObjectAnimator yAnimator = ObjectAnimator.ofObject(mController, "mapCentreY", new DoubleEvaluator(), mapCentreY);
		yAnimator.setInterpolator(new LinearInterpolator());
		yAnimator.setDuration(PAN_DURATION);
		
		// Combine the X and Y animations into a simultaneous animation
		AnimatorSet panAnimator = new AnimatorSet();
		panAnimator.play(xAnimator).with(yAnimator);
		
		// Animate the scale
		ObjectAnimator zoomAnimator = ObjectAnimator.ofObject(mController, "mapInverseScale", new DoubleEvaluator(), mController.getMapInverseScale() * mapInverseScale);
		zoomAnimator.setInterpolator(new LinearInterpolator());
		zoomAnimator.setDuration(ZOOM_DURATION);
		
		// Combine the pand and zoom animations into a simultaneous animation
		mPanAndZoomAnimatorSet = new AnimatorSet();
		mPanAndZoomAnimatorSet.addListener(new MapAnimatorListener());
		mPanAndZoomAnimatorSet.play(panAnimator).with(zoomAnimator);
		
		mPanAndZoomAnimatorSet.start();
	}
	
	// Combines the pan and zoom animations into a single animation using single button push logic
	public void panAndZoomTo(double mapCentreX, double mapCentreY, boolean zoomIn) {		
		if (zoomIn) {
			panAndZoomTo(mapCentreX, mapCentreY, 2.0);
		} else {
			panAndZoomTo(mapCentreX, mapCentreY, 0.5);
		}
	}
	
	// Cancels the animation leaving it where it got up to
	public void cancelPanAndZoom() {
		if (mPanAndZoomAnimatorSet != null) {
			mPanAndZoomAnimatorSet.cancel();
			mPanAndZoomAnimatorSet = null;
		}
	}
	
	// Ends the animation immediately by going to the final position and scale
	public void endPanAndZoom() {
		if (mPanAndZoomAnimatorSet != null) {
			mPanAndZoomAnimatorSet.end();
			mPanAndZoomAnimatorSet = null;
		}
	}
	
	//
	// Shared animation functions
	//

	// This class performs cleanup on finished animation objects
	private class MapAnimatorListener implements Animator.AnimatorListener {

		// Delete the currently finished animation
		// Checks which animation object it was before deleteing
		private void delete(Animator animation) {
			if (mZoomAnimator == animation) {
				mZoomAnimator = null;
			}
			
			if (mPaAnimatorSet == animation) {
				mPaAnimatorSet = null;
			}
			
			if (mPanAndZoomAnimatorSet == animation) {
				mPanAndZoomAnimatorSet = null;
			}
		}
		
		// The animation object was canceled so clean up
		@Override
		public void onAnimationCancel(Animator animation) {
			delete(animation);
		}
		
		// The animation object has ended so clean up
		@Override
		public void onAnimationEnd(Animator animation) {
			delete(animation);
		}

		// Not used
		@Override
		public void onAnimationRepeat(Animator animation) {
			// Ignore
		}

		// Not used
		@Override
		public void onAnimationStart(Animator animation) {
			// Ignore
		}
	}

	// Cancels any animations leaving them where they got up to
	public void cancelAll() {
		cancelZoom();
		cancelPan();
		cancelPanAndZoom();
		mFlingScroller.forceFinished(true);
	}
	
	// Cancels any animations leaving them where they got up to	
	public void endAll() {
		endZoom();
		endPan();
		endPanAndZoom();
		mFlingScroller.forceFinished(true);
	}
	
	// Custom class to interpolate doubles (doubles are used a lot)
	protected class DoubleEvaluator implements TypeEvaluator<Double> {

		@Override
		public Double evaluate(float fraction, Double startValue, Double endValue) {
			return startValue + fraction * (endValue - startValue);
		}
	}
	
	//
	// Fling animation functions
	//
	
	// Passes the fling velocity to the Scroller logic
	public void fling(double velocityX, double velocityY) {
		endAll(); // Stop any ongoing animations
		
		// Pass the fling velocity ignoring the map boundaries for now (so can scroll off into infinity)
		mFlingScroller.fling(0, 0, (int) velocityX, (int) velocityY, Integer.MIN_VALUE, Integer.MAX_VALUE, Integer.MIN_VALUE, Integer.MAX_VALUE);
		
		// Reset the values used to calculate movment deltas
		mFlingLastX = 0.0;
		mFlingLastY = 0.0;
		
		// Fire up the animation system
		mMapView.postInvalidateOnAnimation();
	}
	
	// Called by MapView's computeScroll() to animate the next fling frame
	// Call results from the postInvalidateOnAnimation()
	public void computeFling() {
		// Check if the fling is still running
		if (mFlingScroller.computeScrollOffset()) {
			// Calculate the movement delta between this frame and the last
			// Needed as Scroller does not provide the delta...
			double mDeltaX = mFlingScroller.getCurrX() - mFlingLastX;
			double mDeltaY = mFlingScroller.getCurrY() - mFlingLastY;
			
			// Move the screen by the delta distance
			mController.moveScreenBy(mDeltaX, mDeltaY);
			
			// Save the current position for the next frame
			mFlingLastX = mFlingScroller.getCurrX();
			mFlingLastY = mFlingScroller.getCurrY();
		} else {
			// Animation has finished so clean up
			mFlingLastX = 0.0;
			mFlingLastY = 0.0;
		}
	}
}
