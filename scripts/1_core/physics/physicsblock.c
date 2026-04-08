/*!
\addtogroup Physics
\{
*/

/*!
Disables collisions between two entities.
*/
typedef int[] PhysicsBlock;
class PhysicsBlock
{
	/*!
	Disables collisions between two entities by creating PhysicsBlock. Both entities must exist in the same world. Block must be removed manually on cleanup.
	\param ent1 Entity of the non-colliding pair
	\param ent2 Other entity of the non-colliding pair
	\return PhysicsBlock disabling collision while it exists
	*/
	static proto PhysicsBlock Create(notnull IEntity ent1, notnull IEntity ent2);
	/*!
	Enables collisions between two entities by removing PhysicsBlock.
	\param worldEntity Entity used to obtain world in which entities exist
	*/
	proto external void Remove(notnull IEntity worldEntity);
}

/*!
\}
*/
