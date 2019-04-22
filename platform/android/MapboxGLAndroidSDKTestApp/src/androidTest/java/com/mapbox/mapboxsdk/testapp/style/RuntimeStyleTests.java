package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.graphics.PointF;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.RasterSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import junit.framework.Assert;
import org.hamcrest.Matcher;
import org.junit.Test;
import org.junit.runner.RunWith;
import timber.log.Timber;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

/**
 * Basic smoke tests for Layer and Source
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleTests extends EspressoTest {

  @Test
  public void testListLayers() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        List<Layer> layers = mapboxMap.getStyle().getLayers();
        assertNotNull(layers);
        assertTrue(layers.size() > 0);
        for (Layer layer : layers) {
          assertNotNull(layer);
        }
      }

    });
  }

  @Test
  public void testGetAddRemoveLayer() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new AddRemoveLayerAction());
  }

  @Test
  public void testAddLayerAbove() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {
      @Override
      public void perform(UiController uiController, View view) {
        List<Layer> layers = mapboxMap.getStyle().getLayers();
        Source source = mapboxMap.getStyle().getSources().get(0);

        // Test inserting with invalid above-id
        try {
          mapboxMap.getStyle().addLayerAbove(
            new CircleLayer("invalid-id-layer-test", source.getId()), "no-such-layer-here-man"
          );
          fail("Should have thrown exception");
        } catch (CannotAddLayerException ex) {
          // Yeah
          assertNotNull(ex.getMessage());
        }

        // Insert as last
        CircleLayer last = new CircleLayer("this is the last one", source.getId());
        mapboxMap.getStyle().addLayerAbove(last, layers.get(layers.size() - 1).getId());
        layers = mapboxMap.getStyle().getLayers();
        assertEquals(last.getId(), layers.get(layers.size() - 1).getId());

        // Insert
        CircleLayer second = new CircleLayer("this is the second one", source.getId());
        mapboxMap.getStyle().addLayerAbove(second, layers.get(0).getId());
        layers = mapboxMap.getStyle().getLayers();
        assertEquals(second.getId(), layers.get(1).getId());
      }
    });
  }

  @Test
  public void testRemoveLayerAt() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        // Remove by index
        Layer firstLayer = mapboxMap.getStyle().getLayers().get(0);
        boolean removed = mapboxMap.getStyle().removeLayerAt(0);
        assertTrue(removed);
        assertNotNull(firstLayer);

        // Test remove by index bounds checks
        Timber.i("Remove layer at index > size");
        assertFalse(mapboxMap.getStyle().removeLayerAt(Integer.MAX_VALUE));
      }
    });
  }

  public void testAddLayerAt() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {
      @Override
      public void perform(UiController uiController, View view) {
        List<Layer> layers = mapboxMap.getStyle().getLayers();
        Source source = mapboxMap.getStyle().getSources().get(0);

        // Test inserting out of range
        try {
          mapboxMap.getStyle().addLayerAt(new CircleLayer("invalid-id-layer-test", source.getId()), layers.size());
          fail("Should have thrown exception");
        } catch (CannotAddLayerException ex) {
          // Yeah
          assertNotNull(ex.getMessage());
        }

        // Insert at current last position
        CircleLayer last = new CircleLayer("this is the last one", source.getId());
        mapboxMap.getStyle().addLayerAt(last, layers.size() - 1);
        layers = mapboxMap.getStyle().getLayers();
        assertEquals(last.getId(), layers.get(layers.size() - 2).getId());

        // Insert at start
        CircleLayer second = new CircleLayer("this is the first one", source.getId());
        mapboxMap.getStyle().addLayerAt(second, 0);
        layers = mapboxMap.getStyle().getLayers();
        assertEquals(second.getId(), layers.get(0).getId());
      }
    });
  }


  @Test
  public void testListSources() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        List<Source> sources = mapboxMap.getStyle().getSources();
        assertNotNull(sources);
        assertTrue(sources.size() > 0);
        for (Source source : sources) {
          assertNotNull(source);
        }
      }

    });
  }

  @Test
  public void testAddRemoveSource() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));
      mapboxMap.getStyle().removeSource("my-source");

      // Add initial source
      mapboxMap.getStyle().addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));

      // Remove
      boolean removeOk = mapboxMap.getStyle().removeSource("my-source");
      assertTrue(removeOk);
      assertNull(mapboxMap.getStyle().getLayer("my-source"));

      // Add
      Source source = new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
      mapboxMap.getStyle().addSource(source);

      // Remove, preserving the reference
      mapboxMap.getStyle().removeSource(source);

      // Re-add the reference...
      mapboxMap.getStyle().addSource(source);

      // Ensure it's there
      Assert.assertNotNull(mapboxMap.getStyle().getSource(source.getId()));

      // Test adding a duplicate source
      try {
        Source source2 = new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
        mapboxMap.getStyle().addSource(source2);
        fail("Should not have been allowed to add a source with a duplicate id");
      } catch (CannotAddSourceException cannotAddSourceException) {
        // OK
      }
    });

  }

  @Test
  public void testVectorSourceUrlGetter() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      VectorSource source = new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
      mapboxMap.getStyle().addSource(source);
      assertEquals("mapbox://mapbox.mapbox-terrain-v2", source.getUrl());
    });
  }

  @Test
  public void testRasterSourceUrlGetter() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      RasterSource source = new RasterSource("my-source", "mapbox://mapbox.mapbox-terrain-v2");
      mapboxMap.getStyle().addSource(source);
      assertEquals("mapbox://mapbox.mapbox-terrain-v2", source.getUrl());
    });
  }

  @Test
  public void testGeoJsonSourceUrlGetter() throws MalformedURLException {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = new GeoJsonSource("my-source");
      mapboxMap.getStyle().addSource(source);
      assertNull(source.getUrl());
      try {
        source.setUrl(new URL("http://mapbox.com/my-file.json"));
      } catch (MalformedURLException exception) {
        fail();
      }
      assertEquals("http://mapbox.com/my-file.json", source.getUrl());
    });
  }

  @Test
  public void testRemoveSourceInUse() {
    validateTestSetup();

    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        mapboxMap.getStyle().addSource(new VectorSource("my-source", "mapbox://mapbox.mapbox-terrain-v2"));
        mapboxMap.getStyle().addLayer(new LineLayer("my-layer", "my-source"));
        mapboxMap.getStyle().removeSource("my-source");
        assertNotNull(mapboxMap.getStyle().getSource("my-source"));
      }

    });
  }

  @Test
  public void testRemoveNonExistingSource() {
    invoke(mapboxMap, (uiController, mapboxMap) -> mapboxMap.getStyle().removeSource("source"));
  }

  @Test
  public void testRemoveNonExistingLayer() {
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertFalse(mapboxMap.getStyle().removeLayer("layer"));
      assertFalse(mapboxMap.getStyle().removeLayerAt(mapboxMap.getStyle().getLayers().size() + 1));
      assertFalse(mapboxMap.getStyle().removeLayerAt(-1));
    });
  }

  @Test
  public void testRemoveExistingLayer() {
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      Layer firstLayer = mapboxMap.getStyle().getLayers().get(0);
      assertTrue(mapboxMap.getStyle().removeLayer(firstLayer));

      firstLayer = mapboxMap.getStyle().getLayers().get(0);
      assertTrue(mapboxMap.getStyle().removeLayer(firstLayer.getId()));

      assertTrue(mapboxMap.getStyle().removeLayerAt(0));
    });
  }

  /**
   * https://github.com/mapbox/mapbox-gl-native/issues/7973
   */
  @Test
  public void testQueryRenderedFeaturesInputHandling() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        String[] layerIds = new String[600];
        for (int i = 0; i < layerIds.length; i++) {
          layerIds[i] = "layer-" + i;
        }
        mapboxMap.queryRenderedFeatures(new PointF(100, 100), layerIds);
      }

    });
  }

  private class AddRemoveLayerAction extends BaseViewAction {

    @Override
    public void perform(UiController uiController, View view) {
      // Get initial
      assertNotNull(mapboxMap.getStyle().getLayer("building"));

      // Remove
      boolean removed = mapboxMap.getStyle().removeLayer("building");
      assertTrue(removed);
      assertNull(mapboxMap.getStyle().getLayer("building"));

      // Add
      FillLayer layer = new FillLayer("building", "composite");
      layer.setSourceLayer("building");
      mapboxMap.getStyle().addLayer(layer);
      assertNotNull(mapboxMap.getStyle().getLayer("building"));

      // Assure the reference still works
      layer.setProperties(PropertyFactory.visibility(Property.VISIBLE));

      // Remove, preserving the reference
      mapboxMap.getStyle().removeLayer(layer);

      // Property setters should still work
      layer.setProperties(PropertyFactory.fillColor(Color.RED));

      // Re-add the reference...
      mapboxMap.getStyle().addLayer(layer);

      // Ensure it's there
      Assert.assertNotNull(mapboxMap.getStyle().getLayer(layer.getId()));

      // Test adding a duplicate layer
      try {
        mapboxMap.getStyle().addLayer(new FillLayer("building", "composite"));
        fail("Should not have been allowed to add a layer with a duplicate id");
      } catch (CannotAddLayerException cannotAddLayerException) {
        // OK
      }
    }
  }

  public abstract class BaseViewAction implements ViewAction {

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

  }
}
