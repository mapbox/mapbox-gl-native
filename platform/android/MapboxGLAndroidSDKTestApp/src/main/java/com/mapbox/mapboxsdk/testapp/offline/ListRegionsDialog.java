package com.mapbox.mapboxsdk.testapp.offline;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.util.Log;

import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;

/**
 * Created by antonio on 2/17/16.
 */
public class ListRegionsDialog extends DialogFragment {

    private final static String LOG_TAG = "ListRegionsDialog";

    public final static String ITEMS = "ITEMS";

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        // Read args
        Bundle args = getArguments();
        ArrayList<String> offlineRegionsNames = (args == null ? null : args.getStringArrayList(ITEMS));
        CharSequence[] items = offlineRegionsNames.toArray(new CharSequence[offlineRegionsNames.size()]);

        builder.setTitle("List of offline regions")
                .setIcon(R.drawable.ic_airplanemode_active_black_24dp)
                .setItems(items, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d(LOG_TAG, "Selected item: " + which);
                    }
                })
                .setPositiveButton("Accept", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d(LOG_TAG, "Dialog dismissed");
                    }
                });

        return builder.create();
    }
}
