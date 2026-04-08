/*!
\addtogroup Physics
\{
*/

/*!
6 DOF spring joint simulation.
*/
typedef int[] Physics6DOFSpringJoint;
class Physics6DOFSpringJoint: Physics6DOFJoint
{
	/*!
	Sets spring parameters on a particular axis.
	When stiffness == -1 && damping == -1, spring is disabled.
	\param axis Axis to be modified
	\param stiffness Value of the spring stiffness (Hooke's Law)
	\param damping Value of the spring damping. Between 0 and 1 (1 == no damping)
	*/
	proto external void SetSpring(int axis, float stiffness, float damping);
}

/*!
\}
*/
