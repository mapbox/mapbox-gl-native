package com.mapbox.mapboxsdk.style.layers;

import org.junit.Test;

import static com.mapbox.mapboxsdk.style.layers.Filter.all;
import static com.mapbox.mapboxsdk.style.layers.Filter.any;
import static com.mapbox.mapboxsdk.style.layers.Filter.eq;
import static com.mapbox.mapboxsdk.style.layers.Filter.gt;
import static com.mapbox.mapboxsdk.style.layers.Filter.gte;
import static com.mapbox.mapboxsdk.style.layers.Filter.has;
import static com.mapbox.mapboxsdk.style.layers.Filter.in;
import static com.mapbox.mapboxsdk.style.layers.Filter.lt;
import static com.mapbox.mapboxsdk.style.layers.Filter.lte;
import static com.mapbox.mapboxsdk.style.layers.Filter.neq;
import static com.mapbox.mapboxsdk.style.layers.Filter.none;
import static com.mapbox.mapboxsdk.style.layers.Filter.notHas;
import static com.mapbox.mapboxsdk.style.layers.Filter.notIn;
import static org.junit.Assert.assertArrayEquals;

/**
 * Tests for Filter
 */
public class FilterTest {

  @Test
  public void testAll() {
    assertArrayEquals(all().toArray(), new Object[] {"all"});
    assertArrayEquals(
      all(eq("key", 2), neq("key", 3)).toArray(),
      new Object[] {"all", new Object[] {"==", "key", 2}, new Object[] {"!=", "key", 3}}
    );
  }

  @Test
  public void testAny() {
    assertArrayEquals(any().toArray(), new Object[] {"any"});
    assertArrayEquals(
      any(eq("key", 2), neq("key", 3)).toArray(),
      new Object[] {"any", new Object[] {"==", "key", 2}, new Object[] {"!=", "key", 3}}
    );
  }

  @Test
  public void testNone() {
    assertArrayEquals(none().toArray(), new Object[] {"none"});
    assertArrayEquals(
      none(eq("key", 2), neq("key", 3)).toArray(),
      new Object[] {"none", new Object[] {"==", "key", 2}, new Object[] {"!=", "key", 3}}
    );
  }

  @Test
  public void testHas() {
    assertArrayEquals(has("key").toArray(), new Object[] {"has", "key"});
  }

  @Test
  public void testHasNot() {
    assertArrayEquals(notHas("key").toArray(), new Object[] {"!has", "key"});
  }

  @Test
  public void testEq() {
    assertArrayEquals(eq("key", 1).toArray(), new Object[] {"==", "key", 1});

  }

  @Test
  public void testNeq() {
    assertArrayEquals(neq("key", 1).toArray(), new Object[] {"!=", "key", 1});
  }

  @Test
  public void testGt() {
    assertArrayEquals(gt("key", 1).toArray(), new Object[] {">", "key", 1});
  }

  @Test
  public void testGte() {
    assertArrayEquals(gte("key", 1).toArray(), new Object[] {">=", "key", 1});
  }

  @Test
  public void testLt() {
    assertArrayEquals(lt("key", 1).toArray(), new Object[] {"<", "key", 1});
  }

  @Test
  public void testLte() {
    assertArrayEquals(lte("key", 1).toArray(), new Object[] {"<=", "key", 1});
  }

  @Test
  public void testIn() {
    assertArrayEquals(in("key", 1, 2, "Aap").toArray(), new Object[] {"in", "key", 1, 2, "Aap"});
  }

  @Test
  public void testNotIn() {
    assertArrayEquals(notIn("key", 1, 2, "Noot").toArray(), new Object[] {"!in", "key", 1, 2, "Noot"});
  }
}
