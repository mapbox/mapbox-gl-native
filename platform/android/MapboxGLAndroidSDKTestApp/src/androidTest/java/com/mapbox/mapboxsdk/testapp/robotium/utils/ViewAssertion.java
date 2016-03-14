package com.mapbox.mapboxsdk.testapp.robotium.utils;

import android.view.View;

import static junit.framework.Assert.assertEquals;

public class ViewAssertion {

    public static void isVisible(View view) {
        assertEquals(view.getClass().getSimpleName() + " should be visible", view.getVisibility(), View.VISIBLE);
    }

    public static void isInvisible(View view) {
        assertEquals(view.getClass().getSimpleName() + " should be visible", view.getVisibility(), View.INVISIBLE);
    }

}
