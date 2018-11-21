package com.mapbox.mapboxsdk.testapp.activity.render;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;
import okio.BufferedSource;
import okio.Okio;
import timber.log.Timber;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Activity that generates map snapshots based on the node render test suite.
 */
public class RenderTestActivity extends AppCompatActivity {

  private static final String TEST_BASE_PATH = "integration";
  private static final String RENDER_TEST_BASE_PATH = TEST_BASE_PATH + "/render-tests";

  // We additionally read out excluded tests from `/platform/node/test/ignore.json`
  private static final List<String> EXCLUDED_TESTS = new ArrayList<String>() {
    {
      add("overlay,background-opacity");
      add("collision-lines-pitched,debug");
      add("1024-circle,extent");
      add("empty,empty");
      add("rotation-alignment-map,icon-pitch-scaling");
      add("rotation-alignment-viewport,icon-pitch-scaling");
      add("pitch15,line-pitch");
      add("pitch30,line-pitch");
      add("line-placement-true-pitched,text-keep-upright");
      add("180,raster-rotation");
      add("45,raster-rotation");
      add("90,raster-rotation");
      add("overlapping,raster-masking");
      add("missing,raster-loading");
      add("pitchAndBearing,line-pitch");
      add("overdraw,sparse-tileset");
    }
  };

  private final Map<RenderTestDefinition, Bitmap> renderResultMap = new HashMap<>();
  private List<RenderTestDefinition> renderTestDefinitions;
  private OnRenderTestCompletionListener onRenderTestCompletionListener;
  private MapSnapshotter mapSnapshotter;
  private ImageView imageView;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(imageView = new ImageView(RenderTestActivity.this));
    imageView.setLayoutParams(new FrameLayout.LayoutParams(
      ViewGroup.LayoutParams.MATCH_PARENT,
      ViewGroup.LayoutParams.MATCH_PARENT, Gravity.CENTER)
    );
  }

  @Override
  protected void onStop() {
    super.onStop();
    if (mapSnapshotter != null) {
      mapSnapshotter.cancel();
    }
  }

  //
  // Loads the ignore tests from assets folder
  //
  private static class LoadRenderIgnoreTask extends AsyncTask<Void, Void, List<String>> {

    private WeakReference<RenderTestActivity> renderTestActivityWeakReference;

    LoadRenderIgnoreTask(RenderTestActivity renderTestActivity) {
      this.renderTestActivityWeakReference = new WeakReference<>(renderTestActivity);
    }

    @Override
    protected List<String> doInBackground(Void... voids) {
      return loadIgnoreList(renderTestActivityWeakReference.get().getAssets());
    }

    @Override
    protected void onPostExecute(List<String> strings) {
      super.onPostExecute(strings);
      if (renderTestActivityWeakReference.get() != null) {
        renderTestActivityWeakReference.get().onLoadIgnoreList(strings);
      }
    }
  }


  //
  // Loads the render test definitions from assets folder
  //
  private static class LoadRenderDefinitionTask extends AsyncTask<Void, Void, List<RenderTestDefinition>> {

    private WeakReference<RenderTestActivity> renderTestActivityWeakReference;

    LoadRenderDefinitionTask(RenderTestActivity renderTestActivity) {
      this.renderTestActivityWeakReference = new WeakReference<>(renderTestActivity);
    }

    @Override
    protected List<RenderTestDefinition> doInBackground(Void... voids) {
      List<RenderTestDefinition> definitions = new ArrayList<>();
      AssetManager assetManager = renderTestActivityWeakReference.get().getAssets();
      String[] categories = new String[0];
      try {
        categories = assetManager.list(RENDER_TEST_BASE_PATH);
      } catch (IOException exception) {
        Timber.e(exception);
      }
      for (int counter = categories.length - 1; counter >= 0; counter--) {
        try {
          String[] tests = assetManager.list(String.format("%s/%s", RENDER_TEST_BASE_PATH, categories[counter]));
          for (String test : tests) {
            String styleJson = loadStyleJson(assetManager, categories[counter], test);
            RenderTestStyleDefinition renderTestStyleDefinition = new Gson()
              .fromJson(styleJson, RenderTestStyleDefinition.class);
            RenderTestDefinition definition = new RenderTestDefinition(
              categories[counter], test, styleJson, renderTestStyleDefinition);
            if (!definition.hasOperations()) {
              if (!EXCLUDED_TESTS.contains(definition.getName() + "," + definition.getCategory())) {
                definitions.add(definition);
              }
            } else {
              Timber.e("could not add test, test requires operations: %s from %s", test, categories[counter]);
            }
          }
        } catch (Exception exception) {
          Timber.e(exception);
        }
      }
      return definitions;
    }

    @Override
    protected void onPostExecute(List<RenderTestDefinition> renderTestDefinitions) {
      super.onPostExecute(renderTestDefinitions);
      RenderTestActivity renderTestActivity = renderTestActivityWeakReference.get();
      if (renderTestActivity != null) {
        renderTestActivity.startRenderTests(renderTestDefinitions);
      }
    }
  }

  private static List<String> loadIgnoreList(AssetManager assets) {
    List<String> ignores = new ArrayList<>();
    try (InputStream input = assets.open(String.format("%s/ignores.json", TEST_BASE_PATH))) {
      BufferedSource source = Okio.buffer(Okio.source(input));
      String styleJson = source.readByteString().string(Charset.forName("utf-8"));
      JsonObject object = new Gson().fromJson(styleJson, JsonObject.class);
      for (Map.Entry<String, JsonElement> stringJsonElementEntry : object.entrySet()) {
        String[] parts = stringJsonElementEntry.getKey().split("/");
        ignores.add(String.format("%s,%s", parts[2], parts[1]));
      }
    } catch (IOException exception) {
      Timber.e(exception);
    }
    return ignores;
  }

  private static String loadStyleJson(AssetManager assets, String category, String test) {
    String styleJson = null;
    try (InputStream input = assets.open(String.format("%s/%s/%s/style.json", RENDER_TEST_BASE_PATH, category, test))) {
      BufferedSource source = Okio.buffer(Okio.source(input));
      styleJson = source.readByteString().string(Charset.forName("utf-8"));
    } catch (IOException exception) {
      Timber.e(exception);
    }
    return styleJson;
  }

  private void startRenderTests(List<RenderTestDefinition> renderTestDefinitions) {
    this.renderTestDefinitions = renderTestDefinitions;
    if (!renderTestDefinitions.isEmpty()) {
      render(renderTestDefinitions.get(0), renderTestDefinitions.size());
    } else {
      // no definitions, finish test without rendering
      onRenderTestCompletionListener.onFinish();
    }
  }

  private void render(final RenderTestDefinition renderTestDefinition, final int testSize) {
    Timber.d("Render test %s,%s", renderTestDefinition.getName(), renderTestDefinition.getCategory());
    mapSnapshotter = new RenderTestSnapshotter(this, renderTestDefinition.toOptions());
    mapSnapshotter.start(result -> {
      Bitmap snapshot = result.getBitmap();
      imageView.setImageBitmap(snapshot);
      renderResultMap.put(renderTestDefinition, snapshot);
      if (renderResultMap.size() != testSize) {
        continueTesting(renderTestDefinition);
      } else {
        finishTesting();
      }
    }, error -> Timber.e(error));
  }

  private void continueTesting(RenderTestDefinition renderTestDefinition) {
    int next = renderTestDefinitions.indexOf(renderTestDefinition) + 1;
    Timber.d("Next test: %s / %s", next, renderTestDefinitions.size());
    render(renderTestDefinitions.get(next), renderTestDefinitions.size());
  }

  private void finishTesting() {
    new SaveResultToDiskTask(onRenderTestCompletionListener, renderResultMap).execute();
  }

  //
  // Save tests results to disk
  //
  private static class SaveResultToDiskTask extends AsyncTask<Void, Void, Void> {

    private OnRenderTestCompletionListener onRenderTestCompletionListener;
    private Map<RenderTestDefinition, Bitmap> renderResultMap;

    SaveResultToDiskTask(OnRenderTestCompletionListener onRenderTestCompletionListener,
                         Map<RenderTestDefinition, Bitmap> renderResultMap) {
      this.onRenderTestCompletionListener = onRenderTestCompletionListener;
      this.renderResultMap = renderResultMap;
    }

    @Override
    protected Void doInBackground(Void... voids) {
      if (isExternalStorageWritable()) {
        try {
          File testResultDir = FileUtils.createTestResultRootFolder();
          String basePath = testResultDir.getAbsolutePath();
          for (Map.Entry<RenderTestDefinition, Bitmap> testResult : renderResultMap.entrySet()) {
            writeResultToDisk(basePath, testResult);
          }
        } catch (final Exception exception) {
          Timber.e(exception);
        }
      }
      return null;
    }

    private void writeResultToDisk(String path, Map.Entry<RenderTestDefinition, Bitmap> result) {
      RenderTestDefinition definition = result.getKey();
      String categoryName = definition.getCategory();
      String categoryPath = String.format("%s/%s", path, categoryName);
      FileUtils.createCategoryDirectory(categoryPath);
      String testName = result.getKey().getName();
      String testDir = FileUtils.createTestDirectory(categoryPath, testName);
      FileUtils.writeTestResultToDisk(testDir, result.getValue());
    }

    private boolean isExternalStorageWritable() {
      return Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState());
    }

    @Override
    protected void onPostExecute(Void aVoid) {
      super.onPostExecute(aVoid);
      if (onRenderTestCompletionListener != null) {
        onRenderTestCompletionListener.onFinish();
      }
    }
  }

  //
  // Callback configuration to notify test executor of test finishing
  //
  public interface OnRenderTestCompletionListener {
    void onFinish();
  }

  public void setOnRenderTestCompletionListener(OnRenderTestCompletionListener listener) {
    this.onRenderTestCompletionListener = listener;
    new LoadRenderIgnoreTask(this).execute();
  }

  public void onLoadIgnoreList(List<String> ignoreList) {
    Timber.e("We loaded %s of tests to be ignored", ignoreList.size());
    EXCLUDED_TESTS.addAll(ignoreList);
    new LoadRenderDefinitionTask(this).execute();
  }

  //
  // FileUtils
  //

  private static class FileUtils {

    private static void createCategoryDirectory(String catPath) {
      File testResultDir = new File(catPath);
      if (testResultDir.exists()) {
        return;
      }

      if (!testResultDir.mkdirs()) {
        throw new RuntimeException("can't create root test directory");
      }
    }

    private static File createTestResultRootFolder() {
      File testResultDir = new File(Environment.getExternalStorageDirectory()
        + File.separator + "mapbox" + File.separator + "render");
      if (testResultDir.exists()) {
        // cleanup old files
        deleteRecursive(testResultDir);
      }

      if (!testResultDir.mkdirs()) {
        throw new RuntimeException("can't create root test directory");
      }
      return testResultDir;
    }

    private static void deleteRecursive(File fileOrDirectory) {
      if (fileOrDirectory.isDirectory()) {
        File[] files = fileOrDirectory.listFiles();
        if (files != null) {
          for (File file : files) {
            deleteRecursive(file);
          }
        }
      }

      if (!fileOrDirectory.delete()) {
        Timber.e("can't delete directory");
      }
    }

    private static String createTestDirectory(String basePath, String testName) {
      File testDir = new File(basePath + "/" + testName);
      if (!testDir.exists()) {
        if (!testDir.mkdir()) {
          throw new RuntimeException("can't create sub directory for " + testName);
        }
      }
      return testDir.getAbsolutePath();
    }

    private static void writeTestResultToDisk(String testPath, Bitmap testResult) {
      String filePath = testPath + "/actual.png";
      try (FileOutputStream out = new FileOutputStream(filePath)) {
        testResult.compress(Bitmap.CompressFormat.PNG, 100, out);
      } catch (IOException exception) {
        Timber.e(exception);
      }
    }
  }
}