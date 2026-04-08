/*!
\addtogroup Physics
\{
*/

/*!
Hinge joint simulation
*/
typedef int[] PhysicsHingeJoint;
class PhysicsHingeJoint: PhysicsJoint
{
	/*!
	Sets various joint parameters
	\param low Low limit of the joint
	\param high High limit of the joint
	\param softness Softness of the joint. See \ref jointParameters "details"
	\param biasFactor Bias factor of the joint. See \ref jointParameters "details"
	\param relaxationFactor Relaxation factor of the joint. See \ref jointParameters "details"
	*/
	proto external void SetLimits(float low, float high, float softness, float biasFactor, float relaxationFactor);
	/*!
	Allows motor to move hinge to specific angle. Must be called continuously.
	\param angle Target angle motor tries to reach
	\param dt Time fraction used in update
	\param maxImpulse Maximum impulse motor can overcome
	*/
	proto external void SetMotorTargetAngle(float angle, float dt, float maxImpulse);
	/*!
	Allows motor to move hinge at specific velocity
	\param enable True to enable motor
	\param targetVelocity Target angular velocity motor tries to maintain
	\param maxImpulse Maximum impulse motor can overcome
	*/
	proto external void SetMotor(bool enable, float targetVelocity, float maxImpulse);
	/*!
	Sets new axis of the hinge joint. ent1 refers to one in PhysicsJoint.CreateHinge.
	\param axis New joint axis in reference frame of ent1
	*/
	proto external void SetAxis(vector axis);
}

/*!
\}
*/
