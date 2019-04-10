package com.mapbox.mapboxsdk.downstream.testapp;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class NavigationSettingsActivity extends PreferenceActivity {

  public static final String UNIT_TYPE_CHANGED = "unit_type_changed";
  public static final String LANGUAGE_CHANGED = "language_changed";
  public static final String OFFLINE_CHANGED = "offline_changed";

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    SharedPreferences.OnSharedPreferenceChangeListener listener = (sharedPreferences, key) -> {
      Intent resultIntent = new Intent();
      resultIntent.putExtra(UNIT_TYPE_CHANGED, key.equals(getString(R.string.unit_type_key)));
      resultIntent.putExtra(LANGUAGE_CHANGED, key.equals(getString(R.string.language_key)));
      resultIntent.putExtra(OFFLINE_CHANGED, key.equals(getString(R.string.offline_version_key)));
      setResult(RESULT_OK, resultIntent);
    };

    PreferenceManager.getDefaultSharedPreferences(this).registerOnSharedPreferenceChangeListener(listener);
    getFragmentManager().beginTransaction().replace(
      android.R.id.content, new NavigationViewPreferenceFragment()
    ).commit();
  }

  @Override
  protected boolean isValidFragment(String fragmentName) {
    return super.isValidFragment(fragmentName);
  }

  public static class NavigationViewPreferenceFragment extends PreferenceFragment {

    @Override
    public void onCreate(final Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      addPreferencesFromResource(R.xml.fragment_navigation_preferences);
      String gitHashTitle = String.format("Last Commit Hash: %s", BuildConfig.GIT_HASH);
      findPreference(getString(R.string.git_hash_key)).setTitle(gitHashTitle);
    }

    @Override
    public void onResume() {
      super.onResume();

      getOfflineVersions();
      PreferenceManager.setDefaultValues(getActivity(), R.xml.fragment_navigation_preferences, false);
    }

    private void getOfflineVersions() {
      File file = new File(Environment.getExternalStoragePublicDirectory("Offline"), "tiles");
      if (!file.exists()) {
        file.mkdirs();
      }

      ListPreference offlineVersions = (ListPreference) findPreference(getString(R.string.offline_version_key));
      List<String> list = buildFileList(file);
      if (!list.isEmpty()) {
        String[] entries = list.toArray(new String[list.size() - 1]);
        offlineVersions.setEntries(entries);
        offlineVersions.setEntryValues(entries);
        offlineVersions.setEnabled(true);
      } else {
        offlineVersions.setEnabled(false);
      }
    }

    @NonNull
    private List<String> buildFileList(File file) {
      List<String> list;
      if (file.list() != null && file.list().length != 0) {
        list = new ArrayList<>(Arrays.asList(file.list()));
      } else {
        list = new ArrayList<>();
      }
      return list;
    }
  }
}
