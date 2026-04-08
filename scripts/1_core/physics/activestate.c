/*!
\addtogroup Physics
\{
*/

/*!
Activation state of the physics controller in the physics world.
*/
enum ActiveState
{
	//! body is not simulated (sleeps)
	INACTIVE,
	//! body is simulated
	ACTIVE,
	//! body is simulated and cannot sleep
	ALWAYS_ACTIVE,
}

/*!
\}
*/
