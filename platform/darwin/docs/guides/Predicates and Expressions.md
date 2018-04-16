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

The `key` may be cast explicitly into the key's type. Example: `CAST(key, 'NSNumber')` or
`CAST(key, 'NSString')`

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

To test whether a feature has or lacks a specific attribute, compare the
attribute to `NULL` or `NIL`. Predicates created using the
`+[NSPredicate predicateWithValue:]` method are also supported. String
operators and custom operators are not supported.

For details about the predicate format string syntax, consult the “Predicate
Format String Syntax” chapter of the
_[Predicate Programming Guide](https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Predicates/AdditionalChapters/Introduction.html)_
in Apple developer documentation.

Either the predicate’s left-hand or right-hand expression can be an `NSString` 
(to match strings) or `NSNumber` (to match numbers, including Boolean values) 
or an array of `NSString`s or `NSNumber`s, depending on the operator and the 
type of values expected for the attribute being tested  or, alternatively, 
one of the following special attributes:

<table>
<thead>
<tr><th>Attribute</th><th>Meaning</th></tr>
</thead>
<tbody>
<tr>
   <td><code>$featureIdentifier</code></td>
   <td>
       A value that uniquely identifies the feature in the containing source.
       For details on the types of values that may be associated with this key,
       consult the documentation for the <code>MGLFeature</code> protocol’s
       <code>identifier</code> property.
   </td>
</tr>
<tr>
   <td><code>$geometryType</code></td>
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

For floating-point values, use `-[NSNumber numberWithDouble:]` instead of 
`-[NSNumber numberWithFloat:]` to avoid precision issues.

Automatic type casting is not performed. Therefore, a feature only matches this
predicate if its value for the attribute in question is of the same type as the
value specified in the predicate. Also, operator modifiers such as `c` (for
case insensitivity), `d` (for diacritic insensitivity), and `l` (for locale
sensitivity) are unsupported for comparison and aggregate operators that are
used in the predicate.

It is possible to create expressions that contain special characters in the
predicate format syntax. This includes the `$` in the `$featureIdentifier` and 
`$geometryType` special style attributes and also `hyphen-minus` and `tag:subtag`. 
However, you must use `%@` in the format string to represent these variables:
`@"$geometryType == 'LineString'"`.

## Using expressions to configure layout and paint attributes

### Key paths

A key path expression refers to an attribute of the `MGLFeature` object being
evaluated for display. For example, if a polygon’s `MGLFeature.attributes`
dictionary contains the `floorCount` key, then the key path `floorCount` refers
to the value of the `floorCount` attribute when evaluating that particular
polygon.

### Predefined functions

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
`length:`             | `length('Wapakoneta')`
`castObject:toType:`  | `CAST(ele, 'NSString')`<br>`CAST(ele, 'NSNumber')`

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

### Mapbox-specific functions

For compatibility with the Mapbox Style Specification, the following functions
are defined by this SDK. When setting a style layer property, you can call these
functions just like the predefined functions above, using either the
`+[NSExpression expressionForFunction:arguments:]` method or a convenient format
string syntax:

Initializer parameter | Format string syntax | Description
----------------------|----------------------|------------
`mgl_does:have:` | `mgl_does:have:(SELF, 'key')` or `mgl_does:have:(%@, 'key')` | Returns a Boolean value indicating whether the dictionary has a value for the key or whether the evaluated object (`SELF`) has a value for the feature attribute. Compared to the `mgl_has:` custom function, that function’s target is instead passed in as the first argument to this function. Both functions are equivalent to the syntax `key != NIL` or `%@[key] != NIL` but can be used outside of a predicate.
`mgl_interpolate:withCurveType:parameters:stops:` | `mgl_interpolate:withCurveType:parameters:stops:(x, 'linear', nil, %@)` | Produces continuous, smooth results by interpolating between pairs of input and output values (“stops”). Compared to the `mgl_interpolateWithCurveType:parameters:stops:` custom function, the input expression (that function’s target) is instead passed in as the first argument to this function.
`mgl_step:from:stops:` | `mgl_step:from:stops:(x, 11, %@)` | Produces discrete, stepped results by evaluating a piecewise-constant function defined by pairs of input and output values ("stops"). Compared to the `mgl_stepWithMinimum:stops:` custom function, the input expression (that function’s target) is instead passed in as the first argument to this function.
`mgl_join:` | `mgl_join({'Old', 'MacDonald'})` | Returns the result of concatenating together all the elements of an array in order. Compared to the `stringByAppendingString:` custom function, this function takes only one argument, which is an aggregate expression containing the strings to concatenate.
`mgl_round:` | `mgl_round(1.5)` | Returns the number rounded to the nearest integer. If the number is halfway between two integers, this function rounds it away from zero.
`mgl_coalesce:` | `mgl_coalesce({x, y, z})` | Returns the first non-`nil` value from an array of expressions.
`MGL_LET` | `MGL_LET('age', uppercase('old'), 'name', uppercase('MacDonald'), mgl_join({$age, $name}))` | Any number of variable names interspersed with their assigned `NSExpression` values, followed by an `NSExpression` that may contain references to those variables. Compared to the `mgl_expressionWithContext:` custom function, this function takes the variable names and values inline before the expression that contains references to those variables.
`MGL_MATCH` | `MGL_MATCH(x, 0, 'zero match', 1, 'one match', 'two match', 'default')` | Evaluates the first expression and returns the value that matches the initial condition. After the first expression condition a pair of matching/return value should be added and a default value.
`MGL_IF` | `MGL_IF(1 = 2, YES, 2 = 2, YES, NO)` | Returns the first value that meets the condition otherwise a default value. The expression conditions should be added in pairs of conditional/return value. Unlike `+[NSExpression expressionForConditional:trueExpression:falseExpression:]` or the `TERNARY()` syntax, this function can accept multiple “if else” conditions and is supported on iOS 8._x_ and macOS 10.10._x_; however, each conditional passed into this function must be wrapped in a constant expression.

The following custom functions are also available with the
`+[NSExpression expressionForFunction:selectorName:arguments:]` method or the
`FUNCTION()` format string syntax:

<table>
<thead>
<tr><th>Selector name</th><th>Target</th><th>Arguments</th><th>Returns</th></tr>
</thead>
<tbody>
<tr>
   <td><code>boolValue</code></td>
   <td>
      An <code>NSExpression</code> that evaluates to a number or string.
   </td>
   <td></td>
   <td>
      A Boolean representation of the target: <code>FALSE</code> when then input is an
      empty string, 0, <code>FALSE</code>, <code>NIL</code>, or <code>NaN</code>, otherwise <code>TRUE</code>.
   </td>
</tr>
<tr>
   <td><code>mgl_has:</code></td>
   <td>
      An <code>NSExpression</code> that evaluates to an <code>NSDictionary</code> or the evaluated object (<code>SELF</code>).
   </td>
   <td>
      An <code>NSExpression</code> that evaluates to an <code>NSString</code> representing the key to look up in the dictionary or the feature attribute to look up in the evaluated object (see <code>MGLFeature.attributes</code>).
   </td>
   <td>
      <code>true</code> if the dictionary has a value for the key or if the evaluated object has a value for the feature attribute.
   </td>
<tr>
   <td><code>mgl_expressionWithContext:</code></td>
   <td>
      An <code>NSExpression</code> that may contain references to the variables defined in
      the context dictionary.
   </td>
   <td>
      An <code>NSDictionary</code> with <code>NSString</code>s as keys and <code>NSExpression</code>s as values.
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
      An <code>NSExpression</code> that evaluates to a number and contains a variable or
      key path expression.
   </td>
   <td>
      The first argument is one of the following strings denoting curve types:
      <code>linear</code>, <code>exponential</code>, or <code>cubic-bezier</code>.
      
      The second argument is an expression providing parameters for the curve:
      
      <ul>
         <li>If the curve type is <code>linear</code>, the argument is <code>NIL</code>.</li>
         <li>
            If the curve type is <code>exponential</code>, the argument is an expression
            that evaluates to a number, specifying the base of the exponential
            interpolation.
         </li>
         <li>
            If the curve type is <code>cubic-bezier</code>, the argument is an array or
            aggregate expression containing four expressions, each evaluating to
            a number. The four numbers are control points for the cubic Bézier
            curve.
         </li>
      </ul>
      
      The third argument is an <code>NSDictionary</code> object representing the
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
      An <code>NSExpression</code> that evaluates to a Boolean value, number, or string.
   </td>
   <td>
      Zero or more <code>NSExpression</code>s, each evaluating to a Boolean value or
      string.
   </td>
   <td>
      A numeric representation of the target:
      <ul>
         <li>If the target is <code>NIL</code> or <code>FALSE</code>, the result is 0.</li>
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
      An <code>NSExpression</code> that evaluates to a number and contains a variable or
      key path expression.
   </td>
   <td>
      The first argument is an expression that evaluates to a number, specifying
      the minimum value in case the target is less than any of the stops in the
      second argument.
      
      The second argument is an <code>NSDictionary</code> object representing the
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
      An <code>NSExpression</code> that evaluates to a string.
   </td>
   <td>
      One or more <code>NSExpression</code>s, each evaluating to a string.
   </td>
   <td>
      The target string with each of the argument strings appended in order.
   </td>
</tr>
<tr>
   <td><code>stringValue</code></td>
   <td>
      An <code>NSExpression</code> that evaluates to a Boolean value, number, or string.
   </td>
   <td></td>
   <td>
      A string representation of the target:
      <ul>
         <li>If the target is <code>NIL</code>, the result is the string <code>null</code>.</li>
         <li>
            If the target is a Boolean value, the result is the string <code>true</code> or
            <code>false</code>.
         </li>
         <li>
            If the target is a number, it is converted to a string as specified
            by the
            “<a href="https://tc39.github.io/ecma262/#sec-tostring-applied-to-the-number-type">NumberToString</a>”
            algorithm of the ECMAScript Language Specification.
         </li>
         <li>
            If the target is a color, it is converted to a string of the form
            <code>rgba(r,g,b,a)</code>, where <var>r</var>, <var>g</var>, and <var>b</var>
            are numerals ranging from 0 to 255 and <var>a</var> ranges from 0 to
            1.
         </li>
         <li>
            Otherwise, the target is converted to a string in the format
            specified by the
            <a href="https://tc39.github.io/ecma262/#sec-json.stringify"><code>JSON.stringify()</code></a>
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

The Mapbox Style Specification defines some operators for which no custom
function is available. To use these operators in an `NSExpression`, call the
`MGL_FUNCTION()` function with the same arguments that the operator expects.

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
      variable can only be used with the <code>heatmapColor</code> property.
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
[NSExpression expressionWithFormat:@"MGL_LET('floorCount', 2, $floorCount + 1)"];
```

```swift
NSExpression(format: "MGL_LET(floorCount, 2, $floorCount + 1)")
```
