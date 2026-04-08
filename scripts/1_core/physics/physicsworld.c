/*!
\addtogroup Physics
\{
*/

/*!
Physical simulation of the world
*/
sealed class PhysicsWorld
{
	private void PhysicsWorld();
	private void ~PhysicsWorld();

	/*!
	Modifies interaction matrix of interaction layers
	\param worldEntity Entity used to obtain world in which entities exist
	\param mask1 Bit mask of layers
	\param mask2 Other bit mask of layers
	\param enable True to enable, false to disable interaction
	*/
	static proto void SetInteractionLayer(notnull IEntity worldEntity, int mask1, int mask2, bool enable);
	/*!
	Returns global gravity
	\param worldEntity Entity used to obtain world in which entities exist
	*/
	static proto vector GetGravity(notnull IEntity worldEntity);
	/*!
	Changes global gravity.
	\param worldEntity Entity used to obtain world in which entities exist
	\param g Global gravity vector
	*/
	static proto void SetGravity(notnull IEntity worldEntity, vector g);
	/*!
	Returns current fixed time step of the physics simulation
	\param worldEntity Entity used to obtain world in which entities exist
	*/
	static proto float GetTimeSlice(notnull IEntity worldEntity);
	/*!
	Returns current update rate of the physics simulation
	\param worldEntity Entity used to obtain world in which entities exist
	*/
	static proto int GetUpdateRate(notnull IEntity worldEntity);
	/*!
	Sets update rate to change fixed time step of the physics simulation
	\param worldEntity Entity used to obtain world in which entities exist
	\param updateRate Update rate of the physics simulation. Between 20 and 1000
	*/
	static proto void SetUpdateRate(notnull IEntity worldEntity, int updateRate);
	/*!
	Returns number of dynamic bodies
	\param worldEntity Entity used to obtain world in which entities exist
	*/
	static proto int GetNumDynamicBodies(notnull IEntity worldEntity);
	/*!
	Returns entity of a specified dynamic body
	\param worldEntity Entity used to obtain world in which entities exist
	\param index Index of a dynamic body. Must be within bounds (GetNumDynamicBodies)
	*/
	static proto IEntity GetDynamicBody(notnull IEntity worldEntity, int index);
}

/*!
\}
*/
