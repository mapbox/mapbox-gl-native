package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.PointF;

public class PointFProjection {

    /**
     * Returns distance to segment
     *
     * @param ss
     *            segment start point
     * @param se
     *            segment end point
     * @param p
     *            pofloat to found closest pofloat on segment
     * @return distance to segment
     */
    public static double getDistanceToSegment(PointF ss, PointF se, PointF p)
    {
        return getDistanceToSegment(ss.x, ss.y, se.x, se.y, p.x, p.y);
    }

    /**
     * Returns distance to segment
     *
     * @param sx1
     *            segment x coord 1
     * @param sy1
     *            segment y coord 1
     * @param sx2
     *            segment x coord 2
     * @param sy2
     *            segment y coord 2
     * @param px
     *            pofloat x coord
     * @param py
     *            pofloat y coord
     * @return distance to segment
     */
    public static double getDistanceToSegment(float sx1, float sy1, float sx2, float sy2, float px, float py)
    {
        PointF closestPointF = getClosestPointFOnSegment(sx1, sy1, sx2, sy2, px, py);
        return getDistance(closestPointF.x, closestPointF.y, px, py);
    }
    /**
     * Returns distance between two 2D points
     *
     * @param point1
     *            first point
     * @param point2
     *            second point
     * @return distance between points
     */
    public static double getDistance(PointF point1, PointF point2)
    {
        return getDistance(point1.x, point1.y, point2.x, point2.y);
    }


    /**
     * Returns distance between two sets of coords
     *
     * @param x1
     *            first x coord
     * @param y1
     *            first y coord
     * @param x2
     *            second x coord
     * @param y2
     *            second y coord
     * @return distance between sets of coords
     */
    public static double getDistance(float x1, float y1, float x2, float y2)
    {
        // using long to avoid possible overflows when multiplying
        double dx = x2 - x1;
        double dy = y2 - y1;

        // return Math.hypot(x2 - x1, y2 - y1); // Extremely slow
        // return Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2)); // 20 times faster than hypot
        return Math.sqrt(dx * dx + dy * dy); // 10 times faster then previous line
    }

    /**
     * Returns closest pofloat on segment to point
     *
     * @param ss
     *            segment start point
     * @param se
     *            segment end point
     * @param p
     *            pofloat to found closest pofloat on segment
     * @return closest pofloat on segment to p
     */
    public static PointF getClosestPointFOnSegment(PointF ss, PointF se, PointF p)
    {
        return getClosestPointFOnSegment(ss.x, ss.y, se.x, se.y, p.x, p.y);
    }

    /**
     * Returns closest pofloat on segment to point
     *
     * @param sx1
     *            segment x coord 1
     * @param sy1
     *            segment y coord 1
     * @param sx2
     *            segment x coord 2
     * @param sy2
     *            segment y coord 2
     * @param px
     *            pofloat x coord
     * @param py
     *            pofloat y coord
     * @return closets pofloat on segment to point
     */
    public static PointF getClosestPointFOnSegment(float sx1, float sy1, float sx2, float sy2, float px, float py)
    {
        double xDelta = sx2 - sx1;
        double yDelta = sy2 - sy1;

        if ((xDelta == 0) && (yDelta == 0))
        {
            throw new IllegalArgumentException("Segment start equals segment end");
        }

        double u = ((px - sx1) * xDelta + (py - sy1) * yDelta) / (xDelta * xDelta + yDelta * yDelta);

        final PointF closestPointF;
        if (u < 0)
        {
            closestPointF = new PointF(sx1, sy1);
        }
        else if (u > 1)
        {
            closestPointF = new PointF(sx2, sy2);
        }
        else
        {
            closestPointF = new PointF((int) Math.round(sx1 + u * xDelta), (int) Math.round(sy1 + u * yDelta));
        }

        return closestPointF;
    }
}
