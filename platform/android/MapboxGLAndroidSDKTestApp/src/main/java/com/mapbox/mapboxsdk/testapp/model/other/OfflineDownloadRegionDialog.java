package com.mapbox.mapboxsdk.testapp.model.other;

import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;

import timber.log.Timber;

import android.widget.EditText;

import com.mapbox.mapboxsdk.testapp.R;

public class OfflineDownloadRegionDialog extends DialogFragment {

  public interface DownloadRegionDialogListener {
    void onDownloadRegionDialogPositiveClick(String regionName);
  }

  DownloadRegionDialogListener listener;

  @Override
  public void onAttach(Activity activity) {
    super.onAttach(activity);
    listener = (DownloadRegionDialogListener) activity;
  }

  @NonNull
  @Override
  public Dialog onCreateDialog(Bundle savedInstanceState) {
    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

    // Let the user choose a name for the region
    final EditText regionNameEdit = new EditText(getActivity());

    builder.setTitle("Choose a name for the region")
      .setIcon(R.drawable.ic_airplanemode_active_black)
      .setView(regionNameEdit)
      .setPositiveButton("Start", new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
          String regionName = regionNameEdit.getText().toString();
          listener.onDownloadRegionDialogPositiveClick(regionName);
        }
      }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
          Timber.d("Download cancelled.");
        }
      });

    return builder.create();
  }
}
