package com.mapbox.mapboxsdk.testapp.activity.textureview;

import android.content.Context;
import android.content.res.Resources;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.view.ViewGroup.LayoutParams.MATCH_PARENT;

public class TextureViewOverlayActivity extends AppCompatActivity implements MapboxMap.OnMapLongClickListener {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GLSurfaceView mGLView;
  private boolean expanded;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_textureview_overlay);
    setupGlSurfaceView();
    setupMapView(savedInstanceState);
  }

  private void setupGlSurfaceView() {
    mGLView = new MyGLSurfaceView(this);
    mGLView.setLayoutParams(new RelativeLayout.LayoutParams(MATCH_PARENT, MATCH_PARENT));
    ((ViewGroup) findViewById(R.id.container)).addView(mGLView, 0);
  }

  private void setupMapView(Bundle savedInstanceState) {
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.addOnMapLongClickListener(TextureViewOverlayActivity.this);
    });
  }

  @Override
  public void onMapLongClick(@NonNull LatLng point) {
    toggleScreen();
  }

  private void toggleScreen() {
    if (!expanded) {
      mapView.setLayoutParams(new RelativeLayout.LayoutParams(MATCH_PARENT, MATCH_PARENT));
    } else {
      mapView.setLayoutParams(createBottomLeftParams());
    }
    expanded = !expanded;
  }

  private RelativeLayout.LayoutParams createBottomLeftParams(){
    RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(
      convertDpToPixel(192, this),
      convertDpToPixel(128, this)
    );

    int margin = convertDpToPixel(16, this);
    layoutParams.setMargins(margin, 0, 0, margin);
    layoutParams.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
    layoutParams.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
      layoutParams.addRule(RelativeLayout.ALIGN_PARENT_START);
    }
    return layoutParams;
  }

  @Override
  public void onBackPressed() {
    if (expanded) {
      toggleScreen();
      return;
    }
    super.onBackPressed();
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
    mGLView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
    mGLView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapboxMap.removeOnMapLongClickListener(this);
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  public static int convertDpToPixel(float dp, Context context) {
    Resources resources = context.getResources();
    DisplayMetrics metrics = resources.getDisplayMetrics();
    return (int) (dp * ((float) metrics.densityDpi / DisplayMetrics.DENSITY_DEFAULT));
  }

  /**
   * A view container where OpenGL ES graphics can be drawn on screen.
   * This view can also be used to capture touch events, such as a user
   * interacting with drawn objects.
   */
  public class MyGLSurfaceView extends GLSurfaceView {

    private final MyGLRenderer mRenderer;

    public MyGLSurfaceView(Context context) {
      super(context);

      // Create an OpenGL ES 2.0 context.
      setEGLContextClientVersion(2);

      // Set the Renderer for drawing on the GLSurfaceView
      mRenderer = new MyGLRenderer();
      setRenderer(mRenderer);

      // Render the view only when there is a change in the drawing data
      setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;
    private float mPreviousX;
    private float mPreviousY;

    @Override
    public boolean onTouchEvent(MotionEvent e) {
      // MotionEvent reports input details from the touch screen
      // and other input controls. In this case, you are only
      // interested in events where the touch position changed.

      float x = e.getX();
      float y = e.getY();

      switch (e.getAction()) {
        case MotionEvent.ACTION_MOVE:

          float dx = x - mPreviousX;
          float dy = y - mPreviousY;

          // reverse direction of rotation above the mid-line
          if (y > getHeight() / 2) {
            dx = dx * -1;
          }

          // reverse direction of rotation to left of the mid-line
          if (x < getWidth() / 2) {
            dy = dy * -1;
          }

          mRenderer.setAngle(
            mRenderer.getAngle() +
              ((dx + dy) * TOUCH_SCALE_FACTOR));  // = 180.0f / 320
          requestRender();
      }

      mPreviousX = x;
      mPreviousY = y;
      return true;
    }

  }

  public static class MyGLRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "MyGLRenderer";
    private Triangle mTriangle;
    private Square mSquare;

    // mMVPMatrix is an abbreviation for "Model View Projection Matrix"
    private final float[] mMVPMatrix = new float[16];
    private final float[] mProjectionMatrix = new float[16];
    private final float[] mViewMatrix = new float[16];
    private final float[] mRotationMatrix = new float[16];

    private float mAngle;

    @Override
    public void onSurfaceCreated(GL10 unused, EGLConfig config) {

      // Set the background frame color
      GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

      mTriangle = new Triangle();
      mSquare = new Square();
    }

    @Override
    public void onDrawFrame(GL10 unused) {
      float[] scratch = new float[16];

      // Draw background color
      GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

      // Set the camera position (View matrix)
      Matrix.setLookAtM(mViewMatrix, 0, 0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

      // Calculate the projection and view transformation
      Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mViewMatrix, 0);

      // Draw square
      mSquare.draw(mMVPMatrix);

      // Create a rotation for the triangle

      // Use the following code to generate constant rotation.
      // Leave this code out when using TouchEvents.
      // long time = SystemClock.uptimeMillis() % 4000L;
      // float angle = 0.090f * ((int) time);

      Matrix.setRotateM(mRotationMatrix, 0, mAngle, 0, 0, 1.0f);

      // Combine the rotation matrix with the projection and camera view
      // Note that the mMVPMatrix factor *must be first* in order
      // for the matrix multiplication product to be correct.
      Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);

      // Draw triangle
      mTriangle.draw(scratch);
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
      // Adjust the viewport based on geometry changes,
      // such as screen rotation
      GLES20.glViewport(0, 0, width, height);

      float ratio = (float) width / height;

      // this projection matrix is applied to object coordinates
      // in the onDrawFrame() method
      Matrix.frustumM(mProjectionMatrix, 0, -ratio, ratio, -1, 1, 3, 7);

    }

    /**
     * Utility method for compiling a OpenGL shader.
     * <p>
     * <p><strong>Note:</strong> When developing shaders, use the checkGlError()
     * method to debug shader coding errors.</p>
     *
     * @param type       - Vertex or fragment shader type.
     * @param shaderCode - String containing the shader code.
     * @return - Returns an id for the shader.
     */
    public static int loadShader(int type, String shaderCode) {

      // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
      // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
      int shader = GLES20.glCreateShader(type);

      // add the source code to the shader and compile it
      GLES20.glShaderSource(shader, shaderCode);
      GLES20.glCompileShader(shader);

      return shader;
    }

    /**
     * Utility method for debugging OpenGL calls. Provide the name of the call
     * just after making it:
     * <p>
     * <pre>
     * mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
     * MyGLRenderer.checkGlError("glGetUniformLocation");</pre>
     * <p>
     * If the operation is not successful, the check throws an error.
     *
     * @param glOperation - Name of the OpenGL call to check.
     */
    public static void checkGlError(String glOperation) {
      int error;
      while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
        Log.e(TAG, glOperation + ": glError " + error);
        throw new RuntimeException(glOperation + ": glError " + error);
      }
    }

    /**
     * Returns the rotation angle of the triangle shape (mTriangle).
     *
     * @return - A float representing the rotation angle.
     */
    public float getAngle() {
      return mAngle;
    }

    /**
     * Sets the rotation angle of the triangle shape (mTriangle).
     */
    public void setAngle(float angle) {
      mAngle = angle;
    }

  }

  private static class Square {

    private final String vertexShaderCode =
      // This matrix member variable provides a hook to manipulate
      // the coordinates of the objects that use this vertex shader
      "uniform mat4 uMVPMatrix;" +
        "attribute vec4 vPosition;" +
        "void main() {" +
        // The matrix must be included as a modifier of gl_Position.
        // Note that the uMVPMatrix factor *must be first* in order
        // for the matrix multiplication product to be correct.
        "  gl_Position = uMVPMatrix * vPosition;" +
        "}";

    private final String fragmentShaderCode =
      "precision mediump float;" +
        "uniform vec4 vColor;" +
        "void main() {" +
        "  gl_FragColor = vColor;" +
        "}";

    private final FloatBuffer vertexBuffer;
    private final ShortBuffer drawListBuffer;
    private final int mProgram;
    private int mPositionHandle;
    private int mColorHandle;
    private int mMVPMatrixHandle;

    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 3;
    static float squareCoords[] = {
      -0.5f, 0.5f, 0.0f,   // top left
      -0.5f, -0.5f, 0.0f,   // bottom left
      0.5f, -0.5f, 0.0f,   // bottom right
      0.5f, 0.5f, 0.0f}; // top right

    private final short drawOrder[] = {0, 1, 2, 0, 2, 3}; // order to draw vertices

    private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex

    float color[] = {0.2f, 0.709803922f, 0.898039216f, 1.0f};

    /**
     * Sets up the drawing object data for use in an OpenGL ES context.
     */
    public Square() {
      // initialize vertex byte buffer for shape coordinates
      ByteBuffer bb = ByteBuffer.allocateDirect(
        // (# of coordinate values * 4 bytes per float)
        squareCoords.length * 4);
      bb.order(ByteOrder.nativeOrder());
      vertexBuffer = bb.asFloatBuffer();
      vertexBuffer.put(squareCoords);
      vertexBuffer.position(0);

      // initialize byte buffer for the draw list
      ByteBuffer dlb = ByteBuffer.allocateDirect(
        // (# of coordinate values * 2 bytes per short)
        drawOrder.length * 2);
      dlb.order(ByteOrder.nativeOrder());
      drawListBuffer = dlb.asShortBuffer();
      drawListBuffer.put(drawOrder);
      drawListBuffer.position(0);

      // prepare shaders and OpenGL program
      int vertexShader = MyGLRenderer.loadShader(
        GLES20.GL_VERTEX_SHADER,
        vertexShaderCode);
      int fragmentShader = MyGLRenderer.loadShader(
        GLES20.GL_FRAGMENT_SHADER,
        fragmentShaderCode);

      mProgram = GLES20.glCreateProgram();             // create empty OpenGL Program
      GLES20.glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
      GLES20.glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
      GLES20.glLinkProgram(mProgram);                  // create OpenGL program executables
    }

    /**
     * Encapsulates the OpenGL ES instructions for drawing this shape.
     *
     * @param mvpMatrix - The Model View Project matrix in which to draw
     *                  this shape.
     */
    public void draw(float[] mvpMatrix) {
      // Add program to OpenGL environment
      GLES20.glUseProgram(mProgram);

      // get handle to vertex shader's vPosition member
      mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");

      // Enable a handle to the triangle vertices
      GLES20.glEnableVertexAttribArray(mPositionHandle);

      // Prepare the triangle coordinate data
      GLES20.glVertexAttribPointer(
        mPositionHandle, COORDS_PER_VERTEX,
        GLES20.GL_FLOAT, false,
        vertexStride, vertexBuffer);

      // get handle to fragment shader's vColor member
      mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");

      // Set color for drawing the triangle
      GLES20.glUniform4fv(mColorHandle, 1, color, 0);

      // get handle to shape's transformation matrix
      mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
      MyGLRenderer.checkGlError("glGetUniformLocation");

      // Apply the projection and view transformation
      GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
      MyGLRenderer.checkGlError("glUniformMatrix4fv");

      // Draw the square
      GLES20.glDrawElements(
        GLES20.GL_TRIANGLES, drawOrder.length,
        GLES20.GL_UNSIGNED_SHORT, drawListBuffer);

      // Disable vertex array
      GLES20.glDisableVertexAttribArray(mPositionHandle);
    }

  }

  /**
   * A two-dimensional triangle for use as a drawn object in OpenGL ES 2.0.
   */
  private static class Triangle {

    private final String vertexShaderCode =
      // This matrix member variable provides a hook to manipulate
      // the coordinates of the objects that use this vertex shader
      "uniform mat4 uMVPMatrix;" +
        "attribute vec4 vPosition;" +
        "void main() {" +
        // the matrix must be included as a modifier of gl_Position
        // Note that the uMVPMatrix factor *must be first* in order
        // for the matrix multiplication product to be correct.
        "  gl_Position = uMVPMatrix * vPosition;" +
        "}";

    private final String fragmentShaderCode =
      "precision mediump float;" +
        "uniform vec4 vColor;" +
        "void main() {" +
        "  gl_FragColor = vColor;" +
        "}";

    private final FloatBuffer vertexBuffer;
    private final int mProgram;
    private int mPositionHandle;
    private int mColorHandle;
    private int mMVPMatrixHandle;

    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 3;
    static float triangleCoords[] = {
      // in counterclockwise order:
      0.0f, 0.622008459f, 0.0f,   // top
      -0.5f, -0.311004243f, 0.0f,   // bottom left
      0.5f, -0.311004243f, 0.0f    // bottom right
    };
    private final int vertexCount = triangleCoords.length / COORDS_PER_VERTEX;
    private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex

    float color[] = {0.63671875f, 0.76953125f, 0.22265625f, 0.0f};

    /**
     * Sets up the drawing object data for use in an OpenGL ES context.
     */
    public Triangle() {
      // initialize vertex byte buffer for shape coordinates
      ByteBuffer bb = ByteBuffer.allocateDirect(
        // (number of coordinate values * 4 bytes per float)
        triangleCoords.length * 4);
      // use the device hardware's native byte order
      bb.order(ByteOrder.nativeOrder());

      // create a floating point buffer from the ByteBuffer
      vertexBuffer = bb.asFloatBuffer();
      // add the coordinates to the FloatBuffer
      vertexBuffer.put(triangleCoords);
      // set the buffer to read the first coordinate
      vertexBuffer.position(0);

      // prepare shaders and OpenGL program
      int vertexShader = MyGLRenderer.loadShader(
        GLES20.GL_VERTEX_SHADER, vertexShaderCode);
      int fragmentShader = MyGLRenderer.loadShader(
        GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

      mProgram = GLES20.glCreateProgram();             // create empty OpenGL Program
      GLES20.glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
      GLES20.glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
      GLES20.glLinkProgram(mProgram);                  // create OpenGL program executables

    }

    /**
     * Encapsulates the OpenGL ES instructions for drawing this shape.
     *
     * @param mvpMatrix - The Model View Project matrix in which to draw
     *                  this shape.
     */
    public void draw(float[] mvpMatrix) {
      // Add program to OpenGL environment
      GLES20.glUseProgram(mProgram);

      // get handle to vertex shader's vPosition member
      mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");

      // Enable a handle to the triangle vertices
      GLES20.glEnableVertexAttribArray(mPositionHandle);

      // Prepare the triangle coordinate data
      GLES20.glVertexAttribPointer(
        mPositionHandle, COORDS_PER_VERTEX,
        GLES20.GL_FLOAT, false,
        vertexStride, vertexBuffer);

      // get handle to fragment shader's vColor member
      mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");

      // Set color for drawing the triangle
      GLES20.glUniform4fv(mColorHandle, 1, color, 0);

      // get handle to shape's transformation matrix
      mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
      MyGLRenderer.checkGlError("glGetUniformLocation");

      // Apply the projection and view transformation
      GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
      MyGLRenderer.checkGlError("glUniformMatrix4fv");

      // Draw the triangle
      GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, vertexCount);

      // Disable vertex array
      GLES20.glDisableVertexAttribArray(mPositionHandle);
    }

  }
}