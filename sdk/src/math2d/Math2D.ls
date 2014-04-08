package math2d
{
    import loom2d.math.Point;

    [Native(managed)]
    final public native class LineSegment
    {
        public native var x1:Number;
        public native var y1:Number;
        public native var x2:Number;
        public native var y2:Number;
        public native var dx:Number;
        public native var dy:Number;
        public native var angle:Number;

        public native function LineSegment(newX1:Number=0, newY1:Number=0, newX2:Number=0, newY2:Number=0);

        public native function setP1(x:Number, y:Number);

        public native function setP2(x:Number, y:Number);

        public native function rotateBy(thetaRadians:Number);

        public function toString():String
        {
            return "Segment [" + x1 + ", " + y1 + "] to [" + x2 + ", " + y2 + "]";
        }
    }

    public class Polygon
    {
        public var segments:Vector.<LineSegment> = new Vector.<LineSegment>();
        public var endpoints:Vector.<Point> = null;

        public function Polygon(points:Vector.<Point>)
        {
            endpoints = points;

            var nextIndex:int = 0;
            for (var i:int = 0; i < points.length; ++i)
            {
                nextIndex = (i + 1) % points.length;
                var newSegment = new LineSegment(points[i].x, points[i].y,
                                                 points[nextIndex].x, points[nextIndex].y);
                segments.pushSingle(newSegment);
            }
        }
    }

    [Native(managed)]
    public native class RaycastResult
    {
        public native var hit:Boolean;
        public native var hitIndex:int;
        public native var hitX:Number;
        public native var hitY:Number;
        public native var t:Number;

        public native function RaycastResult();
    }

    final public native class Math2D
    {
        public static native function dotProduct(x1:Number, y1:Number, x2:Number, y2:Number):Number;

        public static native function rotateVector(x:Number, y:Number, thetaRadians:Number):void;

        // Return: A value of t to plug into the formula:
        // intersectionX = rayOrigin.x + (rayDirection.x * t)
        // intersectionY = rayOrigin.y + (rayDirection.y * t)
        //
        // Returns -1 if there is no intersection
        public static native function intersectRayWithSegment(ray:LineSegment, seg:LineSegment):Number;

        // Determine the closest intersection between the given ray and the segments
        // that were passed in.
        //
        // The return struct will include
        // hit: whether or not the ray hit something
        // hitIndex: the index into the vector for the segment that was hit
        // hitX, hitY: the point where the ray hit the segment
        // t: the T value of the ray where the closest intersection occurred
        public static native function raycastToSegments(ray:LineSegment,
                                                        outStruct:RaycastResult,
                                                        segments:Vector.<LineSegment>):void;
    }
}