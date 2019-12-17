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
    public void runBenchmark() throws Exception {
        Log.v("Benchmark", "Start the benchmark");
        mActivityTestRule.launchActivity(null);
        while (TestState.running) {
            Log.v("Benchmark", "Benchmark is running...");
            Thread.sleep(1000L);
        }
        Log.v("Benchmark", "All benchmarks are finished!");
        mActivityTestRule.finishActivity();
        Assert.assertTrue("Benchmark was successfully finished", TestState.testResult);
    }
}
