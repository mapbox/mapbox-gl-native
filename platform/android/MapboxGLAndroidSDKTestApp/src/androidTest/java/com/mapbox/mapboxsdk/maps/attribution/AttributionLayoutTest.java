package com.mapbox.mapboxsdk.maps.attribution;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.test.rule.ActivityTestRule;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import com.mapbox.mapboxsdk.attribution.AttributionParser;
import com.mapbox.mapboxsdk.attribution.AttributionPlacement;
import com.mapbox.mapboxsdk.attribution.AttributionProvider;
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static junit.framework.Assert.assertEquals;

public class AttributionLayoutTest {

  @Rule
  public ActivityTestRule<FeatureOverviewActivity> rule = new ActivityTestRule<>(FeatureOverviewActivity.class);

  private static final String SATELLITE_ATTRIBUTION = "<a href=\"https://www.mapbox.com/about/maps/\" target=\"_blank\">&copy; Mapbox</a> <a href=\"http://www.openstreetmap.org/about/\" target=\"_blank\">&copy; OpenStreetMap</a> <a class=\"mapbox-improve-map\" href=\"https://www.mapbox.com/map-feedback/\" target=\"_blank\">Improve this map</a> <a href=\"https://www.digitalglobe.com/\" target=\"_blank\">&copy; DigitalGlobe</a>\n";
  private static final Bitmap LOGO = Bitmap.createBitmap(128, 56, Bitmap.Config.ARGB_8888);
  private static final Bitmap LOGO_SMALL = Bitmap.createBitmap(56, 56, Bitmap.Config.ARGB_8888);
  private static final String ATTRIBUTION = new AttributionParser.Options()
    .withAttributionData(SATELLITE_ATTRIBUTION)
    .withImproveMap(false)
    .withCopyrightSign(false)
    .build().getAttributionString();

  private Context context;
  private TextView textView;
  private float margin;

  @Before
  public void setUp() throws Exception {
    context = rule.getActivity();
    margin = context.getResources().getDisplayMetrics().density * 4;

    textView = new TextView(context);
    textView.setLayoutParams(new ViewGroup.LayoutParams(
      ViewGroup.LayoutParams.WRAP_CONTENT,
      ViewGroup.LayoutParams.WRAP_CONTENT)
    );
    textView.setSingleLine(true);
    textView.setTextSize(8);
    textView.setText(ATTRIBUTION);
  }

  @Test
  public void testLogoLayout() throws Exception {
    Bitmap snapshot = Bitmap.createBitmap(650, 600, Bitmap.Config.ARGB_8888);
    int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(snapshot.getWidth(), View.MeasureSpec.AT_MOST);
    int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    textView.measure(widthMeasureSpec, heightMeasureSpec);

    AttributionProvider attributionProvider = new AttributionProvider(snapshot, LOGO, LOGO_SMALL, textView, margin);

    PointF expectedPoint = new PointF(snapshot.getWidth() - textView.getMeasuredWidth() - margin, snapshot.getHeight() - textView.getMeasuredHeight() - margin);
    AttributionPlacement expected = new AttributionPlacement(LOGO, expectedPoint);
    AttributionPlacement actual = attributionProvider.calculateAttributionPlacement();
    assertEquals("Calculated placement should match: ", expected, actual);
  }

  @Test
  public void testSmallLogoLayout() {
    Bitmap snapshot = Bitmap.createBitmap(512, 512, Bitmap.Config.ARGB_8888);
    int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(snapshot.getWidth(), View.MeasureSpec.AT_MOST);
    int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    textView.measure(widthMeasureSpec, heightMeasureSpec);

    AttributionProvider attributionProvider = new AttributionProvider(snapshot, LOGO, LOGO_SMALL, textView, margin);

    PointF expectedPoint = new PointF(snapshot.getWidth() - textView.getMeasuredWidth() - margin, snapshot.getHeight() - textView.getMeasuredHeight() - margin);
    AttributionPlacement expected = new AttributionPlacement(LOGO_SMALL, expectedPoint);
    AttributionPlacement actual = attributionProvider.calculateAttributionPlacement();
    assertEquals("Calculated placement should match: ", expected, actual);
  }

  @Test
  public void testNoLogoLayout() {
    Bitmap snapshot = Bitmap.createBitmap(415, 415, Bitmap.Config.ARGB_8888);
    int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    textView.measure(widthMeasureSpec, heightMeasureSpec);

    AttributionProvider attributionProvider = new AttributionProvider(snapshot, LOGO, LOGO_SMALL, textView, margin);

    PointF expectedPoint = new PointF(snapshot.getWidth() - textView.getMeasuredWidth() - margin, snapshot.getHeight() - textView.getMeasuredHeight() - margin);
    AttributionPlacement expected = new AttributionPlacement(null, expectedPoint);
    AttributionPlacement actual = attributionProvider.calculateAttributionPlacement();
    assertEquals("Calculated placement should match: ", expected, actual);
  }

  @Test
  public void testNoAttributionLayout() {
    Bitmap snapshot = Bitmap.createBitmap(25, 25, Bitmap.Config.ARGB_8888);
    int widthMeasureSpec = View.MeasureSpec.makeMeasureSpec(snapshot.getWidth(), View.MeasureSpec.AT_MOST);
    int heightMeasureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
    textView.measure(widthMeasureSpec, heightMeasureSpec);

    AttributionProvider attributionProvider = new AttributionProvider(snapshot, LOGO, LOGO_SMALL, textView, margin);

    AttributionPlacement expected = new AttributionPlacement(null, null);
    AttributionPlacement actual = attributionProvider.calculateAttributionPlacement();
    assertEquals("Calculated placement should match: ", expected, actual);
  }
}