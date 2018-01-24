package com.mapbox.mapboxsdk.style.expressions;

import android.graphics.Color;

import com.mapbox.mapboxsdk.style.layers.PropertyFactory;

import org.junit.Test;

import java.util.Arrays;

import static com.mapbox.mapboxsdk.style.expressions.Expression.acos;
import static com.mapbox.mapboxsdk.style.expressions.Expression.all;
import static com.mapbox.mapboxsdk.style.expressions.Expression.any;
import static com.mapbox.mapboxsdk.style.expressions.Expression.array;
import static com.mapbox.mapboxsdk.style.expressions.Expression.asin;
import static com.mapbox.mapboxsdk.style.expressions.Expression.at;
import static com.mapbox.mapboxsdk.style.expressions.Expression.atan;
import static com.mapbox.mapboxsdk.style.expressions.Expression.bool;
import static com.mapbox.mapboxsdk.style.expressions.Expression.coalesce;
import static com.mapbox.mapboxsdk.style.expressions.Expression.color;
import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.cos;
import static com.mapbox.mapboxsdk.style.expressions.Expression.cubicBezier;
import static com.mapbox.mapboxsdk.style.expressions.Expression.division;
import static com.mapbox.mapboxsdk.style.expressions.Expression.downcase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.e;
import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.geometryType;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gte;
import static com.mapbox.mapboxsdk.style.expressions.Expression.has;
import static com.mapbox.mapboxsdk.style.expressions.Expression.heatmapDensity;
import static com.mapbox.mapboxsdk.style.expressions.Expression.id;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.length;
import static com.mapbox.mapboxsdk.style.expressions.Expression.let;
import static com.mapbox.mapboxsdk.style.expressions.Expression.linear;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.ln;
import static com.mapbox.mapboxsdk.style.expressions.Expression.ln2;
import static com.mapbox.mapboxsdk.style.expressions.Expression.log10;
import static com.mapbox.mapboxsdk.style.expressions.Expression.log2;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lte;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.max;
import static com.mapbox.mapboxsdk.style.expressions.Expression.min;
import static com.mapbox.mapboxsdk.style.expressions.Expression.mod;
import static com.mapbox.mapboxsdk.style.expressions.Expression.neq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.not;
import static com.mapbox.mapboxsdk.style.expressions.Expression.number;
import static com.mapbox.mapboxsdk.style.expressions.Expression.object;
import static com.mapbox.mapboxsdk.style.expressions.Expression.pi;
import static com.mapbox.mapboxsdk.style.expressions.Expression.pow;
import static com.mapbox.mapboxsdk.style.expressions.Expression.product;
import static com.mapbox.mapboxsdk.style.expressions.Expression.properties;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgba;
import static com.mapbox.mapboxsdk.style.expressions.Expression.sin;
import static com.mapbox.mapboxsdk.style.expressions.Expression.sqrt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.step;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.string;
import static com.mapbox.mapboxsdk.style.expressions.Expression.subtract;
import static com.mapbox.mapboxsdk.style.expressions.Expression.sum;
import static com.mapbox.mapboxsdk.style.expressions.Expression.switchCase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.tan;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toBool;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toColor;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toNumber;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toRgba;
import static com.mapbox.mapboxsdk.style.expressions.Expression.typeOf;
import static com.mapbox.mapboxsdk.style.expressions.Expression.upcase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.var;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static junit.framework.Assert.assertTrue;
import static org.junit.Assert.assertEquals;

/**
 * Expression unit tests that validate the expression output with the expected Object[]array representation.
 */
public class ExpressionTest {

  @Test
  public void testRgb() throws Exception {
    Object[] expected = new Object[] {"rgb", 0, 0, 0};
    Object[] actual = rgb(literal(0), literal(0), literal(0)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testRgbLiteral() throws Exception {
    Object[] expected = new Object[] {"rgb", 0, 0, 0};
    Object[] actual = rgb(0, 0, 0).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testRgba() throws Exception {
    Object[] expected = new Object[] {"rgba", 0, 0, 0, 1};
    Object[] actual = rgba(literal(0), literal(0), literal(0), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testRgbaLiteral() throws Exception {
    Object[] expected = new Object[] {"rgba", 0, 0, 0, 1};
    Object[] actual = rgba(0, 0, 0, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testToRgba() throws Exception {
    Object[] expected = new Object[] {"to-rgba", PropertyFactory.colorToRgbaString(Color.RED)};
    Object[] actual = toRgba(color(Color.RED)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testEq() throws Exception {
    Object[] expected = new Object[] {"==", 1, 1};
    Object[] actual = eq(literal(1), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testEqLiteral() throws Exception {
    Object[] expected = new Object[] {"==", 1, 1};
    Object[] actual = eq(1, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testNeq() throws Exception {
    Object[] expected = new Object[] {"!=", 0, 1};
    Object[] actual = neq(literal(0), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testNeqLiteral() throws Exception {
    Object[] expected = new Object[] {"!=", 0, 1};
    Object[] actual = neq(0, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGt() throws Exception {
    Object[] expected = new Object[] {">", 0, 1};
    Object[] actual = gt(literal(0), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGtLiteral() throws Exception {
    Object[] expected = new Object[] {">", 0, 1};
    Object[] actual = gt(0, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLt() throws Exception {
    Object[] expected = new Object[] {"<", 1, 0};
    Object[] actual = lt(literal(1), literal(0)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLtLiteral() throws Exception {
    Object[] expected = new Object[] {"<", 1, 0};
    Object[] actual = lt(1, 0).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGte() throws Exception {
    Object[] expected = new Object[] {">=", 1, 1};
    Object[] actual = gte(literal(1), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGteLiteral() throws Exception {
    Object[] expected = new Object[] {">=", 1, 1};
    Object[] actual = gte(1, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLte() throws Exception {
    Object[] expected = new Object[] {"<=", 1, 1};
    Object[] actual = lte(literal(1), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLteLiteral() throws Exception {
    Object[] expected = new Object[] {"<=", 1, 1};
    Object[] actual = lte(1, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAll() throws Exception {
    Object[] expected = new Object[] {"all", true, true, true};
    Object[] actual = all(literal(true), literal(true), literal(true)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAny() throws Exception {
    Object[] expected = new Object[] {"any", true, false, false};
    Object[] actual = any(literal(true), literal(false), literal(false)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testNot() throws Exception {
    Object[] expected = new Object[] {"!", false};
    Object[] actual = not(literal(false)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testNotLiteral() throws Exception {
    Object[] expected = new Object[] {"!", false};
    Object[] actual = not(false).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSwitchCase() throws Exception {
    Object[] expectedCaseOneGet = new Object[] {"get", "key1"};
    Object[] expectedCaseOne = new Object[] {"==", expectedCaseOneGet, "value1"};
    Object[] expectedCaseTwoGet = new Object[] {"get", "key2"};
    Object[] expectedCaseTwo = new Object[] {"!=", expectedCaseTwoGet, "value2"};
    Object[] expected = new Object[] {"case", expectedCaseOne, expectedCaseTwo};

    Object[] actual = switchCase(
      eq(get(literal("key1")), literal("value1")),
      neq(get(literal("key2")), literal("value2"))
    ).toArray();

    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSwitchCaseLiteral() throws Exception {
    Object[] expectedCaseOneGet = new Object[] {"get", "key1"};
    Object[] expectedCaseOne = new Object[] {"==", expectedCaseOneGet, "value1"};
    Object[] expectedCaseTwoGet = new Object[] {"get", "key2"};
    Object[] expectedCaseTwo = new Object[] {"!=", expectedCaseTwoGet, "value2"};
    Object[] expected = new Object[] {"case", expectedCaseOne, expectedCaseTwo};

    Object[] actual = switchCase(
      eq(get("key1"), literal("value1")),
      neq(get("key2"), literal("value2"))
    ).toArray();

    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMatch() throws Exception {
    Object[] labelZero = new Object[] {"a", "output"};
    Object[] labelOne = new Object[] {"b", "output2"};
    Object[] labelTwo = new Object[] {"c", "output3"};

    Object[] expected = new Object[] {"match", labelZero, labelOne, labelTwo};
    Object[] actual = match(literal(labelZero), literal(labelOne), literal(labelTwo)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCoalesce() throws Exception {
    Object[] expectedGetOne = new Object[] {"get", "invalidKey"};
    Object[] expectedGetTwo = new Object[] {"get", "validKey"};
    Object[] expected = new Object[] {"coalesce", expectedGetOne, expectedGetTwo};

    Object[] actual = coalesce(
      get(literal("invalidKey")),
      get(literal("validKey"))
    ).toArray();

    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCoalesceLiteral() throws Exception {
    Object[] expectedGetOne = new Object[] {"get", "invalidKey"};
    Object[] expectedGetTwo = new Object[] {"get", "validKey"};
    Object[] expected = new Object[] {"coalesce", expectedGetOne, expectedGetTwo};

    Object[] actual = coalesce(
      get("invalidKey"),
      get("validKey")
    ).toArray();

    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testProperties() throws Exception {
    Object[] expected = new Object[] {"properties"};
    Object[] actual = properties().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGeometryType() throws Exception {
    Object[] expected = new Object[] {"geometry-type"};
    Object[] actual = geometryType().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testId() throws Exception {
    Object[] expected = new Object[] {"id"};
    Object[] actual = id().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHeatmapDensity() throws Exception {
    Object[] expected = new Object[] {"heatmap-density"};
    Object[] actual = heatmapDensity().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAt() throws Exception {
    Object[] expected = new Object[] {"at", 3, new Object[] {"one", "two"}};
    Object[] actual = at(literal(3), literal(new Object[] {"one", "two"})).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAtLiteral() throws Exception {
    Object[] expected = new Object[] {"at", 3, new Object[] {"one", "two"}};
    Object[] actual = at(3, literal(new Object[] {"one", "two"})).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAtExpression() throws Exception {
    Object[] expected = new Object[] {"at", 3, new Object[] {"properties"}};
    Object[] actual = at(literal(3), properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGet() throws Exception {
    Object[] expected = new Object[] {"get", "key"};
    Object[] actual = get(literal("key")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGetLiteral() throws Exception {
    Object[] expected = new Object[] {"get", "key"};
    Object[] actual = get("key").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGetObject() throws Exception {
    Object[] expected = new Object[] {"get", "key", new Object[] {"properties"}};
    Object[] actual = get(literal("key"), properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testGetObjectLiteral() throws Exception {
    Object[] expected = new Object[] {"get", "key", new Object[] {"properties"}};
    Object[] actual = get("key", properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHas() throws Exception {
    Object[] expected = new Object[] {"has", "key"};
    Object[] actual = has(literal("key")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHasLiteral() throws Exception {
    Object[] expected = new Object[] {"has", "key"};
    Object[] actual = has("key").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHasObject() throws Exception {
    Object[] expected = new Object[] {"has", "key", new Object[] {"properties"}};
    Object[] actual = has(literal("key"), properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHasObjectLiteral() throws Exception {
    Object[] expected = new Object[] {"has", "key", new Object[] {"properties"}};
    Object[] actual = has("key", properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHasExpression() throws Exception {
    Object[] expected = new Object[] {"has", new Object[] {"get", "key"}, new Object[] {"properties"}};
    Object[] actual = has(get(literal("key")), properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testHasExpressionLiteral() throws Exception {
    Object[] expected = new Object[] {"has", new Object[] {"get", "key"}, new Object[] {"properties"}};
    Object[] actual = has(get("key"), properties()).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLength() throws Exception {
    Object[] expected = new Object[] {"length", "key"};
    Object[] actual = length(literal("key")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLengthLiteral() throws Exception {
    Object[] expected = new Object[] {"length", "key"};
    Object[] actual = length("key").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLengthExpression() throws Exception {
    Object[] expected = new Object[] {"length", new Object[] {"get", "key"}};
    Object[] actual = length(get(literal("key"))).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLn2() throws Exception {
    Object[] expected = new Object[] {"ln2"};
    Object[] actual = ln2().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testPi() throws Exception {
    Object[] expected = new Object[] {"pi"};
    Object[] actual = pi().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testE() throws Exception {
    Object[] expected = new Object[] {"e"};
    Object[] actual = e().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSum() throws Exception {
    Object[] expected = new Object[] {"+", 1, 2};
    Object[] actual = sum(literal(1), literal(2)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSumLiteral() throws Exception {
    Object[] expected = new Object[] {"+", 1, 2};
    Object[] actual = sum(1, 2).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testProduct() throws Exception {
    Object[] expected = new Object[] {"*", 1, 2};
    Object[] actual = product(literal(1), literal(2)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testProductLiteral() throws Exception {
    Object[] expected = new Object[] {"*", 1, 2};
    Object[] actual = product(1, 2).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSubtract() throws Exception {
    Object[] expected = new Object[] {"-", 2, 1};
    Object[] actual = subtract(literal(2), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSubtractLiteral() throws Exception {
    Object[] expected = new Object[] {"-", 2, 1};
    Object[] actual = subtract(2, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testDivision() throws Exception {
    Object[] expected = new Object[] {"/", 2, 1};
    Object[] actual = division(literal(2), literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testDivisionLiteral() throws Exception {
    Object[] expected = new Object[] {"/", 2, 1};
    Object[] actual = division(2, 1).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testDivisionWithNestedGet() throws Exception {
    Object nestedGet = new Object[] {"get", "key"};
    Object[] expected = new Object[] {"/", 2, nestedGet};
    Object[] actual = division(literal(2), get(literal("key"))).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMod() throws Exception {
    Object[] expected = new Object[] {"%", 1, 3};
    Object[] actual = mod(literal(1), literal(3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testModLiteral() throws Exception {
    Object[] expected = new Object[] {"%", 1, 3};
    Object[] actual = mod(1, 3).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testPow() throws Exception {
    Object[] expected = new Object[] {"^", 2, 3};
    Object[] actual = pow(literal(2), literal(3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testPowLiteral() throws Exception {
    Object[] expected = new Object[] {"^", 2, 3};
    Object[] actual = pow(2, 3).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSqrt() throws Exception {
    Object[] expected = new Object[] {"sqrt", 4};
    Object[] actual = sqrt(literal(4)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSqrtLiteral() throws Exception {
    Object[] expected = new Object[] {"sqrt", 4};
    Object[] actual = sqrt(4).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLog10() throws Exception {
    Object[] expected = new Object[] {"log10", 10};
    Object[] actual = log10(literal(10)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLog10Literal() throws Exception {
    Object[] expected = new Object[] {"log10", 10};
    Object[] actual = log10(10).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLn() throws Exception {
    Object[] expected = new Object[] {"ln", 2};
    Object[] actual = ln(literal(2)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLnLiteral() throws Exception {
    Object[] expected = new Object[] {"ln", 2};
    Object[] actual = ln(2).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLog2() throws Exception {
    Object[] expected = new Object[] {"log2", 16};
    Object[] actual = log2(literal(16)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLog2Literal() throws Exception {
    Object[] expected = new Object[] {"log2", 16};
    Object[] actual = log2(16).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSin() throws Exception {
    Object[] expected = new Object[] {"sin", 45};
    Object[] actual = sin(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testSinLiteral() throws Exception {
    Object[] expected = new Object[] {"sin", 45};
    Object[] actual = sin(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCos() throws Exception {
    Object[] expected = new Object[] {"cos", 45};
    Object[] actual = cos(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCosLiteral() throws Exception {
    Object[] expected = new Object[] {"cos", 45};
    Object[] actual = cos(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testTan() throws Exception {
    Object[] expected = new Object[] {"tan", 45};
    Object[] actual = tan(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testTanLiteral() throws Exception {
    Object[] expected = new Object[] {"tan", 45};
    Object[] actual = tan(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAsin() throws Exception {
    Object[] expected = new Object[] {"asin", 45};
    Object[] actual = asin(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAsinLiteral() throws Exception {
    Object[] expected = new Object[] {"asin", 45};
    Object[] actual = asin(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAcos() throws Exception {
    Object[] expected = new Object[] {"acos", 45};
    Object[] actual = acos(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAcosLiteral() throws Exception {
    Object[] expected = new Object[] {"acos", 45};
    Object[] actual = acos(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAtan() throws Exception {
    Object[] expected = new Object[] {"atan", 45};
    Object[] actual = atan(literal(45)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testAtanLiteral() throws Exception {
    Object[] expected = new Object[] {"atan", 45};
    Object[] actual = atan(45).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMin() throws Exception {
    Object[] expected = new Object[] {"min", 0, 1, 2, 3};
    Object[] actual = min(literal(0), literal(1), literal(2), literal(3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMinLiteral() throws Exception {
    Object[] expected = new Object[] {"min", 0, 1, 2, 3};
    Object[] actual = min(0, 1, 2, 3).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMax() throws Exception {
    Object[] expected = new Object[] {"max", 0, 1, 2, 3};
    Object[] actual = max(literal(0), literal(1), literal(2), literal(3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testMaxLiteral() throws Exception {
    Object[] expected = new Object[] {"max", 0, 1, 2, 3};
    Object[] actual = max(0, 1, 2, 3).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testUpcase() throws Exception {
    Object[] expected = new Object[] {"upcase", "string"};
    Object[] actual = upcase(literal("string")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testUpcaseLiteral() throws Exception {
    Object[] expected = new Object[] {"upcase", "string"};
    Object[] actual = upcase("string").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testDowncase() throws Exception {
    Object[] expected = new Object[] {"downcase", "string"};
    Object[] actual = downcase(literal("string")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testDowncaseLiteral() throws Exception {
    Object[] expected = new Object[] {"downcase", "string"};
    Object[] actual = downcase("string").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testConcat() throws Exception {
    Object[] expected = new Object[] {"concat", "foo", "bar"};
    Object[] actual = concat(literal("foo"), literal("bar")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testConcatLiteral() throws Exception {
    Object[] expected = new Object[] {"concat", "foo", "bar"};
    Object[] actual = concat("foo", "bar").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testArray() throws Exception {
    Object[] get = new Object[] {"get", "keyToArray"};
    Object[] expected = new Object[] {"array", get};
    Object[] actual = array(get(literal("keyToArray"))).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testArrayLiteral() throws Exception {
    Object[] get = new Object[] {"get", "keyToArray"};
    Object[] expected = new Object[] {"array", get};
    Object[] actual = array(get("keyToArray")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testTypeOf() throws Exception {
    Object[] expected = new Object[] {"typeof", "value"};
    Object[] actual = typeOf(literal("value")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testString() throws Exception {
    Object[] expected = new Object[] {"string", "value"};
    Object[] actual = string(literal("value")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testNumber() throws Exception {
    Object[] expected = new Object[] {"number", 1};
    Object[] actual = number(literal(1)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testBool() throws Exception {
    Object[] expected = new Object[] {"boolean", true};
    Object[] actual = bool(literal(true)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testObject() throws Exception {
    Object object = new Object();
    Object[] expected = new Object[] {"object", object};
    Object[] actual = object(literal(object)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testToString() throws Exception {
    Object[] expected = new Object[] {"to-string", 3};
    Object[] actual = Expression.toString(literal(3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testToNumber() throws Exception {
    Object[] expected = new Object[] {"to-number", "3"};
    Object[] actual = toNumber(literal("3")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testToBool() throws Exception {
    Object[] expected = new Object[] {"to-boolean", "true"};
    Object[] actual = toBool(literal("true")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testToColor() throws Exception {
    Object[] expected = new Object[] {"to-color", "value"};
    Object[] actual = toColor(literal("value")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLet() throws Exception {
    Object[] expected = new Object[] {"let", "letName", "value"};
    Object[] actual = let(literal("letName"), literal("value")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testVar() throws Exception {
    Object[] expected = new Object[] {"var", "letName"};
    Object[] actual = var(literal("letName")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testVarLiteral() throws Exception {
    Object[] expected = new Object[] {"var", "letName"};
    Object[] actual = var("letName").toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testVarExpression() throws Exception {
    Object[] expected = new Object[] {"var", new Object[] {"get", "letName"}};
    Object[] actual = var(get(literal("letName"))).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testVarExpressionLiteral() throws Exception {
    Object[] expected = new Object[] {"var", new Object[] {"get", "letName"}};
    Object[] actual = var(get("letName")).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testZoom() throws Exception {
    Object[] expected = new Object[] {"zoom"};
    Object[] actual = zoom().toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testStepBasic() throws Exception {
    Object[] expected = new Object[] {"step", 12, 11, 0, 111, 1, 1111};
    Object[] actual = step(literal(12), literal(11), literal(0), literal(111), literal(1), literal(1111)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testStepBasicLiteral() throws Exception {
    Object[] expected = new Object[] {"step", new Object[] {"get", "line-width"}, 11, 0, 111, 1, 1111};
    Object[] actual = step(get("line-width"), literal(11), stop(0, 111), stop(1, 1111)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testStepExpression() throws Exception {
    Object[] input = new Object[] {"get", "key"};
    Object[] number = new Object[] {"to-number", input};
    Object[] expected = new Object[] {"step", number, 11, 0, 111, 1, 1111};
    Object[] actual = step(toNumber(get(literal("key"))),
      literal(11), literal(0), literal(111), literal(1), literal(1111)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testStepExpressionLiteral() throws Exception {
    Object[] input = new Object[] {"get", "key"};
    Object[] number = new Object[] {"to-number", input};
    Object[] expected = new Object[] {"step", number, 11, 0, 111, 1, 1111};
    Object[] actual = step(toNumber(get("key")), literal(11), stop(0, 111), stop(1, 1111)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLinear() throws Exception {
    Object[] stopZero = new Object[] {0, 1};
    Object[] stopOne = new Object[] {1, 2};
    Object[] stopTwo = new Object[] {2, 3};
    Object[] expected = new Object[] {"interpolate", new Object[] {"linear"}, 12, stopZero, stopOne, stopTwo};
    Object[] actual = interpolate(linear(), literal(12),
      literal(stopZero), literal(stopOne), literal(stopTwo)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testLinearStops() throws Exception {
    Object[] expected = new Object[] {"interpolate", new Object[] {"linear"}, 12, 0, 1, 1, 2, 2, 3};
    Object[] actual = interpolate(linear(), literal(12), stop(0, 1), stop(1, 2), stop(2, 3)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testExponential() throws Exception {
    Object[] exponential = new Object[] {"exponential", 12};
    Object[] get = new Object[] {"get", "x"};
    Object[] expected = new Object[] {"interpolate", exponential, get, 0, 100, 200};
    Object[] actual = interpolate(exponential(literal(12)),
      get(literal("x")), literal(0), literal(100), literal(200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testExponentialLiteral() throws Exception {
    Object[] exponential = new Object[] {"exponential", 12};
    Object[] get = new Object[] {"get", "x"};
    Object[] expected = new Object[] {"interpolate", exponential, get, 0, 100, 100, 200};
    Object[] actual = interpolate(exponential(12), get("x"), stop(0, 100), stop(100, 200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testExponentialExpressionLiteral() throws Exception {
    Object[] getX = new Object[] {"get", "x"};
    Object[] exponential = new Object[] {"exponential", getX};
    Object[] getY = new Object[] {"get", "y"};
    Object[] expected = new Object[] {"interpolate", exponential, getY, 0, 100, 100, 200};
    Object[] actual = interpolate(exponential(get("x")), get("y"), stop(0, 100), stop(100, 200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCubicBezier() throws Exception {
    Object[] cubicBezier = new Object[] {"cubic-bezier", 1, 1, 1, 1};
    Object[] get = new Object[] {"get", "x"};
    Object[] expected = new Object[] {"interpolate", cubicBezier, get, 0, 100, 100, 200};
    Object[] actual = interpolate(cubicBezier(literal(1), literal(1), literal(1), literal(1)),
      get(literal("x")), literal(0), literal(100), literal(100), literal(200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCubicBezierLiteral() throws Exception {
    Object[] cubicBezier = new Object[] {"cubic-bezier", 1, 1, 1, 1};
    Object[] get = new Object[] {"get", "x"};
    Object[] expected = new Object[] {"interpolate", cubicBezier, get, 0, 100, 100, 200};
    Object[] actual = interpolate(cubicBezier(1, 1, 1, 1), get("x"), stop(0, 100), stop(100, 200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCubicBezierExpression() throws Exception {
    Object[] getX = new Object[] {"get", "x"};
    Object[] getY = new Object[] {"get", "y"};
    Object[] getZ = new Object[] {"get", "z"};
    Object[] cubicBezier = new Object[] {"cubic-bezier", getZ, 1, getY, 1};
    Object[] expected = new Object[] {"interpolate", cubicBezier, getX, 0, 100, 200};
    Object[] actual = interpolate(cubicBezier(get(literal("z")), literal(1),
      get(literal("y")), literal(1)), get(literal("x")), literal(0), literal(100), literal(200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testCubicBezierExpressionLiteral() throws Exception {
    Object[] getX = new Object[] {"get", "x"};
    Object[] getY = new Object[] {"get", "y"};
    Object[] getZ = new Object[] {"get", "z"};
    Object[] cubicBezier = new Object[] {"cubic-bezier", getZ, 1, getY, 1};
    Object[] expected = new Object[] {"interpolate", cubicBezier, getX, 0, 100, 100, 200};
    Object[] actual = interpolate(cubicBezier(get("z"), literal(1), get("y"),
      literal(1)), get("x"), stop(0, 100), stop(100, 200)).toArray();
    assertTrue("expression should match", Arrays.deepEquals(expected, actual));
  }

  @Test
  public void testExpressionConcatToString() throws Exception {
    String expected = "[\"concat\", foo, bar]";
    String actual = concat(literal("foo"), literal("bar")).toString();
    assertEquals("toString should match", expected, actual);
  }

  @Test
  public void testExpressionMinToString() throws Exception {
    String expected = "[\"min\", 0, 1, 2, 3]";
    String actual = min(0, 1, 2, 3).toString();
    assertEquals("toString should match", expected, actual);
  }

  @Test
  public void testExpressionExponentialToString() throws Exception {
    String expected = "[\"interpolate\", [\"cubic-bezier\", 1, 1, 1, 1], [\"get\", x], 0, 100, 100, 200]";
    String actual = interpolate(cubicBezier(literal(1), literal(1), literal(1), literal(1)),
      get(literal("x")), literal(0), literal(100), literal(100), literal(200)).toString();
    assertEquals("toString should match", expected, actual);
  }
}