/**
 * \defgroup CppEnums Internally defined enums
 * \desc Various enums defined in C++
 * @{
 */
enum ChatChannelType
{
	System, 				//!< CCSystem
	Admin, 					//!< CCAdmin
	Direct, 				//!< CCDirect
	Megaphone, 				//!< CCMegaphone
	Transmitter, 			//!< CCTransmitter
	PublicAddressSystem, 	//!< CCPublicAddressSystem
	BattlEye, 				//!< CCBattlEye
}

enum VoiceLevel
{
	Whisper, 	//!< VoiceLevelWhisper
	Talk, 		//!< VoiceLevelTalk
	Shout 		//!< VoiceLevelShout
}

enum VoiceEffectType
{
	Mumbling, 		//!< VoiceEffectMumbling
	Extortion, 		//!< VoiceEffectExtortion
	Obstruction 	//!< VoiceEffectObstruction
}

enum ObjIntersect
{
	Fire, 	//!< ObjIntersectFire: Fire Geometry
	View, 	//!< ObjIntersectView: View Geometry
	Geom,	//!< ObjIntersectGeom: Geometry
	IFire, 	//!< ObjIntersectIFire: (Indirect) Fire Geometry
	None 	//!< ObjIntersectNone: No Geometry
}

//! UserID of a parent widget usually matches this value, unless overriden in the 'InitWidgetSet' method
enum EffectWidgetsTypes
{
	ROOT = -2, //used when handling the m_Root widget directly
	NONE = 0,
	
	MASK_OCCLUDER = 1,
	MASK_BREATH = 2,
	
	HELMET_OCCLUDER = 10,
	HELMET_BREATH = 11,
	
	MOTO_OCCLUDER = 20,
	MOTO_BREATH = 21,
	
	COVER_FLASHBANG = 50,
	NVG_OCCLUDER = 51,
	PUMPKIN_OCCLUDER = 52,
	EYEPATCH_OCCLUDER = 53,
	
	BLEEDING_LAYER = 60,
}

enum EffectWidgetHandles
{
	FLASHBANG,
}

enum EffectWidgetSuspends
{
	BURLAPSACK,
	UNCON,
}

class HitDirectionConstants
{
	const int ROTATION_DEFAULT = 0; //0 == random
	const float DURATION_BASE = 2.0;
	const float BREAKPOINT_BASE = 0.2;
	const float DISTANCE_ADJUST = 0.0;
	const float SCATTER = 10.0;
	const int COLOR_DEFAULT = 0xffbb0a1e;
}

enum HitDirectionModes
{
	DISABLED,
	STATIC,
	DYNAMIC
}

enum HitIndicatorType
{
	SPLASH,
	SPIKE,
	ARROW
}

enum EInputRestrictors
{
	INVENTORY,
	MAP
}

/** @}*/

//! how often virtual hud checks if there is a difference since last sync
const int VIRTUAL_HUD_UPDATE_INTERVAL = 1000;

/**
 * \defgroup UI
 * \desc constants User Interface
 * @{
 */
const int IDC_OK					= 1;
const int IDC_CANCEL				= 2;
const int IDC_RESTART				= 5;

const int IDC_MAIN_NEXT_CHARACTER	= 50;
const int IDC_MAIN_PREV_CHARACTER	= 51;

const int IDC_MAIN_OPTIONS      	= 102;
const int IDC_MAIN_MULTIPLAYER  	= 105;
const int IDC_MAIN_QUIT         	= 106;
const int IDC_MAIN_CONTINUE			= 114;
const int IDC_MAIN_PLAY         	= 142;
const int IDC_MAIN_CHARACTER		= 143;
const int IDC_MAIN_ONLINE			= 124;
const int IDC_MULTI_REFRESH			= 123;
const int IDC_MULTI_INVITE			= 126;

const int IDC_BOOK_VIEWER_PREV  	= 102;
const int IDC_BOOK_VIEWER_NEXT  	= 103;

//! ingame menu
const int IDC_INT_RETRY				= 105; //default respawn
const int IDC_INT_RETRY_CUSTOM		= 106; //custom respawn
const int IDC_INT_EXIT				= 107;
//const int IDC_INT_RESPAWN			= 108; //respawn dialogue
/** @}*/

/**
 * \defgroup MenuID
 * \desc constants for menu pages
 * @{
 */

const int MENU_ANY									= 1;
const int MENU_NONE									= 2;
//const int MENU_***								= 3; // removed - can be recycled to something else
const int MENU_UNKNOWN								= 4;
const int MENU_CHARACTER							= 5;
const int MENU_CHAT									= 6;
const int MENU_EARLYACCESS							= 7;
const int MENU_SCENE_EDITOR							= 8;
const int MENU_INGAME								= 9;
const int MENU_INSPECT								= 10;
const int MENU_INVENTORY							= 11;
const int MENU_LOADING								= 12;
const int MENU_MAIN									= 13;
const int MENU_OPTIONS								= 14;
const int MENU_STARTUP								= 15;
const int MENU_SCRIPTCONSOLE						= 16;
const int MENU_CHAT_INPUT							= 17;
const int MENU_SCRIPTCONSOLE_DIALOG_PRESET_NAME		= 18;
const int MENU_SCRIPTCONSOLE_DIALOG_PRESET_RENAME 	= 19;
const int MENU_CONTROLS_PRESET						= 20;
const int MENU_NOTE									= 21;
const int MENU_MAP									= 22;
const int MENU_BOOK									= 23;
const int MENU_HELP_SCREEN      					= 24;
const int MENU_GESTURES		      					= 25;
const int MENU_LOGOUT 								= 26;
const int MENU_TITLE_SCREEN							= 27;
const int MENU_XBOX_CONTROLS						= 28;
const int MENU_RADIAL_QUICKBAR						= 29;
const int MENU_LOGIN_QUEUE							= 30;
const int MENU_SERVER_BROWSER						= 31;
const int MENU_CAMERA_TOOLS							= 32;
const int MENU_VIDEO								= 33;
const int MENU_KEYBINDINGS							= 34;
const int MENU_TUTORIAL								= 35;
const int MENU_CREDITS								= 36;
const int MENU_INVITE_TIMER							= 37;
const int MENU_LOGIN_TIME							= 38;
const int MENU_WARNING_ITEMDROP						= 39;
const int MENU_RESPAWN_DIALOGUE						= 40;
const int MENU_WARNING_TELEPORT						= 41;
const int MENU_CONNECT_ERROR						= 42;

const int GUI_WINDOW_MISSION_LOADER = 1;

const string CFG_VEHICLESPATH = "CfgVehicles";
const string CFG_WEAPONSPATH = "CfgWeapons";
const string CFG_MAGAZINESPATH = "CfgMagazines";
const string CFG_AMMO = "CfgAmmo";
const string CFG_WORLDS = "CfgWorlds";
const string CFG_SURFACES = "CfgSurfaces";
const string CFG_RECIPESPATH = "CfgRecipes";
const string CFG_SOUND_SHADERS = "CfgSoundShaders";
const string CFG_SOUND_SETS = "CfgSoundSets";
const string CFG_NONAI_VEHICLES = "CfgNonAIVehicles";

/** @}*/

/**
 * \defgroup CFGConstants Local data saving to files
 * \desc constants for saving local datas
 * @{
 */
const int		CFG_ARRAY_ITEMS_MAX		= 64;
const string	CFG_FILE_FIXED_PROFILE	= "Scripts/profile_fixed.cfg";
const string	CFG_FILE_USER_PROFILE	= "$profile:profile.cfg";
const string	CFG_FILE_DEBUG_PROFILE	= "$profile:debugProfile.cfg";
const string	CFG_FILE_EMOTES_PROFILE	= "$profile:emotesProfile.cfg";
const string	CFG_FILE_SEARCH_HISTORY	= "$profile:search_history.history";
const string	CFG_FILE_ENS_HISTORY	= "$profile:script_enscript.history";
const string	CFG_FILE_ENS_HISTORY_SERVER	= "$profile:script_enscriptServer.history";
const string	CFG_FILE_SCRIPT_LOG		= "$profile:script.log";
const string	CFG_FILE_SCRIPT_LOG_EXT	= "$profile:scriptExt.log";
const string	CFG_FILE_ADDITIONAL_INFO= "$profile:serverInfo.cfg";
/** @}*/

/**
 * \defgroup RPC RPC commands
 * \desc Constants for sending RPC commands on server
 * @{
 */
/*
const int RPC_SYNC_ITEM_VAR                     = 1;
const int RPC_SYNC_STAT                         = 2;
const int RPC_WRITE_NOTE                        = 3;
const int RPC_SYNC_DISPLAY_STATUS               = 4;
const int RPC_RECIPE_SEND    					= 7;
const int RPC_ON_SET_CAPTIVE					= 8;
const int RPC_SYNC_SCENE_OBJECT					= 9;
const int RPC_READ_A_BOOK               		= 10;	
const int RPC_USER_ACTION_PROMPT_SYNCH			= 11;
const int RPC_USER_ACTION_MESSAGE 				= 12;
const int RPC_ITEM_DIAG							= 13;
const int RPC_ITEM_DIAG_CLOSE					= 14;
const int RPC_SET_OBJECT_POSITION 				= 15;
const int RPC_USER_ACTION_MESSAGES 				= 16;
const int RPC_ITEM_SPLIT						= 17;
const int RPC_ITEM_COMBINE						= 18;
const int RPC_PLAYER_STATES_ON					= 19;
const int RPC_PLAYER_STATES_OFF					= 20;
const int RPC_PLACING_DEBUG						= 21;
const int RPC_PLACING_START						= 22;
const int RPC_PLACING_STOP						= 23;
const int RPC_STAMINA							= 25;
const int RPC_DAMAGE_VALUE_SYNC					= 26;
const int RPC_USER_ACTION_PROMPT_PROGRESS		= 27;
const int RPC_PLACING_UPDATE					= 28;
const int RPC_USER_ACTION_PROMPT_CLEAN			= 29;
const int RPC_DISABLE_MODIFIERS					= 30;
const int RPC_KILL_PLAYER						= 31;
const int RPC_ENABLE_INVINCIBILITY				= 32;
const int RPC_ITEM_DEBUG_ACTIONS				= 33;
const int RPC_LOG_PLAYER_STATS					= 34;
const int RPC_EM_IS_PLUGGED						= 35;
const int RPC_EM_IS_UNPLUGGED					= 36;
*/
/** @}*/


/**
 * \defgroup DeveloperRPC Developer RPC commands
 * \desc Constants for sending developer RPC commands on server
 * @{
 */
/*
const int DEV_RPC_SPAWN_GROUND_ITEM             = 101;
const int DEV_RPC_CLEAR_INV                     = 102;
const int DEV_RPC_TELEPORT                      = 103;
const int DEV_RPC_SET_PLAYER_DIRECTION          = 104;
const int DEV_RPC_SEND_SERVER_LOG               = 105;
const int DEV_RPC_STATS_DATA					= 107;
const int DEV_RPC_MODS_DATA						= 108;
const int DEV_RPC_AGENTS_DATA					= 109;
const int DEV_RPC_CREATE_SCENE_OBJECT 			= 110;
const int DEV_RPC_SCENE_LOAD 					= 111;
const int DEV_RPC_SPAWN_INV_ITEM                = 112;
const int DEV_RPC_SPAWN_ATTACHMENT_ITEM         = 113;
const int DEV_RPC_SPAWN_ITEM_ON_CURSOR          = 114;
*/
/** @}*/

/**
 * \defgroup SyncCMD Sync update commands
 * \desc Constants for synchronization update commands
 * @{
 */
	/*
const int DEV_STATS_UPDATE						= 1;
const int DEV_MODS_UPDATE						= 2;
const int DEV_AGENTS_UPDATE						= 3;
	*/
/** @}*/

/**
 * \defgroup MessageCMD Message commands
 * \desc Messaging System - the numbers must be 0 or higher, and the highest number should not be bigger than NUM_OF_CHANNELS-1
 * @{
 */
const int NUM_OF_CHANNELS 		= 100;
const int MSG_TEST_CHANNEL		= 0;
const int MSG_IMMUNE_REACTION	= 1;
const int MSG_STATS_SYNCED		= 2;
const int MSG_NOTIFIERS_TICK	= 3;
/** @}*/
	
/**
 * \defgroup Modifier pools minimum tick times in ms
 * \desc Modifier pools minimum tick times
 * @{
 */
const int MIN_TICK_MDFR_POOL_MAIN = 10000;//in ms
const int MIN_TICK_MDFR_POOL_PLAYER = 3000;
const int MIN_TICK_MDFR_POOL_ENVIRONMENT = 3000;
const int MIN_TICK_MDFR_POOL_DISEASE = 30000;
const int MIN_TICK_NOTIFIERS = 1000;
/** @}*/

/**
 * \defgroup EmoteIDs Emote ids
 * @{
 */
class EmoteConstants
{
	const int ID_EMOTE_GREETING			= 1;
	const int ID_EMOTE_SOS				= 2; //FB
	const int ID_EMOTE_HEART 			= 3;
	const int ID_EMOTE_TAUNT	 		= 4;
	const int ID_EMOTE_LYINGDOWN		= 5;
	const int ID_EMOTE_TAUNTKISS		= 6;
	const int ID_EMOTE_FACEPALM			= 7;
	const int ID_EMOTE_TAUNTELBOW		= 8;
	const int ID_EMOTE_THUMB			= 9;
	const int ID_EMOTE_THROAT		 	= 10;
	const int ID_EMOTE_SUICIDE			= 11; //FB
	const int ID_EMOTE_DANCE 			= 12;
	const int ID_EMOTE_CAMPFIRE 		= 13;
	const int ID_EMOTE_SITA 			= 14;
	const int ID_EMOTE_SITB 			= 15;
	const int ID_EMOTE_THUMBDOWN 		= 16;
	
	const int ID_EMOTE_DABBING 		= 32;
	const int ID_EMOTE_TIMEOUT 		= 35;
	const int ID_EMOTE_CLAP 		= 39;
	const int ID_EMOTE_POINT 		= 40;
	const int ID_EMOTE_SILENT 		= 43;
	const int ID_EMOTE_SALUTE 		= 44;
	const int ID_EMOTE_RPS 			= 45;
	const int ID_EMOTE_WATCHING 	= 46;
	const int ID_EMOTE_HOLD 		= 47;
	const int ID_EMOTE_LISTENING 	= 48;
	const int ID_EMOTE_POINTSELF 	= 49;
	const int ID_EMOTE_LOOKATME 	= 50;
	const int ID_EMOTE_TAUNTTHINK 	= 51;
	const int ID_EMOTE_MOVE 		= 52;
	const int ID_EMOTE_DOWN 		= 53;
	const int ID_EMOTE_COME 		= 54;
	const int ID_EMOTE_RPS_R 		= 55;
	const int ID_EMOTE_RPS_P 		= 56;
	const int ID_EMOTE_RPS_S 		= 57;
	const int ID_EMOTE_NOD 			= 58;
	const int ID_EMOTE_SHAKE 		= 59;
	const int ID_EMOTE_SHRUG 		= 60;
	const int ID_EMOTE_SURRENDER 	= 61;
	const int ID_EMOTE_VOMIT 		= 62;
	const int ID_EMOTE_DEBUG 		= 1000;
	
	/**
	 * \defgroup CustomEmoteIDs Custom animation events ID for emotes
	 * \desc used mainly in suicide emotes
	 * @{
	 */
	const int EMOTE_SUICIDE_DEATH 	= 1;
	const int EMOTE_SUICIDE_BLEED 	= 2;
	const int EMOTE_SUICIDE_SIMULATION_END = 3;
	/** @}*/
}

/**
 * \defgroup ItemGeneratorCfg Configurations for ItemsGenerator class
 * \desc Configurations for ItemsGenerator class
 * @{
 */
const float		ITEMSGEN_TICK = 10; //in seconds
const float		ITEMSGEN_MIN_DISTANCE_TO_REPOSITION = 80; //in meters
const float		ITEMSGEN_SPAWN_DISTANCE = 60; //in meters
const float		ITEMSGEN_MAX_SPREAD = 15; //in meters
const int		ITEMSGEN_ITEMS_AMOUNT = 5;
/** @}*/


/**
 * \defgroup SATIDs Selectable action types ids
 * \desc These ids helsp diferentiate types of selectable actions 
 * @{
 */
const int SAT_INTERACT_ACTION	= 1;
const int SAT_CRAFTING	 		= 2;
const int SAT_DEBUG_ACTION		= 3;
/** @}*/

/**
 * \defgroup UAIDs User action internal state machine states
 * \desc User action internal state machine states
 * @{
 */
const int 		UA_NONE = 0;
const int 		UA_FAILED = 1;
const int 		UA_PROCESSING = 2;
const int 		UA_REPEAT = 3;
const int 		UA_FINISHED = 4;
const int		UA_CANCEL = 5;
const int		UA_INTERRUPT = 6;
const int       UA_START = 7;
const int       UA_STARTT = 8;
const int       UA_CANCELT = 9;
const int       UA_FINISHEDT = 10;
const int    	UA_ANIM_EVENT = 11;
const int 		UA_INITIALIZE = 12;
const int		UA_CHECK_CON = 13;
const int		UA_AM_PENDING = 14;
const int		UA_AM_ACCEPTED = 15;
const int		UA_AM_REJECTED = 16;
const int		UA_IN_START = 17;
const int		UA_IN_END = 18;
const int		UA_SPAWN_DUST_A = 19;
const int		UA_SPAWN_DUST_B = 20;
const int		UA_IN_CRAFTING = 21;
	
const int		UA_ERROR = 24;
	
const int		UA_SETEND_2 = 32;
/** @}*/

/**

 * \defgroup AGTIDs Agent transmission system
 * \desc Agent transmission system
 * @{
 */
const int AGT_INV_IN	 		= 1;
const int AGT_INV_OUT	 		= 2;
const int AGT_UACTION_CONSUME	= 3;
const int AGT_TRANSFER_COPY	= 4;
const int AGT_UACTION_TOUCH		= 5;
const int AGT_WATER_POND		= 6;
const int AGT_AIRBOURNE_BIOLOGICAL	= 7;
const int AGT_UACTION_TO_PLAYER	= 8;
const int AGT_UACTION_TO_ITEM	= 9;
const int AGT_ITEM_TO_FLESH		= 10;
const int AGT_AIRBOURNE_CHEMICAL = 11;

const int DEF_BIOLOGICAL		= 1;
const int DEF_CHEMICAL			= 2;
/** @}*/
			
const int QUANTITY_HIDDEN = 0;
const int QUANTITY_COUNT = 1;
const int QUANTITY_PROGRESS = 2;


			
/**
 * \defgroup LiquidTypes
 * \desc Constants for liquid types
 * @{
 */
// BEWARE ALL INDIVIDUAL LIQUID TYPES ARE ALSO REPRESENTED CONFIG-SIDE AND MUST MATCH(all changes must be made on both sides)
// NOTE ANY NUMBER HERE MUST BE A 0 OR ANY POWER OF TWO, THERE IS A MAXIMUM OF 32 INDIVIDUAL LIQUID TYPES POSSIBLE
const int LIQUID_BLOOD_0_P	= 1;
const int LIQUID_BLOOD_0_N	= 2;
const int LIQUID_BLOOD_A_P	= 4;
const int LIQUID_BLOOD_A_N	= 8;
const int LIQUID_BLOOD_B_P	= 16;
const int LIQUID_BLOOD_B_N	= 32;
const int LIQUID_BLOOD_AB_P	= 64;
const int LIQUID_BLOOD_AB_N = 128;
const int LIQUID_SALINE 	= 256;

const int LIQUID_WATER = 512;
const int LIQUID_RIVERWATER = 1024;
const int LIQUID_VODKA = 2048;
const int LIQUID_BEER = 4096;
const int LIQUID_GASOLINE = 8192;
const int LIQUID_DIESEL = 16384;
const int LIQUID_DISINFECTANT = 32768;
const int LIQUID_SOLUTION = 65536;

// these are groups which do not have to correspond with configs
const int GROUP_LIQUID_BLOOD = 255;
const int GROUP_LIQUID_ALL = -1;//-1 = all bits to 1
/** @}*/

/**
 * \defgroup LiquidThroughputs
 * \desc Constants for liquid transfer speeds
 * @{
 */
const float LIQUID_THROUGHPUT_TINY = 0.1;
const float LIQUID_THROUGHPUT_DEFAULT = 1.0;
const float LIQUID_THROUGHPUT_GASOLINECANISTER = 10.0;
const float LIQUID_THROUGHPUT_CAR_DEFAULT = 10.0;
const float LIQUID_THROUGHPUT_GENERATOR = 10.0;
const float LIQUID_THROUGHPUT_FUELSTATION = 20.0;
const float LIQUID_THROUGHPUT_WELL = 15.0;
const float LIQUID_THROUGHPUT_BARREL = 100.0;

/** @}*/
	
/**
 * \defgroup ChatMessagesVisibility
 * \desc Constants for toggle chat messages type visibility
 * @{
 */
const string SYSTEM_CHAT_MSG 	= "system_chat_msg";
const string GLOBAL_CHAT_MSG 	= "global_chat_msg";
const string DIRECT_CHAT_MSG	= "direct_chat_msg";
const string VEHICLE_CHAT_MSG 	= "vehicle_chat_msg";
const string RADIO_CHAT_MSG 	= "radio_chat_msg";	
const string GAME_CHAT_MSG 		= "game_chat_msg";	
const string ADMIN_CHAT_MSG 	= "admin_chat_msg";	
const string PLAYER_CHAT_MSG 	= "player_chat_msg";	
/** @}*/

/**
 * \defgroup HUDVisibility
 * \desc Constants for toggling of HUD visibility
 * @{
 */
const string SHOW_QUICKBAR 		= "show_quickbar";
const string SHOW_HUD 			= "show_hud";
const string ENABLE_BLEEDINGINDICATION	= "enable_bleedingindication";
//const string SHOW_HUD_AUTOHIDE 	= "hud_autohide";
const string SHOW_CROSSHAIR 	= "show_crosshair";
const string SHOW_SERVERINFO 	= "show_serverinfo";
/** @}*/

/**
 * \defgroup LockConstants Controls locks
 * \desc Constants for toggling of alternate 
 * @{
 */
const string LOCK_SPRINT 	= "lock_sprint";
const string LOCK_FREELOOK 	= "lock_freelook";
const string LOCK_ZOOM 		= "lock_zoom";
/** @}*/

/**
 * \defgroup ItemVariables Item Variables
 * \desc Constants for Item Variables
 * @{
 */
const int VARIABLE_QUANTITY 	= 1;
const int VARIABLE_ENERGY 		= 2;
const int VARIABLE_TEMPERATURE 	= 4;
const int VARIABLE_WET 			= 8;
const int VARIABLE_LIQUIDTYPE	= 16;
const int VARIABLE_ISLIT		= 32;
const int VARIABLE_COLOR		= 64;
const int VARIABLE_CLEANNESS	= 128;
/** @}*/

const float CRAFTING_TIME_UNIT_SIZE = 4.0;// time unit size for crafting, this value is multiplied by the length given by a recipe

const float PROJECTED_CURSOR_DISTANCE = 5;//how long is the raycast from the weapon for projected cursor


/**
  * \ misc
 */
const float DEBUG_QUICK_UNRESTRAIN_TIME = 1.0;
const float MELEE_ITEM_DAMAGE = 1.0;

const int HAIR_SELECTION_COUNT = 45;
/**
* \ input exclude types; DEPRECATED (confusing)
*/
const int INPUT_EXCLUDE_ALL = 0;
const int INPUT_EXCLUDE_INVENTORY = 1;
const int INPUT_EXCLUDE_MOUSE_ALL = 2;
const int INPUT_EXCLUDE_MOUSE_RADIAL = 3;
const int INPUT_EXCLUDE_MAP = 4;

class GameConstants
{
	/**
	 * \defgroup StaminaHandlerCfg Configurations for StaminaHandler class
	 * \desc Configurations for StaminaHandler class
	 * @{
	 */
		// unit = currently percent (stamina max is 100)
	const int 	STAMINA_DRAIN_STANDING_SPRINT_PER_SEC = 4; //in units (how much sprint depletes stamina)
	const int 	STAMINA_DRAIN_CROUCHED_SPRINT_PER_SEC = 1; //in units (how much sprint in crouch depletes stamina)
	const int 	STAMINA_DRAIN_PRONE_SPRINT_PER_SEC = 3; //in units (how much sprint in prone depletes stamina)
	const int	STAMINA_DRAIN_SWIM_FAST_PER_SEC = 5; //in units (how much fast swimming depletes stamina)
	const int	STAMINA_DRAIN_LADDER_FAST_PER_SEC = 8; //in units (how much fast ladder climb depletes stamina)
	
	const float	STAMINA_DRAIN_HOLD_BREATH_START = 0.2; //in units (how much holding breath depletes stamina at the start)
	const float	STAMINA_DRAIN_HOLD_BREATH_END = 1.0; //in units (how much holding breath depletes stamina at the end)
	const float	STAMINA_DRAIN_JUMP = 25;		// in units (how much jumping depletes stamina)
	const float	STAMINA_DRAIN_VAULT = 20;		// in units (how much jumping depletes stamina)
	const float	STAMINA_DRAIN_CLIMB = 42;		// in units (how much jumping depletes stamina)
	const float	STAMINA_DRAIN_MELEE_LIGHT = 5; //in units (how much light melee punch depletes stamina)
	const float	STAMINA_DRAIN_MELEE_HEAVY = 25; //in units (how much heavy melee punch depletes stamina)
	const float	STAMINA_DRAIN_MELEE_EVADE = 8; // in units (how much evade depletes stamina)
	const float	STAMINA_DRAIN_ROLL = 5; // in units (how much roll depletes stamina)
	
	const float STAMINA_DRAIN_HOLD_BREATH_DURATION = 5.0; //in seconds, time it takes to increase stamina drain from STAMINA_DRAIN_HOLD_BREATH_START to STAMINA_DRAIN_HOLD_BREATH_END
	const float	STAMINA_DRAIN_HOLD_BREATH_EXPONENT = 4.0; //holding breath exponent
	
	const int 	STAMINA_GAIN_JOG_PER_SEC = 2; //in units (how much of stamina units is gained while jogging)
	const int 	STAMINA_GAIN_WALK_PER_SEC = 4; //in units (how much of stamina units is gained while walking)
	const int 	STAMINA_GAIN_IDLE_PER_SEC = 5; //in units (how much of stamina units is gained while iddling)
	const int	STAMINA_GAIN_SWIM_PER_SEC = 1; //in units (how much of stamina units is gained while slowly swim)
	const int	STAMINA_GAIN_LADDER_PER_SEC = 1; //in units (how much of stamina units is gained while slowly swim)
	const int	STAMINA_GAIN_ROLL_PER_SEC = 0; //in units (how much of stamina units is gained while rolling)
	const float STAMINA_GAIN_BONUS_CAP = 3.0; //in units (tells how much extra units can be added at best to stamina regain)
	
	const float STAMINA_KG_TO_STAMINAPERCENT_PENALTY = 1.75; //in units (by how many  units is max stamina bar reduced for each 1 kg of load weight)
	const float STAMINA_MIN_CAP = 5; //in units (overload won't reduce max stamina bar under this value)
	const float STAMINA_HOLD_BREATH_THRESHOLD_ACTIVATE = 10; // in units
	const float STAMINA_HOLD_BREATH_THRESHOLD_DRAIN = 0; // in units
	const float STAMINA_JUMP_THRESHOLD = 25; // in units
	const float STAMINA_VAULT_THRESHOLD = 20; // in units
	const float STAMINA_CLIMB_THRESHOLD = 42; // in units
	const float STAMINA_ROLL_THRESHOLD = 5; // in units
	
	
	const float STAMINA_MELEE_HEAVY_THRESHOLD = STAMINA_DRAIN_MELEE_HEAVY; // in units (how many units we need to make a heavy hit in melee)
	const float STAMINA_MELEE_EVADE_THRESHOLD = 8; // in units
	const float STAMINA_REGEN_COOLDOWN_DEPLETION = 0.45; // in secs (how much time we will spend in cooldown before the stamina will starts with regeneration)
	const float STAMINA_REGEN_COOLDOWN_EXHAUSTION = 0.5;
	const float STAMINA_WEIGHT_LIMIT_THRESHOLD = 6000; //! in grams (weight where the player is not penalized by stamina)
	const float STAMINA_KG_TO_GRAMS = 1000; //for kg to g conversion
	const float STAMINA_SYNC_RATE = 0.5; //in secs
	const float STAMINA_MAX = 100;
	/** @}*/
	
	/**
	 * \defgroup EnvironmentCfg Configurations for Environment class
	 * \desc Configurations for Environment class
	 * @{
	 */
	const float ENVIRO_TICK_RATE 						= 2;		//! in secs. how often should enviro effet process
	const float	ENVIRO_TICKS_TO_WETNESS_CALCULATION 	= 2;	  	//! each X (ticks) is processed wetness on items on player
	const float ENVIRO_TICK_ROOF_RC_CHECK 				= 10;	  	//! in secs. how often we should check if player is under the roof (raycast)
	const float ENVIRO_WET_INCREMENT 					= 0.01;	  	//! amount of wetness added to items wet value each tick if is raining
	const float ENVIRO_DRY_INCREMENT 					= 0.00005; 	//! amount of wetness subtracted from items wet value each tick if is not raining due to player heat
	const float ENVIRO_SUN_INCREMENT 					= 0.002;	//! (not used) amount of wetness subtracted from items wet value each tick if is not raining due to sun
	const float ENVIRO_FIRE_INCREMENT					= 23.5;		//! how much is the generic temp effect increased when player is next to a fireplace
	const float ENVIRO_CLOUD_DRY_EFFECT 				= 0.7;		//! how many % of ENVIRO_SUN_INCREMENT is reduced by cloudy sky
	const float ENVIRO_FOG_DRY_EFFECT 					= 0.9;		//! how many % of ENVIRO_SUN_INCREMENT is reduced by fog
	const float ENVIRO_CLOUDS_TEMP_EFFECT 				= 0.35;		//! how many % of environment temperature can be lowered by clouds
	const float ENVIRO_FOG_TEMP_EFFECT 					= 0.24;		//! how many % of environment temperature can be lowered by fog
	const float ENVIRO_WET_PENALTY 						= 0.5;		//! at which state of item wetness (0-1) will heat isolation start having negative effect on heat comfort of item
	const float ENVIRO_WET_PASSTHROUGH_COEF 			= 0.1;		//! how many times slower is wetting/drying items in backpacks
	const float ENVIRO_ITEM_HEAT_TRANSFER_COEF 			= 0.01;		//! converts temperature of items to entities heatcomfort gain
	const float ENVIRO_WATER_TEMPERATURE_COEF 			= 1.5;		//! how many time is water colder than air
	const float ENVIRO_DEFAULT_ENTITY_HEAT 				= 2.5;		//! heat entity generates if not moving
	const float ENVIRO_TEMPERATURE_HEIGHT_REDUCTION 	= 0.02;		//! amount of ?C reduced for each 100 meteres of height above water level
	const float ENVIRO_TEMPERATURE_INSIDE_COEF 			= 0.085;	//! increases temp in interiors
	const float ENVIRO_TEMPERATURE_INSIDE_VEHICLE_COEF	= 0.051;	//! increases temp inside vehicles
	const float ENVIRO_TEMPERATURE_UNDERROOF_COEF		= 0.072;
	const float ENVIRO_TEMPERATURE_WIND_COEF			= 2.5;		//! windchill effect on base temperature
	const float ENVIRO_WIND_EFFECT 						= 0.25;		//! amount of % wind affect drying/wetting
	const float ENVIRO_HIGH_NOON 						= 12;		//! when is sun highest on sky
	
	const float ENVIRO_HEATCOMFORT_HEADPARTS_WEIGHT		= 0.3;		//! how much this head parts (clothing) affects final heatcomfort
	const float ENVIRO_HEATCOMFORT_BODYPARTS_WEIGHT		= 1.0;		//! how much this body parts (clothing) affects final heatcomfort
	const float ENVIRO_HEATCOMFORT_FEETPARTS_WEIGHT		= 0.5;		//! how much this feet parts (clothing) affects final heatcomfort
	const float ENVIRO_HEATISOLATION_BACK_WEIGHT		= 0.45;		//! weight of back for the sum of heat isolation
	const float ENVIRO_HEATISOLATION_VEST_WEIGHT		= 0.64;		//! weight of vest for the sum of heat isolation
	const float ENVIRO_LOW_TEMP_LIMIT					= -40;		//! lowest temperature(deg Celsius) where the player gets lowest possible heat comfort (-1)
	const float ENVIRO_HIGH_TEMP_LIMIT					= 50;		//! highest temperature(deg Celsius) where the player gets highest possible heat comfort (1)
	const float ENVIRO_PLAYER_COMFORT_TEMP				= 26;		//! comfort temperature of environment for the player
	const float ENVIRO_TEMP_EFFECT_ON_PLAYER			= 60;		//! impact of enviro temperature on player (lower value = higher, cannot be zero or below!)
	const float ENVIRO_PLAYER_HEATBUFFER_DECREASE		= 0.34;		//! Multiplier of enviro temperature for heat buffer decrease (after its static timer runs out)
	const float ENVIRO_PLAYER_HEATBUFFER_INCREASE		= 0.51;		//! How much heat buffer increases per one enviro tick
	const float ENVIRO_PLAYER_HEATBUFFER_TICK			= 0.011;	//! Heat buffer static timer tick (set for 2s enviro tick, 180s to 1.0)
	
	//! impact of item wetness to the heat isolation
	const float ENVIRO_ISOLATION_WETFACTOR_DRY			= 1.0;
	const float ENVIRO_ISOLATION_WETFACTOR_DAMP			= 0.9;
	const float ENVIRO_ISOLATION_WETFACTOR_WET			= 0.75;
	const float ENVIRO_ISOLATION_WETFACTOR_SOAKED		= 0.5;
	const float ENVIRO_ISOLATION_WETFACTOR_DRENCHED 	= 0.0;
	//! impact of item health (state) to the heat isolation
	const float ENVIRO_ISOLATION_HEALTHFACTOR_PRISTINE  = 1.0;
	const float ENVIRO_ISOLATION_HEALTHFACTOR_WORN		= 0.9;
	const float ENVIRO_ISOLATION_HEALTHFACTOR_DAMAGED	= 0.8;
	const float ENVIRO_ISOLATION_HEALTHFACTOR_B_DAMAGED = 0.5;
	const float ENVIRO_ISOLATION_HEALTHFACTOR_RUINED  	= 0.0;
	
	const float ENVIRO_TEMP_SOURCES_LOOKUP_RADIUS		= 20.0;
	
	// --
	//! various damage per second constants
	const float FIRE_ATTACHMENT_DAMAGE_PER_SECOND 		= 0.07;		//! damage per second dealt to attachment by fire
	
	/** @}*/
	
	/**
	 * \defgroup CarFluidsConstants Cars Fluids 
	 * \desc Constants for car fluids
	 * @{
	 */
	const int CARS_FLUIDS_TICK 		= 1;
	const int CARS_LEAK_TICK_MIN 	= 0.02;
	const int CARS_LEAK_TICK_MAX 	= 0.05;
	const int CARS_LEAK_THRESHOLD	= 0.5;
	/** @}*/
	
	/**
	 * \defgroup CarContactConstants Cars Contact 
	 * \desc Constants for car contact event
	 * @{
	 */
	const float CARS_CONTACT_DMG_THRESHOLD = 750.0;
	const float CARS_CONTACT_DMG_MIN = 150.0;
	const float CARS_CONTACT_DMG_KILLCREW = 3000.0;
	/** @}*/
	
	/**
	 * \defgroup ItemHealth Item Health States (ItemBase.GetHealthLevel)
	 * \desc Constants for Item Health States
	 * @{
	 */
	const int STATE_RUINED 		 	= 4;
	const int STATE_BADLY_DAMAGED 	= 3;
	const int STATE_DAMAGED 	  	= 2;
	const int STATE_WORN 		  	= 1;
	const int STATE_PRISTINE 	  	= 0;
	/** @}*/
	
	/**
	 * \defgroup ItemDamage Item Damage Relative Values (ItemBase.GetHealth01)
	 * \desc Constants for relative Item Health handling
	 * @{
	 */
	const float DAMAGE_PRISTINE_VALUE 		= 1.0;
	const float DAMAGE_WORN_VALUE 			= 0.7;
	const float DAMAGE_DAMAGED_VALUE 		= 0.5;
	const float DAMAGE_BADLY_DAMAGED_VALUE 	= 0.3;
	const float DAMAGE_RUINED_VALUE 		= 0.0;
	/** @}*/
	
	/**
	 * \defgroup ItemWetness Item Wetness States (ItemBase.GetWet)
	 * \desc Constants for Item Wetness States
	 * @{
	 */
	const float STATE_DRENCHED		= 0.8;
	const float STATE_SOAKING_WET	= 0.5;
	const float STATE_WET			= 0.25;
	const float STATE_DAMP			= 0.05;
	const float STATE_DRY			= 0;
	/** @}*/
	
	const int OPTICS_STATE_DAY	 		= 0; //default state for most optics
	const int OPTICS_STATE_NIGHTVISION 	= 1;
	//const int OPTICS_STATE_OTHER	 	= 2;
	
	const float WETNESS_RATE_WETTING_INSIDE = 0.0020;
	const float WETNESS_RATE_WETTING_LIQUID = 0.015;
	const float WETNESS_RATE_DRYING_INSIDE  = -0.0016;
	const float WETNESS_RATE_DRYING_GROUND  = -0.0008;
	
	const float TEMPERATURE_RATE_COOLING_INSIDE = -0.17;
	const float TEMPERATURE_RATE_COOLING_GROUND = -0.34;
	const float TEMPERATURE_RATE_COOLING_PLAYER = -0.35; // celsius per second
	
	const float HEATISO_THRESHOLD_BAD = 0.2;
	const float HEATISO_THRESHOLD_LOW = 0.4;
	const float HEATISO_THRESHOLD_MEDIUM = 0.6;
	const float HEATISO_THRESHOLD_HIGH = 0.8;
	
	/**
	 * \defgroup Barrel Barrel related functions constants
	 * \desc Barrel related functions constants
	 * @{
	 */
	const int BAREL_LIME_PER_PELT = 100; //grams per pelt 
	const int BAREL_BLEACH_PER_CLOTH = 50; //ml per item
	const float BAREL_LIME_PER_PLANT = 50; //grams per gram
	/** @}*/
	
	/**
	 * \defgroup FOVSettings FOV settings
	 * \desc Constats for setting of DayZPlayerCamera FOV
	 */
	
	//! FOV (vertical angle/2) in radians. Take care to modify also in "basicDefines.hpp"
	const float DZPLAYER_CAMERA_FOV_EYEZOOM		= 0.3926;	// 45deg
	const float DZPLAYER_CAMERA_FOV_IRONSIGHTS	= 0.5236;	// 60deg
	
	const string DEFAULT_CHARACTER_NAME = "#str_cfgvehicles_survivor0"; //experiment, used to be "Survivor"
	
	const int DEFAULT_CHARACTER_MENU_ID = -1;
	
	const float CLEAN_UNRESTRAIN_DAMAGE = 10;
	
	//! Camera shake
	const int CAMERA_SHAKE_GRENADE_DISTANCE = 40;
	const int CAMERA_SHAKE_ARTILLERY_DISTANCE = 200;
	const int CAMERA_SHAKE_ARTILLERY_DISTANCE2 = 40000; // Square distance used for distance check
	
	//! Wood mining GameConstants, scaled by output. Should not be lower than ~1.5s!
	const float MINING_WOOD_FAST = 2.0;
	const float MINING_WOOD_MEDIUM = 2.0;
	const float MINING_WOOD_SLOW = 3.0;
	
	//! Non-lethal projectile damage QuantityConversions
	const float PROJECTILE_CONVERSION_ANIMALS 	= 0.1;
	const float PROJECTILE_CONVERSION_INFECTED 	= 0.44;
	const float PROJECTILE_CONVERSION_PLAYERS 	= 0.1;
	
	const float ROOF_CHECK_RAYCAST_DIST = 20.0;
	
	//! misc
	const float ITEM_TEMPERATURE_TO_EXPLODE_MIN = 100;
	
	/**
	 * \defgroup LifetimeRefresherConstants Lifetime Refresher functionality constants
	 * \desc Constats for lifetime refresher (radius, default lifetime, frequency...)
	 */
	const int REFRESHER_MAX_DURATION_DEFAULT		= 3600 * 24 * 40; 	//max duration of refresher in seconds - 40 days (+ 5 days final refresh )
	const int REFRESHER_FREQUENCY_DEFAULT 			= 3600 * 24 * 5;	//frequency of lifetime refreshes/refresher time decreases - 5 days 
	const float REFRESHER_RADIUS 					= 60; 				//meters
	// lifetime of refresher itself is in db (3600 * 24 * 7 = 604800 )
	
	/**
	 * \defgroup FoodDecayConstants Food decay constants
	 * \desc Constants for decay of various types of food.
	 */
	const float DECAY_FOOD_RAW_MEAT = 21600;
	const float DECAY_FOOD_RAW_CORPSE = 32400;
	const float DECAY_FOOD_RAW_FRVG = 43200;
	const float DECAY_FOOD_BOILED_MEAT = 259200;
	const float DECAY_FOOD_BOILED_FRVG = 172800;
	const float DECAY_FOOD_BAKED_MEAT = 345600;
	const float DECAY_FOOD_BAKED_FRVG = 259200;
	const float DECAY_FOOD_DRIED_MEAT = 691200;
	const float DECAY_FOOD_CAN_OPEN = 172800;
	const int DECAY_FOOD_FRVG_DRIED_CHANCE = 43;
	const int DECAY_TIMER_RANDOM_PERCENTAGE = 25;
	const float DECAY_RATE_ON_PLAYER = 2.5;
	
	const int RESPAWN_MODE_CUSTOM = 0;
	const int RESPAWN_MODE_RANDOM = 1;
	
	//----------------------------------------------------------
	//							AI
	//----------------------------------------------------------
	
	const float AI_ATTACKSPEED = 1.5;
	const float AI_MAX_BLOCKABLE_ANGLE = 60; // The angle under which an infected must be relative to player to be blockable
	const float AI_CONTAMINATION_DMG_PER_SEC = 3; // The amount of damage AI agents take per contaminated area damage event
	
	//----------------------------------------------------------
	//						   MELEE
	//----------------------------------------------------------
	
	const float PVP_MAX_BLOCKABLE_ANGLE = 60; // The angle under which a Player must be relative to player to be blockable
}