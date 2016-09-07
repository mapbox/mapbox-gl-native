package com.mapbox.mapboxsdk.style.layers;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;

/**
 * Utility to build filter expressions more easily:
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-filter">Style spec</a>
 */
public class Filter {

    public abstract static class Statement {
        protected final String operator;

        public Statement(String operator) {
            this.operator = operator;
        }

        public abstract Object[] toArray();
    }

    public static class SimpleStatement extends Statement {
        private final String key;
        private final Object[] values;

        public SimpleStatement(String operator, String key, Object... values) {
            super(operator);
            this.key = key;
            this.values = values;
        }


        @Override
        public Object[] toArray() {
            ArrayList<Object> array = new ArrayList<>(2 + values.length);
            array.add(operator);
            array.add(key);
            Collections.addAll(array, values);
            return array.toArray();
        }
    }

    public static class CompoundStatement extends Statement {
        private final Statement[] statements;

        public CompoundStatement(String operator, Statement... statements) {
            super(operator);
            this.statements = statements;
        }

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

    public static Statement all(Statement... statements) {
        return new CompoundStatement("all", statements);
    }

    public static Statement any(Statement... statements) {
        return new CompoundStatement("any", statements);
    }

    public static Statement none(Statement... statements) {
        return new CompoundStatement("none", statements);
    }

    public static Statement has(String key) {
        return new SimpleStatement("has", key);
    }

    public static Statement notHas(String key) {
        return new SimpleStatement("!has", key);
    }

    public static Statement eq(String key, Object value) {
        return new SimpleStatement("==", key, value);
    }

    public static Statement neq(String key, Object value) {
        return new SimpleStatement("!=", key, value);
    }

    public static Statement gt(String key, Object value) {
        return new SimpleStatement(">", key, value);
    }

    public static Statement gte(String key, Object value) {
        return new SimpleStatement(">=", key, value);
    }

    public static Statement lt(String key, Object value) {
        return new SimpleStatement("<", key, value);
    }

    public static Statement lte(String key, Object value) {
        return new SimpleStatement("<=", key, value);
    }

    public static Statement in(String key, Object... values) {
        return new SimpleStatement("in", key, values);
    }

    public static Statement notIn(String key, Object... values) {
        return new SimpleStatement("!in", key, values);
    }

}
