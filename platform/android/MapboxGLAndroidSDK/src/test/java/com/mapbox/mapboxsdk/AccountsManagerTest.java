package com.mapbox.mapboxsdk;

import android.content.SharedPreferences;
import android.text.format.DateUtils;

import org.junit.Test;

import static com.mapbox.mapboxsdk.constants.MapboxConstants.KEY_PREFERENCE_SKU_TOKEN;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.RETURNS_DEEP_STUBS;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class AccountsManagerTest {
  @Test
  public void testIsExpired() {
    long now = AccountsManager.getNow();

    long defaultValue = 0L;
    long tooOld = now - DateUtils.HOUR_IN_MILLIS - 1;
    long futureValue = now + 1;
    long immediatePast = now - 1;

    assertTrue(AccountsManager.isExpired(now, defaultValue));
    assertTrue(AccountsManager.isExpired(now, tooOld));

    assertFalse(AccountsManager.isExpired(now, futureValue));
    assertFalse(AccountsManager.isExpired(now, immediatePast));
  }

  @Test
  public void checksSkuTokenExternalManagement() {
    SharedPreferences mockedSharedPreferences = mock(SharedPreferences.class);
    when(mockedSharedPreferences.getString(KEY_PREFERENCE_SKU_TOKEN, "")).thenReturn("external-sku-token");
    boolean isNotManaged = false;
    AccountsManager theAccountsManager = new AccountsManager(mockedSharedPreferences, isNotManaged);

    String skuToken = theAccountsManager.getSkuToken();

    assertEquals("external-sku-token", skuToken);
  }

  @Test
  public void checksSkuTokenInternalManagement() {
    SharedPreferences mockedSharedPreferences = mock(SharedPreferences.class, RETURNS_DEEP_STUBS);
    when(mockedSharedPreferences.getString("com.mapbox.mapboxsdk.accounts.userid", ""))
      .thenReturn("any-user-id");
    boolean isManaged = true;
    AccountsManager theAccountsManager = new AccountsManager(mockedSharedPreferences, isManaged);

    String skuToken = theAccountsManager.getSkuToken();

    assertFalse(skuToken.isEmpty());
    assertTrue(skuToken.startsWith("100"));
    assertTrue(skuToken.endsWith("any-user-id"));
  }
}
