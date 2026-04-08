/*!
\addtogroup Physics
\{
*/

/*!
6 DOF joint simulation.

The first 3 DOF axes represent linear motion (translation) and the latter 3 DOF axes represent angular motion.
Each axis can be configured to be either free, locked or limited. All axes are locked initially. Some combinations
can lead to an undefined behavior.

Configuration of the axes:
- upper < lower - free axis
- upper == lower - locked axis
- upper > lower - limited axis

Ranges of the angular limits:
- x axis - (-PI, PI)
- y axis - (-PI/2, PI/2)
- z axis - (-PI, PI)
*/
typedef int[] Physics6DOFJoint;
class Physics6DOFJoint: PhysicsJoint
{
	/*!
	Sets linear limits of the joint.
	\param linearLower Value of the lower linear limit
	\param linearUpper Value of the upper linear limit
	*/
	proto external void SetLinearLimits(vector linearLower, vector linearUpper);
	/*!
	Sets angular limits of the joint.
	\param angularLower Value of the lower angular limit
	\param angularUpper Value of the upper angular limit
	*/
	proto external void SetAngularLimits(vector angularLower, vector angularUpper);
	/*!
	Sets limits of the joint.
	\param axis Axis to be modified. First 3 are linear, next 3 are angular
	\param limitLower Value of the lower limit
	\param limitUpper Value of the upper limit
	*/
	proto external void SetLimit(int axis, float limitLower, float limitUpper);
}

/*!
\}
*/
