package com.mapbox.mapboxsdk.testapp.model.other;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;

import timber.log.Timber;

import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;

public class OfflineListRegionsDialog extends DialogFragment {

  public static final String ITEMS = "ITEMS";

  @NonNull
  @Override
  public Dialog onCreateDialog(Bundle savedInstanceState) {
    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

    // Read args
    Bundle args = getArguments();
    ArrayList<String> offlineRegionsNames = (args == null ? null : args.getStringArrayList(ITEMS));
    CharSequence[] items = offlineRegionsNames.toArray(new CharSequence[offlineRegionsNames.size()]);

    builder.setTitle("List of offline regions")
      .setIcon(R.drawable.ic_airplanemode_active_black)
      .setItems(items, new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
          Timber.d("Selected item: %s", which);
        }
      })
      .setPositiveButton("Accept", new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
          Timber.d("Dialog dismissed");
        }
      });

    return builder.create();
  }
}
