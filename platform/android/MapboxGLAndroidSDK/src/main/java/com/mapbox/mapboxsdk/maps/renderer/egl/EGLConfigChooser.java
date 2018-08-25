package com.mapbox.mapboxsdk.maps.renderer.egl;

import android.opengl.GLSurfaceView;
import android.os.Build;
import android.support.annotation.NonNull;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.log.Logger;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import static com.mapbox.mapboxsdk.utils.Compare.compare;
import static javax.microedition.khronos.egl.EGL10.EGL_ALPHA_MASK_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_ALPHA_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_BLUE_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_BUFFER_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_COLOR_BUFFER_TYPE;
import static javax.microedition.khronos.egl.EGL10.EGL_CONFIG_CAVEAT;
import static javax.microedition.khronos.egl.EGL10.EGL_DEPTH_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_GREEN_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_NONE;
import static javax.microedition.khronos.egl.EGL10.EGL_RED_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_RENDERABLE_TYPE;
import static javax.microedition.khronos.egl.EGL10.EGL_RGB_BUFFER;
import static javax.microedition.khronos.egl.EGL10.EGL_SAMPLES;
import static javax.microedition.khronos.egl.EGL10.EGL_SAMPLE_BUFFERS;
import static javax.microedition.khronos.egl.EGL10.EGL_STENCIL_SIZE;
import static javax.microedition.khronos.egl.EGL10.EGL_SURFACE_TYPE;
import static javax.microedition.khronos.egl.EGL10.EGL_WINDOW_BIT;

/**
 * Selects the right EGLConfig needed for `mapbox-gl-native`
 */
public class EGLConfigChooser implements GLSurfaceView.EGLConfigChooser {

  private static final String TAG = "Mbgl-EGLConfigChooser";

  /**
   * Requires API level 17
   *
   * @see android.opengl.EGL14.EGL_CONFORMANT;
   */
  @SuppressWarnings("JavadocReference")
  private static final int EGL_CONFORMANT = 0x3042;

  /**
   * Requires API level 17
   *
   * @see android.opengl.EGL14.EGL_OPENGL_ES2_BIT;
   */
  @SuppressWarnings("JavadocReference")
  private static final int EGL_OPENGL_ES2_BIT = 0x0004;

  private boolean translucentSurface;

  public EGLConfigChooser() {
    this(false);
  }

  public EGLConfigChooser(boolean translucentSurface) {
    super();
    this.translucentSurface = translucentSurface;
  }

  @Override
  public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
    int[] configAttribs = getConfigAttributes();

    // Determine number of possible configurations
    int[] numConfigs = getNumberOfConfigurations(egl, display, configAttribs);
    if (numConfigs[0] < 1) {
      Logger.e(TAG, "eglChooseConfig() returned no configs.");
      throw new EGLConfigException("eglChooseConfig() failed");
    }

    // Get all possible configurations
    EGLConfig[] possibleConfigurations = getPossibleConfigurations(egl, display, configAttribs, numConfigs);

    // Choose best match
    EGLConfig config = chooseBestMatchConfig(egl, display, possibleConfigurations);
    if (config == null) {
      Logger.e(TAG, "No config chosen");
      throw new EGLConfigException("No config chosen");
    }

    return config;
  }

  private int[] getNumberOfConfigurations(EGL10 egl, EGLDisplay display, int[] configAttributes) {
    int[] numConfigs = new int[1];
    if (!egl.eglChooseConfig(display, configAttributes, null, 0, numConfigs)) {
      Logger.e(TAG, String.format(
        MapboxConstants.MAPBOX_LOCALE, "eglChooseConfig(NULL) returned error %d", egl.eglGetError())
      );
      throw new EGLConfigException("eglChooseConfig() failed");
    }
    return numConfigs;
  }

  private EGLConfig[] getPossibleConfigurations(EGL10 egl, EGLDisplay display,
                                                int[] configAttributes, int[] numConfigs) {
    EGLConfig[] configs = new EGLConfig[numConfigs[0]];
    if (!egl.eglChooseConfig(display, configAttributes, configs, numConfigs[0], numConfigs)) {
      Logger.e(TAG, String.format(
        MapboxConstants.MAPBOX_LOCALE, "eglChooseConfig() returned error %d", egl.eglGetError())
      );
      throw new EGLConfigException("eglChooseConfig() failed");
    }
    return configs;
  }

  // Quality
  enum BufferFormat {
    Format16Bit(3),
    Format32BitNoAlpha(1),
    Format32BitAlpha(2),
    Format24Bit(0),
    Unknown(4);

    int value;

    BufferFormat(int value) {
      this.value = value;
    }
  }

  enum DepthStencilFormat {
    Format16Depth8Stencil(1),
    Format24Depth8Stencil(0);

    int value;

    DepthStencilFormat(int value) {
      this.value = value;
    }
  }

  private EGLConfig chooseBestMatchConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
    class Config implements Comparable<Config> {
      private final BufferFormat bufferFormat;
      private final DepthStencilFormat depthStencilFormat;
      private final boolean isNotConformant;
      private final boolean isCaveat;
      private final int index;
      private final EGLConfig config;

      public Config(BufferFormat bufferFormat, DepthStencilFormat depthStencilFormat,
                    boolean isNotConformant, boolean isCaveat, int index, EGLConfig config) {
        this.bufferFormat = bufferFormat;
        this.depthStencilFormat = depthStencilFormat;
        this.isNotConformant = isNotConformant;
        this.isCaveat = isCaveat;
        this.index = index;
        this.config = config;
      }


      @Override
      public int compareTo(@NonNull Config other) {
        int i = compare(bufferFormat.value, other.bufferFormat.value);
        if (i != 0) {
          return i;
        }

        i = compare(depthStencilFormat.value, other.depthStencilFormat.value);
        if (i != 0) {
          return i;
        }

        i = compare(isNotConformant, other.isNotConformant);
        if (i != 0) {
          return i;
        }

        i = compare(isCaveat, other.isCaveat);
        if (i != 0) {
          return i;
        }

        i = compare(index, other.index);
        if (i != 0) {
          return i;
        }

        return 0;
      }
    }

    List<Config> matches = new ArrayList<>();

    int i = 0;
    for (EGLConfig config : configs) {
      i++;

      int caveat = getConfigAttr(egl, display, config, EGL_CONFIG_CAVEAT);
      int conformant = getConfigAttr(egl, display, config, EGL_CONFORMANT);
      int bits = getConfigAttr(egl, display, config, EGL_BUFFER_SIZE);
      int red = getConfigAttr(egl, display, config, EGL_RED_SIZE);
      int green = getConfigAttr(egl, display, config, EGL_GREEN_SIZE);
      int blue = getConfigAttr(egl, display, config, EGL_BLUE_SIZE);
      int alpha = getConfigAttr(egl, display, config, EGL_ALPHA_SIZE);
      int alphaMask = getConfigAttr(egl, display, config, EGL_ALPHA_MASK_SIZE);
      int depth = getConfigAttr(egl, display, config, EGL_DEPTH_SIZE);
      int stencil = getConfigAttr(egl, display, config, EGL_STENCIL_SIZE);
      int sampleBuffers = getConfigAttr(egl, display, config, EGL_SAMPLE_BUFFERS);
      int samples = getConfigAttr(egl, display, config, EGL_SAMPLES);

      boolean configOk = (depth == 24) || (depth == 16);
      configOk &= stencil == 8;
      configOk &= sampleBuffers == 0;
      configOk &= samples == 0;

      // Filter our configs first for depth, stencil and anti-aliasing
      if (configOk) {
        // Work out the config's buffer format
        BufferFormat bufferFormat;
        if ((bits == 16) && (red == 5) && (green == 6) && (blue == 5) && (alpha == 0)) {
          bufferFormat = BufferFormat.Format16Bit;
        } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
          bufferFormat = BufferFormat.Format32BitNoAlpha;
        } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 8)) {
          bufferFormat = BufferFormat.Format32BitAlpha;
        } else if ((bits == 24) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
          bufferFormat = BufferFormat.Format24Bit;
        } else {
          bufferFormat = BufferFormat.Unknown;
        }

        // Work out the config's depth stencil format
        DepthStencilFormat depthStencilFormat;
        if ((depth == 16) && (stencil == 8)) {
          depthStencilFormat = DepthStencilFormat.Format16Depth8Stencil;
        } else {
          depthStencilFormat = DepthStencilFormat.Format24Depth8Stencil;
        }

        boolean isNotConformant = (conformant & EGL_OPENGL_ES2_BIT) != EGL_OPENGL_ES2_BIT;
        boolean isCaveat = caveat != EGL_NONE;

        // Ignore formats we don't recognise
        if (bufferFormat != BufferFormat.Unknown) {
          matches.add(new Config(bufferFormat, depthStencilFormat, isNotConformant, isCaveat, i, config));
        }
      }

    }

    // Sort
    Collections.sort(matches);

    if (matches.size() == 0) {
      throw new EGLConfigException("No matching configurations after filtering");
    }

    Config bestMatch = matches.get(0);

    if (bestMatch.isCaveat) {
      Logger.w(TAG, "Chosen config has a caveat.");
    }

    if (bestMatch.isNotConformant) {
      Logger.w(TAG, "Chosen config is not conformant.");
    }

    return bestMatch.config;
  }

  private int getConfigAttr(EGL10 egl, EGLDisplay display, EGLConfig config, int attributeName) {
    int[] attributevalue = new int[1];
    if (!egl.eglGetConfigAttrib(display, config, attributeName, attributevalue)) {
      Logger.e(TAG, String.format(
        MapboxConstants.MAPBOX_LOCALE, "eglGetConfigAttrib(%d) returned error %d", attributeName, egl.eglGetError())
      );
      throw new EGLConfigException("eglGetConfigAttrib() failed");
    }
    return attributevalue[0];
  }

  private int[] getConfigAttributes() {
    boolean emulator = inEmulator() || inGenymotion();
    Logger.i(TAG, String.format("In emulator: %s", emulator));

    // Get all configs at least RGB 565 with 16 depth and 8 stencil
    return new int[] {
      EGL_CONFIG_CAVEAT, EGL_NONE,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BUFFER_SIZE, 16,
      EGL_RED_SIZE, 5,
      EGL_GREEN_SIZE, 6,
      EGL_BLUE_SIZE, 5,
      EGL_ALPHA_SIZE, translucentSurface ? 8 : 0,
      EGL_DEPTH_SIZE, 16,
      EGL_STENCIL_SIZE, 8,
      (emulator ? EGL_NONE : EGL_CONFORMANT), EGL_OPENGL_ES2_BIT,
      (emulator ? EGL_NONE : EGL_COLOR_BUFFER_TYPE), EGL_RGB_BUFFER,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
    };
  }

  /**
   * Detect if we are in emulator.
   */
  private boolean inEmulator() {
    return System.getProperty("ro.kernel.qemu") != null;
  }

  /**
   * Detect if we are in genymotion
   */
  private boolean inGenymotion() {
    return Build.MANUFACTURER.contains("Genymotion");
  }

}
