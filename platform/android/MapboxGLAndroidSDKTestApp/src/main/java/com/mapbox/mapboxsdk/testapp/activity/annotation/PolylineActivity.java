package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class PolylineActivity extends AppCompatActivity {

    private static final String STATE_POLYLINE_OPTIONS = "polylineOptions";

    private static final LatLng ANDORRA = new LatLng(42.505777, 1.52529);
    private static final LatLng LUXEMBOURG = new LatLng(49.815273, 6.129583);
    private static final LatLng MONACO = new LatLng(43.738418, 7.424616);
    private static final LatLng VATICAN_CITY = new LatLng(41.902916, 12.453389);
    private static final LatLng SAN_MARINO = new LatLng(43.942360, 12.457777);
    private static final LatLng LIECHTENSTEIN = new LatLng(47.166000, 9.555373);

    private static final float FULL_ALPHA = 1.0f;
    private static final float PARTIAL_ALPHA = 0.5f;
    private static final float NO_ALPHA = 0.0f;
    private static final String TAG = PolylineActivity.class.getSimpleName();

    private List<Polyline> mPolylines;
    private ArrayList<PolylineOptions> mPolylineOptions = new ArrayList<>();
    private MapView mMapView;
    private MapboxMap mMapboxMap;

    private boolean fullAlpha = true;
    private boolean visible = true;
    private boolean width = true;
    private boolean color = true;
    private String[] colors = {"red", "orange", "purple", "green", "brown", "blue", "???"};
    private HashMap<Polyline, String> polylineColor = new HashMap<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_polyline);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        if (savedInstanceState != null) {
            mPolylineOptions = savedInstanceState.getParcelableArrayList(STATE_POLYLINE_OPTIONS);
        } else {
            mPolylineOptions.addAll(getAllPolylines());
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                mPolylines = mapboxMap.addPolylines(mPolylineOptions);
                for (int i = 0; i < mPolylines.size(); i++) {
                    polylineColor.put(mPolylines.get(i), colors[i]);
                    Log.d(TAG, "polyline "+colors[i]+ " added");

                }
                int selectionThresholdInPx = (int) dipToPixels(PolylineActivity.this, (float) 15.0);
                configurePolylineClick(createPolylineClickListener(), selectionThresholdInPx);
            }
        });

        View fab = findViewById(R.id.fab);
        if (fab != null) {
            fab.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mMapboxMap != null) {
                        if (mPolylines != null && mPolylines.size() > 0) {
                            if (mPolylines.size() == 1) {
                                // test for removing annotation
                                mMapboxMap.removeAnnotation(mPolylines.get(0));
                            } else {
                                // test for removing annotations
                                mMapboxMap.removeAnnotations(mPolylines);
                            }
                        }
                        mPolylineOptions.clear();
                        mPolylineOptions.addAll(getRandomLine());
                        mPolylines = mMapboxMap.addPolylines(mPolylineOptions);

                    }
                }
            });
        }

    }

    @NonNull
    private OnPolylineClickListener createPolylineClickListener() {
        return new OnPolylineClickListener() {
            @Override
            public void onPolylineClick(@NonNull Polyline p) {
                String color = polylineColor.get(p);
                Toast.makeText(PolylineActivity.this, "polyline clicked" + p.hashCode()+ " "+color, Toast.LENGTH_SHORT).show();
            }
        };
    }

    private void configurePolylineClick(@NonNull OnPolylineClickListener onPolylineClickListener, int selectionThresholdInPx) {
        mMapboxMap.setOnMapClickListener(createPolylineClickListener(mMapboxMap, onPolylineClickListener, selectionThresholdInPx));
    }

    private MapboxMap.OnMapClickListener createPolylineClickListener(final MapboxMap mMapboxMap, final OnPolylineClickListener onPolylineClickListener, final int selectionThresholdInPx) {
        return new MapboxMap.OnMapClickListener() {
            @Override
            public void onMapClick(@NonNull LatLng point) {
                Polyline p = getProximityPolyline(point, mMapboxMap, selectionThresholdInPx);
                if (p != null) {
                    onPolylineClickListener.onPolylineClick(p);
                }
            }
        };
    }

    @Nullable
    private Polyline getProximityPolyline(@NonNull LatLng latlng, MapboxMap mMapboxMap, int selectionThresholdInPx) {
        Log.d(TAG, "->getProximityPolyline");
        long begin = System.currentTimeMillis();
        List<PolylineInPx> polylinesInPx = convertPolylinesInpx(mMapboxMap, mMapboxMap.getPolylines());
        PointF point = mMapboxMap.getProjection().toScreenLocation(latlng);

        double bestProximity = Double.MAX_VALUE;
        Polyline bestProximityPolyline = null;
        for (PolylineInPx polyline : polylinesInPx) {
            double proximity = bestProximityBeetweenPointAndPolyline(point, polyline);
            if( proximity < polyline.getPolyline().getWidth()/2 + selectionThresholdInPx) {
                Log.d(TAG, "proximity from " + polylineColor.get(polyline.getPolyline()) + " = " + proximity);
                if (proximity < bestProximity) {
                    bestProximityPolyline = polyline.getPolyline();
                }
                bestProximity = Math.min(proximity, bestProximity);
            }
        }
        long time = System.currentTimeMillis() - begin;
        Log.d(TAG, "->getProximityPolyline"+time+" ms");
        return bestProximityPolyline;
    }

    public static float dipToPixels(Context context, float dipValue) {
        DisplayMetrics metrics = context.getResources().getDisplayMetrics();
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dipValue, metrics);
    }

    private List<PolylineInPx> convertPolylinesInpx(MapboxMap mMapboxMap, List<Polyline> polylines) {
        List<PolylineInPx> polylinesInPx = new ArrayList<>();
        for (Polyline polyline : polylines) {
            polylinesInPx.add(convertPolylineInpx(mMapboxMap, polyline));
        }
        return polylinesInPx;
    }

    private PolylineInPx convertPolylineInpx(MapboxMap mMapboxMap, Polyline polyline) {
        PolylineInPx polylineInPx = new PolylineInPx();
        polylineInPx.setPolyline(polyline);
        for (LatLng latLng : polyline.getPoints()) {
            polylineInPx.add(mMapboxMap.getProjection().toScreenLocation(latLng));
        }
        return polylineInPx;
    }

    private static double bestProximityBeetweenPointAndPolyline(PointF point, PolylineInPx polyline) {
        double bestProximity = Double.MAX_VALUE;
        Iterator<Line> lineIterator = polyline.getLineIterator();
        while (lineIterator.hasNext()) {
            bestProximity =  Math.min(computeProximity(point, lineIterator.next()), bestProximity);
        }
        return bestProximity;
    }



    private static double computeProximity(PointF point, Line line) {
        PointF end = line.p2;
        PointF start = line.p1;
//        double normalLength = Math.hypot(end.y - start.y, end.x - start.x);
//        double ret = Math.abs(((point.y - start.y) * (end.x - start.x) - (end.y - start.y))/normalLength );

        return PointFProjection.getDistanceToSegment(line.p1, line.p2, point);
    }

    private List<PolylineOptions> getAllPolylines() {
        List<PolylineOptions> options = new ArrayList<>();
        options.add(generatePolyline(ANDORRA, LUXEMBOURG, "#F44336"));
        options.add(generatePolyline(ANDORRA, MONACO, "#FF5722"));
        options.add(generatePolyline(MONACO, VATICAN_CITY, "#673AB7"));
        options.add(generatePolyline(VATICAN_CITY, SAN_MARINO, "#009688"));
        options.add(generatePolyline(SAN_MARINO, LIECHTENSTEIN, "#795548"));
        options.add(generatePolyline(LIECHTENSTEIN, LUXEMBOURG, "#3F51B5"));
        options.add(parisMarseille("#55AADD"));
        return options;
    }

    private PolylineOptions parisMarseille(String color) {
        PolylineOptions polylineOption = new PolylineOptions();
        List<LatLng> latLngs = decode(ParisMarseille);
        polylineOption.add(latLngs.toArray(new LatLng[latLngs.size()]));
        polylineOption.color(Color.parseColor(color));
        return polylineOption;
    }

    private PolylineOptions generatePolyline(LatLng start, LatLng end, String color) {
        PolylineOptions line = new PolylineOptions();
        line.add(start);
        line.add(end);
        line.color(Color.parseColor(color));
        return line;
    }

    public List<PolylineOptions> getRandomLine() {
        final List<PolylineOptions> randomLines = getAllPolylines();
        Collections.shuffle(randomLines);
        return new ArrayList<PolylineOptions>() {{
            add(randomLines.get(0));
        }};
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
        outState.putParcelableArrayList(STATE_POLYLINE_OPTIONS, mPolylineOptions);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_polyline, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_id_remove:
                // test to remove all annotations
                mPolylineOptions.clear();
                mMapboxMap.clear();
                return true;

            case R.id.action_id_alpha:
                fullAlpha = !fullAlpha;
                for (Polyline p : mPolylines) {
                    p.setAlpha(fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA);
                }
                return true;

            case R.id.action_id_color:
                color = !color;
                for (Polyline p : mPolylines) {
                    p.setColor(color ? Color.RED : Color.BLUE);
                }
                return true;

            case R.id.action_id_width:
                width = !width;
                for (Polyline p : mPolylines) {
                    p.setWidth(width ? 3.0f : 5.0f);
                }
                return true;

            case R.id.action_id_visible:
                visible = !visible;
                for (Polyline p : mPolylines) {
                    p.setAlpha(visible ? (fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA) : NO_ALPHA);
                }
                return true;

            case android.R.id.home:
                onBackPressed();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private static class Line {
        PointF p1;
        PointF p2;

        public Line(PointF p1, PointF p2) {
            this.p1 = p1;
            this.p2 = p2;
        }
    }

    private interface OnPolylineClickListener {
        void onPolylineClick(@NonNull Polyline p);
    }

    private class PolylineInPx {


        private ArrayList<PointF> points = new ArrayList<>();
        private Polyline polyline;

        public void add(PointF pointF) {
            points.add(pointF);
        }

        public Iterator<Line> getLineIterator() {

            return new Iterator<Line>() {
                final Iterator<PointF> it = points.iterator();
                 PointF previousPoint=null;
                @Override
                public boolean hasNext() {
                    return it.hasNext();
                }

                @Override
                public Line next() {
                    if(previousPoint==null) {
                        previousPoint= it.next();
                    }
                    return new Line(previousPoint, it.next());
                }

                @Override
                public void remove() {

                }
            };
        }

        public void setPolyline(Polyline polyline) {
            this.polyline = polyline;
        }

        public Polyline getPolyline() {
            return polyline;
        }
    }


    public static List<LatLng> decode(String encoded) {
        List<LatLng> track = new ArrayList<LatLng>();
        int index = 0;
        int lat = 0, lng = 0;

        while (index < encoded.length()) {
            int b, shift = 0, result = 0;
            do {
                b = encoded.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b >= 0x20);
            int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lat += dlat;

            shift = 0;
            result = 0;
            do {
                b = encoded.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b >= 0x20);
            int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lng += dlng;

            LatLng p = new LatLng( (double)lat/1E6, (double)lng/1E6 );
            track.add(p);
        }

        Collections.reverse(track);
        return track;
    }


    String ParisMarseille = "e~ye|AmcnnCll@`Fq^`eDrjCtK~f@vzBhBdF|wAcsKdq@miFbiL|fFliAhf@ApSzMtqAtB|I{DzV|_IjyDdlLrjF`mCfdAr}BtQ~kGfpDdbCnrAbKzhGsEnHdGxHxqDtrCdRfKrfB~tAphAfaAbAzF`Sbe@|FtA|DgBd@u@r}@lRfHu@~aBlX`ErCvdAvNhm@pQr{@|NhJ\\bbBf~@|KfEx\\urChDwg@db@i{Cjq@ewEnbA}eBrK}\\bT{qAxVcPdQj@~xA~a@z\\lEv\\T|]kFn^_MxUeNbQcN|lGcsFtd@sW`_Ac\\fvA_O|{EZx[{Cn[cGdYwJnk@{Yv`CodBlx@}XnbAuKbv@|AhzDri@n[`Cxm@yAph@oH`v@eUlfGgpBrxByMfPB~fFjXn~L`y@nsAtW`}G`iC|eBtcA~hB~fBlqGjjHx|@zz@nm`@xo]rq@zh@zt@jb@xc@pQphIdxBr}Bxp@|hYdwHphPniEhh@nJts@Xnd@iDpd@gFbj@kRjd@oVpd@{]t\\q]|e@mt@pbLmuRp|A_gBlhMahKljFmaEtq@mn@dr@gz@bNqRh_@{p@v`@o|@ftCwzHru@wrArgEwkFfm@g~@|xB{_Fra@ihA~c@_~AvYsrAdSmqAjtA{aKbZkeAzh@_oArHwL|eNclYv{C_xDhbGu|FbnImiI`oJkvJpcAioApk@}|@xz@a{Alf_@osr@fjB_{BfaBywAn~KquG~yNyeIdcRwqKnc@iYhy@ko@fs@_p@xUcVvgAwsAde@ar@h{QofZrmAulAnaO_lLjt@wo@`x@sz@zp@gz@`w@ejAhr@omAps@{{Axa@icAlyFkpO`qBmkDvyBm_C~z@in@ruBgbAzwYcfIj`]srGdwGqrAbuAy`@ll@iUtOqJlvUcmKn_Bgb@plAyNvkA}CvsJxD|pLpA`zAeCx~AwQj~GajAvk@mMfw@aVdeD{nAr_AiPfq@yE`l@S`tO~Wb{@mAbu@iFvg@wHle@cKn{@qWbv@q[d}@sg@vr@gg@reBodB`^ad@r_FixFxc@mb@xk@oe@ls@ig@r`ZknPzhBeu@bfCys@|~MgqCnaImpCly@o]`~@}d@|w@_i@haAkw@hzNkmM~o@uh@tmAwu@lp@eZd`A}ZfeAuS|`BgLde@_@fqA~FdkE`r@z~@`I|wAj@dTQ~y@aFhq@aHh`AyRxeAm[niJozDvsAcn@jvCocBbdAs`@bp@kPlk@kJ~gAkHvcByErp@aFj|@wM`g@cMn{@c\\xy@kb@bt@{e@`f@k`@p_@}]|v@{|@p~Va}^nyBcaCdx@mq@xz@_l@r~A{x@lrDgkA|xF__BdxAko@ry@yc@ny@sj@nm@}d@bk@mg@d`VwfVbdAklA|cAitAn{@wpAtfAooBj~@imBrt@wfBbdAizC|]sjA~e@kkB`j@}jCd}J_~i@~zFsmWfm@{qBpmAe_Dpi@{fAtj@u`AjeBa`CbwLcsNj{@ujAhg@wy@p\\qo@vb@ebAjg@mwAjtKuj_@ffBavEb_CwrEx}@kyAxuAypB~aBmdCn`Di~El{Ck{D~cGkbGbwImuGrmEykDdgAaaAnjA_iAtvBa`CptAadB|{AqvBdvAsyB|kKsjQ`~C}mEv{CamDtoBenBboWqgUrpBmpBfrAizAjjAcxAh}AywB`nAylBpvAkbCdwAopC|vAk_DlaLo_YrnCukF|oBccDjwHosLnzBawDz{BmeEtvBejEr`BanDdpBuyEh}@}|Bn~AyhEn}AexEviAsrDzx@{qCjoImf[h~BkzGvaCuuFfpLqeThsAamCthAcfCplA}zCpcAwvCb{J_h[xwAqlDl`A_nB~eAmfBdzBw}Cj}GudIh_BwrBplBetCd}@_~Apj@}dAlpAetC|z@iyBd]adAb|@_uC``@_|Ajh@ezBd_@wmBl^kyBv\\odC~]ceD~[_bEtc@{vHt}A{qT`vA{vIphBy_HdrE_rKzxF{zJjlAy|BtlAmoChu@eqBzo@asB`z@m_DhoAevFjy@{xE`_A_~FtrE{jXtlA}`Fj}@gxCjfCohG`|AktCdhA}kBhgAi_BbnBcmCxoNkpPbpBufCbwB_zC|eB{tChhA}rBhvAotCf`AkxBtx@qrBbnS}ii@`uZoij@`s@mtAjw@geBxpI_qSvb@s{@dmAyoBl|AalB~gCeyBtxA_|@fmFooCf{@m`@jdBedA|kBczAdhAwbAhgBqdBjvGa{Gzp@{t@pdBkwBdkAcdB|i\\a{g@fjA}kBhgAqoBjeDuvGrq@mxAbnDk|GtcHyoKpoCa~CjzAwsArzF{rE~sAsiAdpAgoAhhA}jAteB_yB~uUkr\\bl@i_Av|BacEd}LumTtbEclFltHmcJnu@qeAbh@e|@~k@okAh_DkeHbiAmeBffA{kA~pCmqBz{BwpAn}CekB`cAmt@lw@ir@dq@er@|u@_}@blAmdBjmAk`Cvq@meBhVgq@r^gnAv[olArZizAdUopAjOydApUwwBnL_bBjg@m_JpTw}Bxy@g~Ev}@ghDp|B}oFpmAqoBrrAsaBhsBuoBnrBy`B|\\i[xl@qp@t_@eg@rq@}dAdg@u_Ahj@isAbU}l@dc@owAvOgp@lh@mxCpN{lApIs_A|FycAbGuxBhAumAe@ykB}He_DqeAaoUoBciFfDw}AvSwzCpe@{fDdr@s_DjlA_{DfrAokDz~CsjH`uAugDt`AymClg@gbBr[akAxd@upBvQ_|@pb@adCfUq~Ad^cbDrOgmBlOklCrJyzCjF_zDpNkuXfc@ugIlr@cpGfvAgoIz~A_gHxcCacJlmIktXt`ByaG`_AwvDpl@ilCp]mdBf{@yeFjc@ufDpZqwClMebBjQawCxHghBdIonE`Jm}OpLe_Ffp@amIpmAciGryAehEpdC_zE~uB_tDp`AowB`Wcm@j}@sdCxo@ewBtk@a~Bd^yeBdd@wnCf\\eiCzOyaBxSm|CvJe`C~EakCp@arCiEezRtd@cpIpf@}nDbmAchFriA}aD`dDagH|y@csBpi@icBn]esA~Z_eB~Lw|@fP}_BnIeoAtFcaBpAwtAaA{fBajAq{Z{JyqPr[avJ|xAqsM~gByiIxcC{tHh}B_}EdtAy|BfcCqeDtiRymSbb@w`@|`CicB~dCygAlbDks@vjAmJfdCs@`vGbd@zw@\\jhAqFbr@wH`}@aRf}@aZr{@s`@n]}RhpF}yDfuBu~An\\{XxjAeqAdqAgoBt^as@za@y`Abg@uuA~j@yvB`kBasJdcGun[j{A_bFl|@{qBxnAw{B`_G}lHz{AcdB`eA_qA|eAo~A~{@cvAphAevBxi@mmAjz@gvBzl@kfBxo@u|Bfk@o`Cvf@ggCp_@sbCpYq~BrVklCr^o_Glo@e}IxjEecXbsAcdEfaB_}CtbAwqAtoBwcBlo@{a@vqAmq@nbCel@vcAoIlwBlAxdBfVrpIl|B|n@vLfpA~M`v@tCjnAnAxyAeFds@uHlmA}U~cA{YpbAw`@pn@sZlp@u_@t_@sVby@io@t|@qz@dfAklA|r@maAfv@omA|vFyvKjwQ{nXppAuxBbs@}sAtnAanChr@sdB`jAmcDpc@iwA~t@wmCvj@u`CdgA{zFf|DkdV`d@coBxu@sfCzaIa`Uj~CyaJvj@uoBpa@siBreI}td@peBgbIpnB{_HjfD}}IpqEabJr~@e|A|oBuuCraC}yC`vGinGn}AeqAlsAqdA`eH}~F~aAi|@vh@aj@f_@_c@ppA_fBxr@unA|gAmdCxt@csBj_@grAta@ebBrjAkrG|uBk|L~zAkkFvkAspCrxAsfC~~AkrB|dBu`B|tBmuAhmCahAraBk`@t|AqQtdCcEnhJhb@pcBbClpAh@xzBkG~_DaXdpW}nChdB_JtbCkD~_IvJlhAl@jgAsBt~AcLdiAoM`sAqUn`AyUz|A_h@p]sMnxBedA`nAmt@r_B{iAxe@q`@hu@yq@t~AuaB|k@ys@z}Ao}Btl@}_A~}EcrIrnDowFjgDarDx{BqiB|kC{~AhmCwmAv~A{o@rsGavCfu@a_@`o@wc@p}@yw@l_@kc@`c@_n@hmDsjGdl@gr@hjDusCph@mk@xVw[lc@er@l_@su@rScg@pXoy@``@a_BnxAebHtlA{hDldBc{CtjCyqCv|DmbD|uAqfA|kWgoSrcD}cBn{@a`@vmGuvBjh@mTphAmk@z~@em@tn@ef@`~@{{@fdAonApw@uhAvlAewBvZyn@zp@i}Abw@}uBxQ}h@zl@knBtf@ueBjs@{pCxo@kyC~v@ueEri@guDr^s|CxW_uCrTouD`KmyCrDwpCRohA{BgeDgPmgGgk@_~VS_qI`A}Wl@wzJqAmbDgFitDeMseEyu@ctSbEmoIvWmcGdiAchKpxBmmLb~Rmhv@dt@o|Cfn@u~Cxk@gpDzWgrBfUwoBbtJal|@tlIahf@p_@mqBbt@odEnvAgzIf`AkmGpfByuMdcDa{WhaCafS`rAsxGdkBcmFdkQqm^jy@sfBzy@}|Bfj@moBzZmxAlhAwnGp}Gat^xzCkeGxuD{pExu@ifAtbCi`E`oOi{WpvEgfFtxAkhB`bAmwAzjAepBbnBosDrhG}sKneDcpE~eHo}I|iTatXllNsgPh}CqvD|m@qy@~zBedDldA}aBfdA{jBzo@arAh|AakDniAetC`eAowCpnGgoRl|@srCb}AymFttFmlSlcD{xJncDquIzfCabHrvAimEpw@opCdc@qfB~pHsx[nm@aoBvlBcjFdhFcyKb}Fa~IneXei_@rl@{t@rdGwcHtyNg~NllBq_BrnBgpAvhDm_BvqD_eAt}{@mgQp}@oSjtBem@fvAgi@jwAgs@dnAqu@pdAiv@`bAi{@`~@s}@b|@gcAfzAgqBh}@uyAx~@sgBj{@cpBlh@cvArp@otB|`@kzA|k@wiCfd@giC~j@u_EvpIy~q@lp@m}C~z@_wCbe@{nAnl@_qAfbBksCjkBc}B~bDeoCzyNmuIflCieB~yBq_B~bB_uAbwAyqAhjAejA|}AkfBfrAeaBxqAyiBbqAwoBrtAcbCtrAgiChhAubCdoAw}Ch{@caC`jAkqDfgAeaE`u@kbDxr@opDxh@i_D|_A_uGbcGuob@p|D_cNvQus@l^mgBj|@sqE|pAq}DvpBmoDxt@i~@t|AgxA`_CwgBji@qg@zY}\\|d@ep@zh@waA|a@ycAjgB{uExzAmpBjk@wc@`pDsdBxh@c[zc@q[|d@cb@rg@km@di@iz@lk@_hAtuBm~Epg@}w@znA}bArjBo|@xSqMzp@{k@jNgQnj@u~@pvActCvh@cu@tpA}cAhh@uTdeBgUpnBnOveCdd@xi@~Epe@~@df@mBzg@gGnqA}Z~`Ckq@zj@kJbc@eB`g@d@fr@dDhb@DlQs@rl@eHnh@oMhc@aPfk@eYhY}Rdu@k[bhCqb@pdDiA|yA~LvhAbUt|Cl~@ho@hO~}@dPlxAzMnqAjCdO?dkBgEvoCoNr~@}Izs@aOdf@}Mdu@wZhm@y\\lm@cd@lkAqgArfJikJ`r@um@peBerA~jAsi@jkA{b@`nA}k@jpDsrA~sAyk@nb@qUj]kVjXaVzg@ol@fZsc@v^sr@xZwv@vWe~@zQc}@zPorApGmcArBwrA{@k~@eEc_AqJ{aAeqAobImHuxBzSm|DlW_rAfv@ssB~`BcmCnvDkyEzkAmtAvw@uaAteA}|@bv@{l@`i@o\\bf@ga@xh@sh@bdAuzArz@}~Ap~D}`I|cAchAzoAeo@b~@gO``DiFp\\qBnj@kJ|s@eW|vCezAdu@wTfq@_HtqDzGdn@yBrc@qG`f@wNrj@mYn^mXxp@}q@rpAw~Al}Bi~ChlBkrCvwAa}B|X_k@b_@y`AvXwfAlKih@pc@_{CtPejBnDihACgiAcIc|Aub@wrGtEccE~Dq`BSepBoCgbAcw@gkMuDk~BnDsyA~e@{zFjH_tAdA_iAcM_kFv@c}AbQ}vBd[gwAz[q}@hn@ghAlm@_q@xm@ic@`f@{Sxo@mPlpBeWjw@iPtMqDlk@sWvf@q]vPuO~WsZ|Vu]pUcb@``@c_Ar`AgzCl~HedWldQ{}e@f~@{qAz~@}x@rbAej@jgAsVllAiE~c@~BfHfD~mAxWtjAnd@pX|F`l@|SfJhBxcBry@lsKpyEnzZ|uMpkL`}FpnSlxMjz@hn@zi@v[rbFlhDbfBjeAlqAxr@~vAlq@`nAfh@||B|y@hmAz]vlBzf@ppF~pAzeFfwAjkFptBrmIdvDd{V|zKpqBfy@|sDzoAzbBtf@nwAv^noBz_@xaBjVftBbTv|BnN|fCnDblBy@zxAmEzyAgIb{B}RjmBcUz|D{k@rtJg}AlbDeW|sBqCl}AdDhlA|H|~AjQpkFf|@pdCje@~gD`g@nvB|Wfw@hInoBbOnoBnHdtAvBx{@Jh}A{AdxB}HdtAeJt`BeQ|_Dce@f}@gPhjBwb@`x@{ShfBoj@vaCw}@joAkj@nqAsl@dtK_fFjxDwxAfiBcj@jqDyw@`bCc_@fwFk_@z_C_CfiElHjbF|b@x{c@`bHhwIdmArsGp`AzdRbvFdgApTvc@rE`{BbLxbQhZjtA~@fy@cAfh@qEtf@cHlReEbu@qVth@cV|mWgtN~eHawCxtBqi@x}Cuk@p_JmiAvpq@{`JniM{~Ab`RwgB~e@uFjoF{e@nwDe_@tjP}yA~qJk_AdnBoWdtA}Uh`Cij@h{B}p@naByl@x~@u_@xgBsy@~aAef@f~MmcHxzGudCbqD_w@f~Esf@tiFiGfvJfZ|}@Olt@_Dnz@uJplFy{@|NKhvAvBdlBjVl}@dGzkAJ`z@}DndAuRhl@iOb}Awz@rjAe{@x^{[haAccArm@ir@lzA{iBrpBynCvfBsmCd|AmfCnpHklMdfH}tKv_JwtK`zDktDdcC_hBlzC_uA~lAqZx{AqQtfBoB`gBdNfd@`IjpDfbAbvNtsDr`@hI~{@dNzq@lGht@pAre@Cdr@uCdd@aEpkA}QbnAwXfsAa_@x|Aei@v\\mMfh@uWtx@md@f~@cs@dw@}u@|s@kz@l{@yqAth@a}@d_J}{Or_CocDlwAuyA|cBwkAngBsv@pbAgXh_BgXjxCwTfuFwJbfD`OvtCfl@fwBpz@fpGroDlfB`{@`lBvv@t|HzqC`nF~dCduDr_CzbDdhCpjBv`BnyAfjAxdApu@~bAno@x~@dj@b_CtmAteArh@deCzhA~jGlaDxhi@`_[rrDhmBreEriB|nCbeAthC`|@d_Dt|@d{Bpg@|lCjh@vsAxR~uE`h@toArLhlEtXdvLzn@lcFxi@vcGbpAdaAxYraEv|AjaGj}ChcJj}Fv}@rg@r`@nSvzAzo@`mAdb@|eAhZ~qApX|hApQrxHr{@~vBrZvfChc@vgBxb@b_Bhc@hdFthBpzBt_AzfIl|EbaHd{FdkL|iMhnAtjA|fApx@d_A~i@tlAfj@dkAxa@t~Apc@jpBjc@`s@|JjzAtZlkFrcA`gBhk@|cCphAn_DpbBby@p^vfAva@piAp_@|tBtg@ppBz]trEdq@faAjVl|@r\\t`A|i@niBheBxnBfhD`vC~sJ~f@ftAvu@vcB|Tz`@rd@ns@ng@tr@v{@`bArxAjuAzaBntAbhAh{@tiBzkAdd@~VdoAbn@n~@b\\pq@fR~pCjg@xpYt{DnbHd_AvyWnfD`{AtOlfBbIxsBjBfhAkEnxAsJfv@sJ|aAwO|iBuc@jo@iRfvBey@npAcl@ntJ{hF|lEk}BflCiaAh`Bs`@p}BaZvmB{FdnApA`uAvIl}D~r@ffDv_AroEfxAb~@~WxcBvj@`lKbdDrhMjsE~d[btL|pFxpB|kZpfLrjCj}@~dBbd@xgBr]`kAtOpoApKt`BjGlpBx@vrB{Bb{[{`AviE|A`kDzNfjIlz@n|Cbj@vfIzpBj}B~e@rdD~f@tjBlT|p^`bD~lB|]|t@nRreCn~@lsB`iAdw@fi@r|ChlCl~Jn~Jn{BbrBfsAheAdhAfv@nmBvfAr`Bxt@zsBtq@`rA~Zb_AjOvgBpTb`CxOtjFtShnG|d@p{HlbAtqD~o@xyBpf@vjDl}@teDjdAnL`Gb|GbfClmGhzC~bHb}DfoAfq@paA|b@rmAva@jjBlg@`wAxXjiAhNhq@|FlnF`WvkBtFdoBvJhwKd_@lnAx@|}@aCn}@iHrfA_Pnx@yP|{@mWjvLe_EdxIa|CtdCybAfxEqsBv}A{h@fgAyZnnBu_@vrCg[fuAiGx}@iBtqA?roPjIvqAfP|vB|q@bvBnjAjeE`rDpiCtaChsAxhAzoLpqJtkDpvClyAlkAxq@ff@lx@dd@rXzMzx@tZr_@rLpy@bR|eA|Ld|@~Enz@Fbo@oBbbAoIfk@}I~z@oSh\\aK`hA_d@fq@q_@jjAax@baAu~@ld@of@~iHccJbfAiy@~{Buz@jo@gJlpAkDzsAxK~nAr[p|@`a@dgBfeAjk@~Wzs@`Tpe@rJ|_@jEdq@pBjf@e@dv@}FvoIebA|gWyaDftAyVzgBua@|yBwo@xdB{l@vnKqeEhrNmlFbhEu~@l{Eke@|fLma@n|Lsi@v{CwSbtA}OzoA}RrtAsXnwAs_@psA}a@`tAyg@ddAid@zvAou@laB_cAr{AqeAlmAuaAt|CquC|zHixH|eD}gCbeBqfAhqBwbA~dEm_B|{UetFzrEqbA`tK{dCv}@{X`i@kVxq@qb@pg@qa@tkFynFxuEm~ErzCmuCv{AaeAdxHyxCr_@mCdmAdNbz@he@xyB~dChk@nh@lZtRl_@jNdZbF~YvA|`@wAja@yGf`@eQfa@m[~{@o_AbrAm}AnbAawA~Rm\\~u@udBrlE}oJxvA_bC`cJkjPtXym@b^gcAvaAmiDpfBwmDvFuEn^kj@bOe[f~@msAdeAucB~gCmqD|lB{zB~PkYjQqj@lb@kaBzTiiAlIwRtJu]foAi{E~e@gt@`Jg@vKbE|q@nn@fdC~kBf^`\\zuWr_Tpl@tb@vNvDpQEnlBsp@|dI{wA~vDkh@~m@iNn{@qZl`Biu@vs@qWr}@ab@|v@of@|e@ca@po@gq@bj@kw@n[uj@v^ux@`oBetEz]sb@|`J{pIro@uWl_Esm@|j@{O~e@mW`_@e\\h^of@nNkVn|E_{JxcBqkA`wMumBdnMix@lqDlNdsBx`@|tB`t@~rFvaCzrBfw@tdAh^j}Cv_Ab{@fShkBfl@t}CjsApsCheBnbFf`ErkI|aIfeDhrDxmAdpAvtBplBz{AvmA|jBrrAdrB|nAddB~~@j~Axv@hnC`mAhi@nTx~H`rCjwErrBzoFdwChs@|b@lmAh}@jt@rm@jsBlrBdxBnhCbqCt|D~mAn|Al}@f}@vi@rb@by@pb@zk@hRpk@rJpb@dDha@j@pz@mAbtB_NxzAoNjfCo]thBk]bfCem@~uBco@dvAyf@|aFcrB|qGwzCz\\uQ`b@yYtw@iw@`k@ww@nh@g~@|Zes@~eBqnEvnB_{CtaCq{BtnAks@`hCafA~eAyl@~sAs~@buBo~Af`BuqA|rDobDvuAcsA~yBm|BprAo|ArwAejBxzAkxBjw@asAbeAevBjg@ikArt@_qBvc@q{Add@umBbJec@hHsf@lFgi@hC_bAaAmv@aJigE`Pm`Bhj@qbBxp@s~@dq@cc@laHcbCvbAod@~a@}U|u@{j@|hBuxAby@_e@~_Ao]r{@}Oz\\qCv`AQjaAnHdtAr]`dD~oAfh@`Qbu@lL|b@hClkAvCz_AlNdl@~S`r@xc@r}@neAvVle@vRzd@hzBjzHx[~{@zQl`@jh@pw@j\\~^fgDncDbp@feAlnAdaDb\\nk@lThXzj@ne@bd@`Uzb@jLva@|D~\\d@rgAsCdnEgFzrAjc@th@xc@bdDjuEde@fi@`\\nZ~\\|Tz`@vQza@`Lxa@tEnXZrTsAje@eIla@eOzxCqzAbr@uNjy@Abt@nLn_Df~@v^dIlhB|Rb}AfMnIXn_CqI~qBuDryEwN~`ASbxDrIbmCnSpwC~_@dqDxs@byk@d~Lt|@fOn{@hHbl@dBrz@oAvy@oEpv@wKldAyVps@_W`vHwcD`tAoVvnAaI|x@n@zmAlJx}@jQniCtfAtfBjuAzaRxdQ~bLllKv`Ant@nbA`m@z}@dc@~dA``@`qA~^~m@rMb~@zKj[~Blx@vBrb@G~rCqLv`BdB~jJnhAzc@`B~RM`o@kH|f@yNno@e\\l\\aWfkAomAvZoVbaAsd@p\\cIzx@uGnr@`Db`AfQ`eDhs@dgChf@x{AzTllGlr@zoApGna@l@bdAkAj`AiGzs@wIthXa}EtqJ}fB~fFoo@l{Ec\\zaDqL`kIvArbKxY`x@m@bo@wCfx@cJzgAcV~g@yO~x@e\\jeAym@n{@sn@`UgRl_AqcAz|@umAj_Eo_Ghx@sw@~s@yf@njAsg@hcAuUn`ByIzgApF|i@|IfoGz`Br_A~Mjc@hCfa@TbdAsEf{@kMpaAu\\hWkNti@c\\fv@im@ngSgbQtbBufAvyAgo@lvAqa@bz@ePdtAsNf|AiEhrCzKvyKl`Adp@bChsA_@|b@sChq@qJt~@qTrp@{Vv^{PhW_OfkAwy@`g@ae@fs@gz@jh@{v@hrCmgFxgAklArbB_eA`~HkeDhx@_a@`x@yg@fs@al@ha@o_@f~@ggAd{EskFrrDydCbv@i_@jmCubAzqEymAdo@eTv`@eQ~ZuPrg@}]bo@mk@bo@ew@f_@ul@zc@i}@l[yy@l\\_kA~qAefFjz@{dBvNkUxjAesApoAu~@|\\mQfyAmb@|wAoMr{@bAd|ApQrrBzo@vq@|O|o@pHde@dB`c@[bf@wCfk@qHts@gPbZeKrj@eX|s@_f@dy@qs@zoBckBh}AujA|gAwd@pdDwk@fp@_Rvh@iUbm@a^hz@go@fiC_tBra@gWvmBeaA`iBqk@vqB_\\bqBiJ~lA`@f{LrYtcTtc@~}@]|s@wCjgAyMv|@oRny@yX`{@o^dtEocCjoAqc@hxGw}Alp@wLdxLgnCbxBwKjpBpI~eCbi@ryAvn@bxAj_AjvA`sAveEhiFzy@n}@baA~|@njB|xAhgLzzI~w@`e@hn@hX`i@zPjz@|Pbg@`Gt|@tCnq@y@nu@gFzVsDfhDos@tmB_KfuA|Fp~AxO~tBbh@fiBrw@ts@zb@zaCrtBp_AnjAn}AfxBhq@bw@dm@ni@rVlQve@pXvl@nW~t@`Sb_AhNxZrA`_@^naA}D~k@gJ`s@yQl[qLh]kQps@qd@`f@ua@r|BytBx`CmaBjgCymAnaDu~@|iCo^zgVesA~wEjHvzApOlpAlThfGvuApiB`ZfmCzYreAdHjsCzHjbBbAfgByAneCqKjaLwl@|}Q~Uvd@OraBcGdl@oDfrAeOrkBu]lfAaWl`@yK~aAq\\b}@}^`fCctAv|Cc`B`iBii@v_BwS~~@_EbbDeCt{@mDdp@sF~z@uL~tAsYzlBgs@b_DwrAbxAq`@ty@{IjhA]|nBvVfqEl_Azk@xGzm@rA|c@eAp`@qDl`@cIfWoHdg@ySzaBsy@xoA_d@jxFymApuGmq@xu@mQ~\\mLjbC{jAb`@aJxsAiHh`AlIb{Btd@xh@nHbrGd\\biBnc@xaAhh@veArz@hiBxyBxi@bh@ncJlhHby@vk@pr@ha@dr@v[~mAp`@~w@vPrbAxK~kAlEfwBkDtzG_S|yCvKpvBxTzqPdgCnsDv_AzdFhhBjkUbdI|`C|t@zsAxXzvAbTxlB|Nn|@hDr`CFnmAyFb`B{MllAaOj{Fez@zdAkLh`CaN`qAmChyCpChwFzd@lkb@d~FdfYnnE~xBnd@|yKjeDfpCndAbcCtiAbrCvaBr~FnmEpoGdjFv{Dv_Dj`Ep_DlzBpxBxoAbbBjeBpxCdjBhxEts@hkCfp@`iDlfBjdLdk@znC`i@`sBliA`hD~f@jkAxaA`mBj^rm@je@|s@pq@f~@~cA`lAzvA`vAlzAvkAb`A|p@x|@hj@~kCbwAvxJbrEz_O`uG`qBxt@dbA`YdtAfXb`@xG|`BxNtaAlD`q@bA`wAoAjiBoI|hFaZpkB]`tB`KpnDxk@p}G|pBpgArWve@fId_BjQbeHlg@zfBpKxcBnFtsAhBnoB_An|BeIjkB}MlvBoUn|Cae@zcIutAth@gKzdD}z@zeBem@tzAgo@rsAor@hgB{eA|sFqpDztBm_AfnAk`@jqA}Xf~@aIzzAaQn`AuAn_[ljA|xFjd@bkE|o@xjMpqCfhBpXnhCnXhgAlHbbC|Fnx@PrqBaDv_CoHvjHy\\nlBgGhzKqOd~M`A`jC{AflBsE~wDiUheBkPbpCa^niAqRbqCin@|~Aec@bgByk@ntEmsBfrGywCj{GsfCnrQ}iGxpBuo@|zDicA`dDon@f~Gsy@laDgStjCoHlgDd@`mD|VptD~r@phNn`EhqLvgEriChjAliCtvAzqBxsA|sBh`BrsCxlCh{CdmDzsBfvCtyDjzGzlBnxDpjAjyBfqAv{Bhs@|gApjBfgC|{AfcBvcAp_Aju@hm@tv@~j@b|@`k@tqA~r@f_Alc@`k@jUn~Arg@fxAp^x}@tPb}AnSlkAvIr`A~Evm`@te@nkErUpuBbXhsBte@fnA`a@z}A~n@dyAzt@dfIzuEl~BpjA`eEvbBnzAfi@lvCx{@pp@jQt|Bdi@ltCdl@dtBl`@nbDjx@|bB|n@pwA`q@phAll@lvAj`AtyCflCfrGbnHfyAz|A`pAvlAhdBrvA`wA|cAv}A|bA~_Bj~@hu@f`@bdBzt@xy@jZnnB~n@t`AxXbjCpm@dqAbUtwCf`@vkClUrxChNz{BtB`jAq@dmCkGzbIsm@tgc@}nDfcDg]vjAmOvlFifA~lIcfBdbGgx@hpBgPjdFkSfqRgCp~EqFhpCaI|yEsT~fOqbAlsBwRp_BeWzdBi`@pyAoe@hqAch@dxBgeAjsI}`Ef{As[|{BiN|fPz\\`aC~WpjCfw@xcAtf@xyA|bAp|QxaR`cAv`AhsAzjAdkA``AdmC|lBb|KdoHztb@r_YluAhy@fq@rZxk@xSd[bJ|v@xPzdAtOr{Gbs@~|AxLz|@rBrV?pq@qBlp@kF|kA{Rbu@oS`|Akl@|mFu`Cv{Asw@j{@ai@fhAky@fkAqdArjA{mAteGoeHxdD_lCffAyi@jrB}m@|xD{YlzCdCfbAcC~y@}I~o@iMvg@cObs@wYxs@ea@`o@ce@|m@cl@lr@ay@`b@_o@rdAkjBfu@qdBn~DqeKxqCgtFl~Dk`GtdEisExvDk}Cvdl@a``@rfBg{@hhUctIvcAmc@tbAaj@|iGosDl`Awo@rz@}t@~\\}^hbH}_Jvk\\ujc@zn@}eAbc@g}@~f@woAhc@exAz\\{xAnYkeB|LogArx@epLlpEadq@jm@gjDdr@a`Cz|@ouBxcJshP~bDgnGxy@yyA~sB}gCvbA_bAzdEumDhfDsgBx{Aee@dqKw`CleSmlFteDedAvdHwoCbvBkY~sVwj@|{A~FddCtc@~zB~|@d~DbmBxh@|QlXjHtg@zIpj@zEja@`B~_AkD`q@wIdt@sPh_Li|Dve@_K`rBwXzxWcsCpeBrDlvEpn@brB`\\buOnwBn|@tGl\\PbgA}Ftv@eLfj@aNfuXklIf|MutDjfGmeBte@iOd\\}Mj|@ee@|dUemMpwJywFd_Bqj@hnDy[tzIac@la@{Dfa@y@nbk@m_Dvf@z@dkBhPlyNz{Bvi@zDv`@t@du@oCz_@mDla@wG~~@{Wbi@}Vpd@qXns@{h@te@ie@~f@kp@lwC}sE~iAoiAzmA}s@vrEmeA`sAyUrxA}]ddBek@tbAic@vg@_Z|e@i]xl@ek@vc@}h@rg@sw@zX}i@`[kv@|Qcj@tWufAbMkr@tMy{@`K_{@pi@{uGzUw~B`sAexIn{O}cj@jbCmoEp_CunCb}E}pEzbA}fA|hAwwAreA__Bx|D}lGl_DsfDx~AycAljGowCzdA{m@hbAkt@pbBc{Apk@io@j^wc@zpAyeBblAylBzdAilB~xAoeDlt@ylBvz@chCnj@_rBpg@maCniBktJnpAweEdfBkmD|nDkiEl}JqdIvs@{g@du@so@jrGkoEl`EkmBvrBwm@dbAoQ|v@kJzqAcIj~GaBbo@}D~j@qGjw@qNz`@mJjz@{Xbq@o[v}@og@l_Aoq@trEugEv`Cm}Bv_Bk`B~dAmmAh{AyrB|yBaeDhnJ}xNdpOucRd|As{AvjEy}DfzCc~Bb_D{lB`oEgnBfyBcs@heWw_GxeDag@xqAsKtyHaOzkAK`cDuErrBqJtnB{P`}AmS|rAwUprA{YdoAg\\jgBkk@hyAck@v}BeeA|wHafEhrB{q@znBiZ`fCgC~_ClVrzFbaAtfE~m@buCxYfkBlKtpCnI`pK|RrxBiBbxByK`zFwa@~tAyPzj@qJdgBka@~xA}f@voAkj@pkA_p@nfAqs@vcBsmA`vPorMzrBymAj_D}`BboB{~@fc@{PhcEqsBfbCuvAj}@kp@bdAky@lh@md@xfAmeAxtAwzArbIcbJbia@wod@z_@mYtoAmfAhuBgyAveD}kCzu@ss@fhBepB~v@adA|y@_sAnc@{y@jl@_rAlc@klAvi@wfBb\\wnAfc@{jB`gC{gLhZ{_BpxC{fQn_@ggCrtAoiLtx@kwGze@ydC|jBmuF|wC}`FxzCwtCxhNgcJtq@}j@nr@gs@~n@ky@zk@waAtf@whA~|@wgChbBcnEdgAcyAfbAov@vhAwh@zzMeuErzBg_@l|AiGlkAfDbaBtUltB`w@hbD`uBpbI~iI|`At|@h~@ps@f{@~h@r}@ta@rq@fTt}@lQbs@xGxp@vB|s@w@~w@sGndAmRjl@oQjk@kU|u@sb@~q@mf@~~AkxAlzJwgJ`wCecCjkDgeCniEcdCnbAig@piAud@p{Bgu@hwEsiAp{AkXd_NyzAjrBaQbw@yHpmHmh@jjCyBzdBnKd~AtZdaCd`A`aBrhArvBtjBjbArr@tg@jXfv@xZhk@pPfy@jP~p@|Gtf@~Bvm@d@ns@mBr{K{}@rz@gIlgBiUtpA{TvzAg^rkAi^dhBqq@vzAor@tfBobArjAwv@jw@}l@tpAmgAbkA_iA|gBcqB|tKquMl{TurTxkAuoAnw@e|@luAqdBtiBugC~eByrCdhE{xH~n@mbAl{Bq|CfwCidE`iA}fBf_AojB`k@{uAbc@eoAvp@_tBnx@wiD~Uko@tw@omApw@ar@brA}d@z}@kHbkC|Efe@Nxc@yCvmAsR|oAsVzv@_Tj`@wOhp@kc@f\\sY`eEe{E|s@g}@x`@gp@jZap@bPkd@zfBkyFhgA{gBp|@wz@xrI{qFv}BygAj|DmmAbl@yVvh@}^z`@ua@fZsb@`b@uy@pd@uiArqA}zDpx@ylCbPmw@rFw]~Ec`AxOsxFl`@uaCnjA}`C|jAucAjdImeFtZoUv|@yx@~nBycB`_@cPp_A{Kdt@lHf\\dLpr@lg@h~AzlBzVhS|YxNh\\nHt^jAxRgAp\\sH~d@sS|hGcfDzp@qKtdBmHbq@cHrYkJr`@wTzV{V~Sa[`Wyl@peAstCbm@ui@ln@qVphCgB`f@{FtZuJh\\kRnd@}_@pl@gs@~rBozCbrAcfClUq[v|Ca`DdcAs[xx@~BfbBjn@tX~ClWEv\\kGrPeIphCy{AfIy@vfBkt@jrCigArl@jG~|A|^j{Blf@pNxHhdAjSbnCjf@o@oNps@iqFnaEu~CvDnArDyB~@yIYkBzcEgsCpDfCpBgIvJsGdk@{G";
}
