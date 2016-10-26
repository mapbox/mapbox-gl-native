# Info.plist Keys

The Mapbox iOS SDK supports custom `Info.plist` keys in your application in order to configure various settings. 

## MGLMapboxAccessToken

Use this key to set your Mapbox API [access token](https://www.mapbox.com/help/define-access-token/). This key is required for all map use. 

## MGLMapboxAPIBaseURL

Use this key if you need to customize the API base URL used throughout the SDK. If unset, the default Mapbox API is used. 

## MGLMapboxMetricsEnabledSettingShownInApp

If you have implemented custom opt-out of Mapbox Telemetry within the user interface of your app, use this key to disable the built-in check for opt-out support. See [this guide](https://www.mapbox.com/ios-sdk/#telemetry_opt_out) for more details.
