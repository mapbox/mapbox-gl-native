package com.mapbox.mapboxsdk;

import android.text.format.DateUtils;

import org.junit.Assert;
import org.junit.Test;

public class AccountsManagerTest {
  @Test
  public void testIsExpired() {
    long now = AccountsManager.getNow();

    long defaultValue = 0L;
    long tooOld = now - DateUtils.HOUR_IN_MILLIS - 1;
    long futureValue = now + 1;
    long immediatePast = now - 1;

    Assert.assertTrue(AccountsManager.isExpired(now, defaultValue));
    Assert.assertTrue(AccountsManager.isExpired(now, tooOld));

    Assert.assertFalse(AccountsManager.isExpired(now, futureValue));
    Assert.assertFalse(AccountsManager.isExpired(now, immediatePast));
  }
}
