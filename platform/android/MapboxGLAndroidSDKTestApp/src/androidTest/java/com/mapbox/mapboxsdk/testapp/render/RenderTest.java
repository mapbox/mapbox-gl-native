package com.mapbox.mapboxsdk.testapp.render;

import android.Manifest;
import android.support.test.espresso.IdlingPolicies;
import android.support.test.espresso.IdlingRegistry;
import android.support.test.espresso.IdlingResourceTimeoutException;
import android.support.test.rule.ActivityTestRule;
import android.support.test.rule.GrantPermissionRule;
import android.support.test.runner.AndroidJUnit4;

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

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

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