package com.mapbox.mapboxsdk.style.expressions;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;

import com.mapbox.mapboxsdk.style.layers.PropertyFactory;

import java.util.ArrayList;
import java.util.List;

/**
 * The value for any layout property, paint property, or filter may be specified as an expression.
 * An expression defines a formula for computing the value of the property using the operators described below.
 * The set of expression operators provided by Mapbox GL includes:
 * <p>
 * <ul>
 * <li>Element</li>
 * <li>Mathematical operators for performing arithmetic and other operations on numeric values</li>
 * <li>Logical operators for manipulating boolean values and making conditional decisions</li>
 * <li>String operators for manipulating strings</li>
 * <li>Data operators, providing access to the properties of source features</li>
 * <li>Camera operators, providing access to the parameters defining the current map view</li>
 * </ul>
 * </p>
 * <p>
 * Expressions are represented as JSON arrays.
 * The first element of an expression array is a string naming the expression operator,
 * e.g. "*"or "case". Subsequent elements (if any) are the arguments to the expression.
 * Each argument is either a literal value (a string, number, boolean, or null), or another expression array.
 * </p>
 * <p>
 * Data expression: a data expression is any expression that access feature data -- that is,
 * any expression that uses one of the data operators:get,has,id,geometry-type, or properties.
 * Data expressions allow a feature's properties to determine its appearance.
 * They can be used to differentiate features within the same layer and to create data visualizations.
 * </p>
 * <p>
 * Camera expression: a camera expression is any expression that uses the zoom operator.
 * Such expressions allow the the appearance of a layer to change with the map's zoom level.
 * Camera expressions can be used to create the appearance of depth and to control data density.
 * </p>
 * <p>
 * Composition: a single expression may use a mix of data operators, camera operators, and other operators.
 * Such composite expressions allows a layer's appearance to be determined by
 * a combination of the zoom level and individual feature properties.
 * </p>
 *
 * @param <T> the type of the expression
 */
public class Expression<T> {

  private final String operator;
  private final Expression[] arguments;

  /**
   * Creates an empty expression for expression literals
   */
  Expression() {
    operator = null;
    arguments = null;
  }

  /**
   * Creates an expression from its operator and varargs expressions.
   *
   * @param operator  the expression operator
   * @param arguments expressions input
   */
  @SafeVarargs
  public Expression(@NonNull String operator, @Nullable Expression... arguments) {
    this.operator = operator;
    this.arguments = arguments;
  }

  /**
   * Converts the expression to Object array representation.
   * <p>
   * The output will later be converted to a JSON Object array.
   * </p>
   *
   * @return the converted object array expression
   */
  @NonNull
  public Object[] toArray() {
    List<Object> array = new ArrayList<>();
    array.add(operator);
    if (arguments != null) {
      for (Expression argument : arguments) {
        if (argument instanceof Expression.ExpressionLiteral) {
          array.add(toValue((ExpressionLiteral) argument));
        } else {
          array.add(argument.toArray());
        }
      }
    }
    return array.toArray();
  }

  /**
   * Converts the expression value to an Object.
   *
   * @param expressionValue the expression value to convert
   * @return the converted object expression
   */
  private Object toValue(ExpressionLiteral expressionValue) {
    Object value = expressionValue.toValue();
    if (value instanceof Expression.Color) {
      return ((Expression.Color) value).convertColor();
    } else if (value instanceof Expression.ExpressionLiteral) {
      return toValue((ExpressionLiteral) value);
    } else if (value instanceof Expression) {
      return ((Expression) value).toArray();
    }
    return value;
  }

  /**
   * ExpressionLiteral wraps an object to be used as a literal in an expression.
   * <p>
   * ExpressionLiteral is created with {@link #literal(Number)}, {@link #literal(boolean)},
   * {@link #literal(String)} and {@link #literal(Object)}.
   * </p>
   *
   * @param <T>
   */
  private static class ExpressionLiteral<T> extends Expression<T> {

    protected T object;

    /**
     * Create an ExpressionValue wrapper.
     *
     * @param object the object to be wrapped
     */
    ExpressionLiteral(@NonNull T object) {
      this.object = object;
    }

    /**
     * Get the wrapped object.
     *
     * @return the wrapped object
     */
    Object toValue() {
      return object;
    }
  }

  //
  // Types
  //

  /**
   * Expression interpolator type.
   * <p>
   * Is used for first parameter of {@link #interpolate(Expression, Expression, Stop...)}.
   * </p>
   */
  public static class Interpolator {
  }

  /**
   * Expression color type.
   */
  public static class Color {

    private int color;

    /**
     * Creates a color color type from a color int.
     *
     * @param color the int color
     */
    public Color(@ColorInt int color) {
      this.color = color;
    }

    /**
     * Converts the int color to rgba(d, d, d, d) string representation
     *
     * @return
     */
    public String convertColor() {
      return PropertyFactory.colorToRgbaString(color);
    }
  }

  /**
   * Expression array type.
   */
  public static class Array {
  }

  /**
   * Expression stop type.
   * <p>
   * Can be used for {@link #stop(Object, Object)} as part of varargs parameter in
   * {@link #step(Number, Expression, Stop...)} or {@link #interpolate(Expression, Expression, Stop...)}.
   * </p>
   */
  public static class Stop {

    private Object value;
    private Object output;

    public Stop(Object value, Object output) {
      this.value = value;
      this.output = output;
    }
  }

  //
  // Literals
  //

  /**
   * Create a literal number expression.
   *
   * @param number the number
   * @return the expression
   */
  public static Expression<Number> literal(@NonNull Number number) {
    return new ExpressionLiteral<>(number);
  }

  /**
   * Create a literal string expression.
   *
   * @param string the string
   * @return the expression
   */
  public static Expression<String> literal(@NonNull String string) {
    return new ExpressionLiteral<>(string);
  }

  /**
   * Create a literal boolean expression.
   *
   * @param bool the boolean
   * @return the expression
   */
  public static Expression<Boolean> literal(boolean bool) {
    return new ExpressionLiteral<>(bool);
  }

  /**
   * Create a literal object expression
   *
   * @param object the object
   * @return the expression
   */
  public static Expression<Object> literal(@NonNull Object object) {
    return new ExpressionLiteral<>(object);
  }

  //
  // Color
  //

  /**
   * Expression literal utility method to convert a color int to an color expression
   *
   * @param color the int color
   * @return the color expression
   */
  public static Expression<Color> color(@ColorInt int color) {
    return new ExpressionLiteral<>(new Color(color));
  }

  /**
   * Creates a color value from red, green, and blue components, which must range between 0 and 255,
   * and an alpha component of 1.
   * <p>
   * If any component is out of range, the expression is an error.
   * </p>
   *
   * @param red   red color expression
   * @param green green color expression
   * @param blue  blue color expression
   * @return expression
   */
  public static Expression<Color> rgb(@NonNull Expression<Number> red, @NonNull Expression<Number> green,
                                      @NonNull Expression<Number> blue) {
    return new Expression<>("rgb", red, green, blue);
  }

  /**
   * Creates a color value from red, green, and blue components, which must range between 0 and 255,
   * and an alpha component of 1.
   * <p>
   * If any component is out of range, the expression is an error.
   * </p>
   *
   * @param red   red color value
   * @param green green color value
   * @param blue  blue color value
   * @return expression
   */
  public static Expression<Color> rgb(@NonNull Number red, @NonNull Number green, @NonNull Number blue) {
    return rgb(literal(red), literal(green), literal(blue));
  }

  /**
   * Creates a color value from red, green, blue components, which must range between 0 and 255,
   * and an alpha component which must range between 0 and 1.
   * <p>
   * If any component is out of range, the expression is an error.
   * </p>
   *
   * @param red   red color value
   * @param green green color value
   * @param blue  blue color value
   * @param alpha alpha color value
   * @return expression
   */
  public static Expression<Color> rgba(@NonNull Expression<Number> red, @NonNull Expression<Number> green,
                                       @NonNull Expression<Number> blue, @NonNull Expression<Number> alpha) {
    return new Expression<>("rgba", red, green, blue, alpha);
  }

  /**
   * Creates a color value from red, green, blue components, which must range between 0 and 255,
   * and an alpha component which must range between 0 and 1.
   * <p>
   * If any component is out of range, the expression is an error.
   * </p>
   *
   * @param red   red color value
   * @param green green color value
   * @param blue  blue color value
   * @param alpha alpha color value
   * @return expression
   */
  public static Expression<Color> rgba(@NonNull Number red, @NonNull Number green, @NonNull Number blue, @NonNull Number alpha) {
    return rgba(literal(red), literal(green), literal(blue), literal(alpha));
  }

  /**
   * Returns a four-element array containing the input color's red, green, blue, and alpha components, in that order.
   *
   * @param expression an expression to convert to a color
   * @return expression
   */
  public static Expression<Array> toRgba(@NonNull Expression<Color> expression) {
    return new Expression<>("to-rgba", expression);
  }

  //
  // Decision
  //

  /**
   * Returns true if the input values are equal, false otherwise.
   * The inputs must be numbers, strings, or booleans, and both of the same type.
   *
   * @param compareOne the first expression
   * @param compareTwo the second expression
   * @return expression
   */
  public static Expression<Boolean> eq(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>("==", compareOne, compareTwo);
  }

  /**
   * Returns true if the input values are equal, false otherwise.
   *
   * @param compareOne the first boolean
   * @param compareTwo the second boolean
   * @return expression
   */
  public static Expression<Boolean> eq(boolean compareOne, boolean compareTwo) {
    return eq(literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the input values are equal, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> eq(@NonNull String compareOne, @NonNull String compareTwo) {
    return eq(literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the input values are equal, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> eq(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return eq(literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the input values are not equal, false otherwise.
   * The inputs must be numbers, strings, or booleans, and both of the same type.
   *
   * @param compareOne the first expression
   * @param compareTwo the second expression
   * @return expression
   */
  public static Expression<Boolean> neq(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>("!=", compareOne, compareTwo);
  }

  /**
   * Returns true if the input values are equal, false otherwise.
   *
   * @param compareOne the first boolean
   * @param compareTwo the second boolean
   * @return expression
   */
  public static Expression<Boolean> neq(boolean compareOne, boolean compareTwo) {
    return new Expression<>("!=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns `true` if the input values are not equal, `false` otherwise.
   *
   * @param compareOne the first string
   * @param compareTwo the second string
   * @return expression
   */
  public static Expression<Boolean> neq(@NonNull String compareOne, @NonNull String compareTwo) {
    return new Expression<>("!=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns `true` if the input values are not equal, `false` otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> neq(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return new Expression<>("!=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is strictly greater than the second, false otherwise.
   * The inputs must be numbers or strings, and both of the same type.
   *
   * @param compareOne the first expression
   * @param compareTwo the second expression
   * @return expression
   */
  public static Expression<Boolean> gt(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>(">", compareOne, compareTwo);
  }

  /**
   * Returns true if the first input is strictly greater than the second, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> gt(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return new Expression<>(">", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is strictly greater than the second, false otherwise.
   *
   * @param compareOne the first string
   * @param compareTwo the second string
   * @return expression
   */
  public static Expression<Boolean> gt(@NonNull String compareOne, @NonNull String compareTwo) {
    return new Expression<>(">", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is strictly less than the second, false otherwise.
   * The inputs must be numbers or strings, and both of the same type.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> lt(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>("<", compareOne, compareTwo);
  }

  /**
   * Returns true if the first input is strictly less than the second, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> lt(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return new Expression<>("<", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is strictly less than the second, false otherwise.
   *
   * @param compareOne the first string
   * @param compareTwo the second string
   * @return expression
   */
  public static Expression<Boolean> lt(@NonNull String compareOne, @NonNull String compareTwo) {
    return new Expression<>("<", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is greater than or equal to the second, false otherwise.
   * The inputs must be numbers or strings, and both of the same type.
   *
   * @param compareOne the first expression
   * @param compareTwo the second expression
   * @return expression
   */
  public static Expression<Boolean> gte(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>(">=", compareOne, compareTwo);
  }

  /**
   * Returns true if the first input is greater than or equal to the second, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> gte(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return new Expression<>(">=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is greater than or equal to the second, false otherwise.
   *
   * @param compareOne the first string
   * @param compareTwo the second string
   * @return expression
   */
  public static Expression<Boolean> gte(@NonNull String compareOne, @NonNull String compareTwo) {
    return new Expression<>(">=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is less than or equal to the second, false otherwise.
   * The inputs must be numbers or strings, and both of the same type.
   *
   * @param compareOne the first expression
   * @param compareTwo the second expression
   * @return expression
   */
  public static Expression<Boolean> lte(@NonNull Expression compareOne, @NonNull Expression compareTwo) {
    return new Expression<>("<=", compareOne, compareTwo);
  }

  /**
   * Returns true if the first input is less than or equal to the second, false otherwise.
   *
   * @param compareOne the first number
   * @param compareTwo the second number
   * @return expression
   */
  public static Expression<Boolean> lte(@NonNull Number compareOne, @NonNull Number compareTwo) {
    return new Expression<>("<=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns true if the first input is less than or equal to the second, false otherwise.
   *
   * @param compareOne the first string
   * @param compareTwo the second string
   * @return expression
   */
  public static Expression<Boolean> lte(@NonNull String compareOne, @NonNull String compareTwo) {
    return new Expression<>("<=", literal(compareOne), literal(compareTwo));
  }

  /**
   * Returns `true` if all the inputs are `true`, `false` otherwise.
   * <p>
   * The inputs are evaluated in order, and evaluation is short-circuiting:
   * once an input expression evaluates to `false`,
   * the result is `false` and no further input expressions are evaluated.
   * </p>
   *
   * @param input expression input
   * @return expression
   */
  @SafeVarargs
  public static Expression<Boolean> all(@NonNull Expression<Boolean>... input) {
    return new Expression<>("all", input);
  }

  /**
   * Returns `true` if any of the inputs are `true`, `false` otherwise.
   * <p>
   * The inputs are evaluated in order, and evaluation is short-circuiting:
   * once an input expression evaluates to `true`,
   * the result is `true` and no further input expressions are evaluated.
   * </p>
   *
   * @param input expression input
   * @return expression
   */
  @SafeVarargs
  public static Expression<Boolean> any(@NonNull Expression<Boolean>... input) {
    return new Expression<>("any", input);
  }

  /**
   * Logical negation. Returns `true` if the input is `false`, and `false` if the input is `true`.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> not(@NonNull Expression<Boolean> input) {
    return new Expression<>("!", input);
  }

  /**
   * Logical negation. Returns `true` if the input is `false`, and `false` if the input is `true`.
   *
   * @param input boolean input
   * @return expression
   */
  public static Expression<Boolean> not(boolean input) {
    return not(literal(input));
  }

  /**
   * Selects the first output whose corresponding test condition evaluates to true.
   *
   * @param input expression input
   * @return expression
   */
  @SafeVarargs
  public static Expression switchCase(@NonNull @Size(min = 1) Expression... input) {
    return new Expression("case", input);
  }

  /**
   * Selects the output whose label value matches the input value, or the fallback value if no match is found.
   * The `input` can be any string or number expression.
   * Each label can either be a single literal value or an array of values.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression match(@NonNull @Size(min = 2) Expression... input) {
    return new Expression("match", input);
  }

  /**
   * Selects the output whose label value matches the input value, or the fallback value if no match is found.
   * The `input` can be any string or number expression.
   * Each label can either be a single literal value or an array of values.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression match(@NonNull Expression input, @NonNull Stop... stops) {
    Expression[] expressions = new Expression[stops.length * 2];
    for (int i = 0; i < stops.length; i++) {
      expressions[i * 2] = literal(stops[i].value);
      expressions[i * 2 + 1] = literal(stops[i].output);
    }
    return match(join(new Expression[] {input}, expressions));
  }

  /**
   * Evaluates each expression in turn until the first non-null value is obtained, and returns that value.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression coalesce(@NonNull Expression... input) {
    return new Expression("coalesce", input);
  }

  //
  // FeatureData
  //

  /**
   * Gets the feature properties object.
   * <p>
   * Note that in some cases, it may be more efficient to use {@link #get(Expression)}} instead.
   * </p>
   *
   * @return expression
   */
  public static Expression<Object> properties() {
    return new Expression<>("properties");
  }

  /**
   * Gets the feature's geometry type: Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon.
   *
   * @return expression
   */
  public static Expression<String> geometryType() {
    return new Expression<>("geometry-type");
  }

  /**
   * Gets the feature's id, if it has one.
   *
   * @return expression
   */
  public static Expression<Number> id() {
    return new Expression<>("id");
  }

  //
  // Color ramp properties
  //

  /**
   * Gets the kernel density estimation of a pixel in a heatmap layer,
   * which is a relative measure of how many data points are crowded around a particular pixel.
   * Can only be used in the `heatmap-color` property.
   *
   * @return expression
   */
  public static Expression<Number> heatmapDensity() {
    return new Expression<>("heatmap-density");
  }

  /**
   * Gets the progress along a line feature in a line layer, if `line-gradient`
   * is used and correct distance properties are assigned to the feature.
   * Can only be used in the `line-gradient` property.
   *
   * @return expression
   */
  public static Expression<Number> lineProgress() {
    return new Expression<>("line-progress");
  }

  //
  // Lookup
  //

  /**
   * Retrieves an item from an array.
   *
   * @param number     the index expression
   * @param expression the array expression
   * @return expression
   */
  public static Expression<Object> at(@NonNull Expression<Number> number, @NonNull Expression expression) {
    return new Expression<>("at", number, expression);
  }

  /**
   * Retrieves an item from an array.
   *
   * @param number     the index expression
   * @param expression the array expression
   * @return expression
   */
  public static Expression<Object> at(@NonNull Number number, @NonNull Expression expression) {
    return at(literal(number), expression);
  }

  /**
   * Retrieves a property value from the current feature's properties,
   * or from another object if a second argument is provided.
   * Returns null if the requested property is missing.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression get(@NonNull Expression<String> input) {
    return new Expression<>("get", input);
  }

  /**
   * Retrieves a property value from the current feature's properties,
   * or from another object if a second argument is provided.
   * Returns null if the requested property is missing.
   *
   * @param input string input
   * @return expression
   */
  public static Expression get(@NonNull String input) {
    return get(literal(input));
  }

  /**
   * Retrieves a property value from another object.
   * Returns null if the requested property is missing.
   *
   * @param key    a property value key
   * @param object an expression object
   * @return expression
   */
  public static Expression<Object> get(@NonNull Expression<String> key, @NonNull Expression<Object> object) {
    return new Expression<>("get", key, object);
  }

  /**
   * Retrieves a property value from another object.
   * Returns null if the requested property is missing.
   *
   * @param key    a property value key
   * @param object an expression object
   * @return expression
   */
  public static Expression<Object> get(@NonNull String key, @NonNull Expression<Object> object) {
    return get(literal(key), object);
  }

  /**
   * Tests for the presence of an property value in the current feature's properties.
   *
   * @param key the expression property value key
   * @return expression
   */
  public static Expression<Boolean> has(@NonNull Expression<String> key) {
    return new Expression<>("has", key);
  }

  /**
   * Tests for the presence of an property value in the current feature's properties.
   *
   * @param key the property value key
   * @return expression
   */
  public static Expression<Boolean> has(@NonNull String key) {
    return has(literal(key));
  }

  /**
   * Tests for the presence of an property value from another object.
   *
   * @param key    the expression property value key
   * @param object an expression object
   * @return expression
   */
  public static Expression<Boolean> has(@NonNull Expression<String> key, @NonNull Expression<Object> object) {
    return new Expression<>("has", key, object);
  }

  /**
   * Tests for the presence of an property value from another object.
   *
   * @param key    the property value key
   * @param object an expression object
   * @return expression
   */
  public static Expression<Boolean> has(@NonNull String key, @NonNull Expression<Object> object) {
    return has(literal(key), object);
  }

  /**
   * Gets the length of an array or string.
   *
   * @param expression an expression object or expression string
   * @return expression
   */
  public static Expression<Number> length(@NonNull Expression<?> expression) {
    return new Expression<>("length", expression);
  }

  /**
   * Gets the length of an array or string.
   *
   * @param input a string
   * @return expression
   */
  public static Expression<Number> length(@NonNull String input) {
    return length(literal(input));
  }

  //
  // Math
  //

  /**
   * Returns mathematical constant ln(2).
   *
   * @return expression
   */
  public static Expression<Number> ln2() {
    return new Expression<>("ln2");
  }

  /**
   * Returns the mathematical constant pi.
   *
   * @return expression
   */
  public static Expression<Number> pi() {
    return new Expression<>("pi");
  }

  /**
   * Returns the mathematical constant e.
   *
   * @return expression
   */
  public static Expression<Number> e() {
    return new Expression<>("e");
  }

  /**
   * Returns the sum of the inputs.
   *
   * @param numbers the numbers to calculate the sum for
   * @return expression
   */
  @SafeVarargs
  public static Expression<Number> sum(@Size(min = 2) Expression<Number>... numbers) {
    return new Expression<>("+", numbers);
  }

  /**
   * Returns the sum of the inputs.
   *
   * @param numbers the numbers to calculate the sum for
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> sum(@Size(min = 2) Number... numbers) {
    Expression<Number>[] numberExpression = (Expression<Number>[]) new Expression<?>[numbers.length];
    for (int i = 0; i < numbers.length; i++) {
      numberExpression[i] = literal(numbers[i]);
    }
    return sum(numberExpression);
  }

  /**
   * Returns the product of the inputs.
   *
   * @param numbers the numbers to calculate the product for
   * @return expression
   */
  @SafeVarargs
  public static Expression<Number> product(@Size(min = 2) Expression<Number>... numbers) {
    return new Expression<>("*", numbers);
  }

  /**
   * Returns the product of the inputs.
   *
   * @param numbers the numbers to calculate the product for
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> product(@Size(min = 2) Number... numbers) {
    Expression<Number>[] numberExpression = (Expression<Number>[]) new Expression<?>[numbers.length];
    for (int i = 0; i < numbers.length; i++) {
      numberExpression[i] = literal(numbers[i]);
    }
    return product(numberExpression);
  }

  /**
   * Returns the result of subtracting a number from 0.
   *
   * @param number the number subtract from 0
   * @return expression
   */
  public static Expression<Number> subtract(@NonNull Expression<Number> number) {
    return new Expression<>("-", number);
  }

  /**
   * Returns the result of subtracting a number from 0.
   *
   * @param number the number subtract from 0
   * @return expression
   */
  public static Expression<Number> subtract(@NonNull Number number) {
    return subtract(literal(number));
  }

  /**
   * Returns the result of subtracting the second input from the first.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> subtract(@NonNull Expression<Number> first, @NonNull Expression<Number> second) {
    return new Expression<>("-", first, second);
  }

  /**
   * Returns the result of subtracting the second input from the first.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> subtract(@NonNull Number first, @NonNull Number second) {
    return subtract(literal(first), literal(second));
  }

  /**
   * Returns the result of floating point division of the first input by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> division(@NonNull Expression<Number> first, @NonNull Expression<Number> second) {
    return new Expression<>("/", first, second);
  }

  /**
   * Returns the result of floating point division of the first input by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> division(@NonNull Number first, @NonNull Number second) {
    return division(literal(first), literal(second));
  }

  /**
   * Returns the remainder after integer division of the first input by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> mod(@NonNull Expression<Number> first, @NonNull Expression<Number> second) {
    return new Expression<>("%", first, second);
  }

  /**
   * Returns the remainder after integer division of the first input by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> mod(@NonNull Number first, @NonNull Number second) {
    return mod(literal(first), literal(second));
  }

  /**
   * Returns the result of raising the first input to the power specified by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> pow(@NonNull Expression<Number> first, @NonNull Expression<Number> second) {
    return new Expression<>("^", first, second);
  }

  /**
   * Returns the result of raising the first input to the power specified by the second.
   *
   * @param first  the first number
   * @param second the second number
   * @return expression
   */
  public static Expression<Number> pow(@NonNull Number first, @NonNull Number second) {
    return pow(literal(first), literal(second));
  }

  /**
   * Returns the square root of the input
   *
   * @param number the number to take the square root from
   * @return expression
   */
  public static Expression<Number> sqrt(@NonNull Expression<Number> number) {
    return new Expression<>("sqrt", number);
  }

  /**
   * Returns the square root of the input
   *
   * @param number the number to take the square root from
   * @return expression
   */
  public static Expression<Number> sqrt(@NonNull Number number) {
    return sqrt(literal(number));
  }

  /**
   * Returns the base-ten logarithm of the input.
   *
   * @param number the number to take base-ten logarithm from
   * @return expression
   */
  public static Expression<Number> log10(@NonNull Expression<Number> number) {
    return new Expression<>("log10", number);
  }

  /**
   * Returns the base-ten logarithm of the input.
   *
   * @param number the number to take base-ten logarithm from
   * @return expression
   */
  public static Expression<Number> log10(@NonNull Number number) {
    return log10(literal(number));
  }

  /**
   * Returns the natural logarithm of the input.
   *
   * @param number the number to take natural logarithm from
   * @return expression
   */
  public static Expression<Number> ln(Expression<Number> number) {
    return new Expression<>("ln", number);
  }

  /**
   * Returns the natural logarithm of the input.
   *
   * @param number the number to take natural logarithm from
   * @return expression
   */
  public static Expression<Number> ln(Number number) {
    return ln(literal(number));
  }

  /**
   * Returns the base-two logarithm of the input.
   *
   * @param number the number to take base-two logarithm from
   * @return expression
   */
  public static Expression<Number> log2(@NonNull Expression<Number> number) {
    return new Expression<>("log2", number);
  }

  /**
   * Returns the base-two logarithm of the input.
   *
   * @param number the number to take base-two logarithm from
   * @return expression
   */
  public static Expression<Number> log2(@NonNull Number number) {
    return log2(literal(number));
  }

  /**
   * Returns the sine of the input.
   *
   * @param number the number to calculate the sine for
   * @return expression
   */
  public static Expression<Number> sin(@NonNull Expression<Number> number) {
    return new Expression<>("sin", number);
  }

  /**
   * Returns the sine of the input.
   *
   * @param number the number to calculate the sine for
   * @return expression
   */
  public static Expression<Number> sin(@NonNull Number number) {
    return sin(literal(number));
  }

  /**
   * Returns the cosine of the input.
   *
   * @param number the number to calculate the cosine for
   * @return expression
   */
  public static Expression<Number> cos(@NonNull Expression<Number> number) {
    return new Expression<>("cos", number);
  }

  /**
   * Returns the cosine of the input.
   *
   * @param number the number to calculate the cosine for
   * @return expression
   */
  public static Expression<Number> cos(@NonNull Number number) {
    return new Expression<>("cos", literal(number));
  }

  /**
   * Returns the tangent of the input.
   *
   * @param number the number to calculate the tangent for
   * @return expression
   */
  public static Expression<Number> tan(@NonNull Expression<Number> number) {
    return new Expression<>("tan", number);
  }

  /**
   * Returns the tangent of the input.
   *
   * @param number the number to calculate the tangent for
   * @return expression
   */
  public static Expression<Number> tan(@NonNull Number number) {
    return new Expression<>("tan", literal(number));
  }

  /**
   * Returns the arcsine of the input.
   *
   * @param number the number to calculate the arcsine for
   * @return expression
   */
  public static Expression<Number> asin(@NonNull Expression<Number> number) {
    return new Expression<>("asin", number);
  }

  /**
   * Returns the arcsine of the input.
   *
   * @param number the number to calculate the arcsine for
   * @return expression
   */
  public static Expression<Number> asin(@NonNull Number number) {
    return asin(literal(number));
  }

  /**
   * Returns the arccosine of the input.
   *
   * @param number the number to calculate the arccosine for
   * @return expression
   */
  public static Expression<Number> acos(@NonNull Expression<Number> number) {
    return new Expression<>("acos", number);
  }

  /**
   * Returns the arccosine of the input.
   *
   * @param number the number to calculate the arccosine for
   * @return expression
   */
  public static Expression<Number> acos(@NonNull Number number) {
    return acos(literal(number));
  }

  /**
   * Returns the arctangent of the input.
   *
   * @param number the number to calculate the arctangent for
   * @return expression
   */
  public static Expression<Number> atan(@NonNull Expression<Number> number) {
    return new Expression("atan", number);
  }

  /**
   * Returns the arctangent of the input.
   *
   * @param number the number to calculate the arctangent for
   * @return expression
   */
  public static Expression<Number> atan(@NonNull Number number) {
    return atan(literal(number));
  }

  /**
   * Returns the minimum value of the inputs.
   *
   * @param numbers varargs of numbers to get the minimum from
   * @return expression
   */
  @SafeVarargs
  public static Expression<Number> min(@Size(min = 1) Expression<Number>... numbers) {
    return new Expression<>("min", numbers);
  }

  /**
   * Returns the minimum value of the inputs.
   *
   * @param numbers varargs of numbers to get the minimum from
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> min(@Size(min = 1) Number... numbers) {
    Expression<Number>[] numberExpression = (Expression<Number>[]) new Expression<?>[numbers.length];
    for (int i = 0; i < numbers.length; i++) {
      numberExpression[i] = literal(numbers[i]);
    }
    return min(numberExpression);
  }

  /**
   * Returns the maximum value of the inputs.
   *
   * @param numbers varargs of numbers to get the maximum from
   * @return expression
   */
  @SafeVarargs
  public static Expression<Number> max(@Size(min = 1) Expression<Number>... numbers) {
    return new Expression<>("max", numbers);
  }

  /**
   * Returns the maximum value of the inputs.
   *
   * @param numbers varargs of numbers to get the maximum from
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<Number> max(@Size(min = 1) Number... numbers) {
    Expression<Number>[] numberExpression = (Expression<Number>[]) new Expression<?>[numbers.length];
    for (int i = 0; i < numbers.length; i++) {
      numberExpression[i] = literal(numbers[i]);
    }
    return max(numberExpression);
  }

  //
  // String
  //

  /**
   * Returns the input string converted to uppercase.
   * <p>
   * Follows the Unicode Default Case Conversion algorithm
   * and the locale-insensitive case mappings in the Unicode Character Database.
   * </p>
   *
   * @param string the string to upcase
   * @return expression
   */
  public static Expression<String> upcase(@NonNull Expression<String> string) {
    return new Expression<>("upcase", string);
  }

  /**
   * Returns the input string converted to uppercase.
   * <p>
   * Follows the Unicode Default Case Conversion algorithm
   * and the locale-insensitive case mappings in the Unicode Character Database.
   * </p>
   *
   * @param string string to upcase
   * @return expression
   */
  public static Expression<String> upcase(@NonNull String string) {
    return upcase(literal(string));
  }

  /**
   * Returns the input string converted to lowercase.
   * <p>
   * Follows the Unicode Default Case Conversion algorithm
   * and the locale-insensitive case mappings in the Unicode Character Database.
   * </p>
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<String> downcase(@NonNull Expression<String> input) {
    return new Expression<>("downcase", input);
  }

  /**
   * Returns the input string converted to lowercase.
   * <p>
   * Follows the Unicode Default Case Conversion algorithm
   * and the locale-insensitive case mappings in the Unicode Character Database.
   * </p>
   *
   * @param input string to downcase
   * @return expression
   */
  public static Expression<String> downcase(@NonNull String input) {
    return downcase(literal(input));
  }

  /**
   * Returns a string consisting of the concatenation of the inputs.
   *
   * @param input expression input
   * @return expression
   */
  @SafeVarargs
  public static Expression<String> concat(@NonNull Expression<String>... input) {
    return new Expression<>("concat", input);
  }

  /**
   * Returns a string consisting of the concatenation of the inputs.
   *
   * @param input expression input
   * @return expression
   */
  @SuppressWarnings("unchecked")
  public static Expression<String> concat(@NonNull String... input) {
    Expression<String>[] stringExpression = (Expression<String>[]) new Expression<?>[input.length];
    for (int i = 0; i < input.length; i++) {
      stringExpression[i] = literal(input[i]);
    }
    return concat(stringExpression);
  }

  //
  // Types
  //

  /**
   * Asserts that the input is an array (optionally with a specific item type and length).
   * If, when the input expression is evaluated, it is not of the asserted type,
   * then this assertion will cause the whole expression to be aborted.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> array(@NonNull Expression input) {
    return new Expression<>("array", input);
  }

  /**
   * Returns a string describing the type of the given value.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<String> typeOf(@NonNull Expression input) {
    return new Expression<>("typeof", input);
  }

  /**
   * Asserts that the input value is a string.
   * If multiple values are provided, each one is evaluated in order until a string value is obtained.
   * If none of the inputs are strings, the expression is an error.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> string(@NonNull Expression input) {
    return new Expression<>("string", input);
  }

  /**
   * Asserts that the input value is a number.
   * If multiple values are provided, each one is evaluated in order until a number value is obtained.
   * If none of the inputs are numbers, the expression is an error.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> number(@NonNull Expression input) {
    return new Expression<>("number", input);
  }

  /**
   * Asserts that the input value is a boolean.
   * If multiple values are provided, each one is evaluated in order until a boolean value is obtained.
   * If none of the inputs are booleans, the expression is an error.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> bool(@NonNull Expression input) {
    return new Expression<>("boolean", input);
  }

  /**
   * Asserts that the input value is an object. If it is not, the expression is an error
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> object(@NonNull Expression input) {
    return new Expression<>("object", input);
  }

  /**
   * Converts the input value to a string.
   * If the input is null, the result is null.
   * If the input is a boolean, the result is true or false.
   * If the input is a number, it is converted to a string by NumberToString in the ECMAScript Language Specification.
   * If the input is a color, it is converted to a string of the form "rgba(r,g,b,a)",
   * where `r`, `g`, and `b` are numerals ranging from 0 to 255, and `a` ranges from 0 to 1.
   * Otherwise, the input is converted to a string in the format specified by the JSON.stringify in the ECMAScript
   * Language Specification.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<String> toString(@NonNull Expression input) {
    return new Expression<>("to-string", input);
  }

  /**
   * Converts the input value to a number, if possible.
   * If the input is null or false, the result is 0.
   * If the input is true, the result is 1.
   * If the input is a string, it is converted to a number as specified by the ECMAScript Language Specification.
   * If multiple values are provided, each one is evaluated in order until the first successful conversion is obtained.
   * If none of the inputs can be converted, the expression is an error.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Number> toNumber(@NonNull Expression input) {
    return new Expression<>("to-number", input);
  }

  /**
   * "Converts the input value to a boolean. The result is `false` when then input is an empty string, 0, false,
   * null, or NaN; otherwise it is true.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Boolean> toBool(@NonNull Expression input) {
    return new Expression<>("to-boolean", input);
  }

  /**
   * Converts the input value to a color. If multiple values are provided,
   * each one is evaluated in order until the first successful conversion is obtained.
   * If none of the inputs can be converted, the expression is an error.
   *
   * @param input expression input
   * @return expression
   */
  public static Expression<Color> toColor(@NonNull Expression input) {
    return new Expression<>("to-color", input);
  }

  //
  // Variable binding
  //

  /**
   * Binds input to named variables,
   * which can then be referenced in the result expression using {@link #var(String)} or {@link #var(Expression)}.
   *
   * @param input expression input
   * @return expression
   */
  @SafeVarargs
  public static Expression let(@Size(min = 1) Expression... input) {
    return new Expression<>("let", input);
  }

  /**
   * References variable bound using let.
   *
   * @param expression the variable naming expression that was bound with using let
   * @return expression
   */
  public static Expression<Object> var(@NonNull Expression<String> expression) {
    return new Expression<>("var", expression);
  }

  /**
   * References variable bound using let.
   *
   * @param variableName the variable naming that was bound with using let
   * @return expression
   */
  public static Expression var(@NonNull String variableName) {
    return var(literal(variableName));
  }

  //
  // Zoom
  //

  /**
   * Gets the current zoom level.
   * <p>
   * Note that in style layout and paint properties,
   * zoom may only appear as the input to a top-level step or interpolate expression.
   * </p>
   *
   * @return expression
   */
  public static Expression<Number> zoom() {
    return new Expression<>("zoom");
  }

  //
  // Ramps, scales, curves
  //

  public static Stop stop(@NonNull Object stop, @NonNull Object value) {
    return new Stop(stop, value);
  }

  /**
   * Produces discrete, stepped results by evaluating a piecewise-constant function defined by pairs of
   * input and output values (\"stops\"). The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * Returns the output value of the stop just less than the input,
   * or the first input if the input is less than the first stop.
   *
   * @param input the input value
   * @param stops pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression step(@NonNull Number input, @NonNull Expression expression, Expression... stops) {
    return step(literal(input), expression, stops);
  }

  /**
   * Produces discrete, stepped results by evaluating a piecewise-constant function defined by pairs of
   * input and output values (\"stops\"). The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * Returns the output value of the stop just less than the input,
   * or the first input if the input is less than the first stop.
   *
   * @param expression the input expression
   * @param stops      pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression step(@NonNull Expression<Number> input, @NonNull Expression expression, Expression... stops) {
    return new Expression("step", join(new Expression[] {input, expression}, stops));
  }

  /**
   * Produces discrete, stepped results by evaluating a piecewise-constant function defined by pairs of
   * input and output values (\"stops\"). The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * Returns the output value of the stop just less than the input,
   * or the first input if the input is less than the first stop.
   *
   * @param input the input value
   * @param stops pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression step(@NonNull Number input, @NonNull Expression expression, Stop... stops) {
    Expression[] expressions = new Expression[stops.length * 2];
    for (int i = 0; i < stops.length; i++) {
      expressions[i * 2] = literal(stops[i].value);
      expressions[i * 2 + 1] = literal(stops[i].output);
    }
    return step(literal(input), expression, expressions);
  }

  /**
   * Produces discrete, stepped results by evaluating a piecewise-constant function defined by pairs of
   * input and output values (\"stops\"). The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * Returns the output value of the stop just less than the input,
   * or the first input if the input is less than the first stop.
   *
   * @param input the input value
   * @param stops pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression step(@NonNull Expression<Number> input, @NonNull Expression expression, Stop... stops) {
    Expression[] expressions = new Expression[stops.length * 2];
    for (int i = 0; i < stops.length; i++) {
      expressions[i * 2] = literal(stops[i].value);
      expressions[i * 2 + 1] = literal(stops[i].output);
    }
    return step(input, expression, expressions);
  }

  /**
   * Produces continuous, smooth results by interpolating between pairs of input and output values (\"stops\").
   * The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * The output type must be `number`, `array&lt;number&gt;`, or `color`.
   *
   * @param interpolation type of interpolation
   * @param number        the input expression
   * @param stops         pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression interpolate(@NonNull Expression<Interpolator> interpolation,
                                       @NonNull Expression<Number> number, Expression... stops) {
    return new Expression("interpolate", join(new Expression[] {interpolation, number}, stops));
  }

  /**
   * Produces continuous, smooth results by interpolating between pairs of input and output values (\"stops\").
   * The `input` may be any numeric expression (e.g., `[\"get\", \"population\"]`).
   * Stop inputs must be numeric literals in strictly ascending order.
   * The output type must be `number`, `array&lt;number&gt;`, or `color`.
   *
   * @param interpolation type of interpolation
   * @param number        the input expression
   * @param stops         pair of input and output values
   * @return expression
   */
  @SafeVarargs
  public static Expression interpolate(@NonNull Expression<Interpolator> interpolation,
                                       @NonNull Expression<Number> number, Stop... stops) {
    Expression[] expressions = new Expression[stops.length * 2];
    for (int i = 0; i < stops.length; i++) {
      expressions[i * 2] = literal(stops[i].value);
      expressions[i * 2 + 1] = literal(stops[i].output);
    }
    return interpolate(interpolation, number, expressions);
  }

  /**
   * interpolates linearly between the pair of stops just less than and just greater than the input.
   *
   * @return expression
   */
  public static Expression<Interpolator> linear() {
    return new Expression<>("linear");
  }

  /**
   * Interpolates exponentially between the stops just less than and just greater than the input.
   * `base` controls the rate at which the output increases:
   * higher values make the output increase more towards the high end of the range.
   * With values close to 1 the output increases linearly.
   *
   * @param base value controlling the route at which the output increases
   * @return expression
   */
  public static Expression<Interpolator> exponential(@NonNull Number base) {
    return exponential(literal(base));
  }

  /**
   * Interpolates exponentially between the stops just less than and just greater than the input.
   * The parameter controls the rate at which the output increases:
   * higher values make the output increase more towards the high end of the range.
   * With values close to 1 the output increases linearly.
   *
   * @param expression base number expression
   * @return expression
   */
  public static Expression<Interpolator> exponential(@NonNull Expression<Number> expression) {
    return new Expression<>("exponential", expression);
  }

  /**
   * Interpolates using the cubic bezier curve defined by the given control points.
   *
   * @param x1 x value of the first point of a cubic bezier, ranges from 0 to 1
   * @param y1 y value of the first point of a cubic bezier, ranges from 0 to 1
   * @param x2 x value of the second point of a cubic bezier, ranges from 0 to 1
   * @param y2 y value fo the second point of a cubic bezier, ranges from 0 to 1
   * @return expression
   */
  public static Expression<Interpolator> cubicBezier(@NonNull Expression<Number> x1, @NonNull Expression<Number> y1,
                                                     @NonNull Expression<Number> x2, @NonNull Expression<Number> y2) {
    return new Expression<>("cubic-bezier", x1, y1, x2, y2);
  }

  /**
   * Interpolates using the cubic bezier curve defined by the given control points.
   *
   * @param x1 x value of the first point of a cubic bezier, ranges from 0 to 1
   * @param y1 y value of the first point of a cubic bezier, ranges from 0 to 1
   * @param x2 x value of the second point of a cubic bezier, ranges from 0 to 1
   * @param y2 y value fo the second point of a cubic bezier, ranges from 0 to 1
   * @return expression
   */
  public static Expression<Interpolator> cubicBezier(@NonNull Number x1, @NonNull Number y1,
                                                     @NonNull Number x2, @NonNull Number y2) {
    return cubicBezier(literal(x1), literal(y1), literal(x2), literal(y2));
  }

  /**
   * Joins two expressions arrays.
   * <p>
   * This flattens the object array output of an expression from a nested expression hierarchy.
   * </p>
   *
   * @param left  the left part of an expression
   * @param right the right part of an expression
   * @return the joined expression
   */
  private static Expression[] join(Expression[] left, Expression[] right) {
    Expression[] output = new Expression[left.length + right.length];
    System.arraycopy(left, 0, output, 0, left.length);
    System.arraycopy(right, 0, output, left.length, right.length);
    return output;
  }


  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("[\"").append(operator).append("\"");
    if (arguments != null) {
      for (Expression argument : arguments) {
        builder.append(", ");
        if (argument instanceof ExpressionLiteral) {
          builder.append(((ExpressionLiteral) argument).toValue());
        } else {
          builder.append(argument.toString());
        }
      }
    }
    builder.append("]");
    return builder.toString();
  }
}
