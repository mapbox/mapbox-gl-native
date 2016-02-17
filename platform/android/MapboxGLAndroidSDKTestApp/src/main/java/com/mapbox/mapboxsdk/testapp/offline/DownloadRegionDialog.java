package com.mapbox.mapboxsdk.testapp.offline;

import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.widget.EditText;

import com.mapbox.mapboxsdk.testapp.R;

/**
 * Created by antonio on 2/17/16.
 */
public class DownloadRegionDialog extends DialogFragment {

    private final static String LOG_TAG = "DownloadRegionDialog";

    public interface DownloadRegionDialogListener {
        void onDownloadRegionDialogPositiveClick(String regionName);
    }

    DownloadRegionDialogListener mListener;

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        mListener = (DownloadRegionDialogListener) activity;
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        // Let the user choose a name for the region
        final EditText regionNameEdit = new EditText(getActivity());

        builder.setTitle("Choose a name for the region")
                .setIcon(R.drawable.ic_airplanemode_active_black_24dp)
                .setView(regionNameEdit)
                .setPositiveButton("Start", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String regionName = regionNameEdit.getText().toString();
                        mListener.onDownloadRegionDialogPositiveClick(regionName);
                    }
                }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.d(LOG_TAG, "Download cancelled.");
            }
        });

        return builder.create();
    }
}
