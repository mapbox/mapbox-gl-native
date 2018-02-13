package com.mapbox.mapboxsdk.maps.renderer.textureview;

import android.graphics.SurfaceTexture;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.view.TextureView;
import com.mapbox.mapboxsdk.maps.renderer.egl.EGLConfigChooser;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

import timber.log.Timber;

/**
 * The render thread is responsible for managing the communication between the
 * ui thread and the render thread it creates. Also, the EGL and GL contexts
 * are managed from here.
 */
class TextureViewRenderThread extends Thread implements TextureView.SurfaceTextureListener {

  private final TextureViewMapRenderer mapRenderer;
  private final EGLHolder eglHolder;

  // Lock used for synchronization
  private final Object lock = new Object();

  // Guarded by lock
  private final ArrayList<Runnable> eventQueue = new ArrayList<>();
  private SurfaceTexture surface;
  private int width;
  private int height;
  private boolean requestRender;
  private boolean sizeChanged;
  private boolean paused;
  private boolean destroyContext;
  private boolean destroySurface;
  private boolean shouldExit;
  private boolean exited;

  /**
   * Create a render thread for the given TextureView / Maprenderer combination.
   *
   * @param textureView the TextureView
   * @param mapRenderer the MapRenderer
   */
  @UiThread
  TextureViewRenderThread(@NonNull TextureView textureView, @NonNull TextureViewMapRenderer mapRenderer) {
    textureView.setSurfaceTextureListener(this);
    this.mapRenderer = mapRenderer;
    this.eglHolder = new EGLHolder(new WeakReference<>(textureView));
  }

  // SurfaceTextureListener methods

  @UiThread
  @Override
  public void onSurfaceTextureAvailable(final SurfaceTexture surface, final int width, final int height) {
    synchronized (lock) {
      this.surface = surface;
      this.width = width;
      this.height = height;
      this.requestRender = true;
      lock.notifyAll();
    }
  }

  @Override
  @UiThread
  public void onSurfaceTextureSizeChanged(SurfaceTexture surface, final int width, final int height) {
    synchronized (lock) {
      this.width = width;
      this.height = height;
      this.sizeChanged = true;
      this.requestRender = true;
      lock.notifyAll();
    }
  }

  @Override
  @UiThread
  public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
    synchronized (lock) {
      this.surface = null;
      this.destroySurface = true;
      this.requestRender = false;
      lock.notifyAll();
    }
    return true;
  }

  @Override
  @UiThread
  public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    // Ignored
  }

  // MapRenderer delegate methods

  /**
   * May be called from any thread
   */
  void requestRender() {
    synchronized (lock) {
      requestRender = true;
      lock.notifyAll();
    }
  }

  /**
   * May be called from any thread
   */
  void queueEvent(Runnable runnable) {
    if (runnable == null) {
      throw new IllegalArgumentException("runnable must not be null");
    }
    synchronized (lock) {
      eventQueue.add(runnable);
      lock.notifyAll();
    }
  }


  @UiThread
  void onPause() {
    synchronized (lock) {
      this.paused = true;
      lock.notifyAll();
    }
  }

  @UiThread
  void onResume() {
    synchronized (lock) {
      this.paused = false;
      lock.notifyAll();
    }
  }


  @UiThread
  void onDestroy() {
    synchronized (lock) {
      this.shouldExit = true;
      lock.notifyAll();

      // Wait for the thread to exit
      while (!this.exited) {
        try {
          lock.wait();
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
        }
      }
    }
  }

  // Thread implementation

  @Override
  public void run() {
    try {

      while (true) {
        Runnable event = null;
        boolean initializeEGL = false;
        boolean recreateSurface = false;
        int w = -1;
        int h = -1;

        // Guarded block
        synchronized (lock) {
          while (true) {

            if (shouldExit) {
              return;
            }

            // If any events are scheduled, pop one for processing
            if (!eventQueue.isEmpty()) {
              event = eventQueue.remove(0);
              break;
            }

            if (destroySurface) {
              eglHolder.destroySurface();
              destroySurface = false;
              break;
            }

            if (destroyContext) {
              eglHolder.destroyContext();
              destroyContext = false;
              break;
            }

            if (surface != null && !paused && requestRender) {

              w = width;
              h = height;

              // Initialize EGL if needed
              if (eglHolder.eglContext == EGL10.EGL_NO_CONTEXT) {
                initializeEGL = true;
                break;
              }

              // (re-)Initialize EGL Surface if needed
              if (eglHolder.eglSurface == EGL10.EGL_NO_SURFACE) {
                recreateSurface = true;
                break;
              }

              // Reset the request render flag now, so we can catch new requests
              // while rendering
              requestRender = false;

              // Break the guarded loop and continue to process
              break;
            }


            // Wait until needed
            lock.wait();

          } // end guarded while loop

        } // end guarded block

        // Run event, if any
        if (event != null) {
          event.run();
          continue;
        }

        GL10 gl = eglHolder.createGL();

        // Initialize EGL
        if (initializeEGL) {
          eglHolder.prepare();
          if (!eglHolder.createSurface()) {
            synchronized (lock) {
              // Cleanup the surface if one could not be created
              // and wait for another to be ready.
              destroySurface = true;
            }
            continue;
          }
          mapRenderer.onSurfaceCreated(gl, eglHolder.eglConfig);
          mapRenderer.onSurfaceChanged(gl, w, h);
          continue;
        }

        // If the surface size has changed inform the map renderer.
        if (recreateSurface) {
          eglHolder.createSurface();
          mapRenderer.onSurfaceChanged(gl, w, h);
          continue;
        }

        if (sizeChanged) {
          mapRenderer.onSurfaceChanged(gl, w, h);
          sizeChanged = false;
          continue;
        }

        // Don't continue without a surface
        if (eglHolder.eglSurface == EGL10.EGL_NO_SURFACE) {
          continue;
        }

        // Time to render a frame
        mapRenderer.onDrawFrame(gl);

        // Swap and check the result
        int swapError = eglHolder.swap();
        switch (swapError) {
          case EGL10.EGL_SUCCESS:
            break;
          case EGL11.EGL_CONTEXT_LOST:
            Timber.w("Context lost. Waiting for re-aquire");
            synchronized (lock) {
              surface = null;
              destroySurface = true;
              destroyContext = true;
            }
            break;
          default:
            Timber.w("eglSwapBuffer error: %s. Waiting or new surface", swapError);
            // Probably lost the surface. Clear the current one and
            // wait for a new one to be set
            synchronized (lock) {
              surface = null;
              destroySurface = true;
            }
        }

      }

    } catch (InterruptedException err) {
      // To be expected
    } finally {
      // Cleanup
      eglHolder.cleanup();

      // Signal we're done
      synchronized (lock) {
        this.exited = true;
        lock.notifyAll();
      }
    }
  }

  /**
   * Holds the EGL state and offers methods to mutate it.
   */
  private static class EGLHolder {
    private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    private final WeakReference<TextureView> textureViewWeakRef;

    private EGL10 egl;
    private EGLConfig eglConfig;
    private EGLDisplay eglDisplay = EGL10.EGL_NO_DISPLAY;
    private EGLContext eglContext = EGL10.EGL_NO_CONTEXT;
    private EGLSurface eglSurface = EGL10.EGL_NO_SURFACE;

    EGLHolder(WeakReference<TextureView> textureViewWeakRef) {
      this.textureViewWeakRef = textureViewWeakRef;
    }

    void prepare() {
      this.egl = (EGL10) EGLContext.getEGL();

      // Only re-initialize display when needed
      if (eglDisplay == EGL10.EGL_NO_DISPLAY) {
        this.eglDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

        if (eglDisplay == EGL10.EGL_NO_DISPLAY) {
          throw new RuntimeException("eglGetDisplay failed");
        }

        int[] version = new int[2];
        if (!egl.eglInitialize(eglDisplay, version)) {
          throw new RuntimeException("eglInitialize failed");
        }
      }

      if (textureViewWeakRef == null) {
        // No texture view present
        eglConfig = null;
        eglContext = EGL10.EGL_NO_CONTEXT;
      } else if (eglContext == EGL10.EGL_NO_CONTEXT) {
        eglConfig = new EGLConfigChooser().chooseConfig(egl, eglDisplay);
        int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
        eglContext = egl.eglCreateContext(eglDisplay, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
      }

      if (eglContext == EGL10.EGL_NO_CONTEXT) {
        throw new RuntimeException("createContext");
      }
    }

    GL10 createGL() {
      return (GL10) eglContext.getGL();
    }

    boolean createSurface() {
      // The window size has changed, so we need to create a new surface.
      destroySurface();

      // Create an EGL surface we can render into.
      TextureView view = textureViewWeakRef.get();
      if (view != null) {
        int[] surfaceAttribs = {EGL10.EGL_NONE};
        eglSurface = egl.eglCreateWindowSurface(eglDisplay, eglConfig, view.getSurfaceTexture(), surfaceAttribs);
      } else {
        eglSurface = EGL10.EGL_NO_SURFACE;
      }

      if (eglSurface == null || eglSurface == EGL10.EGL_NO_SURFACE) {
        int error = egl.eglGetError();
        if (error == EGL10.EGL_BAD_NATIVE_WINDOW) {
          Timber.e("createWindowSurface returned EGL_BAD_NATIVE_WINDOW.");
        }
        return false;
      }

      return makeCurrent();
    }

    boolean makeCurrent() {
      if (!egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        // Could not make the context current, probably because the underlying
        // SurfaceView surface has been destroyed.
        Timber.w("eglMakeCurrent: %s", egl.eglGetError());
        return false;
      }

      return true;
    }

    int swap() {
      if (!egl.eglSwapBuffers(eglDisplay, eglSurface)) {
        return egl.eglGetError();
      }
      return EGL10.EGL_SUCCESS;
    }

    private void destroySurface() {
      if (eglSurface == EGL10.EGL_NO_SURFACE) {
        return;
      }

      if (!egl.eglDestroySurface(eglDisplay, eglSurface)) {
        Timber.w("Could not destroy egl surface. Display %s, Surface %s", eglDisplay, eglSurface);
      }

      eglSurface = EGL10.EGL_NO_SURFACE;
    }

    private void destroyContext() {
      if (eglContext == EGL10.EGL_NO_CONTEXT) {
        return;
      }

      if (!egl.eglDestroyContext(eglDisplay, eglContext)) {
        Timber.w("Could not destroy egl context. Display %s, Context %s", eglDisplay, eglContext);
      }

      eglContext = EGL10.EGL_NO_CONTEXT;
    }

    private void terminate() {
      if (eglDisplay == EGL10.EGL_NO_DISPLAY) {
        return;
      }

      if (!egl.eglTerminate(eglDisplay)) {
        Timber.w("Could not terminate egl. Display %s", eglDisplay);
      }
      eglDisplay = EGL10.EGL_NO_DISPLAY;
    }

    void cleanup() {
      destroySurface();
      destroyContext();
      terminate();
    }
  }
}
