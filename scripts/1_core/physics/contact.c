/*!
\addtogroup Physics
\{
*/

/*!
Output structure for reporting collisions.
*/
sealed class Contact
{
	private void Contact() {}
	private void ~Contact() {}

	Physics Physics1;
	Physics Physics2;
	//! Surface properties of Object1
	SurfaceProperties Material1; 
	//! Surface properties of Object2
	SurfaceProperties Material2;
	//! Impulse applied to resolve the collision
	float Impulse;
	//! Index of collider on Object1
	int ShapeIndex1;
	//! Index of collider on Object2
	int ShapeIndex2;
	//! Collision axis at the contact point
	vector	Normal;
	//! Position of the contact point (world space)
	vector	Position;
	//! Penetration depth on Object1
	float PenetrationDepth;

	float		RelativeNormalVelocityBefore;
	float		RelativeNormalVelocityAfter;
	vector		RelativeVelocityBefore;
	vector		RelativeVelocityAfter;
	
	//! Velocity of Object1 before collision (world space)
	vector	VelocityBefore1;
	//! Velocity of Object2 before collision (world space)
	vector	VelocityBefore2;
	//! Velocity of Object1 after collision (world space)
	vector	VelocityAfter1;
	//! Velocity of Object2 after collision (world space)
	vector	VelocityAfter2;

	proto native vector GetNormalImpulse();
	proto native float GetRelativeVelocityBefore(vector vel);
	proto native float GetRelativeVelocityAfter(vector vel);
}

/*!
\}
*/
