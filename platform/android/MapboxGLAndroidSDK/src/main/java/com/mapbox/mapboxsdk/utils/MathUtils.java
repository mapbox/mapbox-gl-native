package com.mapbox.mapboxsdk.utils;

public class MathUtils {

    /**
     * Test a value in specified range, returning minimum if it's below, and maximum if it's above
     * @param value Value to test
     * @param min Minimum value of range
     * @param max Maximum value of range
     * @return value if it's between min and max, min if it's below, max if it's above
     */
    public static double clamp(double value, double min, double max) {
        return Math.max(min, Math.min(max, value));
    }

}
