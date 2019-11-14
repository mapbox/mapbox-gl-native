package com.mapbox.mapboxsdk.testapp.render;

import android.Manifest;
import androidx.test.espresso.IdlingPolicies;
import androidx.test.espresso.IdlingRegistry;
import androidx.test.espresso.IdlingResourceTimeoutException;
import androidx.test.rule.ActivityTestRule;
import androidx.test.rule.GrantPermissionRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.mapboxsdk.AppCenter;
import com.mapbox.mapboxsdk.testapp.activity.render.RenderTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.SnapshotterIdlingResource;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import timber.log.Timber;

import java.util.concurrent.TimeUnit;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

/**
 * Instrumentation render tests
 */
@RunWith(AndroidJUnit4.class)
public class RenderTest extends AppCenter {

  private static final int RENDER_TEST_TIMEOUT = 30;
  private SnapshotterIdlingResource idlingResource;

  @Rule
  public ActivityTestRule<RenderTestActivity> activityRule = new ActivityTestRule<>(RenderTestActivity.class);

  @Rule
  public GrantPermissionRule writeRule = GrantPermissionRule.grant(Manifest.permission.WRITE_EXTERNAL_STORAGE);

  @Rule
  public GrantPermissionRule readRule = GrantPermissionRule.grant(Manifest.permission.READ_EXTERNAL_STORAGE);

  @Before
  public void beforeTest() {
    IdlingPolicies.setMasterPolicyTimeout(RENDER_TEST_TIMEOUT, TimeUnit.MINUTES);
    setupIdlingResource();
  }

  private void setupIdlingResource() {
    try {
      Timber.e("@Before test: register idle resource");
      IdlingPolicies.setIdlingResourceTimeout(RENDER_TEST_TIMEOUT, TimeUnit.MINUTES);
      IdlingRegistry.getInstance().register(idlingResource = new SnapshotterIdlingResource(activityRule.getActivity()));
    } catch (IdlingResourceTimeoutException idlingResourceTimeoutException) {
      throw new RuntimeException("Idling out!");
    }
  }

  @Test
  @Ignore
  public void testRender() {
    onView(withId(android.R.id.content)).check(matches(isDisplayed()));
  }

  @After
  public void afterTest() {
    Timber.e("@After test: unregister idle resource");
    IdlingRegistry.getInstance().unregister(idlingResource);
  }
}