package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.Nullable;
import android.util.Log;

/**
 * Properties for Layer
 */
public class PropertyValue<T> {
    private static final String TAG = PropertyValue.class.getSimpleName();

    private final Object value;

    /* package */ PropertyValue(Object value) {
        this.value = value;
    }

    public boolean isNull() {
        return value == null;
    }

    public boolean isFunction() {
        return !isNull() && value instanceof Function;
    }

    public boolean isValue() {
        return !isNull() && !isFunction();
    }

    @Nullable
    public Function<T> getFunction() {
        if (isFunction()) {
            //noinspection unchecked
            return (Function<T>) value;
        } else {
            Log.w(TAG, "not a function, try value");
            return null;
        }
    }

    @Nullable
    public T getValue() {
        if (isValue()) {
            //noinspection unchecked
            return (T) value;
        } else {
            Log.w(TAG, "not a value, try function");
            return null;
        }
    }

    @Override
    public String toString() {
        return String.format("%s (%s)", getClass().getSimpleName(), value != null ? value.getClass().getSimpleName() : null);
    }
}
