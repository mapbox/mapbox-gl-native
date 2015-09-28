package com.mapbox.mapboxgl.views.widget;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.support.v7.app.AlertDialog;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.views.MapView;
import com.mapbox.mapboxgl.views.R;

import java.lang.ref.WeakReference;

public class AttributionView extends ImageView {

    public AttributionView(Context context) {
        this(context, null);
    }

    public AttributionView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public AttributionView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context);
    }

    private void initialize(Context context) {
        // configure
        setImageResource(R.drawable.ic_info_selector);
        setContentDescription(getResources().getString(R.string.attributionsIconContentDescription));
        setClickable(true);

        // increase touch surface
        int attrPadding = (int) context.getResources().getDimension(R.dimen.seven_dp);
        setPadding(attrPadding, attrPadding, attrPadding, attrPadding);
        setAdjustViewBounds(true);
        setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
    }

    public static class AttributionClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

        private static final int INDEX_IMPROVE_THIS_MAP = 2;

        private WeakReference<MapView> mMapView;

        public AttributionClickListener(final MapView mapView) {
            mMapView = new WeakReference<>(mapView);
        }

        // Called when someone presses the attribution icon
        @Override
        public void onClick(View v) {
            Context context = v.getContext();
            String[] items = context.getResources().getStringArray(R.array.attribution_names);
            AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.AttributionAlertDialogStyle);
            builder.setTitle(R.string.attributionsDialogTitle);
            builder.setAdapter(new ArrayAdapter<>(context, R.layout.attribution_list_item, items), this);
            builder.show();
        }

        // Called when someone selects an attribution, 'Improve this map' adds location data to the url
        @Override
        public void onClick(DialogInterface dialog, int which) {
            final MapView mapView = mMapView.get();
            if(mapView!=null) {
                Context context = mapView.getContext();
                String url = context.getResources().getStringArray(R.array.attribution_links)[which];
                if (which == INDEX_IMPROVE_THIS_MAP) {
                    LatLng latLng = mapView.getCenterCoordinate();
                    url = String.format(url, latLng.getLongitude(), latLng.getLatitude(), (int) mapView.getZoomLevel());
                }
                Intent intent = new Intent(Intent.ACTION_VIEW);
                intent.setData(Uri.parse(url));
                context.startActivity(intent);
            }
        }
    }
}
