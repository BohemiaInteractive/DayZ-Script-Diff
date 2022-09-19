// These are flags which are persistent for the player entity, ie. are saved when the player is disconnected and loaded after connection.
// Normal persistence events OnStoreSave/OnStoreLoad are used to save/load these flags within a single int, the purpose is to unify the getter/setter interface so that every system that needs a bool does not need to create its own handling methods/logic.
// !!! Every value needs to be a power of 2 !!!

enum PersistentFlag
{
	AREA_PRESENCE = 1;
	//ADDITIONAL_FLAG_1 = 2
	//ADDITIONAL_FLAG_2 = 4
	//ADDITIONAL_FLAG_3 = 8
	//ADDITIONAL_FLAG_4 = 16
}