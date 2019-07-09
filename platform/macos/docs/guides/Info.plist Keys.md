# Info.plist Keys

The Mapbox Maps SDK for macOS supports custom `Info.plist` keys in your application in order to configure various settings.

## MGLMapboxAccessToken

Set the [Mapbox access token](https://www.mapbox.com/help/define-access-token/) to be used by all instances of `MGLMapView` in the current application.

Mapbox-hosted vector tiles and styles require an API access token, which you can obtain from the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access tokens associate requests to Mapbox’s vector tile and style APIs with your Mapbox account. They also deter other developers from using your styles without your permission.

As an alternative, you can use `MGLAccountManager.accessToken` to set a token in code. See [our guide](https://www.mapbox.com/help/ios-private-access-token/) for some tips on keeping access tokens in open source code private.

## MGLMapboxAPIBaseURL

Use this key if you need to customize the API base URL used throughout the SDK. If unset, the default Mapbox API is used.

The default value is `https://api.mapbox.com`.

## MGLIdeographicFontFamilyName

The name of the font family to use for client-side text rendering of CJK ideographs.

Set `MGLIdeographicFontFamilyName` in your containing application's Info.plist to font family name(s) that will be available at run time, such as “PingFang TC” or “Marker Felt”. This plist key accepts:

- A string value of a single font family name.

- An array of font family names. Fonts will be used in the defined order, eventually falling back to default system font if none are available.

- A boolean value `NO` to disable client-side rendering of CJK glyphs — remote fonts specified in your style will be used instead.

## MGLCollisionBehaviorPre4_0

If this key is set to YES (`true`), collision detection is performed only between symbol style layers based on the same source, as in versions 0.1–0.7 of the Mapbox Maps SDK for iOS. In other words, symbols in an `MGLSymbolStyleLayer` based on one source (for example, an `MGLShapeSource`) may overlap with symbols in another layer that is based on a different source (such as the Mapbox Streets source). This is the case regardless of the `MGLSymbolStyleLayer.iconAllowsOverlap`, `MGLSymbolStyleLayer.iconIgnoresPlacement`, `MGLSymbolStyleLayer.textAllowsOverlap`, and `MGLSymbolStyleLayer.textIgnoresPlacement` properties.

Beginning in version 0.7, the SDK also performs collision detection between style layers based on different sources by default. For the default behavior, omit the `MGLCollisionBehaviorPre4_0` key or set it to NO (`false`). This property is so named because version 0.7 of the Mapbox Maps SDK for macOS corresponds to version 4.0 of the Mapbox Maps SDK for iOS.
