package android.app;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;
import androidx.test.rule.ActivityTestRule;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

@LargeTest
@RunWith(AndroidJUnit4.class)
public class NativeActivityTest {

    @Rule
    public ActivityTestRule<NativeActivity> mActivityTestRule = new ActivityTestRule<>(NativeActivity.class);

    @Test(timeout = 50000000L)
    public void runRenderTests() throws Exception {
        while (TestState.running) {
            Thread.sleep(1000L);
        }
    }
}