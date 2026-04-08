/*!
\addtogroup Physics
\{
*/

/*!
Slider joint simulation
*/
typedef int[] PhysicsSliderJoint;
class PhysicsSliderJoint: PhysicsJoint
{
	/*!
	Sets linear limits of the joint.
	\param lowerLimit Value of the lower linear limit
	\param upperLimit value of the upper linear limit
	*/
	proto external void SetLinearLimits(float lowerLimit, float upperLimit);
	//! Returns value of the linear position
	proto external float GetLinearPos();
	//! Returns value of the angular position
	proto external float GetAngularPos();
	/*!
	Sets angular limits of the joint.
	\param lowerLimit Value of the lower angular limit
	\param upperLimit Value of the upper angular limit
	*/
	proto external void SetAngularLimits(float lowerLimit, float upperLimit);
	/*!
	Sets parameters relevant for moving inside linear limits
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetDirLinear(float softness, float restitution, float damping);
	/*!
	Sets parameters relevant for moving inside angular limits
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetDirAngular(float softness, float restitution, float damping);
	/*!
	Sets parameters relevant for hitting linear limit
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetLimLinear(float softness, float restitution, float damping);
	/*!
	Sets parameters relevant for hitting angular limit
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetLimAngular(float softness, float restitution, float damping);
	/*!
	Sets parameters relevant for case (against constraint axis)
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetOrthoLinear(float softness, float restitution, float damping);
	/*!
	Sets parameters relevant for case (against constraint axis)
	\param softness
	\param restitution
	\param damping
	*/
	proto external void SetOrthoAngular(float softness, float restitution, float damping);
	/*!
	Allows motor to move slider at specific linear velocity
	\param velocity Target linear velocity motor tries to maintain
	\param force Maximum linear force motor can overcome. 0 to turn motor off.
	*/
	proto external void SetLinearMotor(float velocity, float force);
	/*!
	Allows motor to move slider at specific angular velocity
	\param velocity Target angular velocity motor tries to maintain
	\param force Maximum angular force motor can overcome. 0 to turn motor off.
	*/
	proto external void SetAngularMotor(float velocity, float force);
}

/*!
\}
*/
