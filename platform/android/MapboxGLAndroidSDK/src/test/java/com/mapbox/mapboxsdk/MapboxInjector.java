package com.mapbox.mapboxsdk;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.lang.reflect.Field;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MapboxInjector {

  private static final String FIELD_INSTANCE = "INSTANCE";
  private static final String FIELD_ACCOUNTS = "accounts";

  public static void inject(@NonNull Context context, @NonNull String accessToken) {
    inject(context, accessToken, null);
  }

  public static void inject(@NonNull Context context, @NonNull String accessToken, @Nullable String skuToken) {
    Mapbox mapbox = new Mapbox(context, accessToken);
    try {
      Field instance = Mapbox.class.getDeclaredField(FIELD_INSTANCE);
      instance.setAccessible(true);
      instance.set(mapbox, mapbox);

      Field accounts = Mapbox.class.getDeclaredField(FIELD_ACCOUNTS);
      accounts.setAccessible(true);

      AccountsManager manager = mock(AccountsManager.class);
      when(manager.getSkuToken()).thenReturn(skuToken);
      accounts.set(mapbox, manager);
    } catch (Exception exception) {
      throw new AssertionError();
    }
  }

  public static void clear() {
    try {
      Field field = Mapbox.class.getDeclaredField(FIELD_INSTANCE);
      field.setAccessible(true);
      field.set(field, null);
    } catch (Exception exception) {
      throw new AssertionError();
    }
  }
}
