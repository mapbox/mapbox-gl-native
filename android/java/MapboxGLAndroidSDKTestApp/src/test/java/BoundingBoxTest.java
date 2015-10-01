import com.mapbox.mapboxgl.geometry.BoundingBox;
import com.mapbox.mapboxgl.geometry.CoordinateSpan;
import com.mapbox.mapboxgl.geometry.LatLng;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

public class BoundingBoxTest {

    private BoundingBox mBoundingBox;

    @Before
    public void setup() {
        mBoundingBox = new BoundingBox(20.0, 10.0, 0.0, 0.0);
    }

    @Test
    public void testSanity() {
        BoundingBox boundingBox = new BoundingBox(0.0, 0.0, 0.0, 0.0);
        assertNotNull("boundingBox should not be null", boundingBox);
    }

    @Test
    public void testDefaultConstructor() {
        mBoundingBox = new BoundingBox(0, 0, 0, 0);
        assertEquals(mBoundingBox, new BoundingBox());
    }

    @Test
    public void testEqualityConstructors() {
        BoundingBox cornerBoundingBox = new BoundingBox(new LatLng(20.0, 10.0), new LatLng(0.0, 0.0));
        BoundingBox cloneBoundingBox = new BoundingBox(mBoundingBox);
        assertEquals("boundingBoxes should match", mBoundingBox, cornerBoundingBox);
        assertEquals("boundingBoxes should match", mBoundingBox, cloneBoundingBox);
    }

    @Test
    public void testValidBox() {
        assertTrue(mBoundingBox.isValid());
    }

    @Test
    public void testInvalidLatitudeSpanBox() {
        mBoundingBox = new BoundingBox(0.0, 10.0, 10.0, 0.0);
        assertFalse(mBoundingBox.isValid());
    }

    @Test
    public void testInvalidLongitudeSpanBox() {
        mBoundingBox = new BoundingBox(10.0, 0.0, 0.0, 20.0);
        assertFalse(mBoundingBox.isValid());
    }

    @Test
    public void testCenterCoordinate() {
        LatLng center = new LatLng(10.0, 5.0);
        assertEquals("boundingBox center should match", center, mBoundingBox.getCenter());
    }

    @Test
    public void testUnionSameBox() {
        assertEquals(mBoundingBox, mBoundingBox.union(mBoundingBox));
    }

    @Test
    public void testUnionInnerBox() {
        BoundingBox innerBox = new BoundingBox(2.0, 2.0, 1.0, 1.0);
        assertEquals(mBoundingBox.union(innerBox), mBoundingBox);
    }

    @Test
    public void testUnionOverlapBox() {
        BoundingBox resultBox = new BoundingBox(22.0, 12.0, 0.0, 0.0);
        BoundingBox overlapBox = new BoundingBox(22.0, 12.0, 1.0, 1.0);
        assertEquals(resultBox, mBoundingBox.union(overlapBox));
    }

    @Test
    public void testUnionOuterBox() {
        BoundingBox resultBox = new BoundingBox(40.0, 40.0, 0.0, 0.0);
        BoundingBox outerBox = new BoundingBox(40.0, 40.0, 0.0, 0.0);
        assertEquals(resultBox, mBoundingBox.union(outerBox));
    }

    @Test
    public void testIntersectSameBox() {
        assertEquals(mBoundingBox, mBoundingBox.intersect(mBoundingBox));
    }

    @Test
    public void testIntersectInnerBox() {
        BoundingBox innerBox = new BoundingBox(2.0, 2.0, 1.0, 1.0);
        assertEquals(innerBox, mBoundingBox.intersect(innerBox));
    }

    @Test
    public void testIntersectOuterBox() {
        BoundingBox outerBox = new BoundingBox(40.0, 40.0, 0.0, 0.0);
        assertEquals(mBoundingBox, mBoundingBox.intersect(outerBox));
    }

    @Test
    public void testIntersectOverlapBox() {
        BoundingBox resultBox = new BoundingBox(20.0, 10.0, 1.0, 1.0);
        BoundingBox overlapBox = new BoundingBox(22.0, 12.0, 1.0, 1.0);
        assertEquals(resultBox, mBoundingBox.intersect(overlapBox));
    }

    @Test
    public void testIntersectNoneBox() {
        BoundingBox outsideBox = new BoundingBox(100.0, 100.0, 50.0, 50.0);
        assertNull(mBoundingBox.intersect(outsideBox));
    }

    @Test
    public void testContainsCoordinate() {
        LatLng coordinate = new LatLng(1.0, 1.0);
        assertTrue(mBoundingBox.contains(coordinate));
    }

    @Test
    public void testContainsCoordinateOnBoundary() {
        LatLng coordinate = new LatLng(20.0, 9.0);
        assertFalse(mBoundingBox.contains(coordinate));
    }

    @Test
    public void testContainsNoCoordinate() {
        LatLng coordinate = new LatLng(100.0, 100.0);
        assertFalse(mBoundingBox.contains(coordinate));
    }

    @Test
    public void testSpan() {
        CoordinateSpan span = new CoordinateSpan(20.0, 10.0);
        assertTrue(mBoundingBox.getSpan().equals(span));
    }

    @Test
    public void testLatitudeSpan() {
        double height = 20.0;
        assertTrue(mBoundingBox.getLatitudeSpan() == height);
    }

    @Test
    public void testLongitudeSpan() {
        double width = 10.0;
        assertTrue(mBoundingBox.getLongitudeSpan() == width);
    }

    @Test
    public void testEmptySpanEmptyNotEmptyBox(){
        assertFalse(mBoundingBox.isEmpty());
    }

    @Test
    public void testEmptySpanEmptyBox() {
        mBoundingBox = new BoundingBox();
        assertTrue(mBoundingBox.isEmpty());
    }

    @Test
    public void testEmptySpanEmptyLatitude() {
        mBoundingBox = new BoundingBox(1.0, 2.0, 0.0, 2.0);
        assertTrue(mBoundingBox.isEmpty());
    }

    @Test
    public void testEmptySpanEmptyLongitude(){
        mBoundingBox = new BoundingBox(0.0, 3.0, 0.0, 1.0);
        assertTrue(mBoundingBox.isEmpty());
    }

}
