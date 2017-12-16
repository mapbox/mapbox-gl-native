# Info.plist Keys

The Mapbox Maps SDK for macOS supports custom `Info.plist` keys in your application in order to configure various settings.

## MGLMapboxAccessToken

Set the [Mapbox access token](https://www.mapbox.com/help/define-access-token/) to be used by all instances of `MGLMapView` in the current application.

Mapbox-hosted vector tiles and styles require an API access token, which you can obtain from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access tokens associate requests to Mapbox’s vector tile and style APIs with your Mapbox account. They also deter other developers from using your styles without your permission.

As an alternative, you can use `+[MGLAccountManager setAccessToken:]` to set a token in code. See [our guide](https://www.mapbox.com/help/ios-private-access-token/) for some tips on keeping access tokens in open source code private.

## MGLMapboxAPIBaseURL

Use this key if you need to customize the API base URL used throughout the SDK. If unset, the default Mapbox API is used.

The default value is `https://api.mapbox.com`.

## MGLIdeographicFontFamilyName                                                                                                                                                          

The name of the font family to use for client-side text rendering of CJK ideographs. Set this to the name of a font family which will be available at run time, e.g. `PingFang TC` (iOS 9+), `Heiti TC` (iOS 8+), another appropriate built-in font, or a font provided by your application. Note that if a non-existent font is specified, iOS will fall back to using Helvetica which is likely not to include support for the glyphs needed to render maps in your application.