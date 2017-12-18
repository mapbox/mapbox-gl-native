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
in Apple developer documentation.

## Using predicates to filter vector data

Most style layer classes display `MGLFeature` objects that you can show or hide
based on the feature’s attributes. Use the `MGLVectorStyleLayer.predicate`
property to include only the features in the source layer that satisfy a
condition that you define.

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

The following compound operators are supported:

`NSCompoundPredicateType` | Format string syntax
--------------------------|---------------------
`NSAndPredicateType`      | `predicate1 AND predicate2`<br />`predicate1 && predicate2`
`NSOrPredicateType`       | `predicate1 OR predicate2`<br />`predicate1 || predicate2`
`NSNotPredicateType`      | `NOT predicate`<br />`!predicate`

The following aggregate operators are supported:

`NSPredicateOperatorType`         | Format string syntax
----------------------------------|---------------------
`NSInPredicateOperatorType`       | `key IN { 'iOS', 'macOS', 'tvOS', 'watchOS' }`
`NSContainsPredicateOperatorType` | `{ 'iOS', 'macOS', 'tvOS', 'watchOS' } CONTAINS key`

To test whether a feature has or lacks a specific attribute, compare the
attribute to `NULL` or `NIL`. Predicates created using the
`+[NSPredicate predicateWithValue:]` method are also supported. String
operators and custom operators are not supported.

For details about the predicate format string syntax, consult the “Predicate
Format String Syntax” chapter of the
_[Predicate Programming Guide](https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Predicates/AdditionalChapters/Introduction.html)_
in Apple developer documentation.

The predicate's left-hand expression must be a string that identifies a feature
attribute or, alternatively, one of the following special attributes:

<table>
<thead>
<tr><th>Attribute</th><th>Meaning</th></tr>
</thead>
<tbody>
<tr>
   <td><code>$id</code></td>
   <td>
       A value that uniquely identifies the feature in the containing source.
       For details on the types of values that may be associated with this key,
       consult the documentation for the <code>MGLFeature</code> protocol’s
       <code>identifier</code> property.
   </td>
</tr>
<tr>
   <td><code>$type</code></td>
   <td>
       The type of geometry represented by the feature. A feature’s type is
       guaranteed to be one of the following strings:
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
   </td>
</tr>
<tr>
   <td><code>point_count</code></td>
   <td>The number of point features in a given cluster.</td>
</tr>
</tbody>
</table>

The predicate’s right-hand expression must be an `NSString` (to match strings)
or `NSNumber` (to match numbers, including Boolean values) or an array of
`NSString`s or `NSNumber`s, depending on the operator and the type of values
expected for the attribute being tested. For floating-point values, use
`-[NSNumber numberWithDouble:]` instead of `-[NSNumber numberWithFloat:]`
to avoid precision issues.

Automatic type casting is not performed. Therefore, a feature only matches this
predicate if its value for the attribute in question is of the same type as the
value specified in the predicate. Also, operator modifiers such as `c` (for
case insensitivity), `d` (for diacritic insensitivity), and `l` (for locale
sensitivity) are unsupported for comparison and aggregate operators that are
used in the predicate.

It is possible to create expressions that contain special characters in the
predicate format syntax. This includes the `$` in the `$id` and `$type` special
style attributes and also `hyphen-minus` and `tag:subtag`. However, you must use
`%K` in the format string to represent these variables:
`@"%K == 'LineString'", @"$type"`.

## Using expressions to configure layout and paint attributes

### Functions

#### Key paths

A key path expression refers to an attribute of the `MGLFeature` object being
evaluated for display. For example, if a polygon’s `MGLFeature.attributes`
dictionary contains the `floorCount` key, then the key path `floorCount` refers
to the value of the `floorCount` attribute when evaluating that particular
polygon.

#### Predefined functions

Of the
[functions predefined by the `+[NSExpression expressionForFunction:arguments:]` method](https://developer.apple.com/documentation/foundation/nsexpression/1413747-init#discussion),
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

The following predefined functions are not supported:

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

#### Mapbox-specific functions

For compatibility with the Mapbox Style Specification, the following functions
are defined by this SDK for use with style layers. Because these functions are
not predefined by `NSExpression`, you must use the
`+[NSExpression expressionForFunction:selectorName:arguments:]` method or the
`FUNCTION()` format string syntax instead. 

<table>
<thead>
<tr><th>Selector name</th><th>Target</th><th>Arguments</th><th>Returns</th></tr>
</thead>
<tbody>
<tr>
   <td><code>boolValue</code></td>
   <td>
      An `NSExpression` that evaluates to a number or string.
   </td>
   <td></td>
   <td>
      A Boolean representation of the target: `FALSE` when then input is an
      empty string, 0, `FALSE`, `NIL`, or NaN, otherwise `TRUE`.
   </td>
</tr>
<tr>
   <td><code>mgl_expressionWithContext:</code></td>
   <td>
      An `NSExpression` that may contain references to the variables defined in
      the context dictionary.
   </td>
   <td>
      An `NSDictionary` with `NSString`s as keys and `NSExpression`s as values.
      Each key is a variable name and each value is the variable’s value within
      the target expression.
   </td>
   <td>
      The target expression with variable subexpressions replaced with the
      values defined in the context dictionary.
   </td>
</tr>
<tr>
   <td><code>mgl_interpolateWithCurveType:parameters:stops:</code></td>
   <td>
      An `NSExpression` that evaluates to a number and contains a variable or
      key path expression.
   </td>
   <td>
      The first argument is one of the following strings denoting curve types:
      `linear`, `exponential`, or `cubic-bezier`.
      
      The second argument is an expression providing parameters for the curve:
      
      <ul>
         <li>If the curve type is `linear`, the argument is `NIL`.</li>
         <li>
            If the curve type is `exponential`, the argument is an expression
            that evaluates to a number, specifying the base of the exponential
            interpolation.
         </li>
         <li>
            If the curve type is `cubic-bezier`, the argument is an array or
            aggregate expression containing four expressions, each evaluating to
            a number. The four numbers are control points for the cubic Bézier
            curve.
         </li>
      </ul>
      
      The third argument is an `NSDictionary` object representing the
      interpolation’s stops, with numeric zoom levels as keys and expressions as
      values.
   </td>
   <td>
      A value interpolated along the continuous mathematical function defined by
      the arguments, with the target as the input to the function.
   </td>
</tr>
<tr>
   <td>
      <code>mgl_numberWithFallbackValues:</code>,
      <code>doubleValue</code>,
      <code>floatValue</code>,
      <code>decimalValue</code>
   </td>
   <td>
      An `NSExpression` that evaluates to a Boolean value, number, or string.
   </td>
   <td>
      Zero or more `NSExpression`s, each evaluating to a Boolean value or
      string.
   </td>
   <td>
      A numeric representation of the target:
      <ul>
         <li>If the target is `NIL` or `FALSE`, the result is 0.</li>
         <li>If the target is true, the result is 1.</li>
         <li>
            If the target is a string, it is converted to a number as specified
            by the
            “<a href="https://tc39.github.io/ecma262/#sec-tonumber-applied-to-the-string-type">ToNumber Applied to the String Type</a>”
            algorithm of the ECMAScript Language Specification.
         </li>
         <li>
            If multiple values are provided, each one is evaluated in order
            until the first successful conversion is obtained.
         </li>
      </ul>
   </td>
</tr>
<tr>
   <td><code>mgl_stepWithMinimum:stops:</code></td>
   <td>
      An `NSExpression` that evaluates to a number and contains a variable or
      key path expression.
   </td>
   <td>
      The first argument is an expression that evaluates to a number, specifying
      the minimum value in case the target is less than any of the stops in the
      second argument.
      
      The second argument is an `NSDictionary` object representing the
      interpolation’s stops, with numeric zoom levels as keys and expressions as
      values.
   </td>
   <td>
      The output value of the stop whose key is just less than the evaluated
      target, or the minimum value if the target is less than the least of the
      stops’ keys.
   </td>
</tr>
<tr>
   <td><code>stringByAppendingString:</code></td>
   <td>
      An `NSExpression` that evaluates to a string.
   </td>
   <td>
      One or more `NSExpression`s, each evaluating to a string.
   </td>
   <td>
      The target string with each of the argument strings appended in order.
   </td>
</tr>
<tr>
   <td><code>stringValue</code></td>
   <td>
      An `NSExpression` that evaluates to a Boolean value, number, or string.
   </td>
   <td></td>
   <td>
      A string representation of the target:
      <ul>
         <li>If the target is `NIL`, the result is the string `null`.</li>
         <li>
            If the target is a Boolean value, the result is the string `true` or
            `false`.
         </li>
         <li>
            If the target is a number, it is converted to a string as specified
            by the
            “<a href="https://tc39.github.io/ecma262/#sec-tostring-applied-to-the-number-type">NumberToString</a>”
            algorithm of the ECMAScript Language Specification.
         </li>
         <li>
            If the target is a color, it is converted to a string of the form
            `rgba(r,g,b,a)`, where <var>r</var>, <var>g</var>, and <var>b</var>
            are numerals ranging from 0 to 255 and <var>a</var> ranges from 0 to
            1.
         </li>
         <li>
            Otherwise, the target is converted to a string in the format
            specified by the
            [`JSON.stringify()`](https://tc39.github.io/ecma262/#sec-json.stringify)
            function of the ECMAScript Language Specification.
         </li>
      </ul>
   </td>
</tr>
</tbody>
</table>

Some of these functions are defined as methods on their respective target
classes, but you should not call them directly outside the context of an
expression, because the result may differ from the evaluated expression’s result
or may result in undefined behavior.

### Variables

The following variables are defined by this SDK for use with style layers:

<table>
<thead>
<tr><th>Variable</th><th>Type</th><th>Meaning</th></tr>
</thead>
<tbody>
<tr>
   <td><code>$heatmapDensity</code></td>
   <td>Number</td>
   <td>
      The
      <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">kernel density estimation</a>
      of a screen point in a heatmap layer; in other words, a relative measure
      of how many data points are crowded around a particular pixel. This
      variable can only be used with the `heatmapColor` property.
   </td>
</tr>
<tr>
   <td><code>$zoomLevel</code></td>
   <td>Number</td>
   <td>
      The current zoom level. In style layout and paint properties, this
      variable may only appear as the target of a top-level interpolation or
      step expression.
   </td>
</tr>
</tbody>
</table>

In addition to these variables, you can define your own variables and refer to
them elsewhere in the expression. The syntax for defining a variable makes use
of a [Mapbox-specific function](#mapbox-specific-functions) that takes an
`NSDictionary` as an argument:

```objc
[NSExpression expressionWithFormat:@"FUNCTION($floorCount + 1, 'mgl_expressionWithContext:', %@)",
 {@"floorCount": @2}];
```

```swift
NSExpression(format: "FUNCTION($floorCount + 1, 'mgl_expressionWithContext:', %@)",
             ["floorCount": 2])
```
