enum PGPolyFlags
{
	NONE,

	WALK,			// Ability to walk (ground, grass, road)
	DISABLED,		// Disabled polygon
	DOOR,			// Ability to move through doors
	INSIDE,			// Ability to move inside buildings

	SWIM,			// Ability to swim (water)
	SWIM_SEA,		// Ability to swim (sea water)

	LADDER,			// Ability to climb on ladders
	JUMP_OVER,		// Ability to do jumps overs
	JUMP_DOWN,		// Ability to jump down
	CLIMB,			// Ability to climb up
	CRAWL,			// Ability to crawl
	CROUCH,			// Ability to crouch

	UNREACHABLE,

	ALL,

	JUMP,			// JUMP_OVER | JUMP_DOWN
	SPECIAL			// JUMP | CLIMB | CRAWL | CROUCH
}

enum PGAreaType
{
	NONE,

	TERRAIN,

	WATER,
	WATER_DEEP,
	WATER_SEA,
	WATER_SEA_DEEP,
	OBJECTS_NOFFCON,

	OBJECTS,
	BUILDING,
	ROADWAY,
	TREE,
	ROADWAY_BUILDING,

	DOOR_OPENED,
	DOOR_CLOSED,

	LADDER,
	CRAWL,
	CROUCH,
	FENCE_WALL,
	JUMP
}

/*!
Filter for FindPath, RaycastNavMesh, SampleNavmeshPosition
*/
class PGFilter : Managed
{
	// Uses PGPolyFlags bitmasks
	proto native int GetIncludeFlags();
	proto native int GetExcludeFlags();
	proto native int GetExlusiveFlags();
	proto native void SetFlags(int includeFlags, int excludeFlags, int exclusiveFlags);

	proto native void SetCost(PGAreaType areaType, float cost);
}

class AIWorld : Managed
{
	private void AIWorld();
	private void ~AIWorld();

	/*!
	Creates group with group behaviour specified by templateName param.
	AIGroups lifetime is managed by AIWorld, e.g. empty groups are deleted automatically.
	*/
	proto native AIGroup CreateGroup(string templateName);
	/*!
	Creates group with no group behaviour
	*/
	proto native AIGroup CreateDefaultGroup();
	/*!
	Destroys group and all AIAgents attached
	*/
	proto native void DeleteGroup(notnull AIGroup group);
	
	/*!
	Finds path on navmesh using path graph filter. 
	
	/param 	[in]	from 			starting position
	/param 	[in] 	to 				ending position
	/param 	[in] 	pgFilter 		filter used for searching
	/param 	[out] 	waypoints		waypoints array including starting and ending position
	/returns	true - if path has been found
	*/
	proto native bool FindPath(vector from, vector to, PGFilter pgFilter, out TVectorArray waypoints);
	
	/*!
	Raytest in navmesh
	
	/param 	[in]	from 			starting position
	/param 	[in] 	to 				ending position
	/param 	[in] 	pgFilter 		filter used for searching
	/param 	[out] 	hitPos			hit position
	/param 	[out] 	hitNormal		hit normal
	/returns	true - if ray hits navmesh edge
	*/
	proto native bool RaycastNavMesh(vector from, vector to, PGFilter pgFilter, out vector hitPos, out vector hitNormal);
	
	/*!
	Finds closest point on navmesh within maxDistance radius using path graph filter. 
	
	/param 	[in]	position 			position wanted
	/param 	[in] 	maxDistance 		search radius
	/param 	[in] 	pgFilter 		filter used for searching
	/param 	[out] 	sampledPosition		closest position on navmesh to position
	/returns	true - function succeedes and found position is written to output paramter "sampledPosition"
	/returns	false - function failed to find position on navmesh within given radius, output paramter is left intact
	*/
	proto native bool SampleNavmeshPosition(vector position, float maxDistance, PGFilter pgFilter, out vector sampledPosition);	
}