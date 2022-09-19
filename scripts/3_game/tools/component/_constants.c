// Commands using action data chanel sync
const int INPUT_UDT_STANDARD_ACTION_START			= 1;
const int INPUT_UDT_STANDARD_ACTION_END_REQUEST		= 2;
const int INPUT_UDT_STANDARD_ACTION_INPUT_END		= 3;
const int INPUT_UDT_QUICKABARSHORTCUT				= 4;
const int INPUT_UDT_MELEE_TARGET					= 5;
const int INPUT_UDT_WEAPON_ACTION					= 6;
const int INPUT_UDT_ITEM_MANIPULATION				= 7;
const int INPUT_UDT_INVENTORY						= 8;
const int INPUT_UDT_ADVANCED_PLACEMENT				= 9;
//const int INPUT_UDT_WEAPON_REMOTE_SYNC			= 9; // obsolette
const int INPUT_UDT_WEAPON_REMOTE_EVENT				= 10;
const int INPUT_UDT_USER_MUTE_XBOX					= 11;
const int INPUT_UDT_HAND_REMOTE_EVENT				= 12;
//const int INPUT_UDT_USER_SYNC_PERMISSIONS			= 12;// obsolete
const int INPUT_UDT_WEAPON_LIFT_EVENT 				= 13;
const int INPUT_UDT_RESET_ADS						= 14;
const int INPUT_UDT_GESTURE							= 15;

// ELECTRICITY SYSTEM CONSTANTS

// Plug types
const int PLUG_UNDEFINED 				= 0;
const int PLUG_BATTERY_SLOT 			= 1;
const int PLUG_COMMON_APPLIANCE 		= 2;
const int PLUG_CROCODILE_PINS 			= 3;
const int PLUG_CROCODILE_PINS_CHARGER 	= 4;
const int PLUG_PAS_DEVICE	 			= 5;
const int PLUG_BARBED_WIRE				= 6;
const int PLUG_GAS_CAN_SLOT				= 7;
const int PLUG_VEHICLE_BATTERY_PINS		= 8;

// Automatic attachment actions
const int NO_ATTACHMENT_ACTION 			= 0;
const int PLUG_THIS_INTO_ATTACHMENT 	= 1;
const int PLUG_ATTACHMENTS_INTO_THIS 	= 2;