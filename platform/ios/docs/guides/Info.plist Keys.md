# Info.plist Keys

The Mapbox Maps SDK for iOS supports custom `Info.plist` keys in your application in order to configure various settings.

## MGLMapboxAccessToken

Set the [Mapbox access token](https://www.mapbox.com/help/define-access-token/) to be used by all instances of `MGLMapView` in the current application.

Mapbox-hosted vector tiles and styles require an API access token, which you can obtain from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access tokens associate requests to Mapbox’s vector tile and style APIs with your Mapbox account. They also deter other developers from using your styles without your permission.

As an alternative, you can use `MGLAccountManager.accessToken` to set a token in code. See [our guide](https://www.mapbox.com/help/ios-private-access-token/) for some tips on keeping access tokens in open source code private.

## MGLMapboxAPIBaseURL

Use this key if you need to customize the API base URL used throughout the SDK. If unset, the default Mapbox API is used.

The default value is `https://api.mapbox.com`.

## MGLMapboxMetricsEnabledSettingShownInApp

If you have implemented custom opt-out of Mapbox Telemetry within the user interface of your app, use this key to disable the built-in check for opt-out support. See [this guide](https://docs.mapbox.com/help/how-mapbox-works/attribution/#mapbox-maps-sdk-for-ios) for more details.

## MGLIdeographicFontFamilyName

The name of the font family to use for client-side text rendering of CJK ideographs.

Set `MGLIdeographicFontFamilyName` in your containing application's Info.plist to font family name(s) that will be available at run time, such as “PingFang TC” or “Marker Felt”. This plist key accepts:

- A string value of a single font family name.

- An array of font family names. Fonts will be used in the defined order, eventually falling back to default system font if none are available.

- A boolean value `NO` to disable client-side rendering of CJK glyphs — remote fonts specified in your style will be used instead.

## MGLCollisionBehaviorPre4_0

 If this key is set to YES (`true`), collision detection is performed only between symbol style layers based on the same source, as in versions 2.0–3.7 of the Mapbox Maps SDK for iOS. In other words, symbols in an `MGLSymbolStyleLayer` based on one source (for example, an `MGLShapeSource`) may overlap with symbols in another layer that is based on a different source (such as the Mapbox Streets source). This is the case regardless of the `MGLSymbolStyleLayer.iconAllowsOverlap`, `MGLSymbolStyleLayer.iconIgnoresPlacement`, `MGLSymbolStyleLayer.textAllowsOverlap`, and `MGLSymbolStyleLayer.textIgnoresPlacement` properties.

Beginning in version 4.0, the SDK also performs collision detection between style layers based on different sources by default. For the default behavior, omit the `MGLCollisionBehaviorPre4_0` key or set it to NO (`false`).

This property may also be set using `[[NSUserDefaults standardUserDefaults] setObject:@(YES) forKey:@"MGLCollisionBehaviorPre4_0"]`; it will override any value specified in the `Info.plist`.

