package com.mapbox.mapboxgl;

import java.util.LinkedList;

//This class controls the display of the map in the MapView
public class MapController {

	// International standard conversion between inches and metres
	private static final double METRES_PER_INCH = 0.0254;

	// Width and height of render tiles in pixels
	private static final int TILE_SIZE = 256;
	
	// Map unites are in metres
	private double mMapCentreX, mMapCentreY;
	
	// Scale units are like 1:100k = 100k
	private double mMapScale;
	
	// DPI conversion
	private double mScreenMetresPerPixelX, mScreenMetresPerPixelY;

	// Screen units are in pixels
	private double mScreenWidth, mScreenHeight;
	
	// Holds all the currently attached change listeners
	private LinkedList<OnChangeListener> onChangeListeners;
	
	// The constructor
	// Assumes the DPI cannot change on the fly
	public MapController(double mapCentreX, double mapCentreY, double mapScale, 
			double screenDpiX, double screenDpiY, double screenWidth, double screenHeight) {
		mMapCentreX = mapCentreX;
		mMapCentreY = mapCentreY;
		mMapScale = mapScale;
		
		// Calculate the DPI conversion
		mScreenMetresPerPixelX = 1.0 / screenDpiX * METRES_PER_INCH;
		mScreenMetresPerPixelY = -1.0 / screenDpiY * METRES_PER_INCH;
		
		mScreenWidth = screenWidth;
		mScreenHeight = screenHeight;
		
		// Holds all the currently attached change listeners
		onChangeListeners = new LinkedList<MapController.OnChangeListener>();
		
		// Inform the change listeners of the initial values
		onChange();
	}
	
	//
	// Map position functions
	//

	// Sets the map's current centre coordinates
	// Only function allowed to change the position properties
	public void moveMapTo(double mapCentreX, double mapCentreY) {
		mMapCentreX = mapCentreX;
		mMapCentreY = mapCentreY;

		// Inform the change listeners of the new values
		onChange();
	}
	
	// Offset the map's current centre coordinates
	public void moveMapBy(double mapOffsetX, double mapOffsetY) {
		moveMapTo(mMapCentreX + mapOffsetX, mMapCentreY + mapOffsetY);
	}
	
	// Gets the map's current centre X coordinate
	public double getMapCentreX() {
		return mMapCentreX;
	}

	// Sets the map's current centre X coordinate
	public void setMapCentreX(double mapCentreX) {
		moveMapTo(mapCentreX, mMapCentreY);
	}
	
	// Gets the map's current centre Y coordinate
	public double getMapCentreY() {
		return mMapCentreY;
	}
	
	// Sets the map's current centre Y coordinate
	public void setMapCentreY(double mapCentreY) {
		moveMapTo(mMapCentreX, mapCentreY);
	}
	
	//
	// Map scale functions
	//
	
	// Gets the map's current map scale
	public double getMapScale() {
		return mMapScale;
	}

	// Sets the map's current map scale
	// Only function allowed to change the scale property
	public void setMapScale(double mapScale) {
		mMapScale = mapScale;
		
		// Inform the change listeners of the new value
		onChange();
	}
	
	// Gets the inverse of the map's current map scale
	public double getMapInverseScale() {
		return 1.0 / mMapScale;
	}

	// Sets the inverse of the map's current map scale
	public void setMapInverseScale(double mapInverseScale) {
		setMapScale(1.0 / mapInverseScale);
	}
	
	//
	// Map DPI functions
	//
	
	// Calculate the map's DPI for X
	public double getMapMetresPerPixelX() {
		return mScreenMetresPerPixelX * mMapScale;
	}

	// Calculate the map's DPI for Y
	public double getMapMetresPerPixelY() {
		return mScreenMetresPerPixelY * mMapScale;
	}
	
	//
	// Screen dimension functions
	//

	// Set the screen dimensions
	// Only function allowed to change the screen dimensions
	public void setScreenSize(double screenWidth, double screenHeight) {
		mScreenWidth = screenWidth;
		mScreenHeight = screenHeight;
		
		// Inform the change listeners of the new values
		onChange();
	}
	
	// Get the screen's width
	public double getScreenWidth() {
		return mScreenWidth;
	}

	// Set the screen's width
	public void setScreenWidth(double screenWidth) {
		setScreenSize(screenWidth, mScreenHeight);
	}
	
	// Get the screen's height
	public double getScreenHeight() {
		return mScreenHeight;
	}
	
	// Set the screen's height
	public void setScreenHeight(double screenHeight) {
		setScreenSize(mScreenWidth, screenHeight);
	}
	
	//
	// Screen position functions
	//
	
	// Offset the screens's current coordinates 
	public void moveScreenBy(double screenX, double screenY) {
		moveMapBy(screenX * getMapMetresPerPixelX(), screenY * getMapMetresPerPixelY());
	}
	
	//
	// Map dimension functions
	//
	
	// Get the screen's width in map units
	public double getMapWidth() {
		return mScreenWidth * getMapMetresPerPixelX();
	}

	// Get the screen's height in map units
	public double getMapHeight() {
		return mScreenHeight * getMapMetresPerPixelY();
	}
	
	// Get the screen's left coordinate in map units
	public double getMapLeft() {
		return mMapCentreX - getMapWidth() / 2.0;
	}

	// Get the screen's right coordinate in map units
	public double getMapRight() {
		return mMapCentreX + getMapWidth() / 2.0;
	}
	
	// Get the screen's top coordinate in map units
	public double getMapTop() {
		return mMapCentreY - getMapHeight() / 2.0;
	}
	
	// Get the screen's bottom coordinate in map units
	public double getMapBottom() {
		return mMapCentreY + getMapHeight() / 2.0;
	}
	
	//
	// Coordinate conversion functions
	//
	
	// Convert the screen X coordinate into map units
	public double screenToMapX(double screenX) {
		return getMapLeft() + screenX * getMapMetresPerPixelX();
	}
	
	// Convert the screen Y coordinate into map units
	public double screenToMapY(double screenY) {
		return getMapTop() + screenY * getMapMetresPerPixelY();
	}
	
	// Convert the map X coordinate into screen units
	public double mapToScreenX(double mapX) {
		return (mapX - getMapLeft()) / getMapMetresPerPixelX(); 
	}

	// Convert the map Y coordinate into screen units
	public double mapToScreenY(double mapY) {
		return (mapY - getMapTop()) / getMapMetresPerPixelY(); 
	}
	
	//
	// Tile dimension functions
	//

	// Get a tile's width in map units
	public double getTileWidth() {
		return TILE_SIZE  * getMapMetresPerPixelX();
	}

	// Get a tile's height in map units
	public double getTileHeight() {
		return TILE_SIZE  * getMapMetresPerPixelY();
	}

	// Get the screen's left coordinate in tiles
	public int getTileLeft() {
		return (int) (getMapLeft() / getTileWidth());
	}

	// Get the screen's right coordinate in tiles
	public int getTileRight() {
		return (int) (getMapRight() / getTileWidth());
	}

	// Get the screen's top coordinate in tiles
	public int getTileTop() {
		return (int) (getMapTop() / getTileHeight());
	}

	// Get the screen's top coordinate in tiles
	public int getTileBottom() {
		return (int) (getMapBottom() / getTileHeight());
	}
	
	//
	// Change listener functions
	//
	
	// Classes implement this to get change notifications
	public interface OnChangeListener {
		
		// Called when a value changes
		public abstract void onChange(MapController mapController);
		
	}
	
	// Called when a value changes	
	protected void onChange() {
		// Notify each listener
		for (OnChangeListener listener : onChangeListeners) {
			listener.onChange(this);
		}
	}

	// Add a change listener to the list
	public void addOnChangeListener(OnChangeListener listener) {
		onChangeListeners.add(listener);
	}

	// Remove a change listener from the list
	public void removeOnChangeListener(OnChangeListener listener) {
		onChangeListeners.remove(listener);
	}	
}
