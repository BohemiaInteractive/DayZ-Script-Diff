/*!
\addtogroup Physics
\{
*/

/*!
Wrapper over joint simulation

\anchor jointParameters Joint parameters
- softness
	- 0->1, recommend ~0.8->1
	- describes % of limits where movement is free
	- beyond this softness %, the limit is gradually enforced until the "hard" (1.0) limit is reached
- biasFactor
	- 0->1?, recommend 0.3 +/-0.3 or so
	- strength with which constraint resists zeroth order (angular, not angular velocity) limit violation
- relaxationFactor
	- 0->1, recommend to stay near 1
	- the lower the value, the less the constraint will fight velocities which violate the angular limits
*/
typedef int[] PhysicsJoint;
class PhysicsJoint
{
	/*!
	Creates hinge joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		PhysicsJoint.CreateHinge(entity1, entity2, vector.Right, vector.Up, -vector.Right, vector.Up, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param point1 Joint position in reference frame of ent1
	\param axis1 Joint axis in reference frame of ent1
	\param point2 Joint position in reference frame of ent2
	\param axis2 Joint axis in reference frame of ent2
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsHingeJoint CreateHinge(notnull IEntity ent1, IEntity ent2, vector point1, vector axis1, vector point2, vector axis2, bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates hinge joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		vector mat1[4] = { vector.Right, -vector.Forward, vector.Up, vector.Right };
		vector mat2[4] = { vector.Right, -vector.Forward, vector.Up, -vector.Right };
		PhysicsJoint.CreateHinge2(entity1, entity2, mat1, mat2, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param matrix1 Joint transformation in reference frame of ent1 (hinge is z-axis)
	\param matrix2 Joint transformation in reference frame of ent2 (hinge is z-axis)
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsHingeJoint CreateHinge2(notnull IEntity ent1, IEntity ent2, vector matrix1[4], vector matrix2[4], bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates ball socket joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		PhysicsJoint.CreateBallSocket(entity1, entity2, vector.Right, -vector.Right, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param point1 Joint position in reference frame of ent1
	\param point2 Joint position in reference frame of ent2
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsBallSocketJoint CreateBallSocket(notnull IEntity ent1, IEntity ent2, vector point1, vector point2, bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates fixed joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		PhysicsJoint.CreateFixed(entity1, entity2, vector.Right, -vector.Right, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param point1 Joint position in reference frame of ent1
	\param point2 Joint position in reference frame of ent2
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsFixedJoint CreateFixed(notnull IEntity ent1, IEntity ent2, vector point1, vector point2, bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates cone twist joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		vector mat1[4] = { vector.Right, vector.Up, vector.Forward, vector.Right };
		vector mat2[4] = { vector.Right, vector.Up, vector.Forward, -vector.Right };
		PhysicsJoint.CreateConeTwist(entity1, entity2, mat1, mat2, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param matrix1 Joint transformation in reference frame of ent1 (twist is x-axis, swings are z and y axes)
	\param matrix2 Joint transformation in reference frame of ent2 (twist is x-axis, swings are z and y axes)
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsConeTwistJoint CreateConeTwist(notnull IEntity ent1, IEntity ent2, vector matrix1[4], vector matrix2[4], bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates slider joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		vector mat1[4] = { vector.Right, vector.Up, vector.Forward, vector.Right };
		vector mat2[4] = { vector.Right, vector.Up, vector.Forward, -vector.Right };
		PhysicsJoint.CreateSlider(entity1, entity2, mat1, mat2, false);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param matrix1 Joint transformation in reference frame of ent1 (slide is x-axis)
	\param matrix2 Joint transformation in reference frame of ent2 (slide is x-axis)
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto PhysicsSliderJoint CreateSlider(notnull IEntity ent1, IEntity ent2, vector matrix1[4], vector matrix2[4], bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates 6 DOF joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		vector mat1[4] = { vector.Right, vector.Up, vector.Forward, vector.Right };
		vector mat2[4] = { vector.Right, vector.Up, vector.Forward, -vector.Right };
		Physics6DOFJoint joint = PhysicsJoint.Create6DOF(entity1, entity2, mat1, mat2, false);
		joint.SetLimit(0, -1, 1);
		joint.SetAngularLimits(vector.Zero, vector.Zero);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param matrix1 Joint transformation in reference frame of ent1
	\param matrix2 Joint transformation in reference frame of ent2
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto Physics6DOFJoint Create6DOF(notnull IEntity ent1, IEntity ent2, vector matrix1[4], vector matrix2[4], bool disableCollisions, float breakThreshold = -1);
	/*!
	Creates DOF spring joint. Entities must be dynamic objects. Using only ent1 anchors it to the world.
	\code
		IEntity entity1 = GetWorld().FindEntityByName("entity1");
		IEntity entity2 = GetWorld().FindEntityByName("entity1");
		vector mat1[4] = { vector.Right, vector.Up, vector.Forward, vector.Right };
		vector mat2[4] = { vector.Right, vector.Up, vector.Forward, -vector.Right };
		Physics6DOFSpringJoint joint = PhysicsJoint.Create6DOF(entity1, entity2, mat1, mat2, false);
		joint.SetLimit(0, -1, 1);
		joint.SetAngularLimits(vector.Zero, vector.Zero);
		joint.SetSpring(0, 1, 1);
	\endcode
	\param ent1 Entity that will be part of the joint, mandatory
	\param ent2 Other entity that will be part of the joint, optional
	\param matrix1 Joint transformation in reference frame of ent1
	\param matrix2 Joint transformation in reference frame of ent2
	\param disableCollisions True to disable collisions between connected entities
	\param breakThreshold Maximum impulse to break the joint. Use -1 for unbreakable joint
	*/
	static proto Physics6DOFSpringJoint Create6DOFSpring(notnull IEntity ent1, IEntity ent2, vector matrix1[4], vector matrix2[4], bool disableCollisions, float breakThreshold = -1);
	//! Destroys joint
	proto external void Destroy();
}

/*!
\}
*/
