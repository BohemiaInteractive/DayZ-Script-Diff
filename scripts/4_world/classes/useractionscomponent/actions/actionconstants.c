/**@class	Constants for Quantity Consumed by action per action
 */
class UAQuantityConsumed
{
	const float DEFAULT = 1;
	const float DISINFECT = 10;
	const float EAT_SMALL = 10;
	const float EAT_NORMAL = 15;
	const float EAT_BIG = 25;
	const float FILL_LIQUID = 200;
	const float POUR_LIQUID = 100;
	const float DRAIN_LIQUID = 100;
	const float DRINK = 65;
	const float BLOOD = 50;
	const float SALINE = 100;
	const float FUEL = 500;
	const float FIREPLACE_EXTINGUISHER = 200;
	const float FIREPLACE_LIQUID = 100;
	const float GARDEN_FERTILIZE = 10;
	const float GARDEN_WATER = 150;
	const float GARDEN_DISINFECT_PLANT = 0.06;	
};

/**@class	Constants for Time spent while doing action
 */
class UATimeSpent
{
	const float DEFAULT = 1;
	const float DEFAULT_PICK = 1.5;
	const float DEFAULT_FILL = 8;
	const float DEFAULT_SORT = 2;
	const float DEFAULT_CONSTRUCT = 5;
	const float DEFAULT_DECONSTRUCT = 15;
	const float DEFAULT_REPAIR_CYCLE = 5;
	const float DEFAULT_DESTROY = 60;
	const float DEFAULT_PLACE = 0;
	const float DEFAULT_DEPLOY = 5;
	const float DEFAULT_CRAFT = 6;
	
	const float BANDAGE = 4;
	const float ADD_FUEL_TO_TORCH = 4;
	const float APPLY_SPLINT = 10;
	const float SEW_WOUNDS = 2;
	const float SEW_CUTS = 8;
	const float COLLECT_BLOOD = 10;
	const float COLLECT_SAMPLE = 3;
	const float TEST_BLOOD = 5;
	const float DEFIBRILATE = 3;
	const float INJECT = 3;
	const float MEASURE_TEMP = 12;
	const float COVER_HEAD = 0.5;
	const float RESTRAIN = 10;
	const float UNRESTRAIN = 20;
	const float SHAVE = 12.75;
	const float SKIN = 10;
	
	const float BASEBUILDING_CONSTRUCT_FAST = 5;
	const float BASEBUILDING_CONSTRUCT_MEDIUM = 7.5;
	const float BASEBUILDING_CONSTRUCT_SLOW = 10;
	const float BASEBUILDING_DECONSTRUCT_FAST = 10;
	const float BASEBUILDING_DECONSTRUCT_MEDIUM = 12;
	const float BASEBUILDING_DECONSTRUCT_SLOW = 15;
	const float BASEBUILDING_REPAIR_FAST = 3;
	const float BASEBUILDING_REPAIR_MEDIUM = 5;
	const float BASEBUILDING_REPAIR_SLOW = 10;

	const float DRINK_POND = 2.0;
	const float DRINK_WELL = 3.5;

	const float DIG_GARDEN = 4;
	const float DIG_STASH = 10;
	const float DIG_WORMS = 10;

	const float BURY_ASHES = 6;
	const float BURY_BODY = 12;

	const float LOCK = 2;
	const float UNLOCK = 2;
	const float UNPACK = 2;
	const float WASH_HANDS = 3;
	const float DISARM_MINE = 15;
	const float DISARM_EXPLOSIVE_REMOTE_PAIRED = 6;
	const float DISARM_EXPLOSIVE_REMOTE_UNPAIRED = 12;
	
	const float MAG_EMPTY = 0.5;
	const float MAG_LOAD = 0.5;
	
	const float FIREPLACE_IGNITE = 4;
	//const float FIREPLACE_HANDDRILL = 8;
	const float SALINE = 2;
	const float BLOOD = 2;
	
	const float START_ENGINE = 0.5;
	//const float INFINITE = -1;
	
	const float EXPLOSIVE_ARM = 6;
};

/**@class	Constants for maximal Distances where from the action can be done
 */
class UAMaxDistances
{
	const float SMALL = 1.3;
	const float DEFAULT = 2.0;
	const float REPAIR = 3.0;
	const float LARGE = 8.0;
	const float LADDERS = 1.3;
	const float BASEBUILDING = 20;
	const float BASEBUILDING_SHORT = 1.0;
	const float EXPLOSIVE_REMOTE_ACTIVATION = 100.0;
};

/**@class	Constants for specialty weight and type
 */
class UASoftSkillsWeight
{
	const float ROUGH_LOW = 0.01;
	const float ROUGH_MEDIUM = 0.02;
	const float ROUGH_HIGH = 0.03;
	const float PRECISE_LOW = -0.01;
	const float PRECISE_MEDIUM = -0.02;
	const float PRECISE_HIGH = -0.03;
};

/**@class	Constants for damage values that are applied to item when using certain action
 */
class UADamageApplied
{
	const float BUILD 			= 4;
	const float DISMANTLE 		= 8;
	const float REPAIR 			= 3;
	const float DESTROY 		= 25;
	const float SAW_LOCK 		= 150;
	const float SKINNING 		= 4;
	const float SAW_PLANKS		= 4;
	const float DEFUSE_TOOLS	= 2;
};

/**@class	Constants for water type
 */
class UAWaterType
{
	const string ALL = "water";
	const string FRESH = "fresh_water";
}

/**@class	Miscelaneous constants for use in actions
 */
class UAMisc
{
	const float FLAG_STEP_INCREMENT = 0.2; //0..1
};