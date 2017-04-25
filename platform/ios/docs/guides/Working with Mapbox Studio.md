# Working with Mapbox Studio

[Mapbox Studio’s Styles editor](http://mapbox.com/studio) is Mapbox’s tool for creating custom map styles. It also serves as an excellent tool for rapidly prototyping dynamic maps and [runtime styling](runtime-styling.html) interactions for iOS.

## Creating a base style

Start by heading to [mapbox.com/studio](https://www.mapbox.com/studio) and creating a new style. Any style that’s close to what you’ll be using in your app is ideal.

## Prototyping with data

The goal in using Mapbox Studio for prototyping runtime styling implementations is to test data presentation assumptions as quickly as possible. With the Mapbox Studio tools, you can import a small subset of your own real data, fake data as a placeholder, or prototype with existing Mapbox data.

### Prototyping with Mapbox data
The default [Mapbox Streets tileset](https://www.mapbox.com/studio/tilesets/mapbox.mapbox-streets-v7/) might offer data similar to your own that you can use to style before you swap in your own data at runtime.

For example, if you’re looking to prototype points of interest, consider the `poi_label` layer; if you want to style GPS traces, the `roads` layer might be a good proxy. Take a look at what’s available in [Mapbox Streets](https://www.mapbox.com/studio/tilesets/mapbox.mapbox-streets-v7/): there’s probably a layer that closely matches your data.

### Importing real data
If you can’t find a good approximation for your data in Mapbox Streets, consider uploading a small subset of your data into Mapbox Studio as a custom tileset.

From the [Mapbox Studio Dashboard](https://www.mapbox.com/studio/), click `Tilesets` in the sidebar, then click `New Tileset` to get started with most common geo file formats including KML, GPX, GeoJSON, Shapefiles, and CSV.

### Faking placeholder data
If you don’t have any custom data on hand in a format that works easily with the Tileset importer, you can fake placeholder data with the Dataset Editor.

From the [Mapbox Studio Dashboard](https://www.mapbox.com/studio/), click `Datasets` in the sidebar, then click on `New Dataset` to get started.

Zoom into your desired location and use the draw tools on the left to start creating a set of sample data.

![create shapes](img/studio-workflow/create-polygons.gif)

Next, add data properties you’d like to use to drive your style. Consider categorical properties or numeric properties that you’d use to filter and group your data. Text properties can be used to display icons or labels.

![add properties](img/studio-workflow/add-properties.gif)

**General Guidelines:**

* Text along a line: add line with a text property
* Text at specific points on a line or polygon: in addition to the line, create points at the specific points you’d like with text properties
* If you want circles where scale doesn’t matter relative to the geography (e.g. always 20 pixels), you can add as a point and style with a circle layer or a symbol
* If you want circles or arcs where the scale matters (e.g. 10 mile radius), you’ll need to approximately freehand a polygon that you can create more precisely later in code.

When you’re done, save your dataset and export as a tileset. When that’s complete, add your tileset to your style.

### Import into your style

1. Click `New Layer`
2. Select your tileset
3. Select your shape type:
    * `Symbol`: best for text and icons
    * `Line`: best for lines or adding strokes to polygons
    * `Fill`: best for filling polygons
    * `Circle`: for styling points or nodes along a line or polygon as circles. If you need circles of a fixed radius (e.g. 1 mile radius), you should model your data as a polygon.
4. Add filters if necessary
    * You can selectively filter your data by their properties to group and style separately
5. Click on `Create Layer`

## Styling with Mapbox Studio

Mapbox Studio shines for styling your data and the process is much faster than attempting to style natively.

There are some nuances to understand between the different layer types and how they work together. Don’t be afraid to use the layers sidebar to peek into the techniques used to style the stock Mapbox maps. You can duplicate these layers, re-point the source to your own data, and tweak as needed.

**Best Practices:**

* Layers are cheap, so duplicate and update filters liberally.
* If you’d like to stroke polygons you’ll need to use two layers: one a fill and one a stroked line.
* If you want to stroke a line, create two layers, one for the default stroke and one with a wider width for its casing
* If you intend to animate properties or transition between values, consider creating separate layers for each state and toggling visibility to visualize the difference.

## Implement on iOS with runtime styling

Once you’re happy with the styles you’ve created, it’s time to [get setup with Mapbox in your app](https://www.mapbox.com/ios-sdk/).

To implement your prototypes with runtime styling:

1. Implement `-[MGLMapViewDelegate mapView:didFinishLoadingStyle:]`.
2. Add your real data as a source:
    * This can be done using vector data from tileset editor ([example](https://www.mapbox.com/ios-sdk/examples/dds-circle-layer)), custom vector tiles, added as GeoJSON ([example](https://www.mapbox.com/ios-sdk/examples/runtime-add-line), or added manually through the app via `MGLShapeSource` ([example](https://www.mapbox.com/ios-sdk/examples/runtime-multiple-annotations))
3. For each layer you’ve prototyped in Studio, add its corresponding `MGLStyleLayer` subclass. See [“Configuring the map content’s appearance”](for-style-authors.html#configuring-the-map-content-s-appearance) for the available style layer classes.

**Translating style attributes from Studio**
For each property you’ve edited in Studio, you can hover over the property name to find the corresponding property in the iOS SDK. It’ll generally be the camelCased version of the Property ID, but see [“Configuring the map content’s appearance”](for-style-authors.html#configuring-the-map-content-s-appearance) for a table of properties that differ between Mapbox Studio and the iOS SDK.

![property values](img/studio-workflow/property-values.png)

**Translating stop functions**
It’s possible to use stop functions in Mapbox Studio to transition the style of a layer by its zoom level (e.g. a line that gets wider as you zoom in). These can be translated in the mobile SDKs using `+[MGLSyleValue valueWithInterpolationBase:stops:]`. The rate of change between stops in Studio is represented by `interpolationBase`.

![Stop functions](img/studio-workflow/stop-functions.png)

## Resources

* [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-style-spec/)
* [Mapbox Studio](https://www.mapbox.com/mapbox-studio/)
* [iOS code examples](https://www.mapbox.com/ios-sdk/examples/)
