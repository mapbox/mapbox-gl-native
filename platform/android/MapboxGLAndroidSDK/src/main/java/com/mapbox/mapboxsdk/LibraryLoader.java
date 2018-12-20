package com.mapbox.mapboxsdk;

/**
 * Loads the mapbox-gl shared library
 * <p>
 * By default uses the {@link System#loadLibrary(String)},
 * use {@link Mapbox#setModuleProvider(ModuleProvider)} to provide an alternative library loading hook.
 * </p>
 */
public interface LibraryLoader {

  void load();

}