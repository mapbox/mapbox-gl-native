# Mapbox GL Cocoa FAQ

Mapbox GL is a completely new renderer technology which will eventually replace and/or merge Mapbox's existing iOS toolsets, the [Mapbox iOS SDK](http://www.mapbox.com/mapbox-ios-sdk/) and [MBXMapKit](https://www.mapbox.com/mbxmapkit/). This FAQ shares our current plans for that migration. The plans are subject to change as Mapbox GL matures, but we wanted to both clarify our thinking as well as set expectations for where things are headed. 

### When will Mapbox GL be released? 

The library is [open source](https://github.com/mapbox/mapbox-gl-cocoa) right now, but an official, production-recommended release will come later in 2014. 

### What iOS versions will be supported? 

Mapbox GL currently supports iOS 7 and later. This will continue to be the case. MBXMapKit also supports iOS 7 and greater, but this does mean that moving from the iOS SDK will leave behind iOS 5 and 6. 

### Will the API be similar to the Mapbox iOS SDK/MBXMapKit/MapKit? 

Yes. We are shooting for bringing the Mapbox GL API in line with Apple's MapKit for the easiest transition ability. 

MBXMapKit is already an add-on to MapKit, so Apple's `MKMapView` API is used directly. 

The Mapbox iOS SDK is "workalike", since it descends from an [upstream open source project](https://github.com/Alpstein/route-me) that predates Apple's own MapKit. It uses similar concepts like annotations (with the difference that the map view delegate provides `CALayer` instances instead of `UIView`, the intention being that Mapbox GL will support `UIView`), similar API for managing the map view center, bounds, and zoom levels, and an `RMUserLocation` API that is very much like `MKUserLocation`. But the Mapbox iOS SDK also features unique APIs like extensible tile sources, offline caching, UTFGrid interactivity, and point annotation clustering. 

### Will the iOS SDK's extra APIs make it over to Mapbox GL? 

The intention is yes. This includes: 

#### Tile sources

The SDK uses the `RMTileSource` protocol to allow for extensible remote and local raster tile sources, as well as custom on-the-fly raster tile generation. We are planning to build a transitional API in Mapbox GL that acts as a wrapper to `RMTileSource` so that existing sources can be used to plug into the main Mapbox GL source API. 

#### Offline caching

Both the Mapbox iOS SDK and MBXMapKit feature robust support for offline caching of raster tiles and the SDK features support for [MBTiles](http://mbtiles.org) tile sources. 

We're likely going to take a similar approach to both performance caching and offline maps as is done with the `0.3.0` release of MBXMapKit. That is, `NSURLCache` will be used for performance caching and specific, separate offline map database management will be used for fetching areas of map tiles. This is a cleaner design than exists in the SDK, which combines performance and offline caches and has a non-intuitive configuration API. 

We're not sure yet if we're going to port over MBTiles support to Mapbox GL. 

#### UTFGrid interactivity

[UTFGrid](https://github.com/mapbox/utfgrid-spec) is a technology for enabling interactive features in raster tiles. Support for the technology is provided in the Mapbox iOS SDK with categories in `RMInteractiveSource` in combination with either `RMMapboxSource` or `RMMBTilesSource`. 

Current plans include allowing the basic key-value querying functionality of UTFGrid with Mapbox GL Cocoa sources. 

#### Annotation clustering

We're planning on adding support for point clustering to Mapbox GL and it will likely have a similar API to the SDK's. Clustering will happen on an individual style layer level. The performance for clustering, as well as annotations in general, will hopefully be better than in the SDK, since it will be done with direct OpenGL rendering instead of Core Animation layers. 
  
### What will the migration path look like? 

Ideally, the migration will be pretty lightweight because of the APIs supported above. There may be slight syntax changes, but they likely won't be more than would be expected from something like Mapbox iOS SDK version `1.x` to a hypothetical `2.x`. 
