'use strict';

const fs = require('fs');
const ejs = require('ejs');

global.iff = function (condition, val) {
  return condition() ? val : "";
}

global.camelize = function (str) {
  return str.replace(/(?:^|-)(.)/g, function (_, x) {
    return x.toUpperCase();
  });
}


const excludeActivities = ["BaseLocationActivity","MockLocationEngine","DeleteRegionActivity","RealTimeGeoJsonActivity","UpdateMetadataActivity","CarDrivingActivity","MyLocationTrackingModeActivity","MyLocationToggleActivity","MyLocationTintActivity","MyLocationDrawableActivity","DoubleMapActivity", "LocationPickerActivity","GeoJsonClusteringActivity","RuntimeStyleTestActivity", "AnimatedMarkerActivity", "ViewPagerActivity","MapFragmentActivity","SupportMapFragmentActivity","SnapshotActivity","NavigationDrawerActivity", "QueryRenderedFeaturesBoxHighlightActivity", "MultiMapActivity", "MapInDialogActivity", "SimpleMapActivity"];
const appBasePath = 'platform/android/MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity';
const testBasePath = 'platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/testapp/activity/gen';
const subPackages = fs.readdirSync(appBasePath);
const ejsConversionTask = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/testapp/activity/activity.junit.ejs', 'utf8'), {strict: true});

if (!fs.existsSync(testBasePath)){
  fs.mkdirSync(testBasePath);
}

console.log("Generating test activities:");
for(const subPackage of subPackages) {
  if(!(subPackage.slice(-5) == '.java')) {
    const activities = fs.readdirSync(appBasePath+'/'+subPackage);

    // create directories for package
    if (!fs.existsSync(testBasePath+"/"+subPackage)){
        fs.mkdirSync(testBasePath+"/"+subPackage);
    }

    for (const activity of activities) {
      // strip .java from input file
      const activityName = activity.slice(0, -5);

      // create path for test file
      const filePath = testBasePath+"/"+subPackage+"/"+activityName+'Test.java';

      // try removing previous generated files
      try {
        fs.accessSync(filePath, fs.F_OK);
        fs.unlinkSync(filePath);
        console.log("Removed file: "+filePath);
      } catch (e) {
        console.log("No file found: "+filePath);
      }

      // only generate test file if not part of exclude list
      if (!(excludeActivities.indexOf(activityName) > -1)) {
        console.log("Created file:  "+filePath);
        fs.writeFileSync(filePath, ejsConversionTask([activityName, subPackage]));
      }else{
        console.log("Excluding file:  "+filePath);
      }
    }
  }
}
