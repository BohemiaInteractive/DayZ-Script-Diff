/**
\brief Lock types enum, meant for bitwise usage
\note Keep in sync with config defines!
\note Also, these are regular integers, whereas values from config are in bitwise format already, so use accordingly.
\note (1 << EBuildingLockType.LOCKPICK)
*/	
enum EBuildingLockType
{
	NONE = 0, //unlockable only via script
	LOCKPICK,
	SHIP_CONTAINER_0,
	SHIP_CONTAINER_1,
	SHIP_CONTAINER_2,
	SHIP_CONTAINER_3,
	SHIP_CONTAINER_4
}