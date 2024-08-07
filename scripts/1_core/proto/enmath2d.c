/**
 * \defgroup Math2D Math2D library
 * @{
 */

//-----------------------------------------------------------------------------

//! exposed from C++ (do not change)
enum WindingOrder
{
	CounterClockwise,	// 0
	Clockwise,			// 1
	Invalid				// 2
}


class Math2D
{
	private void Math2D();
	private void ~Math2D();

	/*!
	Check if a `point` is inside `polygon`.
	Takes polygon and point only in the XZ plane.
	*/
	static proto bool IsPointInPolygonXZ(notnull array<vector> polygon, vector point);
	//! Check if `point` is inside triangle specified by points `p1`, `p2` and `p3`.
	static proto bool IsPointInTriangleXZ(vector p1, vector p2, vector p3, vector point);
	/*!
	Check if a point is inside `polygon`.
	\param polygon Must be an array of Vector2, every two consecutive floats represent one point in polygon.
	\param x X coordinate of point to test
	\param y Y coordinate of point to test
	*/
	static proto bool IsPointInPolygon(notnull array<float> polygon, float x, float y);
	/*!
	Determines winding order of triangle given by points `a`, `b`, `c`. If the triangles
	form a line, WindingOrder.Invalid is returned.
	*/
	static proto WindingOrder TriangleWindingXZ(vector a, vector b, vector c);
}

//@}
