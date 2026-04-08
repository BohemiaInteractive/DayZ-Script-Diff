/*!
\addtogroup Physics
\{
*/

/*!
Cone twist joint simulation
*/
typedef int[] PhysicsConeTwistJoint;
class PhysicsConeTwistJoint: PhysicsJoint
{
	/*!
	Sets span values of limits. Span represents range from -limit to limit.
	\param limitIndex Index of the limit (3 - twist, 4 - swing2, 5 - swing1)
	\param limitValue Value of the limit
	*/
	proto external void SetLimit(int limitIndex, float limitValue);
	/*!
	Sets various joint parameters. Span value represents range from -limit to limit.
	\param swingSpan1 Value of the swing1 limit (along z-axis)
	\param swingSpan2 Value of the swing2 limit (along y-axis)
	\param twistSpan Value of the twist limit (along x-axis)
	\param softness Softness of the joint. See \ref jointParameters "details"
	\param biasFactor Bias factor of the joint. See \ref jointParameters "details"
	\param relaxationFactor Relaxation factor of the joint. See \ref jointParameters "details"
	*/
	proto external void SetLimits(float swingSpan1, float swingSpan2, float twistSpan, float softness, float biasFactor, float relaxationFactor);
}

/*!
\}
*/
