package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.maps.MapboxMap;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

public class AnnotationTest {

  @InjectMocks
  private MapboxMap mapboxMap = mock(MapboxMap.class);
  private Annotation annotation;
  private Annotation compare = new Annotation() {
    @Override
    public long getId() {
      return 1;
    }
  };

  @Before
  public void beforeTest() {
    annotation = new Annotation() {
      // empty child
    };
  }

  @Test
  public void testSanity() {
    assertNotNull("markerOptions should not be null", annotation);
  }

  @Test
  public void testRemove() {
    annotation.setId(1);
    annotation.setMapboxMap(mapboxMap);
    annotation.remove();
    verify(mapboxMap, times(1)).removeAnnotation(annotation);
  }

  @Test
  public void testRemoveUnboundMapboxMap() {
    annotation.setId(1);
    annotation.remove();
    verify(mapboxMap, times(0)).removeAnnotation(annotation);
  }

  @Test
  public void testCompareToEqual() {
    annotation.setId(1);
    assertEquals("conparable equal", 0, annotation.compareTo(compare));
  }

  @Test
  public void testCompareToHigher() {
    annotation.setId(3);
    assertEquals("conparable higher", -1, annotation.compareTo(compare));
  }

  @Test
  public void testCompareTolower() {
    annotation.setId(0);
    assertEquals("conparable lower", 1, annotation.compareTo(compare));
  }

  @Test
  public void testEquals() {
    Annotation holder = null;
    assertFalse(annotation.equals(holder));
    holder = annotation;
    assertTrue(annotation.equals(holder));
    assertFalse(annotation.equals(new Object()));
  }

  @Test
  public void testHashcode() {
    int id = 1;
    annotation.setId(id);
    assertSame("hashcode should match", annotation.hashCode(), id);
  }

}
