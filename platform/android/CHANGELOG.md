# Changelog for the Mapbox Maps SDK for Android

Mapbox welcomes participation and contributions from everyone.  If you'd like to do so please see the [`Contributing Guide`](https://github.com/mapbox/mapbox-gl-native/blob/master/CONTRIBUTING.md) first to get started.

## master

 - HeatmapLayer [#11046](https://github.com/mapbox/mapbox-gl-native/pull/11046)

## 6.0.0-beta.2 - February 13, 2018
 - Deprecate LocationEngine [#11185](https://github.com/mapbox/mapbox-gl-native/pull/11185)
 - Remove LOST from SDK [11186](https://github.com/mapbox/mapbox-gl-native/pull/11186)
 - Transparent surface configuration on TextureView [#11065](https://github.com/mapbox/mapbox-gl-native/pull/11065)
 - Constrained setLatLng documentation, expose setLatLngZoom method [#11184](https://github.com/mapbox/mapbox-gl-native/pull/11184)
 - Integration of new events library [#10999](https://github.com/mapbox/mapbox-gl-native/pull/10999)
 - AddImage performance improvement [#11111](https://github.com/mapbox/mapbox-gl-native/pull/11111)
 - Migrate MAS to 3.0.0, refactor GeoJson integration [#11149](https://github.com/mapbox/mapbox-gl-native/pull/11149)
 - Remove @jar and @aar dependency suffixes [#11161](https://github.com/mapbox/mapbox-gl-native/pull/11161)
 
## 5.4.1 - February 9, 2018
 - Don't recreate TextureView surface as part of view resizing, solves OOM crashes [#11148](https://github.com/mapbox/mapbox-gl-native/pull/11148)
 - Don't invoke OnLowMemory before map is ready, solves startup crash on low memory devices [#11109](https://github.com/mapbox/mapbox-gl-native/pull/11109)
 - Programmatically create GLSurfaceView, solves fragment bug [#11124](https://github.com/mapbox/mapbox-gl-native/pull/11124)
 - Proguard config for optional location provider, solves obfuscation warnings [#11127](https://github.com/mapbox/mapbox-gl-native/pull/11127)
 - MapView weak reference in global layout listener, solves memory leak [#11128](https://github.com/mapbox/mapbox-gl-native/pull/11128)

## 5.4.0 - January 30, 2018
 - Blacklist Adreno 2xx GPU for VAO support [#11047](https://github.com/mapbox/mapbox-gl-native/pull/11047)
 - Bearing tracking mode GPS_NORTH_FACING [#11095](https://github.com/mapbox/mapbox-gl-native/pull/11095)
 - Disable logging for missing location permissions when location is disabled [#11084](https://github.com/mapbox/mapbox-gl-native/pull/11084)
 - Create offline handler using the main thread looper [#11021](https://github.com/mapbox/mapbox-gl-native/pull/11021)

## 6.0.0-beta.1 - January 26, 2018
 - Binding integration for expressions [#10654](https://github.com/mapbox/mapbox-gl-native/pull/10654)
 - CustomGeometrySource [#9983](https://github.com/mapbox/mapbox-gl-native/pull/9983)
 - HillshadeLayer and RasterDemSource [#11031](https://github.com/mapbox/mapbox-gl-native/pull/11031)
 - Revisit marker placement for snapshot [#11029](https://github.com/mapbox/mapbox-gl-native/pull/11029)
 - SafeVarargs annotation for expressions [#11027](https://github.com/mapbox/mapbox-gl-native/pull/11027)
 - Expression#toString [#11024](https://github.com/mapbox/mapbox-gl-native/pull/11024)
 - Rename initRenderSurface to onSurfaceCreated [#11023](https://github.com/mapbox/mapbox-gl-native/pull/11023)
 - Expose attribution manager as public API [#10942](https://github.com/mapbox/mapbox-gl-native/pull/10942)
 - Replace Mapzen vector source example with Mapillary [#10931](https://github.com/mapbox/mapbox-gl-native/pull/10931)
 - Add Hebrew localization [#10967](https://github.com/mapbox/mapbox-gl-native/pull/10967)
 - Cleanup gradle configuration files [#10903](https://github.com/mapbox/mapbox-gl-native/pull/10903)
 - Send double tap event only once [#10855](https://github.com/mapbox/mapbox-gl-native/pull/10855)
 - Parameter validation for LatLngBounds#from [#10831](https://github.com/mapbox/mapbox-gl-native/pull/10831)
 - Replace JSON parsing [#10815](https://github.com/mapbox/mapbox-gl-native/pull/10815)
 - Orientation change regression test [#10814](https://github.com/mapbox/mapbox-gl-native/pull/10814)
 - Max & min LatLng constants [#10780](https://github.com/mapbox/mapbox-gl-native/pull/10780)
 - LatLng#wrap return new instance of LatLng [#10769](https://github.com/mapbox/mapbox-gl-native/pull/10769)
 - Custom library loader [#10733](https://github.com/mapbox/mapbox-gl-native/pull/10733)
 - Inconsistent parameters for LatLngBounds.union [#10728](https://github.com/mapbox/mapbox-gl-native/pull/10728)
 - Gradle 4.1 / AS 3.0 [#10549](https://github.com/mapbox/mapbox-gl-native/pull/10549)
 
## 5.3.2 - January 22, 2018
 - Validate surface creation before destroying [#10890](https://github.com/mapbox/mapbox-gl-native/pull/10890)
 - Add filesource activation ot OfflineRegion [#10904](https://github.com/mapbox/mapbox-gl-native/pull/10904)
 - Save configuration of UiSettings [#10908](https://github.com/mapbox/mapbox-gl-native/pull/10908)
 - Do not overwrite user-set focal point [#10910](https://github.com/mapbox/mapbox-gl-native/pull/10910)
 - Camera callbacks for velocity animated movements [#10925](https://github.com/mapbox/mapbox-gl-native/pull/10925)
 - Allow changing the used OkHttpClient [#10948](https://github.com/mapbox/mapbox-gl-native/pull/10948)
 - Validate zoom level before creating telemetry event [#10959](https://github.com/mapbox/mapbox-gl-native/pull/10959)
 - Handle null call instances in HttpRequest [#10987](https://github.com/mapbox/mapbox-gl-native/pull/10987)
 
## 5.3.1 - January 10, 2018
 - Blacklist binary program loading for Vivante GC4000 GPUs [#10862](https://github.com/mapbox/mapbox-gl-native/pull/10862)
 - Support Genymotion [#10841](https://github.com/mapbox/mapbox-gl-native/pull/10841)
 - Getting bitmap from drawable fix [#10763](https://github.com/mapbox/mapbox-gl-native/pull/10763)
 - Use US locale when converting between int color and RGBA string [#10810](https://github.com/mapbox/mapbox-gl-native/pull/10810)
 - Harden map destruction [#10811](https://github.com/mapbox/mapbox-gl-native/pull/10811)
 - RTL support [#10828](https://github.com/mapbox/mapbox-gl-native/pull/10828)
 - Allow configuring Http url logging when a request fails [#10830](https://github.com/mapbox/mapbox-gl-native/pull/10830)
 - Don't send double tap event multiple times for telemetry [#10854](https://github.com/mapbox/mapbox-gl-native/pull/10854)
 - Fix code generation [#10856](https://github.com/mapbox/mapbox-gl-native/pull/10856)
 - Use the correct cancelable callback after posting cancel [#10871](https://github.com/mapbox/mapbox-gl-native/pull/10871) 
  
## 5.3.0 - December 20, 2017
 - Add support for TinySDF [#10706](https://github.com/mapbox/mapbox-gl-native/pull/10706)
 - Save restore MyLocationViewSettings [#10746](https://github.com/mapbox/mapbox-gl-native/pull/10746) 
 - Post animation callback invocation [#10664](https://github.com/mapbox/mapbox-gl-native/pull/10664)
 - Allow configuring Http logging [#10681](https://github.com/mapbox/mapbox-gl-native/pull/10681)
 - Fix reverse scale gesture [#10688](https://github.com/mapbox/mapbox-gl-native/pull/10688)
 - Update offline region metadata documentation [#10693](https://github.com/mapbox/mapbox-gl-native/pull/10693)
 - Post camera listener invocation [#10690](https://github.com/mapbox/mapbox-gl-native/pull/10690)
 - Activate filesource for offline region creation [#10718](https://github.com/mapbox/mapbox-gl-native/pull/10718)
 - Update Spanish/Vietnamese translations [#10740](https://github.com/mapbox/mapbox-gl-native/pull/10740)
 - Update instrumented make target [#10724](https://github.com/mapbox/mapbox-gl-native/pull/10724)
 - Remove black flash on start for fragments [#10717](https://github.com/mapbox/mapbox-gl-native/pull/10717)
 - CompassView decode crash [#10717](https://github.com/mapbox/mapbox-gl-native/pull/10717)
 - Android SDK renaming [#10609](https://github.com/mapbox/mapbox-gl-native/pull/10609)
 - Map touch listener based lists [#10749](https://github.com/mapbox/mapbox-gl-native/pull/10749)

## 5.2.1 - December 6, 2017
 - Close race condition in RunLoop [#10537](https://github.com/mapbox/mapbox-gl-native/pull/10537)
 - OkHttp 3.9.1 [#10515](https://github.com/mapbox/mapbox-gl-native/pull/10515)
 - Attribution anchor point fix [#10558](https://github.com/mapbox/mapbox-gl-native/pull/10558)
 - Pre API 19 VerifyError [#10579](https://github.com/mapbox/mapbox-gl-native/pull/10579)
 - Set larger Http request limit [#10567](https://github.com/mapbox/mapbox-gl-native/pull/10567)
 - Remove jar generation from maven publish [#10625](https://github.com/mapbox/mapbox-gl-native/pull/10625)
 - Enable Map Rendering when paused for multiple window support [#10509](https://github.com/mapbox/mapbox-gl-native/pull/10509)
 - Activate FileSource when listing offline regions [#10531](https://github.com/mapbox/mapbox-gl-native/pull/10531)
 - Harden MarkerView integration by checking for null bitmap [#10532](https://github.com/mapbox/mapbox-gl-native/pull/10532)
 - Use concurrent lists for camera change listeners [#10542](https://github.com/mapbox/mapbox-gl-native/pull/10542)
 - Handle destroy activity as part of theme switching [#10589](https://github.com/mapbox/mapbox-gl-native/pull/10589)
 - add FileSource activation/deactivation to MapSnapshotter [#10556](https://github.com/mapbox/mapbox-gl-native/pull/10556)

## 5.2.0 - November 17, 2017

- Monkey crashes [#10472](https://github.com/mapbox/mapbox-gl-native/pull/10472)

## 5.2.0-beta.5 - November 14, 2017

- MapSnapshot attribution [#10362](https://github.com/mapbox/mapbox-gl-native/pull/10362)
- Downgrade min sdk to 14 [#10355](https://github.com/mapbox/mapbox-gl-native/pull/10355)
- Harden deselection mechanism for markers [#10403](https://github.com/mapbox/mapbox-gl-native/pull/10403)
- Cherry picks to agua [#10442](https://github.com/mapbox/mapbox-gl-native/pull/10442)
- Rework test activity gen script setup [#10365](https://github.com/mapbox/mapbox-gl-native/pull/10365)
- Fix broken android unit tests, update test make target to SDK [#10387](https://github.com/mapbox/mapbox-gl-native/pull/10387)
- Check for null value when calling mapboxMap.clear [#10388](https://github.com/mapbox/mapbox-gl-native/pull/10388)

## 5.2.0-beta.4 - November 3, 2017

- Revert adding mapbox-android-core dependency (#10354) [#10380](https://github.com/mapbox/mapbox-gl-native/pull/10380)
- Asynchronous TextureView [#10370](https://github.com/mapbox/mapbox-gl-native/pull/10370)
- Workaround OkHttp bug on Android O [10366](https://github.com/mapbox/mapbox-gl-native/pull/10366)
- Revisit logo resize [10553](https://github.com/mapbox/mapbox-gl-native/pull/10353)
- Logo resize for MapSnapshotter [#10312](https://github.com/mapbox/mapbox-gl-native/pull/10312)
- Make location provider optional [#10354](https://github.com/mapbox/mapbox-gl-native/pull/10354)
- Check for positive animation value [#10348](https://github.com/mapbox/mapbox-gl-native/pull/10348)
- Fix IAE of ease/animate [#10338](https://github.com/mapbox/mapbox-gl-native/pull/10338)
- Run full test suite on CI [#10333](https://github.com/mapbox/mapbox-gl-native/pull/10333)
- Make sure camera position gets updated in onFinish() callback after camera.move [#10324](https://github.com/mapbox/mapbox-gl-native/pull/10324)
- throw IAE in animate() and easeCamera() when duration <= 0 [#10321](https://github.com/mapbox/mapbox-gl-native/pull/10321)
- Don't save state if map hasn't been initialised [#10320](https://github.com/mapbox/mapbox-gl-native/pull/10320)
- Make map snapshot optional [#10310](https://github.com/mapbox/mapbox-gl-native/pull/10310)
- Synchronise locationlastions with Transifex [#10309](https://github.com/mapbox/mapbox-gl-native/pull/10309)
- MapboxMap#addImages [#10281](https://github.com/mapbox/mapbox-gl-native/pull/10281)
- Move shape annotation click handling to core [#10267](https://github.com/mapbox/mapbox-gl-native/pull/10267)
- Map snapshotter additions [#10163](https://github.com/mapbox/mapbox-gl-native/pull/10163)
- Add velocity to gestures / port animations to SDK animators [#10202](https://github.com/mapbox/mapbox-gl-native/pull/10202)
- Don't save state if map hasn't been initialised [#10320](https://github.com/mapbox/mapbox-gl-native/pull/10320)
- android.hardware.location.gps feature should not be required [#10347](https://github.com/mapbox/mapbox-gl-native/pull/10347)

## 5.2.0-beta.3 - October 26, 2017

- Reorganize dependencies [#10268](https://github.com/mapbox/mapbox-gl-native/pull/10268)
- Blacklist VAO usage on adreno 3xx [#10291](https://github.com/mapbox/mapbox-gl-native/pull/10291)
- On stop null check [#10259](https://github.com/mapbox/mapbox-gl-native/pull/10259)

## 5.2.0-beta.2 - October 19, 2017

- Wire up MapZoomButtonController with camera change events [#10221](https://github.com/mapbox/mapbox-gl-native/pull/10221)
- Execute callbacks only when not idle [#10220](https://github.com/mapbox/mapbox-gl-native/pull/10220)
- Cleanup unused gradle plugins [#10211](https://github.com/mapbox/mapbox-gl-native/pull/10211)
- add FileSource pause/resume [#9977](https://github.com/mapbox/mapbox-gl-native/pull/9977)
- add make target for ndk-stack [#10185](https://github.com/mapbox/mapbox-gl-native/pull/10185)
- Add interpolator examples [#10067](https://github.com/mapbox/mapbox-gl-native/pull/10067)
- Add an UnsatisfiedLinkError safeguard [#10180](https://github.com/mapbox/mapbox-gl-native/pull/10180)
- Hold off handling hover events untill map has been created [#10142](https://github.com/mapbox/mapbox-gl-native/pull/10142)
- Added `MapboxMap.getCameraForGeometry()` to get a camera with zoom level and center coordinate computed to fit a shape [#10107](https://github.com/mapbox/mapbox-gl-native/pull/10107)
- Fine tune gesture zoom & rotation [#10134](https://github.com/mapbox/mapbox-gl-native/pull/10134)

## 5.2.0-beta.1 - October 6, 2017

- Allow multiple listeners for camera events, deprecate old API [#10141](https://github.com/mapbox/mapbox-gl-native/pull/10141)
- Update symbol layer example with location [#10092](https://github.com/mapbox/mapbox-gl-native/pull/10092)
- Make OfflineTilePyramidRegionDefinition parceable [#10080](https://github.com/mapbox/mapbox-gl-native/pull/10080)
- Fix 5.2.0-SNAPSHOT CI build failing [#10079](https://github.com/mapbox/mapbox-gl-native/pull/10079)
- Deprecate MarkerView [#9782](https://github.com/mapbox/mapbox-gl-native/pull/9782)
- Hide overlain views on initalisation [#10068](https://github.com/mapbox/mapbox-gl-native/pull/10068)
- API for platform side animations [#10001](https://github.com/mapbox/mapbox-gl-native/pull/10001)
- Android asynchronous rendering [#9576](https://github.com/mapbox/mapbox-gl-native/pull/9576)
- Set error handler when starting snapshotter [#10035](https://github.com/mapbox/mapbox-gl-native/pull/10035)
- Hook camera events into compass [#10019](https://github.com/mapbox/mapbox-gl-native/pull/10019)
- Testapp cleanup [#10006](https://github.com/mapbox/mapbox-gl-native/pull/10006)
- Update zoom function example with selected state [#9987](https://github.com/mapbox/mapbox-gl-native/pull/9987)
- Add style inspection to debug activity [#9773](https://github.com/mapbox/mapbox-gl-native/pull/9773)
- Bump external dependencies [#9972](https://github.com/mapbox/mapbox-gl-native/pull/9972)
- Don't recycle bitmap for icon reuse. [#9966](https://github.com/mapbox/mapbox-gl-native/pull/9966)
- Android snapshotter [#9748](https://github.com/mapbox/mapbox-gl-native/pull/9748)
- Revert #9764 [#9851](https://github.com/mapbox/mapbox-gl-native/pull/9851)
- Update docs replacing Bitrise mentions with CircleCI [#9515](https://github.com/mapbox/mapbox-gl-native/pull/9515)
- Style image accessor [#9763](https://github.com/mapbox/mapbox-gl-native/pull/9763)
- Update readme with checkstyle and ndk-stack [#9788](https://github.com/mapbox/mapbox-gl-native/pull/9788)
- make android-check [#9787](https://github.com/mapbox/mapbox-gl-native/pull/9787)
- Deprecate MyLocationView in favor of LocationLayer plugin [#9771](https://github.com/mapbox/mapbox-gl-native/pull/9771)
- Increase firebase timeout for CI testing [#9774](https://github.com/mapbox/mapbox-gl-native/pull/9774)
- Restore max zoom to 25.5 [#9765](https://github.com/mapbox/mapbox-gl-native/pull/9765)
- Update example of camera zoom function on a symbol layer. [#9743](https://github.com/mapbox/mapbox-gl-native/pull/9743)
- Optimise icon management [#9643](https://github.com/mapbox/mapbox-gl-native/pull/9643)
- Expose setStyleJson and getStyleJson [#9714](https://github.com/mapbox/mapbox-gl-native/pull/9714)
- update LatLngBounds activity with BottomSheet interaction [#9736](https://github.com/mapbox/mapbox-gl-native/pull/9736)
- post updating InfoWindow update for InfoWindowAdapter [#9716](https://github.com/mapbox/mapbox-gl-native/pull/9716)
- Annotate MapboxMap class with UiThread [#9712](https://github.com/mapbox/mapbox-gl-native/pull/9712)
- Move ZoomButtonController creation to view initalisation [#9587](https://github.com/mapbox/mapbox-gl-native/pull/9587)
- Solve lint issues, reduce baseline [#9627](https://github.com/mapbox/mapbox-gl-native/pull/9627)
- Remove wear module from project [#9618](https://github.com/mapbox/mapbox-gl-native/pull/9618)
- Add zMediaOverlay configuration + bottom sheet integration [#9592](https://github.com/mapbox/mapbox-gl-native/pull/9592)
- Forward getMapAsync to map for fragment [#9621](https://github.com/mapbox/mapbox-gl-native/pull/9621)
- Make target for dumping system gfx information [#9616](https://github.com/mapbox/mapbox-gl-native/pull/9616)
- Make target documentation [#9617](https://github.com/mapbox/mapbox-gl-native/pull/9617)
- onGlobalLayout hook for map creation [#9607](https://github.com/mapbox/mapbox-gl-native/pull/9607)
- Custom viewpager for horizontal swiping [#9601](https://github.com/mapbox/mapbox-gl-native/pull/9601)
- Disable program caching on Adreno 3xx, 4xx, and 5xx GPUs due to known bugs [#9574](https://github.com/mapbox/mapbox-gl-native/pull/9574)
- Avoid creating InfoWindow iterator if no InfoWindows are shown [#9477](https://github.com/mapbox/mapbox-gl-native/pull/9477)
- Rewire map initialisation [#9462](https://github.com/mapbox/mapbox-gl-native/pull/9462)
- Trying to update non-existent polyline fix [#9544](https://github.com/mapbox/mapbox-gl-native/pull/9544)
- Location accuracy threshold [#9472](https://github.com/mapbox/mapbox-gl-native/pull/9472)
- Rewire gesture handling and telemetry event push [#9494](https://github.com/mapbox/mapbox-gl-native/pull/9494)
- run style instrumentation tests on CI [#9353](https://github.com/mapbox/mapbox-gl-native/pull/9353)
- Fix javadoc comment for public setOfflineMapboxTileCountLimit method [#9454](https://github.com/mapbox/mapbox-gl-native/pull/9454)
- add Map change & visibility test activities [#9425](https://github.com/mapbox/mapbox-gl-native/pull/9425)
- build release package once during ci build [#9351](https://github.com/mapbox/mapbox-gl-native/pull/9351)
- Add support for ImageSource [#9110](https://github.com/mapbox/mapbox-gl-native/pull/9110)
- Increased the default maximum zoom level from 20 to 22. [#9835](https://github.com/mapbox/mapbox-gl-native/pull/9835)

## 5.1.5 - October 31, 2017

* Remove obsolete terminate context/display calls [#10162](https://github.com/mapbox/mapbox-gl-native/pull/10162)
* Determine need for clip ID based on actual layers/tiles [#10216](https://github.com/mapbox/mapbox-gl-native/pull/10216)
* Correctly alter sprite URLs [#10217](https://github.com/mapbox/mapbox-gl-native/pull/10217)
* Russian and Ukrainian localizations [#9945](https://github.com/mapbox/mapbox-gl-native/pull/9945)

## 5.1.4 - September 25, 2017

* Update translations [#10033](https://github.com/mapbox/mapbox-gl-native/pull/10033) & [#9945](https://github.com/mapbox/mapbox-gl-native/pull/9945)
* Continue rendering tiles despite erros [#10012](https://github.com/mapbox/mapbox-gl-native/pull/10012)
* Fix layer z-fighting [#9942](https://github.com/mapbox/mapbox-gl-native/pull/9942)
* Align line vertex to 4-byte boundary [#9943](https://github.com/mapbox/mapbox-gl-native/pull/9943)
* Bump proguard config for OkHttp [#9970](https://github.com/mapbox/mapbox-gl-native/pull/9970)
* Remove database on schema downgrade [#9837](https://github.com/mapbox/mapbox-gl-native/pull/9837)
* Disable rotation gesture when pinch zooming [#10026](https://github.com/mapbox/mapbox-gl-native/pull/10026)
* Do not check if connection is local request [#9968](https://github.com/mapbox/mapbox-gl-native/pull/9968)
* Harden offline region deletion [#9967](https://github.com/mapbox/mapbox-gl-native/pull/9967)
* Clear out mapCallback's OnMapReadyListeners on destroy [#9957](https://github.com/mapbox/mapbox-gl-native/pull/9957)
* Avoid adding duplicate points to bounds [#9955](https://github.com/mapbox/mapbox-gl-native/pull/9955)
* Download is complete fix [#9913](https://github.com/mapbox/mapbox-gl-native/pull/9913)
* MAS 2.2.3 [#9901](https://github.com/mapbox/mapbox-gl-native/pull/9901)
* Russian and Ukrainian localizations [#9945](https://github.com/mapbox/mapbox-gl-native/pull/9945)

## 5.1.3 - August 18, 2017

* Use separate attribute component for line normals [#9753](https://github.com/mapbox/mapbox-gl-native/pull/9753)
* Track state of initial overlain views margins [#9391](https://github.com/mapbox/mapbox-gl-native/pull/9391)
* Compability for Samsung devices forcing 3-4 array vector length [#9746](https://github.com/mapbox/mapbox-gl-native/pull/9746)
* Smallest LatLngBounds when visible region crosses dateline [#9747](https://github.com/mapbox/mapbox-gl-native/pull/9747)
* Readd ProjectMeters [#9766](https://github.com/mapbox/mapbox-gl-native/pull/9766)
* Enable location change animation of MyLocationView by default [#9779](https://github.com/mapbox/mapbox-gl-native/pull/9779)
* Avoid IndexOutOfBoundsException when destroying map object [#9789](https://github.com/mapbox/mapbox-gl-native/pull/9789)
* MAS 2.2.1 [#9796](https://github.com/mapbox/mapbox-gl-native/pull/9796)

## 5.1.2 - August 2, 2017

* Disable program caching on Ardreno GPU 3xx, 4xx and 5xx [#9625](https://github.com/mapbox/mapbox-gl-native/pull/9625)
* GeoJSON proguard issue [#9577](https://github.com/mapbox/mapbox-gl-native/pull/9577)
* Harden click events of shape annotations [#9585](https://github.com/mapbox/mapbox-gl-native/pull/9585)
* Validate Marker before opening InfoWindow [#9586](https://github.com/mapbox/mapbox-gl-native/pull/9586)
* Fix added to map check [#9602](https://github.com/mapbox/mapbox-gl-native/pull/9602)
* Don't query shape annotations if none were added [#9606](https://github.com/mapbox/mapbox-gl-native/pull/9606)
* Fix compass direction [#9632](https://github.com/mapbox/mapbox-gl-native/pull/9632)
* Remove preview image integration [#9657](https://github.com/mapbox/mapbox-gl-native/pull/9657)
* Try/catch initialization of telemetry [#9658](https://github.com/mapbox/mapbox-gl-native/pull/9658)
* Fix typo OnCameraMoveStartedListener [#9664](https://github.com/mapbox/mapbox-gl-native/pull/9664)
* Bump MAS dependency to 5.2.0 [#9671](https://github.com/mapbox/mapbox-gl-native/pull/9671)

## 5.1.1 - July 21, 2017

* Rework attribution binding [#9433](https://github.com/mapbox/mapbox-gl-native/pull/9433)
* BackendScope changes [#9538](https://github.com/mapbox/mapbox-gl-native/pull/9538)
* Invisible MarkerView performance fix [#9420](https://github.com/mapbox/mapbox-gl-native/pull/9420)
* Polyline/Polygon click listeners [#9443](https://github.com/mapbox/mapbox-gl-native/pull/9443)
* Hit test Marker/MarkerViews [#9424](https://github.com/mapbox/mapbox-gl-native/pull/9424)
* Fine tune fling gesture [#9532](https://github.com/mapbox/mapbox-gl-native/pull/9532)
* Bump OkHttp 3.8.0, Android 7.x crashes [#9522](https://github.com/mapbox/mapbox-gl-native/pull/9522)
* MyLocationView tint fix [#9410](https://github.com/mapbox/mapbox-gl-native/pull/9410)
* VisibleRegion logic [#9428](https://github.com/mapbox/mapbox-gl-native/pull/9428)
* Validate motion event [#9434](https://github.com/mapbox/mapbox-gl-native/pull/9434)
* Add French localization [#9545](https://github.com/mapbox/mapbox-gl-native/pull/9545)
* Fix public.xml [#9525](https://github.com/mapbox/mapbox-gl-native/pull/9525/files)
* Manage InfoWindow selection in AnnotationManager [#9567](https://github.com/mapbox/mapbox-gl-native/pull/9567)
* Increase touch target size [#9565](https://github.com/mapbox/mapbox-gl-native/pull/9565)
* Work around Adreno 2xx GPU bugs [#9573](https://github.com/mapbox/mapbox-gl-native/pull/9573)

## 5.1.0 - June 30, 2017

* Update to MAS 2.1.3 [#9402](https://github.com/mapbox/mapbox-gl-native/pull/9402)
* Downgrade LOST to v1.1.1 [#9394](https://github.com/mapbox/mapbox-gl-native/pull/9394)
* OnCameraIdle hook into quickzoom gesture [#9339](https://github.com/mapbox/mapbox-gl-native/pull/9339)
* LatLngBounds conversion regression, add test [#9324](https://github.com/mapbox/mapbox-gl-native/pull/9324)

## 5.1.0-beta.5 - June 21, 2017

* Update MAS dependency to 2.1.2 [#9311](https://github.com/mapbox/mapbox-gl-native/pull/9311)
* Update LOST dependency to 3.0.1 [#9302](https://github.com/mapbox/mapbox-gl-native/pull/9302)
* Fix Pulse example Parcelable creator [#9283](https://github.com/mapbox/mapbox-gl-native/pull/9283)
* Custom marker view anchoring [#9282](https://github.com/mapbox/mapbox-gl-native/pull/9282)
* Update Activity test generation [#9276](https://github.com/mapbox/mapbox-gl-native/pull/9276)
* Validate camera position before transforming [#9275](https://github.com/mapbox/mapbox-gl-native/pull/9275)
* Revisit javadoc [#9266](https://github.com/mapbox/mapbox-gl-native/pull/9266)
* Build with NDK 15 [#9263](https://github.com/mapbox/mapbox-gl-native/pull/9263)
* Snapshot with view content [#9263](https://github.com/mapbox/mapbox-gl-native/pull/9263)
* Update source changed javadoc [#9243](https://github.com/mapbox/mapbox-gl-native/pull/9243)
* Run tests on UI-thread [#9198](https://github.com/mapbox/mapbox-gl-native/pull/9198)
* Fix trackball on worker thread [#9305](https://github.com/mapbox/mapbox-gl-native/pull/9305)

## 5.1.0-beta.4 - June 9, 2017

* Option to disable location change animation [#9210](https://github.com/mapbox/mapbox-gl-native/pull/9210)
* Invalidating MyLocationView bearing when not following [#9212](https://github.com/mapbox/mapbox-gl-native/pull/9212)
* Remove upgrade runtime exceptions [#9191](https://github.com/mapbox/mapbox-gl-native/pull/9191)
* Check source usage before removal [#9129](https://github.com/mapbox/mapbox-gl-native/pull/9129)
* Fix tracking mode + camera race condition [#9133](https://github.com/mapbox/mapbox-gl-native/pull/9133)
* Harden orientation changes [#9128](https://github.com/mapbox/mapbox-gl-native/pull/9128)
* Infinite location animation updates [#9194](https://github.com/mapbox/mapbox-gl-native/pull/9194)
* Invoke callback with valid fling gestures [#9192](https://github.com/mapbox/mapbox-gl-native/pull/9192)
* Keep location tracking after screen rotation [#9187](https://github.com/mapbox/mapbox-gl-native/pull/9187)
* Update components with camera values when animating [#9174](https://github.com/mapbox/mapbox-gl-native/pull/9174)
* Validate if gestures should execute [#9173](https://github.com/mapbox/mapbox-gl-native/pull/9173)
* Custom location source and LOST integration [#9142](https://github.com/mapbox/mapbox-gl-native/pull/9142)

## 5.1.0-beta.3 - May 26, 2017

* Add binding support for Light [#9013](https://github.com/mapbox/mapbox-gl-native/pull/9013)
* Update attribution with new Mapbox wordmark [#8774](https://github.com/mapbox/mapbox-gl-native/pull/8774)
* LatLngBounds bearing default value [#9102](https://github.com/mapbox/mapbox-gl-native/pull/9102)
* Stop location updates when toggling MyLocationView [#9099](https://github.com/mapbox/mapbox-gl-native/pull/9099)
* Horizontally rotated in snapshot [#9083](https://github.com/mapbox/mapbox-gl-native/pull/9083)
* Disable letter spacing for Arabic text [#9071](https://github.com/mapbox/mapbox-gl-native/pull/9071)
* Correct bearing conversion when animating the map [#9050](https://github.com/mapbox/mapbox-gl-native/pull/9050)
* Don't leak selected markers when removing [#9047](https://github.com/mapbox/mapbox-gl-native/pull/9047)
* Bump tools and support lib version [#9046](https://github.com/mapbox/mapbox-gl-native/pull/9046)
* MarkerView deselect events with OnMarkerViewClickListener [#9047](https://github.com/mapbox/mapbox-gl-native/pull/9047)
* LOST update to v3.0.0 [#9112](https://github.com/mapbox/mapbox-gl-native/pull/9112)
* Convert dp to pixels for meters per pixel at latitude [#9048](https://github.com/mapbox/mapbox-gl-native/pull/9048)

## 5.1.0-beta.2 - May 12, 2017

5.1.0-beta.2 builds further on 5.1.0-beta.1 and adds:

* When a layer is added, reload its source's tiles [#8963](https://github.com/mapbox/mapbox-gl-native/pull/8963)
* Update release script to support CircleCI builds [#8950](https://github.com/mapbox/mapbox-gl-native/pull/8950)
* URL getter on Sources [#8959](https://github.com/mapbox/mapbox-gl-native/pull/8959)
* Build SNAPSHOT from release branch CI configuration [#8958](https://github.com/mapbox/mapbox-gl-native/pull/8958)
* Fix UI test filter in Makefile [#8960](https://github.com/mapbox/mapbox-gl-native/pull/8960)
* Bump & fixup dependencies [#8921](https://github.com/mapbox/mapbox-gl-native/pull/8921)
* Ignore already deleted region [#8920](https://github.com/mapbox/mapbox-gl-native/pull/8920)
* Keep offline observer when timeout occurs [#8919](https://github.com/mapbox/mapbox-gl-native/pull/8919)
* Show error message when no browser installed [#8920](https://github.com/mapbox/mapbox-gl-native/pull/8920)
* Reset observers of removed Sources and Layers [#8900](https://github.com/mapbox/mapbox-gl-native/pull/8900)
* Only build custom layer .so for debug builds [#8885](https://github.com/mapbox/mapbox-gl-native/pull/8885)
* Update localizations [#8883](https://github.com/mapbox/mapbox-gl-native/pull/8883)
* Reset observers of removed Sources and Layers [#8862](https://github.com/mapbox/mapbox-gl-native/pull/8862)
* Remove force style cascade [#8866](https://github.com/mapbox/mapbox-gl-native/pull/8866)
* Update proguard config [#8944](https://github.com/mapbox/mapbox-gl-native/pull/8944)
* Update LOST to 2.3.0-SNAPSHOT [#8872](https://github.com/mapbox/mapbox-gl-native/pull/8872)
* Update logo [#8774](https://github.com/mapbox/mapbox-gl-native/pull/8774)
* Camera change listener v2.0 [#8644](https://github.com/mapbox/mapbox-gl-native/pull/8644)
* Allow filesource url transform reset [#8957](https://github.com/mapbox/mapbox-gl-native/pull/8957)

## 5.1.0-beta.1 - May 2, 2017

5.1.0-beta.1 builds further on 5.0.2 and adds:

* Support for FillExtrusionLayer [#8431](https://github.com/mapbox/mapbox-gl-native/pull/8431)
* Limit Viewport [#8622](https://github.com/mapbox/mapbox-gl-native/pull/8622)
* Transition Properties for Layer attributes [#8509](https://github.com/mapbox/mapbox-gl-native/pull/8509)
* Style wide transition duration and transition offset in milliseconds [#8576](https://github.com/mapbox/mapbox-gl-native/pull/8576)
* Transifex integration, Catalan & Dutch translations [#8556](https://github.com/mapbox/mapbox-gl-native/pull/8556)
* LatLngBounds includes with another bounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* LatLngBounds includes takes in account LatLng on the edges (cfr. core) [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* LatLngBounds facility getters/setters for LatLnbg on the edges of the bounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* Expose world bounds in LatLngBounds [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* OfflineRegion are validated if the bounds is found in the world bounds, else onError will be invoked [#8517](https://github.com/mapbox/mapbox-gl-native/pull/8517)
* Polygon holes [#8557](https://github.com/mapbox/mapbox-gl-native/pull/8557) and [#8722](https://github.com/mapbox/mapbox-gl-native/pull/8722)
* Custom location source [#8710](https://github.com/mapbox/mapbox-gl-native/pull/8710)
* Expose source layer identifier [#8709](https://github.com/mapbox/mapbox-gl-native/pull/8709)
* Ensure surface is created after display and context [#8759](https://github.com/mapbox/mapbox-gl-native/pull/8759)
* Harden telemetry event dispatch [#8767](https://github.com/mapbox/mapbox-gl-native/pull/8767)
* Move LatLngBounds calculation for CameraUpdateFactory to core [#8765](https://github.com/mapbox/mapbox-gl-native/pull/8765)
* Spanish, Lithuanian, and Vietnamese localizations [#8852](https://github.com/mapbox/mapbox-gl-native/pull/8852)
* Warning when updating a non-added annotation [#8832](https://github.com/mapbox/mapbox-gl-native/pull/8832)
* Share location source between components [#8825](https://github.com/mapbox/mapbox-gl-native/pull/8825)
* Test application runtime permissions for pre lollipop devices [#8823](https://github.com/mapbox/mapbox-gl-native/pull/8823)
* Do not call OnMarkerClick listener twice [#8804](https://github.com/mapbox/mapbox-gl-native/pull/8804)
* Rework instrumentation tests [#8793](https://github.com/mapbox/mapbox-gl-native/pull/8793)
* Set first render flag to false when destroying the surface [#8739](https://github.com/mapbox/mapbox-gl-native/pull/8739)
* Post resetting tracking settings to avoid race condition reset [#8738](https://github.com/mapbox/mapbox-gl-native/pull/8738)
* Expose Source Layer identifier [#8709](https://github.com/mapbox/mapbox-gl-native/pull/8709)
* Derived source attribution [#8630](https://github.com/mapbox/mapbox-gl-native/pull/8630)
* Consistent use of duration unit [#8578](https://github.com/mapbox/mapbox-gl-native/pull/8578)
* Swedish localization [#8883](https://github.com/mapbox/mapbox-gl-native/pull/8883)
* Streets v10, Outdoors v10, Satellite Streets v10, Traffic Day v2, Traffic Night v2 [#6301](https://github.com/mapbox/mapbox-gl-native/pull/6301)

## 5.0.2 - April 3, 2017

5.0.2 is a patch release that contains the following changes:

* Binary shader caching [#8604](https://github.com/mapbox/mapbox-gl-native/pull/8604)
* Fix resource transform callback [#8582](https://github.com/mapbox/mapbox-gl-native/pull/8582)
* Restore onTouch behaviour to 4.x version [#8585](https://github.com/mapbox/mapbox-gl-native/pull/8585)
* Restore anchoring after updating MarkerView Icon [#8519](https://github.com/mapbox/mapbox-gl-native/pull/8519)

## 5.0.1 - March 22nd, 2017

5.0.1 is a patch release that addresses a shader precision issue that created a rendering problem on some devices.

* Use `highp` for color attributes [#8385](https://github.com/mapbox/mapbox-gl-native/issues/8385)

## 5.0.0 - March 17th, 2017

5.0.0 final release contains:

* Support for data-driven styles [#7752](https://github.com/mapbox/mapbox-gl-native/pull/7752)
* Additional APIs to manage runtime styling layers [#8195](https://github.com/mapbox/mapbox-gl-native/pull/8195)
* Allow query all features in sources (`querySourceFeatures`) [#5792](https://github.com/mapbox/mapbox-gl-native/issues/5792)
* Improve accessibility TalkBack support by including content description in views [#8230](https://github.com/mapbox/mapbox-gl-native/pull/8230)
* Consistent double tap zoom acceleration [#7514](https://github.com/mapbox/mapbox-gl-native/issues/7514)
* Allow zooming/scaling to use `AnimationOptions` [#8181](https://github.com/mapbox/mapbox-gl-native/pull/8181)
* Cleanup inconsistencies `float` vs `double` [#4445](https://github.com/mapbox/mapbox-gl-native/issues/4445)
* Add `mapbox_` prefix to attributes [#6482](https://github.com/mapbox/mapbox-gl-native/issues/6482)
* Update LOST to 2.x [#6573](https://github.com/mapbox/mapbox-gl-native/issues/6537)
* MAS submodules (`geojson`, `telemetry`) are now smaller to reduce the overall method count [#7642](https://github.com/mapbox/mapbox-gl-native/pull/7642)
* Telemetry Service is now include automatically via Manifest merge to simplify set up [#8338](https://github.com/mapbox/mapbox-gl-native/pull/8338)
* Support for Android Nougat [#5910](5910-move-listener-logic-for-nougat)
  - Move location listening logic to `onStart`/`onStop` activity lifecylce methods
* Removal of `accessToken` on `MapView` and `MapboxMap` [#5621](https://github.com/mapbox/mapbox-gl-native/issues/5621)
* Introduction of new make targets [#5940](https://github.com/mapbox/mapbox-gl-native/issues/5940)
  - Targets for testing, running and using external tools directly from the command line
* Cleanup Gradle files [#6009](https://github.com/mapbox/mapbox-gl-native/issues/6009)
  - Introducing single purpose `.gradle` files
* Checkstyle integration [#7442](https://github.com/mapbox/mapbox-gl-native/pull/7442)
* Transform refactor [#6532](https://github.com/mapbox/mapbox-gl-native/issues/6532)
  - All camera related code is moved to a dedicated transform class cfr. core architecture
* Encapsulate gestures [#6557](https://github.com/mapbox/mapbox-gl-native/issues/6557)
  - All gesture event code is encapsulated in `MapGestureDetector`
  - All key event code is encapsulated in `MapKeyListener`
* Transparent background for `MyLocationView` [#7116](https://github.com/mapbox/mapbox-gl-native/issues/7116)
* Correctly scale accuracy circle for `MyLocationView` [#8255](https://github.com/mapbox/mapbox-gl-native/pull/8255)
* Update documentation on generate sanity tests [#7147](https://github.com/mapbox/mapbox-gl-native/issues/7147)
* Update documentation on style templates [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Fix `NullPointerException` during save instance state [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Move management style to core [#7275](https://github.com/mapbox/mapbox-gl-native/issues/7275)
* Fix `OnCameraChange` not called for the final position of animation bug [#7350](https://github.com/mapbox/mapbox-gl-native/issues/7350)
* `MapboxAccountManager` refactor [#6534](https://github.com/mapbox/mapbox-gl-native/issues/6534)
  - Consolidating the location where tokens are set
  - Rename to reflect the increased responsibilities introduced in prior releases
* AnnotationManager refactor [#6067](https://github.com/mapbox/mapbox-gl-native/issues/6067)
  - Extracting all business logic related to annotations into a separate class cfr. to core and the iOS codebase
* Gesture handling improvements:
  - Fix missing call to `onFling` when while pinch zooming [#7666](https://github.com/mapbox/mapbox-gl-native/issues/7666)
  - Round tap-zoom gestures to nearest integer [#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027)
* Support for style-wide transition animation duration and delay [#6779](https://github.com/mapbox/mapbox-gl-native/issues/6779)
* Allow callback parameter for style setting [#8262](https://github.com/mapbox/mapbox-gl-native/issues/8262)
* Support for all animated camera changes to configure dismissing tracking modes [#7854](https://github.com/mapbox/mapbox-gl-native/issues/7854)
* Avoid disabling location tracking on all gestures [#7878](https://github.com/mapbox/mapbox-gl-native/issues/7878)
* `LatLng` objects produced by the SDK are wrapped by default [#4522](https://github.com/mapbox/mapbox-gl-native/issues/4522)
* Proguard update for new SDK packages [#8437](https://github.com/mapbox/mapbox-gl-native/pull/8437)
* Javadoc cleanup [#8438](https://github.com/mapbox/mapbox-gl-native/pull/8438)
* MarkerViewClickListener and MapClickListener called simultaneously [#8447](https://github.com/mapbox/mapbox-gl-native/pull/8447)
* Fix LOST v2.0.0 integration [#8445](https://github.com/mapbox/mapbox-gl-native/pull/8445)
* Crash with OnMapChangeListener [#8380](https://github.com/mapbox/mapbox-gl-native/pull/8380)
* Feature query crash [#8374](https://github.com/mapbox/mapbox-gl-native/pull/8374)
* flyTo fix [#8302](https://github.com/mapbox/mapbox-gl-native/pull/8302)
* Don't dispatch MarkerView motion event up to MapView [#8447](https://github.com/mapbox/mapbox-gl-native/pull/8447)

## 5.0.0-beta.3 - March 10th, 2017

5.0.0-beta.3 focuses on addressing stability issues and bug fixes. It also includes:

* Allow query all features in sources (`querySourceFeatures`) [#5792](https://github.com/mapbox/mapbox-gl-native/issues/5792)
* Improve accessibility TalkBack support by including content description in views [#8230](https://github.com/mapbox/mapbox-gl-native/pull/8230)
* Allow zooming/scaling to use `AnimationOptions` [#8181](https://github.com/mapbox/mapbox-gl-native/pull/8181)
* Telemetry Service is now include automatically via Manifest merge to simplify set up [#8338](https://github.com/mapbox/mapbox-gl-native/pull/8338)
* Correctly scale accuracy circle for `MyLocationView` [#8255](https://github.com/mapbox/mapbox-gl-native/pull/8255)
* Round tap-zoom gestures to nearest integer [#8027](https://github.com/mapbox/mapbox-gl-native/pull/8027)
* Allow callback parameter for style setting [#8262](https://github.com/mapbox/mapbox-gl-native/issues/8262)
* Avoid disabling location tracking on all gestures [#7878](https://github.com/mapbox/mapbox-gl-native/issues/7878)

## 5.0.0-beta.2 - March 1st, 2017

5.0.0-beta.2 focuses on addressing stability issues and bug fixes. It also includes:

* Data-driven styling for `{text,icon}-{color,opacity,halo-color,halo-blur,halo-width}` [#7939](https://github.com/mapbox/mapbox-gl-native/pull/7939)
* Additional APIs to manage runtime styling layers [#8195](https://github.com/mapbox/mapbox-gl-native/pull/8195)

## 5.0.0-beta.1 - February 14th, 2017

* Support for data-driven styles [#7752](https://github.com/mapbox/mapbox-gl-native/pull/7752)
* Consistent double tap zoom acceleration [#7514](https://github.com/mapbox/mapbox-gl-native/issues/7514)
* Cleanup inconsistencies `float` vs `double` [#4445](https://github.com/mapbox/mapbox-gl-native/issues/4445)
* Add `mapbox_` prefix to attributes [#6482](https://github.com/mapbox/mapbox-gl-native/issues/6482)
* Update LOST to 2.0.0 [#6573](https://github.com/mapbox/mapbox-gl-native/issues/6537)
* MAS submodules (geojson, telemetry) are now smaller to reduce the overall method count [#7642](https://github.com/mapbox/mapbox-gl-native/pull/7642)
* Support for Android Nougat [#5910](5910-move-listener-logic-for-nougat)
  - Move location listening logic to onStart/onStop Activity lifecylce methods
* Removal of `accessToken` on `MapView` and `MapboxMap` [#5621](https://github.com/mapbox/mapbox-gl-native/issues/5621)
* Introduction of new make targets [#5940](https://github.com/mapbox/mapbox-gl-native/issues/5940)
  - Targets for testing, running and using external tools directly from the command line
* Cleanup Gradle files [#6009](https://github.com/mapbox/mapbox-gl-native/issues/6009)
  - Introducing single purpose `.gradle` files
* Checkstyle integration [#7442](https://github.com/mapbox/mapbox-gl-native/pull/7442)
* Transform refactor [#6532](https://github.com/mapbox/mapbox-gl-native/issues/6532)
  - All camera related code is moved to a dedicated transform class cfr. core architecture
* Encapsulate Gestures [#6557](https://github.com/mapbox/mapbox-gl-native/issues/6557)
  - All gesture event code is encapsulated in `MapGestureDetector`
  - All key event code is encapsulated in `MapKeyListener`
* Transparent background for `MyLocationView` [#7116](https://github.com/mapbox/mapbox-gl-native/issues/7116)
* Update documentation on generate sanity tests [#7147](https://github.com/mapbox/mapbox-gl-native/issues/7147)
* Update documentation on style templates [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* `NullPointerException` during save instance state [#7157](https://github.com/mapbox/mapbox-gl-native/issues/7157)
* Move management style to core [#7275](https://github.com/mapbox/mapbox-gl-native/issues/7275)
* `OnCameraChange` not called for the final position of animation bug [#7350](https://github.com/mapbox/mapbox-gl-native/issues/7350)
* `MapboxAccountManager` refactor [#6534](https://github.com/mapbox/mapbox-gl-native/issues/6534)
  - Consolidating the location where tokens are set
  - Rename to reflect the increased responsibilities introduced in prior releases
* AnnotationManager refactor [#6067](https://github.com/mapbox/mapbox-gl-native/issues/6067)
  - Extracting all business logic related to annotations into a separate class cfr. to core and the iOS codebase
* Gesture handling bugs
  - Avoid calls to onFling when while pinch zooming [#7666](https://github.com/mapbox/mapbox-gl-native/issues/7666)
* Support for style-wide transition animation duration and delay [#6779](https://github.com/mapbox/mapbox-gl-native/issues/6779)
* Support for all animated camera changes to configure dismissing tracking modes [#7854](https://github.com/mapbox/mapbox-gl-native/issues/7854)
* `LatLng` objects produced by the SDK are wrapped by default for [compatibility](https://developers.google.com/android/reference/com/google/android/gms/maps/model/LatLng) with the Google Maps API on Android [#4522](https://github.com/mapbox/mapbox-gl-native/issues/4522)

## 4.2.2 - January 27, 2017

Mapbox Android 4.2.2 builds further on v4.2.1 and resolves:
* OnFling gesture improvements
 - Improve fling by calculating animation time based on velocity [#7676](https://github.com/mapbox/mapbox-gl-native/pull/7676)
 - Avoid triggering a fling after a scale gesture [#7675](https://github.com/mapbox/mapbox-gl-native/issues/7675)
* Deselect marker on infowindow click [#7784](https://github.com/mapbox/mapbox-gl-native/issues/7784)
* Update build tools to Bitrise supported one [#7729](https://github.com/mapbox/mapbox-gl-native/issues/7729)
* Removal of redudant log statements in native_map_view.cpp [#7801](https://github.com/mapbox/mapbox-gl-native/issues/7801)
* Invalidate markersviews when updating their position [#7794](https://github.com/mapbox/mapbox-gl-native/issues/7794)
* Avoid clustering unclusterable GeoJSON [#7633](https://github.com/mapbox/mapbox-gl-native/pull/7633)
* Prevent null pointer exceptions when receiving connecitivy change events [#6858](https://github.com/mapbox/mapbox-gl-native/issues/6858)

## 4.2.1 - December 22, 2016

Mapbox Android 4.2.1 is a bugfix release build on top of 4.2.0 and resolves:

* Gesture handling bugs
  - Reverse fling when tilted [#7383](https://github.com/mapbox/mapbox-gl-native/pull/7383)
  - Rotation is hard trigger [#7299](https://github.com/mapbox/mapbox-gl-native/pull/7299)
* Null pointer exception on TrackingSettings [#7505](https://github.com/mapbox/mapbox-gl-native/pull/7505)
* Patches to our CI integration [#7363](https://github.com/mapbox/mapbox-gl-native/pull/7363) & [#7385](https://github.com/mapbox/mapbox-gl-native/pull/7385)
* Screen density is now correctly applied for attributes [#7327](https://github.com/mapbox/mapbox-gl-native/pull/7327)
* Usage of LocalBroadCastReceiver [#7511](https://github.com/mapbox/mapbox-gl-native/pull/7511)

## 4.2.0 - December 14, 2016

Mapbox Android 4.2.0 contains all 4.2.0-beta.5 changes and adds:

* Adds additional documentation to APIs
* Resolved issue with marker views occasionally not showing up until a gesture was performed on map [#7239](https://github.com/mapbox/mapbox-gl-native/pull/7239)
* Added horizontal accuracy to location events [#7237](https://github.com/mapbox/mapbox-gl-native/pull/7237)
* Resolved issue with changing visibility of a layer [#7242](https://github.com/mapbox/mapbox-gl-native/pull/7242)
* Proguard improvement and fixes

## 4.2.0-beta.5 - October 25, 2016

Mapbox Android 4.2.0-beta.5 adds no new features, only bug fixes.

## 4.2.0-beta.4 - October 25, 2016

Mapbox Android 4.2.0-beta.4 contains all 4.2.0-beta.3 changes and adds:

* Query rendered features by source [#6516](https://github.com/mapbox/mapbox-gl-native/issues/6516)
* Decreased logging for release builds [#6787](https://github.com/mapbox/mapbox-gl-native/pull/6787)
* setConnected method to manually set a connected flag [#6618](https://github.com/mapbox/mapbox-gl-native/pull/6618)

## 4.2.0-beta.3 - September 21, 2016

Mapbox Android 4.2.0-beta.3 contains all 4.2.0-beta.2 changes and adds:

* Configurable API endpoint [#6309](https://github.com/mapbox/mapbox-gl-native/pull/6309)
* Adjusted logging level for failed OkHttp requests [6356](https://github.com/mapbox/mapbox-gl-native/issues/6356)
* Offline improvements:
  - Fix SQLite crashes by reverting WAL journal mode [#6320](https://github.com/mapbox/mapbox-gl-native/pull/6320)

## 4.2.0-beta.2 - August 25, 2016

Mapbox Android 4.2.0-beta.2 contains all 4.2.0-beta.1 changes and adds:

* Query rendered features [#5869](https://github.com/mapbox/mapbox-gl-native/issues/5869)
* Clustering [#5852](https://github.com/mapbox/mapbox-gl-native/issues/5852)
* Integrated support for [Mapbox Android Services](https://www.mapbox.com/android-sdk/#mapbox_android_services)
* Offline improvements:
  - Enable external storage to host the tiles database [#5589](https://github.com/mapbox/mapbox-gl-native/issues/5589)
  - Stop delivering status updates when offline download is canceled [#5538](https://github.com/mapbox/mapbox-gl-native/issues/5538)
  - Update SQLite schema with WAL journal mode and normal sync [#5796](https://github.com/mapbox/mapbox-gl-native/pull/5796)
  - Check for network connectivity before requesting resources [#6123](https://github.com/mapbox/mapbox-gl-native/pull/6123)

## 4.2.0-beta.1 - August 6, 2016

Mapbox Android 4.2.0 introduces the Runtime Style API and improves performance.

* Runtime Style API ([#5610](https://github.com/mapbox/mapbox-gl-native/issues/5610))
* Earcut ([#2444](https://github.com/mapbox/mapbox-gl-native/pull/2444))
* Map view rendered with `SurfaceView` for improved performance [#5000](https://github.com/mapbox/mapbox-gl-native/issues/5000)

## 4.1.1 - July 19, 2016

Mapbox Android 4.1.1 is a patch release and is recommended to upgrade to it as soon as possible.

* Default Styles Caching Crash ([#5722](https://github.com/mapbox/mapbox-gl-native/issues/5722))

## 4.1.0 - June 29, 2016

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.1.0-beta.3 - June 20, 2016

* New samples:
  * [Location picker](https://github.com/mapbox/mapbox-gl-native/pull/5391)
  * [Animate and rotate multiple markers](https://github.com/mapbox/mapbox-gl-native/issues/5299)
  * [Scaling marker activity](https://github.com/mapbox/mapbox-gl-native/issues/5409)
* Marker improvements:
  * [Expose MarkerView alpha](https://github.com/mapbox/mapbox-gl-native/pull/5329)
  * [Icon should be optional for MarkerView](https://github.com/mapbox/mapbox-gl-native/pull/5328)
  * [Expose an API to enable selection/deselection of markers on a map tap](https://github.com/mapbox/mapbox-gl-native/pull/5312)
  * [Bring selected MarkerView to the front](https://github.com/mapbox/mapbox-gl-native/pull/5294)
* [Make gesture focal point configurable](https://github.com/mapbox/mapbox-gl-native/pull/5332)

## 4.1.0-beta.2 - June 8, 2016

* Dynamically Update InfoWindow ([#5237](https://github.com/mapbox/mapbox-gl-native/issues/5237))
* armeabi ABI Work On armv7 Devices ([#3985](https://github.com/mapbox/mapbox-gl-native/issues/3985))
* Remove Adapter Requirement For MarkerView ([#5214](https://github.com/mapbox/mapbox-gl-native/issues/5214))
* Always Current Version Style URL Constants ([#5193](https://github.com/mapbox/mapbox-gl-native/issues/5193))
* Random NullPointerException On Telemetry ([#5186](https://github.com/mapbox/mapbox-gl-native/issues/5186))

## 4.1.0-beta.1 - May 26, 2016

Mapbox Android 4.1.0 builds off our ambitious 4.0.0 version with 3 major new features being released.  To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.1.0 Final Release.

* View Based Annotations ([#3276](https://github.com/mapbox/mapbox-gl-native/issues/3276))
* UserLocationView Refactor ([#4396](https://github.com/mapbox/mapbox-gl-native/issues/4396))
* MapboxAccountManager ([#5004](https://github.com/mapbox/mapbox-gl-native/issues/5004))

## 4.0.1 - May 12, 2016

Mapbox Android 4.0.1 is a patch release to make this bug fix available sooner.

* MapboxMap.removeAnnotations() doesn't remove markers ([#4553](https://github.com/mapbox/mapbox-gl-native/issues/4553))

## 4.0.0 - March 30, 2016

Mapbox Android 4.0.0 contains the following 3 major new features.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 4.0.0-rc.1 - March 25, 2016

* Default Value Bug Fix for MapboxMapOptions ([#4398](https://github.com/mapbox/mapbox-gl-native/issues/4398))
* NullPointerException When Scrolling ([#4424](https://github.com/mapbox/mapbox-gl-native/issues/4424))
* Platform Specific CHANGELOGS ([#4432](https://github.com/mapbox/mapbox-gl-native/issues/4432))
* Introduce LatLng.wrap() ([#4475](https://github.com/mapbox/mapbox-gl-native/issues/4475))

## 4.0.0-beta.2 - March 21, 2016

* arm64 ABI Support ([#3128](https://github.com/mapbox/mapbox-gl-native/issues/3128))
* Unify Offline And Cache Databases ([#4362](https://github.com/mapbox/mapbox-gl-native/issues/4362))
* Offline Database Vacuuming ([#4342](https://github.com/mapbox/mapbox-gl-native/pull/4342))
* Telemetry User Agent Fix ([#4328](https://github.com/mapbox/mapbox-gl-native/issues/4328))
* OnCameraChange Bug Fix ([#4327](https://github.com/mapbox/mapbox-gl-native/issues/4327))
* OnCameraChangeListener vs getCameraPosition ([#4326](https://github.com/mapbox/mapbox-gl-native/issues/4326))

## 4.0.0-beta.1 - March 15, 2016

Mapbox Android 4.0.0 is the most ambitious Android release to date with 3 major new features being released. To help us produce the highest quality SDK possible we're releasing an official Beta release first so that everyone has time to explore it and help hardened it before the official 4.0.0 Final Release.

* MapboxMap API Change ([#3145](https://github.com/mapbox/mapbox-gl-native/issues/3145))
* Offline Maps ([#3891](https://github.com/mapbox/mapbox-gl-native/issues/3891))
* Telemetry ([#2421](https://github.com/mapbox/mapbox-gl-native/issues/2421))

## 3.2.0 - January 28, 2016

* Fixed crash caused by annotation image with non-integer width or height ([#3031](https://github.com/mapbox/mapbox-gl-native/issues/3031))
* Tracking Mode Reverses Bearing Fix ([#3664](https://github.com/mapbox/mapbox-gl-native/issues/3664))
* GPS Extra Rotation Fix ([#3661](https://github.com/mapbox/mapbox-gl-native/issues/3661))
* Added new methods for getting and setting the min and max zoom levels: `getMinZoom`, `setMinZoom`, `getMaxZoom`, `setMaxZoom`. ([#509](https://github.com/mapbox/mapbox-gl-native/issues/509))

## 3.1.0 - January 20, 2016

* Camera API Callback Improvements ([#3412](https://github.com/mapbox/mapbox-gl-native/issues/3412))
* Coordinate Deprecated For LatLng ([#3309](https://github.com/mapbox/mapbox-gl-native/issues/3309))
* Responsive User Dot Location Tracking ([#2049](https://github.com/mapbox/mapbox-gl-native/issues/2049))

## 3.0.0 - December 21, 2015

* Added Camera API ([#3244](https://github.com/mapbox/mapbox-gl-native/issues/3244))
* Custom Layer Support ([#3248](https://github.com/mapbox/mapbox-gl-native/issues/3348))
* Reverse Tilt Gesutre Detection ([#3245](https://github.com/mapbox/mapbox-gl-native/issues/3245))
* Decoupled Location Provider ([#2954](https://github.com/mapbox/mapbox-gl-native/issues/2954))

## 2.3.0 - December 4, 2015

* Added Tilt / Pitch Support ([#2805](https://github.com/mapbox/mapbox-gl-native/issues/2805))
* Keep InfoWindow Open When Panning ([#3121](https://github.com/mapbox/mapbox-gl-native/issues/3121))
* Concurrent Multiple Open InfoWindows ([#3115](https://github.com/mapbox/mapbox-gl-native/issues/3115))
* OkHttp Replace curl ([#2856](https://github.com/mapbox/mapbox-gl-native/issues/2856))
* GPS and Compass Customization Support ([#2858](https://github.com/mapbox/mapbox-gl-native/issues/2858))

## 2.2.0 - October 28, 2015

- New User Dot location graphics ([#2732](https://github.com/mapbox/mapbox-gl-native/issues/2732))
- Custom Drawable Markers ([#2744](https://github.com/mapbox/mapbox-gl-native/issues/2744))
- `MapView.setOnInfoWindowClickListener()` support ([#2448](https://github.com/mapbox/mapbox-gl-native/issues/2448))
- Completed Annotations API ([#1716](https://github.com/mapbox/mapbox-gl-native/issues/1716))
- Satellite Streets Style ([#2739](https://github.com/mapbox/mapbox-gl-native/issues/2739))
- **RESOLVED** Black Screen On Ice Cream Sandwich and Jelly Bean devices ([#2802](https://github.com/mapbox/mapbox-gl-native/issues/2802))

## 2.1.0 - October 21, 2015

- Initial Android release.
