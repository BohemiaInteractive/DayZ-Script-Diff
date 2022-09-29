
/** @file */

// -------------------------------------------------------------------------
// object (SF) Spawn Flags (use to setup behavior and/ or trigger functionality)
//
const int ECE_NONE							= 0;
	
const int ECE_SETUP							= 2;	// process full entity setup (when creating NEW entity)
const int ECE_TRACE							= 4;	// trace under entity when being placed (from the point of creation)
const int ECE_CENTER						= 8;	// use center from shape (model) for placement

const int ECE_UPDATEPATHGRAPH				= 32;	// update navmesh when object placed upon it

const int ECE_ROTATIONFLAGS					= 512;	// enable rotation flags for object placement
const int ECE_CREATEPHYSICS					= 1024;	// create collision envelope and related physics data (if object has them)
const int ECE_INITAI						= 2048; // init ai
const int ECE_AIRBORNE						= 4096;	// create flying unit in the air

const int ECE_EQUIP_ATTACHMENTS				= 8192;		// equip with configured ATTACHMENTS
const int ECE_EQUIP_CARGO					= 16384;	// equip with configured CARGO
const int ECE_EQUIP							= 24576;	// equip with configured (ATTACHMENTS + CARGO)
const int ECE_EQUIP_CONTAINER				= 2097152;	// populate DE/ group CONTAINER during spawn
const int ECE_LOCAL							= 1073741824; // create object locally

const int ECE_NOSURFACEALIGN				= 262144;	// do not align object on surface/ terrain
const int ECE_KEEPHEIGHT					= 524288;	// keep height when creating object (do not use trace or placement on surface)

const int ECE_NOLIFETIME					= 4194304;	// do not set lifetime when creating the object

// note: use predefined combination when not solving something specific
//
const int ECE_IN_INVENTORY					= 787456;	// ECE_CREATEPHYSICS|ECE_KEEPHEIGHT|ECE_NOSURFACEALIGN
const int ECE_PLACE_ON_SURFACE				= 1060;		// ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH|ECE_TRACE
const int ECE_OBJECT_SWAP					= 787488;	// ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH|ECE_KEEPHEIGHT|ECE_NOSURFACEALIGN

const int ECE_FULL							= 25126;	// ECE_SETUP|ECE_TRACE|ECE_ROTATIONFLAGS|ECE_UPDATEPATHGRAPH|ECE_EQUIP

// -------------------------------------------------------------------------
// object (RF) Rotation Flags (use to force and/ or invoke placement rotation)
//
const int	RF_NONE					= 0;

const int	RF_FRONT				= 1;	// front side placement
const int	RF_TOP					= 2;	// top side placement
const int	RF_LEFT					= 4;	// left side placement
const int	RF_RIGHT				= 8;	// right side placement
const int	RF_BACK					= 16;	// back side placement
const int	RF_BOTTOM				= 32;	// bottom side placement

const int	RF_ALL					= 63;	// RF_FRONT|RF_TOP|RF_LEFT|RF_RIGHT|RF_BACK|RF_BOTTOM

const int	RF_IGNORE				= 64;	// ignore placement RF flags - object will spawn as model was created

const int	RF_TOPBOTTOM			= 34;	// RF_TOP|RF_BOTTOM
const int	RF_LEFTRIGHT			= 12;	// RF_LEFT|RF_RIGHT
const int	RF_FRONTBACK			= 17;	// RF_FRONT|RF_BACK

const int	RF_RANDOMROT			= 64;	// allow random rotation around axis when placing
const int	RF_ORIGINAL				= 128;	// use default placement setuped on object in config
const int	RF_DECORRECTION			= 256;	// angle correction when spawning InventoryItem at Building angle
const int	RF_DEFAULT				= 512;	// use default placement setuped on object in config

// -------------------------------------------------------------------------
//! Categories for CEApi.EconomyLog
class EconomyLogCategories
{
	private void EconomyLogCategories();
	private void ~EconomyLogCategories();
	
	const string Economy				= "economy";
	const string EconomyRespawn			= "economy_respawn";
	const string RespawnQueue			= "respawn_queue";
	const string Container				= "container";
	const string Matrix					= "matrix";
	const string UniqueLoot				= "uniqueloot";
	const string Bind					= "bind";
	const string SetupFail				= "setupfail";
	const string Storage				= "storage";
	const string Classes				= "class";
	const string Category				= "category";
	const string Tag					= "tag";
	const string SCategory				= "s_category";
	const string STag					= "s_tag";
	const string SAreaflags				= "s_areaflags";
	const string SCrafted				= "s_crafted";
	const string MapGroup				= "map_group";
	const string MapComplete			= "map_complete";
	const string InfectedZone			= "infected_zone";
}

// -------------------------------------------------------------------------
//! Special strings for CEApi.EconomyMap
class EconomyMapStrings
{
	private void EconomyMapStrings();
	private void ~EconomyMapStrings();
	
	/**
	\brief Generates string that will make CEApi.EconomyMap use all items with this category
	\note This is refering to the CE categories of loot defined in cfglimitsdefinition.xml
	\warning This is persistent per session, when you set it, it will persist until it is set to something else
		\param category \p string The desired loot category
		\return \p string The string to pass into CEApi.EconomyMap
	@code
		GetCEApi().EconomyMap(EconomyMapStrings.Category("food"));
	@endcode
	*/
	static string Category(string category)
	{
		return string.Format("category:%1", category);
	}
	
	/**
	\brief Generates string that will make CEApi.EconomyMap use all items with this tag
	\note This is refering to the CE tags of loot defined in cfglimitsdefinition.xml
	\warning This is persistent per session, when you set it, it will persist until it is set to something else
		\param category \p string The desired loot tag
		\return \p string The string to pass into CEApi.EconomyMap
	@code
		GetCEApi().EconomyMap(EconomyMapStrings.Tag("floor"));
	@endcode
	*/
	static string Tag(string tag)
	{
		return string.Format("tag:%1", tag);
	}

	
	
	//! Everything
	const string ALL_ALL = "all:all";
	//! All loot
	const string ALL_LOOT = "all:loot";
	//! All vehicles
	const string ALL_VEHICLE = "all:vehicle";
	//! All infected
	const string ALL_INFECTED = "all:infected";
	//! All animals
	const string ALL_ANIMAL = "all:animal";
	//! All players
	const string ALL_PLAYER = "all:player";
	//! All proxies
	const string ALL_PROXY = "all:proxy";
	//! All static loot spawns
	const string ALL_PROXY_STATIC = "all:proxystatic";
	//! All dynamic loot spawns
	const string ALL_PROXY_DYNAMIC = "all:proxydynamic";
	//! All abandoned loot spawns
	const string ALL_PROXY_ABANDONED = "all:proxyabandoned";
}

// -------------------------------------------------------------------------
//! Special strings for CEApi.EconomyOutput
class EconomyOutputStrings
{
	private void EconomyOutputStrings();
	private void ~EconomyOutputStrings();
	
	/**
	\brief Lists stats regarding which loot spawn) that are linked together and how many there are
		\note In logs: "[linked_groups]"
	*/
	const string LINKS = "links";
	/**
	\brief Lists loot spawns that have more loot than their maximum + 4
		\note In logs: "[autotest:ListSuspiciousGroups]"
	*/
	const string SUSPICIOUS = "suspicious";
	/**
	\brief Lists DE spawns that have positions that are within supplied range (< fRange, not equal)
		\note In logs: "[ClosePosition]"
	*/
	const string DE_CLOSE_POINT = "declosepoint";
	/**
	\brief Lists loot spawns that have been marked as abandoned
		\note In logs: "[autotest:ListAbandonedGroups]"
	*/ 
	const string ABANDONED = "abandoned";
	/**
	\brief Lists loot spawns that are not abandoned but have no loot
		\note In logs: "[autotest:ListEmptyGroups]"
	*/  
	const string EMPTY = "empty";
	/**
	\brief Lists loot spawns that are not abandoned and within the supplied range  (< fRange, not equal)
		\note In logs: "[autotest:ListCloseGroups]"
		\note Is the same as CEApi.ListCloseProxy
	*/
	const string CLOSE = "close";
	/**
	\brief Lists the number of objects inside of categories
		\note In logs: "[autotest:ProcessDebugLog]"
	*/
	const string WORLD = "world";
	/**
	\brief Lists overall CE stats
		\note In logs: "[autotest:OverallStatus]"
	*/
	const string STATUS = "status";
	/**
	\brief Lists the maxlootsize of all CE items
		\note In logs: "[autotest:ListLootSize]"
	*/
	const string LOOT_SIZE = "lootsize";
}

// -------------------------------------------------------------------------
enum ESpawnVolumeVis
{
	OFF = 0,
	ADAPTIVE,
	VOLUME,
	OCCUPIED,
}

// -------------------------------------------------------------------------
/**
	\brief API to interact with Central Economy
	\note Accessible through GetCEApi, make sure to read the documentation for that as well
	\note Any mention of 'storage/' means the '$mission:storage_#' folder
*/
class CEApi
{
	private void CEApi() {}
	private void ~CEApi() {}
	
	/**
	\brief Regenerates "storage/spawnpoints.bin" if necessary
	\note Already happens automatically when server starts before it is loaded in
	@code
		GetCEApi().ExportSpawnData();
	@endcode
	*/
	proto native void ExportSpawnData();
	/**
	\brief Generates "storage/export/mapgrouppos.xml"
		\param vCenter \p vector Center of area to generate from
			\note When using a zero vector, this will automatically get the world center
		\param fRadius \p float Radius in meters of area to generate from
			\note When 0, this will automatically get a radius covering the map
	@code
		// Export for whole map
		GetCEApi().ExportProxyData();
	@endcode
	*/
	proto native void ExportProxyData( vector vCenter = vector.Zero, float fRadius = 0 );
	/**
	\brief Generates "storage/export/mapgroupcluster.xml"
	@code
		GetCEApi().ExportClusterData();
	@endcode
	*/
	proto native void ExportClusterData();
	/**
	\brief Generates "storage/export/mapgroupproto.xml"
	@code
		GetCEApi().ExportProxyProto();
	@endcode
	*/
	proto native void ExportProxyProto();
	/**
	\brief Invalidates loot spawn points which are closer than the radius supplied
		\note Will output a message indicating how many points were invalidated
		\param fRadius \p float The minimum desired distance in meters between loot spawn points
			\note When 0 it will use a radius of 0.5
		\param bAllSelections \p bool When false, will only check the points within a container against each other, when true will against all points
	@code
		// Example: I want there to be a minimum distance of 0.3m between any loot spawn point
		GetCEApi().MarkCloseProxy(0.3, true);
	@endcode
	*/
	proto native void MarkCloseProxy( float fRadius, bool bAllSelections );
	/**
	\brief Removes all invalid points
		\note Is best paired with a MarkCloseProxy call first to invalidate unwanted points
		\note Will output a message indicating the amount of deleted points
	@code
		GetCEApi().RemoveCloseProxy();
	@endcode
	*/
	proto native void RemoveCloseProxy();
	/**
	\brief Outputs a list of all loot points closer than specified radius
	\note This will automatically be checking all points against all points
	\note This is a better way of handling it than the above two methods, as it can be then be removed manually from the xmls
	\note Is the same as CEApi.EconomyOutput(EconomyOutputStrings.CLOSE, fRadius)
		\param fRadius \p float The minimum desired distance in meters between loot spawn points
	@code
		GetCEApi().ListCloseProxy(0.3);
	@endcode
	*/
	proto native void ListCloseProxy( float fRadius );

	/**
	\brief Will emulate the spawning of the item which is being looked at and generate images (.tga) in storage/lmap and output logs
	\note DEVELOPER/DIAG ONLY
	\note This will only work if the loot spawner is active
		\param sClassName \p string The class name of the desired item to analyze
			\note When using "*" as class name, instead of generating images, it will generate a .csv in storage/log containing the output of all spawnable items
		\return \p bool Whether the operation was successful, it might fail in certain scenarios if the CE doesn't update in time
	@code
		GetCEApi().SpawnAnalyze("FlatCap_Grey");
	@endcode
	*/
	proto native bool SpawnAnalyze( string sClassName );

	/**
	\brief Subtracts the supplied value from the current lifetime of all items in the world
	\note Uses RadiusLifetimeDecrease under the hood and then calculates the parameters to fit the world
	\note Will automatically clamp result between min and max lifetime  [3, 316224000] (3 seconds to 10 years)
		\param fShift \p float The value in seconds which will be subtracted from the current lifetimes
	@code
		// Shortens the lifetimes of all items in the world by 3 seconds
		GetCEApi().TimeShift(3);
	@endcode
	*/
	proto native void TimeShift( float fShift );
	/**
	\brief Fills in the Debug Lifetime, which will be used for any new DE spawned
		\param fLifeTime \p float The lifetime for any DE spawned after this call
			\note When this is set back to 0, it will stop using the Debug Lifetime
	@code
		// Any DE spawned after this call will have a lifetime of 300 seconds
		GetCEApi().OverrideLifeTime(300);
	@endcode
	*/
	proto native void OverrideLifeTime( float fLifeTime );

	/**
	\brief Force spawn specific prototype group + loot at position
	\note DEVELOPER/DIAG ONLY
	\note This can also spawn in other CE & DE related objects, but is best used exclusively for testing prototype groups
		\param sGroupName \p string The group name to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fAngle \p float Angle to spawn the Entity with in degrees [0, 360]
			\note When a negative value, will pick a random one
		\return \p Entity The spawned Entity or null if unsuccessful
	@code
		GetCEApi().SpawnGroup("Land_Shed_M1", Vector(5, 10, 5));
	@endcode
	*/
	proto native Entity SpawnGroup( string sGroupName, vector vPos, float fAngle = -1 );
	/**
	\brief Force spawn specific dynamic event
	\note DEVELOPER/DIAG ONLY
	\note Versatile, can be used for Animal, Infected, Static, Item and Vehicle
	\note Is the only one capable of spawning DE's that have DE Groups
	\note Is the same as the next one, but has ECE_FULL flags and a hidden flag which overrides herd limit
	\note This is also FORCE spawn, so it will bypass any limit and avoidance checks
		\param sEvName \p string The DE to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fAngle \p float Angle to spawn the Entity with in degrees [0, 360]
			\note When a negative value, will pick a random one
		\return \p bool Whether the spawn was successful
	@code
		GetCEApi().SpawnDE("StaticHeliCrash", Vector(5, 10, 5));
	@endcode
	*/
	proto native void SpawnDE( string sEvName, vector vPos, float fAngle = -1 );
	/**
	\brief Force spawn specific dynamic event
	\note DEVELOPER/DIAG ONLY
	\note Is the same one as above but with the possibility of custom flags
		\param sEvName \p string The DE to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fAngle \p float Angle to spawn the Entity with in degrees [0, 360]
			\note When a negative value, will pick a random one
		\param uFlags \p int ECE flags to apply while spawning the DE
		\return \p bool Whether the spawn was successful
	@code
		GetCEApi().SpawnDEEx("StaticHeliCrash", Vector(5, 10, 5), ECE_FULL);
	@endcode
	*/
	proto native void SpawnDEEx( string sEvName, vector vPos, float fAngle, int uFlags );
	/**
	\brief Spawn an item through CE
	\note DEVELOPER/DIAG ONLY
		\param sEvName \p string The DE to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fAngle \p float Angle to spawn the Entity with in degrees [0, 360]
			\note When a negative value, will pick a random one
		\param iCount \p int The amount of items
		\param fRange \p float Circle radius
			\note When iCount is larger than 1, changing this will make it so that they are spawned in a circle around vPos
	@code
		GetCEApi().SpawnLoot("Deagle", Vector(5, 10, 5), 0, 9, 1);
	@endcode
	*/
	proto native void SpawnLoot( string sEvName, vector vPos, float fAngle, int iCount = 1, float fRange = 1 );

	/**
	\brief Spawn all entities with dynamic category through CE
	\note DEVELOPER/DIAG ONLY
	\note Will print additional logs to RPT
		\param vPos \p vector The position to spawn the Entities at
			\note This will be the starting position, the items will be split into segments by CE BBox size (smallest to largest)
		\param bShowCylinders \p bool Whether to draw the CE BBox with debug shapes
			\note Red: Invalid BBox; Yellow: Not in DB; Green: All ok
		\param fDefaultDistance \p float The default distance to use when the entity does not have a CE BBox
			\note This means that all the ones with invalid CE BBoxes will be piled up at the starting position when this is 0
	@code
		GetCEApi().SpawnDynamic(Vector(5, 10, 5), true);
	@endcode
	*/
	proto native void SpawnDynamic( vector vPos, bool bShowCylinders = true, float fDefaultDistance = 0 );
	/**
	\brief Spawn all entities with vehicles category through CE
	\note DEVELOPER/DIAG ONLY
	\note Will print additional logs to RPT
		\param vPos \p vector The position to spawn the Entities at
			\note This will be the starting position, vehicles spawn with 20m spacing
			\note Make sure you do it on a surface as flat and open as possible, they need a lot of space
		\param bShowCylinders \p bool Does nothing for Vehicles, left for backwards compat
		\param fDefaultDistance \p float The default distance to use when the entity does not have a CE BBox
			\note This means that all the ones with invalid CE BBoxes will be piled up at the starting position when this is 0
	@code
		GetCEApi().SpawnVehicles(Vector(5, 10, 5));
	@endcode
	*/
	proto native void SpawnVehicles( vector vPos, bool bShowCylinders = false, float fDefaultDistance = 20 );
	/**
	\brief Spawn all entities with building category through CE
	\note DEVELOPER/DIAG ONLY
	\note Will print additional logs to RPT
		\param vPos \p vector The position to spawn the Entities at
			\note This will be the starting position, the items will be split into segments by CE BBox size (smallest to largest)
		\param bShowCylinders \p bool Whether to draw the CE BBox with debug shapes
			\note Red: Invalid BBox; Yellow: Not in DB; Green: All ok
			\note These will most likely just always be red
		\param fDefaultDistance \p float The default distance to use when the entity does not have a CE BBox
			\note This means that all the ones with invalid CE BBoxes will be piled up at the starting position when this is 0
	@code
		GetCEApi().SpawnBuilding(Vector(5, 10, 5), true);
	@endcode
	*/
	proto native void SpawnBuilding( vector vPos, bool bShowCylinders = false, float fDefaultDistance = 20 );

	/**
	\brief Spawn an entity through CE
	\note DEVELOPER/DIAG ONLY
	\note Is similar to SpawnLoot, but works better on Animal/Infected/Vehicle
		\param sClassName \p string The entity to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fRange \p float Circle radius
			\note When iCount is larger than 1, changing this will make it so that they are spawned in a circle around vPos
		\param iCount \p int The amount of items
	@code
		GetCEApi().SpawnEntity("Deagle", Vector(5, 10, 5), 1, 9);
	@endcode
	*/
	proto native void SpawnEntity( string sClassName, vector vPos, float fRange, int iCount );
	/**
	\brief Spawn an entity through CE
	\note DEVELOPER/DIAG ONLY
	\note Is the same as SpawnEntity, but spawns only one and returns the spawned Object
		\param sClassName \p string The entity to spawn
		\param vPos \p vector The position to spawn the Entity at
		\return \p Object The spawned Object
	@code
		GetCEApi().SpawnSingleEntity("Deagle", Vector(5, 10, 5));
	@endcode
	*/
	proto native Object SpawnSingleEntity( string sClassName, vector vPos );
	/**
	\brief Spawn an entity through CE
	\note DEVELOPER/DIAG ONLY
	\note Is the same as SpawnEntity, under the hood it just defaults to RF_ORIGINAL
		\param sClassName \p string The entity to spawn
		\param vPos \p vector The position to spawn the Entity at
		\param fRange \p float Circle radius
			\note When iCount is larger than 1, changing this will make it so that they are spawned in a circle around vPos
		\param iCount \p int The amount of items
		\param iFlags \p int The rotation flags to apply
	@code
		GetCEApi().SpawnRotation("Deagle", Vector(5, 10, 5), 1, 9, RF_ALL);
	@endcode
	*/
	proto native void SpawnRotation( string sClassName, vector vPos, float fRange, int iCount, int iFlags );
	/**
	\brief Spawn an entity through CE, x amount of times in a grid
	\note DEVELOPER/DIAG ONLY
	\note The name may imply it does something else, but a profiler is needed to actually benchmark it
	\note The position starts at 0 0 0
		\param sClassName \p string The entity to spawn
		\param iCount \p int The amount of items
	@code
		GetCEApi().SpawnPerfTest("Deagle", 30);
	@endcode
	*/
	proto native void SpawnPerfTest( string sClassName, int iCount );
	
	/**
	\brief Queue up the depleting of lifetime of everything in the world
	\note DEVELOPER/DIAG ONLY
	@code
		GetCEApi().CleanMap();
	@endcode
	*/
	proto native void CleanMap();

	/**
	\brief Outputs debug file to storage/log/*.csv
	\note DEVELOPER/DIAG ONLY
		\param sLogType \p string The type of log (EconomyLogCategories)
	@code
		GetCEApi().EconomyLog(EconomyLogCategories.Economy);
	@endcode
	*/
	proto native void EconomyLog( string sLogType );
	
	/**
	\brief Outputs debug file to storage/lmap/*.tga showing the current places this is spawned
	\note DEVELOPER/DIAG ONLY
	\note This works best when you have an image of the map to overlay the tga with afterwards
		\param sMapType \p string The type of lmap, can be just a class name or a special string
			\note There are several prefixes and sets of strings that can be passed in here, see EconomyMapStrings
	@code
		// To check only a specific item
		GetCEApi().EconomyMap("Deagle");
	@endcode
	*/
	proto native void EconomyMap( string sMapType );
	/**
	\brief Outputs debug logs into server log or rpt
	\note DEVELOPER/DIAG ONLY
		\param sOutputType \p string One of the strings specified in EconomyOutputStrings
		\param fRange \p float Range parameter used for some of the options
	@code
		GetCEApi().EconomyOutput(EconomyOutputStrings.CLOSE, 3);
	@endcode
	*/
	proto native void EconomyOutput( string sOutputType, float fRange );

	/**
	\brief Process lifetime increase within radius by value (sec)
	\note Will automatically clamp result between min and max lifetime  [3, 316224000] (3 seconds to 10 years)
		\param vCenter \p vector The center point
		\param fRadius \p float The radius around the center point
		\param fValue \p float The value to increase the lifetime by
			\note When 0, this will reset it to default instead (but it's better to use RadiusLifetimeReset still)
	@code
		GetCEApi().RadiusLifetimeIncrease(Vector(500, 0, 500), 3, 500);
	@endcode
	*/
	proto native void RadiusLifetimeIncrease( vector vCenter, float fRadius, float fValue );
	/**
	\brief Process lifetime decrease within radius by value (sec)
	\note Will automatically clamp result between min and max lifetime  [3, 316224000] (3 seconds to 10 years)
		\param vCenter \p vector The center point
		\param fRadius \p float The radius around the center point
		\param fValue \p float The value to decrease the lifetime by
	@code
		GetCEApi().RadiusLifetimeDecrease(Vector(500, 0, 500), 3, 500);
	@endcode
	*/
	proto native void RadiusLifetimeDecrease( vector vCenter, float fRadius, float fValue );
	/**
	\brief Process lifetime reset to default value from DB within radius
		\param vCenter \p vector The center point
		\param fRadius \p float The radius around the center point
	@code
		GetCEApi().RadiusLifetimeReset(Vector(500, 0, 500), 3);
	@endcode
	*/
	proto native void RadiusLifetimeReset( vector vCenter, float fRadius );
	
	
	
	/** \name Globals API
		Get values from globals.xml
	*/
	//@{
	
	/**
	\brief Get int from globals.xml
	\note type="0"
		\param varName \p string The name of the entry
		\return \p int The value or int.MIN if not found/not an int
	*/
	proto native int GetCEGlobalInt(string varName);
	/**
	\brief Get float from globals.xml
	\note type="1"
		\param varName \p string The name of the entry
		\return \p float The value or float.MIN if not found/not a float
	*/
	proto native float GetCEGlobalFloat(string varName);
	/**
	\brief Get string from globals.xml
	\note type="2"
		\param varName \p string The name of the entry
		\return \p string The value or empty string if not found/not a string
	*/
	proto native string GetCEGlobalString(string varName);
	
	//@}
	
	
	
	/** \name Avoidance API
		Optimized internal methods that the CE uses to avoid spawning DE within certain distances
	*/
	//@{
	
	/**
	\brief Check if there is a player within a radius
		\param vPos \p vector The center point
		\param fDistance \p float The radius around the center point
		\return \p bool Returns false when there is a player inside supplied area, true when it successfully avoided players
	@code
		GetCEApi().AvoidPlayer(Vector(500, 0, 500), 20);
	@endcode
	*/
	proto native bool AvoidPlayer(vector vPos, float fDistance); // return false when there is a player
	/**
	\brief Check if there is a vehicle within a radius
		\param vPos \p vector The center point
		\param fDistance \p float The radius around the center point
		\param sDEName \p string A specific DE to avoid, if left empty, will avoid all vehicles
		\return \p bool Returns false when there is a vehicle inside supplied area, true when it successfully avoided vehicles
	@code
		GetCEApi().AvoidVehicle(Vector(500, 0, 500), 500, "VehicleCivilianSedan");
	@endcode
	*/
	proto native bool AvoidVehicle(vector vPos, float fDistance, string sDEName = "");
	/**
	\brief Check if there is a vehicle within a radius
		\param vPos \p vector The center point
		\param fRange \p float The radius around the center point
		\return \p int The amoutn of players inside supplied area
	@code
		GetCEApi().CountPlayersWithinRange(Vector(500, 0, 500), 500);
	@endcode
	*/
	proto native int CountPlayersWithinRange(vector vPos, float fRange);
	
	//@}
	
	
	
	/** \name CE Debug menu Script API
		DIAG ONLY: These options are available from the in-game debug menu on Diag exe (Game > Central Economy), documentation can be found on wiki
	*/
	//@{
	
	// "Loot Spawn Edit"
	//{	
		//! "Spawn Volume Vis"
		proto native void LootSetSpawnVolumeVisualisation(ESpawnVolumeVis mode);
		//! "Setup Vis"
		proto native void LootToggleSpawnSetup(bool mode);
		//! "Edit Volume"
		proto native void LootToggleVolumeEditing(bool mode);
		//! "Re-Trace Group Points"
		proto native void LootRetraceGroupPoints();
		//! "Export Group >>"
		proto native void LootExportGroup();
		//! "Export All Groups >>>>" / GetCEApi.ExportProxyProto();
		proto native void LootExportAllGroups();
		//! "<<< Export Map" / GetCEApi.ExportProxyData(vector.Zero, 0);
		proto native void LootExportMap();
		//! "<<< Export Clusters" / GetCEApi().ExportClusterData()
		proto native void LootExportClusters();
	//}

	// "Loot Tool"
	//{
		//! "Deplete Lifetime"
		proto native void LootDepleteLifetime();
		//! "Set Damage = 1.0"
		proto native void LootSetDamageToOne();
		//! "Damage + Deplete"
		proto native void LootDepleteAndDamage();
	//}

	// "Infected"
	//{
		//! "Infected Vis"
		proto native void InfectedToggleVisualisation(bool mode);
		//! "Infected Zone Info"
		proto native void InfectedToggleZoneInfo(bool mode);
		//! "Infected Spawn"
		proto native void InfectedSpawn();
		//! "Reset Cleanup"
		proto native void InfectedResetCleanup();
	//}

	// "Animal"
	//{
		//! "Animal Vis"
		proto native void AnimalToggleVisualisation(bool mode);
		//! "Animal Spawn"
		proto native void AnimalSpawn();
		//! "Ambient Spawn"
		proto native void AnimalAmbientSpawn();
	//}

	//! "Vehicle&Wreck Vis"
	proto native void ToggleVehicleAndWreckVisualisation(bool mode);
	//! "Loot Vis"
	proto native void ToggleLootVisualisation(bool mode);
	//! "Cluster Vis"
	proto native void ToggleClusterVisualisation(bool mode);

	//! "Dynamic Events Status"
	proto native void ToggleDynamicEventStatus(bool mode);
	//! "Dynamic Events Vis"
	proto native void ToggleDynamicEventVisualisation(bool mode);
	//! "Dynamic Events Spawn"
	proto native void DynamicEventSpawn();
	//! "Export Dyn Event >>"
	proto native void DynamicEventExport();
	
	//! "Overall Stats"
	proto native void ToggleOverallStats(bool mode);
	
	//@}
	
	
	
	/** \name DEPRECATED/LEGACY
		These don't do anything anymore but are left for backwards compatibility
	*/
	//@{

	proto native void PlatformStatTest();
	
	proto native void LootToggleProxyEditing(bool mode);
	
	proto native void OnUpdate();
	
	//@}
};

/**
	\brief Get the CE API
	\warning Only exists when the game has CE enabled
	\note This means that this will work on anything which is running a mission with an initialized Hive
	\note Client does not have Hive when connected to a server, only the server does if it was set up in the init.c
*/
proto native CEApi GetCEApi();

// -------------------------------------------------------------------------
class CEItemProfile
{
	private void CEItemProfile() {}
	private void ~CEItemProfile() {}
	
	proto native int GetNominal(); // nominal - how many items should be aproximately in map
	proto native int GetMin(); // min - minimal count should be available in map

	proto native float GetQuantityMin(); // min quantity (0.0 - 1.0) (like ammobox - this determine how many bullets are there, or water bottle)
	proto native float GetQuantityMax(); // max quantity (0.0 - 1.0) (like ammobox - this determine how many bullets are there, or water bottle)

	proto native float GetQuantity(); // random quantity (0.0 - 1.0)

	proto native float GetLifetime(); // maximum lifetime in (seconds) - what is the idle before item abandoned at ground gets deleted
	proto native float GetRestock(); // restock is oposite of lifetime - idle before item is allowed to respawn when required

	proto native int GetCost(); // cost of item determines its 'value' for players (this serve as priority during respawn and cleanup operation)

	proto native int GetUsageFlags(); // area usage flags (each bit has assigned group - which as part of map overlay effectively affects spawning)
	proto native int GetValueFlags(); // area value flags (each bit has assigned group - which as part of map overlay effectively affects spawning)
};
