package android.app;


import androidx.test.filters.LargeTest;
import androidx.test.rule.ActivityTestRule;
import androidx.test.runner.AndroidJUnit4;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

@LargeTest
@RunWith(AndroidJUnit4.class)
public class NativeActivityTest {

    @Rule
    public ActivityTestRule<NativeActivity> mActivityTestRule = new ActivityTestRule<>(NativeActivity.class);

    @Test
    public void nativeActivityTest() {
        mActivityTestRule.launchActivity(null);
    }
}
