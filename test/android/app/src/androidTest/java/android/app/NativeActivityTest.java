package android.app;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.LargeTest;
import androidx.test.rule.ActivityTestRule;

import org.junit.Rule;
import org.junit.Test;
import org.junit.Assert;
import org.junit.runner.RunWith;
import android.util.Log;

@LargeTest
@RunWith(AndroidJUnit4.class)
public class NativeActivityTest {

    @Rule
    public ActivityTestRule<NativeActivity> mActivityTestRule = new ActivityTestRule<>(NativeActivity.class, false, false);

    @Test(timeout = 1200000L)
    public void runUnitTest() throws Exception {
        Log.v("UnitTestRunner", "Start the unit test");
        mActivityTestRule.launchActivity(null);
        while (TestState.running) {
            Log.v("UnitTestRunner", "Unit test is running...");
            Thread.sleep(1000L);
        }
        Log.v("UnitTestRunner", "All unit tests are finished!");
        mActivityTestRule.finishActivity();
        Assert.assertTrue("UnitTestRunner was not successfully finished", TestState.testResult);
    }
}
