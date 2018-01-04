package com.mapbox.mapboxsdk.style.layers;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Utility to build filter expressions more easily.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-filter">The online documentation</a>
 */
public class Filter {

  /**
   * Base Filter statement. Subclassed to provide concrete statements.
   */
  public abstract static class Statement {
    protected final String operator;

    public Statement(String operator) {
      this.operator = operator;
    }

    /**
     * Generate a raw array representation of the filter
     *
     * @return the filter represented as an array
     */
    public abstract Object[] toArray();
  }

  /**
   * Represents a {@link Filter} statement. Can be unary (eg `has()`, etc) or take any number of values.
   */
  private static class SimpleStatement extends Statement {
    private final String key;
    private final Object[] values;

    /**
     * @param operator the operator (eg `=`, etc)
     * @param key      the property key
     * @param values   the values to operate on, if any
     */
    SimpleStatement(String operator, String key, Object... values) {
      super(operator);
      this.key = key;
      this.values = values;
    }


    /**
     * {@inheritDoc}
     */
    @Override
    public Object[] toArray() {
      ArrayList<Object> array = new ArrayList<>(2 + values.length);
      array.add(operator);
      array.add(key);
      Collections.addAll(array, values);
      return array.toArray();
    }
  }

  /**
   * Represents a collection of {@link Statement}s with an operator that describes their relationship
   */
  private static class CompoundStatement extends Statement {
    private final Statement[] statements;

    /**
     * @param operator   the relationship operator
     * @param statements the statements to compound
     */
    CompoundStatement(String operator, Statement... statements) {
      super(operator);
      this.statements = statements;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Object[] toArray() {
      ArrayList<Object> array = new ArrayList<>(1 + statements.length);
      array.add(operator);
      for (Statement statement : statements) {
        array.add(statement.toArray());
      }
      return array.toArray();
    }
  }

  /**
   * Groups a collection of statements in an 'all' relationship
   *
   * @param statements the collection of statements
   * @return the statements compounded
   */
  public static Statement all(Statement... statements) {
    return new CompoundStatement("all", statements);
  }

  /**
   * Groups a collection of statements in an 'any' relationship
   *
   * @param statements the collection of statements
   * @return the statements compounded
   */
  public static Statement any(Statement... statements) {
    return new CompoundStatement("any", statements);
  }

  /**
   * Groups a collection of statements in an 'none' relationship
   *
   * @param statements the collection of statements
   * @return the statements compounded
   */
  public static Statement none(Statement... statements) {
    return new CompoundStatement("none", statements);
  }

  /**
   * Check the property's existence
   *
   * @param key the property key
   * @return the statement
   */
  public static Statement has(String key) {
    return new SimpleStatement("has", key);
  }

  /**
   * Check the property's existence, negated
   *
   * @param key the property key
   * @return the statement
   */
  public static Statement notHas(String key) {
    return new SimpleStatement("!has", key);
  }

  /**
   * Check the property equals the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement eq(String key, Object value) {
    return new SimpleStatement("==", key, value);
  }

  /**
   * Check the property does not equals the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement neq(String key, Object value) {
    return new SimpleStatement("!=", key, value);
  }

  /**
   * Check the property exceeds the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement gt(String key, Object value) {
    return new SimpleStatement(">", key, value);
  }

  /**
   * Check the property exceeds or equals the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement gte(String key, Object value) {
    return new SimpleStatement(">=", key, value);
  }

  /**
   * Check the property does not exceeds the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement lt(String key, Object value) {
    return new SimpleStatement("<", key, value);
  }

  /**
   * Check the property equals or does not exceeds the given value
   *
   * @param key   the property key
   * @param value the value to check against
   * @return the statement
   */
  public static Statement lte(String key, Object value) {
    return new SimpleStatement("<=", key, value);
  }

  /**
   * Check the property is within the given set
   *
   * @param key    the property key
   * @param values the set of values to check against
   * @return the statement
   */
  public static Statement in(String key, Object... values) {
    return new SimpleStatement("in", key, values);
  }

  /**
   * Check the property is not within the given set
   *
   * @param key    the property key
   * @param values the set of values to check against
   * @return the statement
   */
  public static Statement notIn(String key, Object... values) {
    return new SimpleStatement("!in", key, values);
  }

}
