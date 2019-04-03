# Predicates and expressions

Style layers use predicates and expressions to determine what to display and how
to format it. _Predicates_ are represented by the same `NSPredicate` class that
filters results from Core Data or items in an `NSArray` in Objective-C.
Predicates are based on _expressions_, represented by the `NSExpression` class.
Somewhat unusually, style layers also use expressions on their own.

This document discusses the specific subset of the predicate and expression
syntax supported by this SDK. For a more general introduction to predicates and
expressions, consult the
_[Predicate Programming Guide](https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Predicates/AdditionalChapters/Introduction.html)_
in Apple developer documentation. For additional detail on how this SDK has
extended the `NSExpression` class, consult the [`NSExpression(MGLAdditions)`](./Categories/NSExpression(MGLAdditions).html)
section of this documentation.

## Using predicates to filter vector data

Most style layer classes display `MGLFeature` objects that you can show or hide
based on the feature’s attributes. Use the `MGLVectorStyleLayer.predicate`
property to include only the features in the source layer that satisfy a
condition that you define.

### Operators

The following comparison operators are supported:

`NSPredicateOperatorType`                     | Format string syntax
----------------------------------------------|---------------------
`NSEqualToPredicateOperatorType`              | `key = value`<br />`key == value`
`NSGreaterThanOrEqualToPredicateOperatorType` | `key >= value`<br />`key => value`
`NSLessThanOrEqualToPredicateOperatorType`    | `key <= value`<br />`key =< value`
`NSGreaterThanPredicateOperatorType`          | `key > value`
`NSLessThanPredicateOperatorType`             | `key < value`
`NSNotEqualToPredicateOperatorType`           | `key != value`<br />`key <> value`
`NSBetweenPredicateOperatorType`              | `key BETWEEN { 32, 212 }`

To test whether a feature has or lacks a specific attribute, compare the
attribute to `NULL` or `NIL`. Predicates created using the
`+[NSPredicate predicateWithValue:]` method are also supported. String
operators and custom operators are not supported.

The following compound operators are supported:

`NSCompoundPredicateType` | Format string syntax
--------------------------|---------------------
`NSAndPredicateType`      | `predicate1 AND predicate2`<br />`predicate1 && predicate2`
`NSOrPredicateType`       | `predicate1 OR predicate2`<br /><code>predicate1 &vert;&vert; predicate2</code>
`NSNotPredicateType`      | `NOT predicate`<br />`!predicate`

The following aggregate operators are supported:

`NSPredicateOperatorType`         | Format string syntax
----------------------------------|---------------------
`NSInPredicateOperatorType`       | `key IN { 'iOS', 'macOS', 'tvOS', 'watchOS' }`
`NSContainsPredicateOperatorType` | `{ 'iOS', 'macOS', 'tvOS', 'watchOS' } CONTAINS key`

The following comparison predicate options are supported for comparison and
aggregate operators that are used in the predicate:

`NSComparisonPredicateOptions`          | Format string syntax
----------------------------------------|---------------------
`NSCaseInsensitivePredicateOption`      | `'QUEBEC' =[c] 'Quebec'`
`NSDiacriticInsensitivePredicateOption` | `'Québec' =[d] 'Quebec'`

Other comparison predicate options are unsupported, namely `l`
(for locale sensitivity) and `n` (for normalization). A comparison is
locale-sensitive as long as it is case- or diacritic-insensitive.

### Operands

Operands in predicates can be [variables](#variables), [key paths](#key-paths),
or almost anything else that can appear
[inside an expression](#using-expressions-to-configure-layout-and-paint-attributes).

Automatic type casting is not performed. Therefore, a feature only matches a
predicate if its value for the attribute in question is of the same type as the
value specified in the predicate. Use the `CAST()` operator to convert a key
path or variable into a matching type:

* To cast a value to a number, use `CAST(key, 'NSNumber')`.
* To cast a value to a string, use `CAST(key, 'NSString')`.
* To cast a value to a color, use `CAST(key, 'UIColor')` on iOS and `CAST(key, 'NSColor')` on macOS.
* To cast an `NSColor` or `UIColor` object to an array, use `CAST(noindex(color), 'NSArray')`.

For details about the predicate format string syntax, consult the “Predicate
Format String Syntax” chapter of the
_[Predicate Programming Guide](https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Predicates/AdditionalChapters/Introduction.html)_
in Apple developer documentation.

## Using expressions to configure layout and paint attributes

An expression can contain subexpressions of various types. Each of the supported
types of expressions is discussed below.

### Constant values

A constant value can be of any of the following types:

In Objective-C        | In Swift
----------------------|---------
`NSColor` (macOS)<br>`UIColor` (iOS) | `NSColor` (macOS)<br>`UIColor` (iOS)
`NSString`            | `String`
`NSString`            | `String`
`NSNumber.boolValue`  | `NSNumber.boolValue`
`NSNumber.doubleValue` | `NSNumber.doubleValue`
`NSArray<NSNumber>` | `[Float]`
`NSArray<NSString>` | `[String]`
`NSValue.CGVectorValue` (iOS)<br>`NSValue` containing `CGVector` (macOS) | `NSValue.cgVectorValue` (iOS)<br>`NSValue` containing `CGVector` (macOS)
`NSValue.UIEdgeInsetsValue` (iOS)<br>`NSValue.edgeInsetsValue` (macOS) | `NSValue.uiEdgeInsetsValue` (iOS)<br>`NSValue.edgeInsetsValue` (macOS)

For literal floating-point values, use `-[NSNumber numberWithDouble:]` instead
of `-[NSNumber numberWithFloat:]` to avoid precision issues.

### Key paths

A key path expression refers to an attribute of the `MGLFeature` object being
evaluated for display. For example, if a polygon’s `MGLFeature.attributes`
dictionary contains the `floorCount` key, then the key path `floorCount` refers
to the value of the `floorCount` attribute when evaluating that particular
polygon.

The following special attributes are also available on features that are produced
as a result of clustering multiple point features together in a shape source:

<table>
<thead>
<tr><th>Attribute</th><th>Type</th><th>Meaning</th></tr>
</thead>
<tbody>
<tr>
   <td><code>cluster</code></td>
   <td>Bool</td>
   <td>True if the feature is a point cluster. If the attribute is false (or not present) then the  feature should not be considered a cluster.</td>
</tr>
<tr>
   <td><code>cluster_id</code></td>
   <td>Number</td>
   <td>Identifier for the point cluster.</td>
</tr>
<tr>
   <td><code>point_count</code></td>
   <td>Number</td>
   <td>The number of point features in a given cluster.</td>
</tr>
</tbody>
</table>

Some characters may not be used directly as part of a key path in a format
string. For example, if a feature’s attribute is named `ISO 3166-1:2006`, an
expression format string of `lowercase(ISO 3166-1:2006)` or a predicate format
string of `ISO 3166-1:2006 == 'US-OH'` would raise an exception. Instead, use a
`%K` placeholder or the `+[NSExpression expressionForKeyPath:]` initializer:

```objc
[NSPredicate predicateWithFormat:@"%K == 'US-OH'", @"ISO 3166-1:2006"];
[NSExpression expressionForFunction:@"lowercase:"
                          arguments:@[[NSExpression expressionForKeyPath:@"ISO 3166-1:2006"]]]
```

```swift
NSPredicate(format: "%K == 'US-OH'", "ISO 3166-1:2006")
NSExpression(forFunction: "lowercase:",
             arguments: [NSExpression(forKeyPath: "ISO 3166-1:2006")])
```

### Functions

Of the
[functions predefined](https://developer.apple.com/documentation/foundation/nsexpression/1413747-init#discussion)
by the
[`+[NSExpression expressionForFunction:arguments:]` method](https://developer.apple.com/documentation/foundation/nsexpression/1413747-init),
the following subset is supported in layer attribute values:

Initializer parameter | Format string syntax
----------------------|---------------------
`average:`            | `average({1, 2, 2, 3, 4, 7, 9})`
`sum:`                | `sum({1, 2, 2, 3, 4, 7, 9})`
`count:`              | `count({1, 2, 2, 3, 4, 7, 9})`
`min:`                | `min({1, 2, 2, 3, 4, 7, 9})`
`max:`                | `max({1, 2, 2, 3, 4, 7, 9})`
`add:to:`             | `1 + 2`
`from:subtract:`      | `2 - 1`
`multiply:by:`        | `1 * 2`
`divide:by:`          | `1 / 2`
`modulus:by:`         | `modulus:by:(1, 2)`
`sqrt:`               | `sqrt(2)`
`log:`                | `log(10)`
`ln:`                 | `ln(2)`
`raise:toPower:`      | `2 ** 2`
`exp:`                | `exp(0)`
`ceiling:`            | `ceiling(0.99999)`
`abs:`                | `abs(-1)`
`trunc:`              | `trunc(6378.1370)`
`floor:`              | `floor(-0.99999)`
`uppercase:`          | `uppercase('Elysian Fields')`
`lowercase:`          | `lowercase('DOWNTOWN')`
`noindex:`            | `noindex(0 + 2 + c)`
`length:`             | `length('Wapakoneta')`
`castObject:toType:`  | `CAST(ele, 'NSString')`<br>`CAST(ele, 'NSNumber')`

A number of [Mapbox-specific functions](#mapbox-specific-functions) are also
available.

The following predefined functions are **not** supported:

Initializer parameter | Format string syntax
----------------------|---------------------
`median:`             | `median({1, 2, 2, 3, 4, 7, 9})`
`mode:`               | `mode({1, 2, 2, 3, 4, 7, 9})`
`stddev:`             | `stddev({1, 2, 2, 3, 4, 7, 9})`
`random`              | `random()`
`randomn:`            | `randomn(10)`
`now`                 | `now()`
`bitwiseAnd:with:`    | `bitwiseAnd:with:(5, 3)`
`bitwiseOr:with:`     | `bitwiseOr:with:(5, 3)`
`bitwiseXor:with:`    | `bitwiseXor:with:(5, 3)`
`leftshift:by:`       | `leftshift:by:(23, 1)`
`rightshift:by:`      | `rightshift:by:(23, 1)`
`onesComplement:`     | `onesComplement(255)`
`distanceToLocation:fromLocation:` | `distanceToLocation:fromLocation:(there, here)`

### Conditionals

Conditionals are supported via the built-in
`+[NSExpression expressionForConditional:trueExpression:falseExpression:]`
method and `TERNARY()` operator. If you need to express multiple cases
(“else-if”), you can either nest a conditional within a conditional or use the
[`MGL_IF()`](#code-mgl_if-code) or [`MGL_MATCH()`](#code-mgl_match-code) function.

### Aggregates

Aggregate expressions can contain arrays of expressions. In some cases, it is
possible to use the array itself instead of wrapping the array in an aggregate
expression.

### Variables

The following variables are defined by this SDK for use with style layers:

<table>
<thead>
<tr><th>Variable</th><th>Type</th><th>Meaning</th></tr>
</thead>
<tbody>
<tr>
   <td><code>$featureIdentifier</code></td>
   <td>
      Any
      <a href="working-with-geojson-data.html#about-geojson-deserialization">GeoJSON data type</a>
   </td>
   <td>
      A value that uniquely identifies the feature in the containing source.
      This variable corresponds to the
      <code>NSExpression.featureIdentifierVariableExpression</code> property.
   </td>
</tr>
<tr>
   <td><code>$geometryType</code></td>
   <td>String</td>
   <td>
       The type of geometry represented by the feature. A feature’s type is one
       of the following strings:
       <ul>
           <li>
               <code>Point</code> for point features, corresponding to the
               <code>MGLPointAnnotation</code> class
           </li>
           <li>
               <code>LineString</code> for polyline features, corresponding to
               the <code>MGLPolyline</code> class
           </li>
           <li>
               <code>Polygon</code> for polygon features, corresponding to the
               <code>MGLPolygon</code> class
           </li>
       </ul>
       This variable corresponds to the
       <code>NSExpression.geometryTypeVariableExpression</code> property.
   </td>
</tr>
<tr>
   <td><code>$heatmapDensity</code></td>
   <td>Number</td>
   <td>
      The
      <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">kernel density estimation</a>
      of a screen point in a heatmap layer; in other words, a relative measure
      of how many data points are crowded around a particular pixel. This
      variable can only be used with the <code>heatmapColor</code> property.
      This variable corresponds to the
      <code>NSExpression.heatmapDensityVariableExpression</code> property.
   </td>
</tr>
<tr>
   <td><code>$zoomLevel</code></td>
   <td>Number</td>
   <td>
      The current zoom level. In style layout and paint properties, this
      variable may only appear as the target of a top-level interpolation or
      step expression. This variable corresponds to the
      <code>NSExpression.zoomLevelVariableExpression</code> property.
   </td>
</tr>
<tr>
   <td><code>$lineProgress</code></td>
   <td>Number</td>
   <td>
      A number that indicates the relative distance along a line at a given
      point along the line. This variable evaluates to 0 at the beginning of the
      line and 1 at the end of the line. It can only be used with the
      `MGLLineStyleLayer.lineGradient` property. It corresponds to the
      <code>NSExpression.lineProgressVariableExpression</code> property.
   </td>
</tr>
</tbody>
</table>

In addition to these variables, you can define your own variables and refer to
them elsewhere in the expression. The syntax for defining a variable makes use
of a [Mapbox-specific function](#mapbox-specific-functions) that takes an
`NSDictionary` as an argument:

```objc
[NSExpression expressionWithFormat:@"MGL_LET('floorCount', 2, $floorCount + 1)"];
```

```swift
NSExpression(format: "MGL_LET(floorCount, 2, $floorCount + 1)")
```

## Mapbox-specific functions

For compatibility with the Mapbox Style Specification, the following functions
are defined by this SDK. When setting a style layer property, you can call these
functions just like the predefined functions above, using either the
`+[NSExpression expressionForFunction:arguments:]` method or a convenient format
string syntax:

### `mgl_does:have:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_does:have:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_does:have:(SELF, '🧀🍔')</code> or <code>mgl_does:have:(%@, '🧀🍔')</code></dd>
</dl>

Returns a Boolean value indicating whether the dictionary has a value for the
key or whether the evaluated object (`SELF`) has a value for the feature
attribute. Compared to the [`mgl_has:`](#code-mgl_has-code) custom function,
that function’s target is instead passed in as the first argument to this
function. Both functions are equivalent to the syntax `key != NIL` or
`%@[key] != NIL` but can be used outside of a predicate.

### `mgl_interpolate:withCurveType:parameters:stops:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_interpolate:withCurveType:parameters:stops:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_interpolate:withCurveType:parameters:stops:(x, 'linear', nil, %@)</code></dd>
</dl>

Produces continuous, smooth results by interpolating between pairs of input and
output values (“stops”). Compared to the
[`mgl_interpolateWithCurveType:parameters:stops:`](#code-mgl_interpolatewithcurvetype-parameters-stops-code)
custom function, the input expression (that function’s target) is instead passed
in as the first argument to this function.

### `mgl_step:from:stops:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_step:from:stops:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_step:from:stops:(x, 11, %@)</code></dd>
</dl>

Produces discrete, stepped results by evaluating a piecewise-constant function
defined by pairs of input and output values ("stops"). Compared to the
[`mgl_stepWithMinimum:stops:`](#code-mgl_stepwithminimum-stops-code) custom
function, the input expression (that function’s target) is instead passed in as
the first argument to this function.

### `mgl_join:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_join:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_join({'Old', 'MacDonald'})</code></dd>
</dl>

Returns the result of concatenating together all the elements of an array in
order. Compared to the
[`stringByAppendingString:`](#code-stringbyappendingstring-code) custom
function, this function takes only one argument, which is an aggregate
expression containing the strings to concatenate.

### `mgl_acos:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_acos:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_acos(1)</code></dd>
</dl>

Returns the arccosine of the number.

This function corresponds to the
[`acos`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-acos)
operator in the Mapbox Style Specification.

### `mgl_asin:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_asin:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_asin(0)</code></dd>
</dl>

Returns the arcsine of the number.

This function corresponds to the
[`asin`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-asin)
operator in the Mapbox Style Specification.

### `mgl_atan:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_atan:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_atan(20)</code></dd>
</dl>

Returns the arctangent of the number.

This function corresponds to the
[`atan`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-atan)
operator in the Mapbox Style Specification.

### `mgl_cos:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_cos:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_cos(0)</code></dd>
</dl>

Returns the cosine of the number.

This function corresponds to the
[`cos`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-cos)
operator in the Mapbox Style Specification.

### `mgl_log2:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_log2:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_log2(1024)</code></dd>
</dl>

Returns the base-2 logarithm of the number.

This function corresponds to the
[`log2`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-log2)
operator in the Mapbox Style Specification.

### `mgl_round:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_round:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_round(1.5)</code></dd>
</dl>

Returns the number rounded to the nearest integer. If the number is halfway
between two integers, this function rounds it away from zero.

This function corresponds to the
[`round`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-round)
operator in the Mapbox Style Specification.

### `mgl_sin:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_sin:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_sin(0)</code></dd>
</dl>

Returns the sine of the number.

This function corresponds to the
[`sin`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-sin)
operator in the Mapbox Style Specification.

### `mgl_tan:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_tan:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_tan(0)</code></dd>
</dl>

Returns the tangent of the number.

This function corresponds to the
[`tan`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-tan)
operator in the Mapbox Style Specification.

### `mgl_coalesce:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_coalesce:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_coalesce({x, y, z})</code></dd>
</dl>

Returns the first non-`nil` value from an array of expressions.

This function corresponds to the
[`coalesce`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-coalesce)
operator in the Mapbox Style Specification.

### `mgl_attributed:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_attributed:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>mgl_attributed({x, y, z})</code></dd>
</dl>

Concatenates and returns the array of `MGLAttributedExpression` objects, for use 
with the `MGLSymbolStyleLayer.text` property.

`MGLAttributedExpression.attributes` valid attributes.

 Key | Value Type
 --- | ---
 `MGLFontNamesAttribute` | An `NSExpression` evaluating to an `NSString` array.
 `MGLFontScaleAttribute` | An `NSExpression` evaluating to an `NSNumber` value.
 `MGLFontColorAttribute` | An `NSExpression` evaluating to an `UIColor` (iOS) or `NSColor` (macOS).

This function corresponds to the
[`format`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-types-format)
operator in the Mapbox Style Specification.

### `MGL_LET`

<dl>
<dt>Selector:</dt>
<dd><code>MGL_LET:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>MGL_LET('age', uppercase('old'), 'name', uppercase('MacDonald'), mgl_join({$age, $name}))</code></dd>
<dt>Arguments:</dt>
<dd>
   Any number of variable names interspersed with their assigned
   <code>NSExpression</code> values, followed by an <code>NSExpression</code>
   that may contain references to those variables.
</dd>
</dl>

Returns the result of evaluating an expression with the given variable values.
Compared to the
[`mgl_expressionWithContext:`](#code-mgl_expressionwithcontext-code) custom
function, this function takes the variable names and values inline before the
expression that contains references to those variables.

### `MGL_MATCH`

<dl>
<dt>Selector:</dt>
<dd><code>MGL_MATCH:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>MGL_MATCH(x, 0, 'zero match', 1, 'one match', 2, 'two match', 'default')</code></dd>
<dt>Arguments:</dt>
<dd>
   An input expression, then any number of argument pairs, followed by a default
   expression. Each argument pair consists of a constant value followed by an
   expression to produce as a result of matching that constant value.
   If the input value is an aggregate expression, then any of the constant values within
   that aggregate expression result in the following argument. This is shorthand for
   specifying an argument pair for each of the constant values within that aggregate
   expression. It is not possible to match the aggregate expression itself.
</dd>
</dl>

Returns the result of matching the input expression against the given constant
values.

This function corresponds to the
`+[NSExpression(MGLAdditions) mgl_expressionForMatchingExpression:inDictionary:defaultExpression:]`
method and the
[`match`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-match)
operator in the Mapbox Style Specification.

### `MGL_IF`

<dl>
<dt>Selector:</dt>
<dd><code>MGL_IF:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>MGL_IF(1 = 2, YES, 2 = 2, YES, NO)</code></dd>
<dt>Arguments:</dt>
<dd>
   Alternating <code>NSPredicate</code> conditionals and resulting expressions,
   followed by a default expression.
</dd>
</dl>

Returns the first expression that meets the condition; otherwise, the default
value. Unlike
`+[NSExpression expressionForConditional:trueExpression:falseExpression:]` or
the `TERNARY()` syntax, this function can accept multiple “if else” conditions
and is supported on iOS 8._x_ and macOS 10.10._x_; however, each conditional
passed into this function must be wrapped in a constant expression.

This function corresponds to the
`+[NSExpression(MGLAdditions) mgl_expressionForConditional:trueExpression:falseExpresssion:]`
method and the
[`case`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-case)
operator in the Mapbox Style Specification.

### `MGL_FUNCTION`

<dl>
<dt>Selector:</dt>
<dd><code>MGL_FUNCTION:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>MGL_FUNCTION('typeof', mystery)</code></dd>
<dt>Arguments:</dt>
<dd>
   Any arguments required by the expression operator.
</dd>
</dl>

An expression exactly as defined by the
[Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions).

## Custom functions

The following custom functions are also available with the
`+[NSExpression expressionForFunction:selectorName:arguments:]` method or the
`FUNCTION()` format string syntax.

Some of these functions are defined as methods on their respective target
classes, but you should not call them directly outside the context of an
expression, because the result may differ from the evaluated expression’s result
or may result in undefined behavior.

The Mapbox Style Specification defines some operators for which no custom
function is available. To use these operators in an `NSExpression`, call the
[`MGL_FUNCTION()`](#code-mgl_function-code) function with the same arguments
that the operator expects.

### `boolValue`

<dl>
<dt>Selector:</dt>
<dd><code>boolValue</code></dd>
<dt>Format string syntax:</dt>
<dd><code>FUNCTION(1, 'boolValue')</code></dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to a number or string.
</dd>
<dt>Arguments:</dt>
<dd>None.</dd>
</dl>

A Boolean representation of the target: <code>FALSE</code> when then input is an
empty string, 0, <code>FALSE</code>, <code>NIL</code>, or <code>NaN</code>,
otherwise <code>TRUE</code>.

### `mgl_has:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_has:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>FUNCTION($featureAttributes, 'mgl_has:', '🧀🍔')</code></dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to an <code>NSDictionary</code>
   or the evaluated object (<code>SELF</code>).
</dd>
<dt>Arguments:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to an <code>NSString</code>
   representing the key to look up in the dictionary or the feature attribute to
   look up in the evaluated object (see <code>MGLFeature.attributes</code>).
</dd>
</dl>

<code>true</code> if the dictionary has a value for the key or if the evaluated
object has a value for the feature attribute.

This function corresponds to the
[`has`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-has)
operator in the Mapbox Style Specification. See also the
[`mgl_does:have:`](#code-mgl_does-have-code) function, which is used on its own
without the `FUNCTION()` operator. You can also check whether an object has an
attribute by comparing the key path to `NIL`, for example `cheeseburger != NIL`
or `burger.cheese != NIL`

### `mgl_expressionWithContext:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_expressionWithContext:</code></dd>
<dt>Format string syntax:</dt>
<dd>
   <code>FUNCTION($ios + $macos, 'mgl_expressionWithContext:', %@)</code> with
   a dictionary containing <code>ios</code> and <code>macos</code> keys
</dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that may contain references to the variables
   defined in the context dictionary.
</dd>
<dt>Arguments:</dt>
<dd>
   An <code>NSDictionary</code> with <code>NSString</code>s as keys and
   <code>NSExpression</code>s as values. Each key is a variable name and each
   value is the variable’s value within the target expression.
</dd>
</dl>

The target expression with variable subexpressions replaced with the values
defined in the context dictionary.

This function corresponds to the
[`let`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-let)
operator in the Mapbox Style Specification. See also the
[`MGL_LET`](#code-mgl_let-code) function, which is used on its own without the
`FUNCTION()` operator.

### `mgl_interpolateWithCurveType:parameters:stops:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_interpolateWithCurveType:parameters:stops:</code></dd>
<dt>Format string syntax:</dt>
<dd>
   <code>FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', NIL, %@)</code>
   with a dictionary containing zoom levels or other constant values as keys
</dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to a number and contains a
   variable or key path expression.
</dd>
<dt>Arguments:</dt>
<dd>
   The first argument is one of the following strings denoting curve types:
   <code>linear</code>, <code>exponential</code>, or <code>cubic-bezier</code>.

   The second argument is an expression providing parameters for the curve:

   <ul>
      <li>If the curve type is <code>linear</code>, the argument is <code>NIL</code>.</li>
      <li>
         If the curve type is <code>exponential</code>, the argument is an
         expression that evaluates to a number, specifying the base of the
         exponential interpolation.
      </li>
      <li>
         If the curve type is <code>cubic-bezier</code>, the argument is an
         array or aggregate expression containing four expressions, each
         evaluating to a number. The four numbers are control points for the
         cubic Bézier curve.
      </li>
   </ul>

   The third argument is an <code>NSDictionary</code> object representing the
   interpolation’s stops, with numeric zoom levels as keys and expressions as
   values.
</dd>
</dl>

A value interpolated along the continuous mathematical function defined by the
arguments, with the target as the input to the function.

The input expression is matched against the keys in the stop dictionary. The
keys may be feature attribute values, zoom levels, or heatmap densities. The
values may be constant values or `NSExpression` objects. For example, you can
use a stop dictionary with the zoom levels 0, 10, and 20 as keys and the colors
yellow, orange, and red as the values.

This function corresponds to the
`+[NSExpression(MGLAdditions) mgl_expressionForInterpolatingExpression:withCurveType:parameters:stops:]`
method and the
[`interpolate`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-interpolate)
operator in the Mapbox Style Specification. See also the
[`mgl_interpolate:withCurveType:parameters:stops:`](#code-mgl_interpolate-withcurvetype-parameters-stops-code)
function, which is used on its own without the `FUNCTION()` operator.

### `mgl_numberWithFallbackValues:`

<dl>
<dt>Selector:</dt>
<dd>
   <code>mgl_numberWithFallbackValues:</code>,
   <code>doubleValue</code>,
   <code>floatValue</code>, or
   <code>decimalValue</code>
</dd>
<dt>Format string syntax:</dt>
<dd><code>FUNCTION(ele, 'mgl_numberWithFallbackValues:', 0)</code></dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to a Boolean value, number, or
   string.
</dd>
<dt>Arguments:</dt>
<dd>
   Zero or more <code>NSExpression</code>s, each evaluating to a Boolean value
   or string.
</dd>
</dl>

A numeric representation of the target:

* If the target is <code>NIL</code> or <code>FALSE</code>, the result is 0.
* If the target is true, the result is 1.
  * If the target is a string, it is converted to a number as specified by the
    “[ToNumber Applied to the String Type](https://tc39.github.io/ecma262/#sec-tonumber-applied-to-the-string-type)”
    algorithm of the ECMAScript Language Specification.
  * If multiple values are provided, each one is evaluated in order until the
    first successful conversion is obtained.

This function corresponds to the
[`to-number`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-types-to-number)
operator in the Mapbox Style Specification. You can also cast a value to a
number by passing the value and the string `NSNumber` into the `CAST()`
operator.

### `mgl_stepWithMinimum:stops:`

<dl>
<dt>Selector:</dt>
<dd><code>mgl_stepWithMinimum:stops:</code></dd>
<dt>Format string syntax:</dt>
<dd>
   <code>FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', 0, %@)</code> with
   a dictionary with zoom levels or other constant values as keys
</dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to a number and contains a
   variable or key path expression.
</dd>
<dt>Arguments:</dt>
<dd>
   The first argument is an expression that evaluates to a number, specifying
   the minimum value in case the target is less than any of the stops in the
   second argument.

   The second argument is an <code>NSDictionary</code> object representing the
   interpolation’s stops, with numeric zoom levels as keys and expressions as
   values.
</dd>
</dl>

The output value of the stop whose key is just less than the evaluated target,
or the minimum value if the target is less than the least of the stops’ keys.

The input expression is matched against the keys in the stop dictionary. The
keys may be feature attribute values, zoom levels, or heatmap densities. The
values may be constant values or `NSExpression` objects. For example, you can
use a stop dictionary with the zoom levels 0, 10, and 20 as keys and the colors
yellow, orange, and red as the values.

This function corresponds to the
`+[NSExpression(MGLAdditions) mgl_expressionForSteppingExpression:fromExpression:stops:]`
method and the
[`step`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-step)
operator in the Mapbox Style Specification.

### `stringByAppendingString:`

<dl>
<dt>Selector:</dt>
<dd><code>stringByAppendingString:</code></dd>
<dt>Format string syntax:</dt>
<dd><code>FUNCTION('Old', 'stringByAppendingString:', 'MacDonald')</code></dd>
<dt>Target:</dt>
<dd>An <code>NSExpression</code> that evaluates to a string.</dd>
<dt>Arguments:</dt>
<dd>One or more <code>NSExpression</code>s, each evaluating to a string.</dd>
</dl>

The target string with each of the argument strings appended in order.

This function corresponds to the
`-[NSExpression(MGLAdditions) mgl_expressionByAppendingExpression:]`
method and is similar to the
[`concat`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-concat)
operator in the Mapbox Style Specification. See also the
[`mgl_join:`](#code-mgl_join-code) function, which concatenates multiple
expressions and is used on its own without the `FUNCTION()` operator.

### `stringValue`

<dl>
<dt>Selector:</dt>
<dd><code>stringValue</code></dd>
<dt>Format string syntax:</dt>
<dd><code>FUNCTION(ele, 'stringValue')</code></dd>
<dt>Target:</dt>
<dd>
   An <code>NSExpression</code> that evaluates to a Boolean value, number, or
   string.
</dd>
<dt>Arguments:</dt>
<dd>None.</dd>
</dl>

A string representation of the target:

* If the target is <code>NIL</code>, the result is the empty string.
* If the target is a Boolean value, the result is the string `true` or `false`.
* If the target is a number, it is converted to a string as specified by the
  “[NumberToString](https://tc39.github.io/ecma262/#sec-tostring-applied-to-the-number-type)”
  algorithm of the ECMAScript Language Specification.
* If the target is a color, it is converted to a string of the form
  `rgba(r,g,b,a)`, where <var>r</var>, <var>g</var>, and <var>b</var> are
  numerals ranging from 0 to 255 and <var>a</var> ranges from 0 to 1.
* Otherwise, the target is converted to a string in the format specified by the
  [`JSON.stringify()`](https://tc39.github.io/ecma262/#sec-json.stringify)
  function of the ECMAScript Language Specification.

This function corresponds to the
[`to-string`](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-types-to-string)
operator in the Mapbox Style Specification. You can also cast a value to a
string by passing the value and the string `NSString` into the `CAST()`
operator.
