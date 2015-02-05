# mapbox-gl-cocoa

[![Build Status](https://travis-ci.org/mapbox/mapbox-gl-cocoa.svg)](https://travis-ci.org/mapbox/mapbox-gl-cocoa)

This project is Cocoa API bindings for [`mapbox-gl-native`](https://github.com/mapbox/mapbox-gl-native). Use or edit this project to get access to vector maps (via [Mapbox Vector Tiles](https://www.mapbox.com/blog/vector-tiles)) and dynamic OpenGL-based styling in your iOS apps by using `MGLMapView`. 

![](https://raw.githubusercontent.com/mapbox/mapbox-gl-cocoa/master/pkg/screenshot.png)

## Installation

See [`./dist/README.md`](./dist/README.md) for installation instructions. Everything you need is in `./dist`. Mapbox GL is provided prebuilt as both a static library, headers folder, and resource bundle (iOS 7+), and as a dynamic framework (iOS 8+). 

You can also make use of [CocoaPods](http://cocoapods.org) by adding this line to your `Podfile` (the library has not yet been added to the CocoaPods specs repository):

```ruby
pod 'MapboxGL', :git => 'https://github.com/mapbox/mapbox-gl-cocoa.git'
```

## Example usage

### Objective-C

```objective-c
MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 400, 400)
                                            accessToken:@"<access token string>"];

[mapView setCenterCoordinate:CLLocationCoordinate2DMake(28.369334, -80.743779) 
                   zoomLevel:13 
                    animated:NO];

[mapView useBundledStyleNamed:@"outdoors"];

[self.view addSubview:mapView];
```

### Swift

```swift
let mapView = MGLMapView(frame: CGRect(x: 0, y: 0, width: 400, height: 400),
                         accessToken: "<access token string>")

mapView.setCenterCoordinate(CLLocationCoordinate2D(latitude: 46.049900, longitude: -122.095678),
        zoomLevel: 12,
        animated: false)

mapView.useBundledStyleNamed("outdoors")

view.addSubview(mapView)
```

## Development

If you'd like to contribute to this project, go instead to [Mapbox GL native](https://github.com/mapbox/mapbox-gl-native) and clone that project. This project is a submodule of that project and is pulled into the overarching build process there, which consists of a cross-platform C++ library and this Objective-C wrapper library, together with an iOS demo app. 

## Packaging

This library, when standalone, makes use of inclusion of [Mapbox GL](https://github.com/mapbox/mapbox-gl-native), the underlying C++ library. To package a version for release, run `./pkg/package.sh` while this project is checked out inside of Mapbox GL. This will update the contents of `./dist`. This includes [`versions.txt`](./dist/versions.txt), which stores the hashes that the static library and framework were built from. 

## Testing

Tests are in `./test` and make use of the [KIF](https://github.com/kif-framework/KIF) framework. Since this project relies on the underlying C++ library, in order to be independently testable, the tests run an Xcode project which uses the static library build. Thus, to fully test the framework, you should first package a build per the above instructions so that the test app can link against `./dist/static/libMapboxGL.a`. See the [`.travis.yml`](https://github.com/mapbox/mapbox-gl-cocoa/blob/master/.travis.yml) for more info on the steps required. 

## Requirements

 * iOS 7+
 * a sense of adventure

## Styling

See the [online style reference](https://www.mapbox.com/mapbox-gl-style-spec/) for the latest documentation. Contained within the `MapboxGL.bundle` assets are a couple of starter styles in JSON format. 

The Cocoa programmatic styling API is currently under renovation per [#31](https://github.com/mapbox/mapbox-gl-cocoa/issues/31). In the meantime, just edit the stylesheet manually. 

## Related Projects

 * https://github.com/mapbox/mapbox-gl-native
 * https://github.com/mapbox/mapbox-gl-style-spec
 * https://github.com/mapbox/mapbox-gl-js
 * https://github.com/mapbox/vector-tile-spec

## Other notes

Under early development, this project was called MVKMapKit (Mapbox... Vector Kit?), in case you see any lingering references to it. 
