package com.mapbox.mapboxsdk.activity.utils;

import android.support.annotation.IdRes;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.doubleClick;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

public class GestureUtils {

    public static void doubleClickGesture(@IdRes int id){
        onView(withId(id)).perform(doubleClick());
    }

}
