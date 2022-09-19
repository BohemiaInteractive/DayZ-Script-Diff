enum EClientKicked
{
	UNKNOWN = -1,		// -1 must always be UNKNOWN
	OK = 0,				// 0 must always be OK
	
	SERVER_EXIT,		// Server last ditch effort to try and disconnect players
	KICK_ALL_ADMIN,		// Admin kicked all (RCON)
	KICK_ALL_SERVER,	// Server kicked all
	
	TIMEOUT,			// Players network timed out
	LOGOUT,				// Player logged out (does not need a message as it is not an "error")
	KICK,				// Generic kick	
	BAN,				// Player was banned
	PING,				// Player went over ping limit
	MODIFIED_DATA,		// Player has modified data or has a corrupted install
	UNSTABLE_NETWORK,	// Player internet is too unstable
	SERVER_SHUTDOWN,	// Server is shutting down and therefore kicks all players	
	
	NOT_WHITELISTED,	// Player is not whitelisted
	NO_IDENTITY,		// No identity received while creating identity
	INVALID_UID,		// UID was incorrect while creating identity
	BANK_COUNT,			// Amount of banks was changed
	ADMIN_KICK,			// Kicked by admin
	INVALID_ID,			// Player has an invalid ID
	
	INPUT_HACK,			// Player is sending more Inputs than possible
	
	QUIT,				// Player closed the game
	
	// LoginMachine kicks (0x0030)
	LOGIN_MACHINE_ERROR = 48,	// Generic LoginMachine error (fallback)
	PLAYER_STATE_TIMEOUT,		// Player spent too much time in one state of the LoginMachine
	IDENTITY_NOT_CREATED,		// Player was assigned but identity was not yet created
	DUPLICATE_UID,				// Player tried to log in while not being logged out yet
	DB_PING_RESPONSE,			// Unknown/unexpected database response
	DB_IDENTITY,				// Identity not found
	DB_TIMEOUT,					// Database connection timed out
	DB_BAD_VERSION,				// Unsupported version of character data
	DB_ERROR,					// Database error
	DB_APP_ERROR,				// Something went wrong on server
	DB_APP_EXIT,				// Application is exiting
	DB_BAD_DATA,				// Character data check failed
	DB_GET_RESPONSE,			// Unknown/unexpected database response
	NO_PLAYER,					// Player was not created on server
	CREATE_NEW_CLIENT,			// Creating of the client with a new character failed
	CREATE_LOADED_CHAR,			// Creating the existing character failed
	LOAD_LOADED_CHAR,			// Loading the existing character failed
	NO_NETWORK_INFO,			// Network info for player not found
	CREATE_READY_CLIENT,		// Creating of the client with loaded character failed
	RECONNECT_INFO,				// No info for reconnecting character
	RECONNECT_IDENTITY,			// No identity for reconnecting character
	RECONNECT_CHAR,				// No character for reconnecting character
	
	// RespawnMachine Kicks (0x0050)
	INIT_RESPAWN_IDENTITY = 80,	// No identity when initializing respawn
	RESPAWN_IDENTITY,			// No identity when respawning
	NEW_RESPAWN_NETWORK,		// No network info when respawning
	NEW_RESPAWN_IDENTITY,		// No identity when respawning
	NEW_RESPAWN_INFO,			// No info when respawning
	RESPAWN_NEW_CLIENT,			// Creating of client with new respawned character failed
	
	// PlayerVerification Kicks (0x0060)
	PLAYER_VERIFICATION = 96,	// Generic PlayerVerification error (fallback)
	NO_INFO,					// No player info
	BROKEN_DATA,				// Data could not be verified because it is corrupt

	// VerificationError (0x0070)
	VERIFICATION_ERROR = 112,	// Generic Verification error (fallback)
	VE_UNKNOWN,					// Unknown/unexpected error
	VE_DECOMPRESS,				// Data received by Server is corrupted
	VE_MISSING_MOD,				// Client is missing a mod which is on Server
	VE_EXTRA_MOD,				// Client has a mod which is not on Server
	VE_CLIENT_CORRUPT,			// Client installation is corrupt
	VE_SERVER_CORRUPT,			// Server installation is corrupt
	VE_UNEXPECTED_MOD_PBO,		// Client has a PBO which is not part of Server
	VE_UM_ROGUE_PBO,			// Client has a PBO which is not part of Server
	VE_UM_CLIENT_UPDATED,		// Extra PBO most likely because Client has a newer version of mod
	VE_UM_SERVER_UPDATED,		// Extra PBO most likely because Server has a newer version of mod
	VE_UNEXPECTED_SOURCE,		// The source of the Extra PBO could not be determined
	VE_PATCHED_PBO,				// Client has a patched PBO
	VE_INTEGRITY,				// Client has a corrupted file
	VE_MISSING_BISIGN,			// Client has a PBO which is missing a .bisign
	VE_MISSING_PBO,				// Missing PBO
	VE_M_ROGUE_PBO,				// Missing PBO from unknown origins
	VE_M_CLIENT_UPDATED,		// Missing PBO most likely because Client has a newer version of mod
	VE_M_SERVER_UPDATED,		// Missing PBO most likely because Server has a newer version of mod
	VE_M_SERVER_CORRUPT,		// Server installation is corrupt
	VE_M_CLIENT_CORRUPT,		// Client installation is corrupt
	VE_M_UNEXPECTED_SOURCE,		// The source of the Missing PBO could not be determined
	VE_GPROJ,					// The Client and Server .gproj are not equal
	
	// PBO Mismatch (0x0090)
	PBO_MISMATCH = 144,			// Server and Client are using different PBO
	PBO_VERSION_MISMATCH,		// Server and Client are using different PBO versions
	PBO_CLIENT_UPDATED,			// Client has a more recent version
	PBO_SERVER_UPDATED,			// Server has a more recent version
	
	// AuthError Kicks (0x00A0)
	AUTH_ERROR = 160,						// Generic Auth error (fallback)
	AUTH_NO_USER,							// The user was not found
	
	// Steam Validate (0x00B0)
	AUTH_INVALID_STEAM_VALIDATE = 176,		// Unknown error (API update needed?)
	AUTH_NO_SERVER,							// Server not initialized properly
	AUTH_INVALID_TICKET,					// Ticket is not valid.
	AUTH_DUPLICATE_REQUEST,					// A ticket has already been submitted for this steamID
	AUTH_INVALID_VERSION,					// Ticket is from an incompatible interface version
	AUTH_GAME_MISMATCH,						// Ticket is not for this game
	AUTH_EXPIRED_TICKET,					// Ticket has expired
	
	// Steam OnValidate (0x00C0)
	AUTH_INVALID_STEAM_ONVALIDATE = 192,	// Unknown error (API update needed?)
	AUTH_USER_NOT_CONNECTED,				// The user in question is not connected to steam
	AUTH_LICENCE,							// The license has expired.
	AUTH_SERVICE_BAN,						// The user is VAC banned for this game.
	AUTH_LOGGED_IN_ELSEWHERE,				// The user account has logged in elsewhere.
	AUTH_TIMEOUT,							// VAC has been unable to perform anti-cheat checks on this user
	AUTH_CANCELED,							// The ticket has been canceled by the issuer
	AUTH_ALREADY_USED,						// This ticket has already been used, it is not valid.
	AUTH_TICKET_INVALID,					// This ticket is not from a user instance currently connected to steam.
	AUTH_PUBLISHER_BAN,						// The user is banned for this game.
	
	// Bios Validate (0x00D0)
	AUTH_INVALID_BIOS_VALIDATE = 208,		// Unknown error (API update needed?)
	AUTH_BIOS_REQUEST_CREATE,				// Failed to create the request to identify user.
	AUTH_BIOS_REQUEST_SEND,					// Failed to send request
	AUTH_BIOS_REQUEST_WAIT,					// Failed to receive response
	AUTH_UNKNOWN_BIOS_VALIDATE,				// Unknown error
	
	// Bios OnValidate (0x00E0)
	AUTH_INVALID_BIOS_ONVALIDATE = 224,		// Unknown error (API update needed?)
	AUTH_BIOS_WRONG_RESPONSE,				// Response has unexpected format
	AUTH_UNKNOWN_BIOS_ONVALIDATE,			// Unknown error

	// BattlEye Kicks (0x00F0)
	BATTLEYE = 240,				// Kick administred by BattlEye (Can also be Admin Kick)
	
	// Unauthorized Network Message (0x0100)
	UNM1 = 256,
	UNM2,
	UNM3,
	UNM4,
	UNM5,
	UNM6,
};

#ifdef PLATFORM_WINDOWS
const string AUTH_PREFIX = "#STR_MP_STEAM_AUTH_FAILED\n";
#else
#ifdef PLATFORM_XBOX
const string AUTH_PREFIX = "#STR_xbox_authentification_fail\n";
#else
#ifdef PLATFORM_PS4
const string AUTH_PREFIX = "#STR_ps4_authentification_fail\n";
#else
const string AUTH_PREFIX = "#STR_authentification_fail\n";
#endif
#endif
#endif

const string LOGIN_PREFIX = "#STR_LOGIN_PREFIX"+"\n";
const string RESPAWN_PREFIX = "#STR_RESPAWN_PREFIX"+"\n";
const string VE_PREFIX = "#STR_VE_PREFIX"+"\n";

const string VERIFY = "\n"+"#STR_VERIFY";
const string MODS = "\n"+"#STR_MODS0"+"\n"+"#STR_MODS1"+"\n";

const string SERVER_MUST_UPDATE = "\n"+"#STR_S_UPDATE";
const string CLIENT_MUST_UPDATE = "\n"+"#STR_C_UPDATE" + VERIFY;

const string UNM_MESSAGE = "#STR_UNM_MESSAGE";

class ClientKickedModule : ErrorHandlerModuleScript
{
	void ClientKickedModule()
	{
		SetCategory(ErrorCategory.ClientKicked);
	}
	
	override void InitOptionalVariables()
	{
		super.InitOptionalVariables();
		
		m_Header = "#menu_warning";
		m_Prefix = "#STR_MP_KICKED_CLIENT\n";
	}
	
	override void FillErrorDataMap()
	{
		super.FillErrorDataMap();
		
		InsertDialogueErrorProperties(EClientKicked.SERVER_EXIT,						"#STR_server_shutdown");
		InsertDialogueErrorProperties(EClientKicked.KICK_ALL_ADMIN,						"#STR_kickAll_admin");
		InsertDialogueErrorProperties(EClientKicked.KICK_ALL_SERVER,					"#STR_kickAll_server");
		
		InsertDialogueErrorProperties(EClientKicked.TIMEOUT,							"#STR_MP_TIMEOUT_CLIENT");
		InsertErrorProperties(EClientKicked.LOGOUT); // No handling, just register it exists
		InsertDialogueErrorProperties(EClientKicked.KICK,								"");
		InsertDialogueErrorProperties(EClientKicked.BAN,								"#STR_MP_BANNED_CLIENT");
		InsertDialogueErrorProperties(EClientKicked.PING,								"#STR_MP_MAX_PING");
		InsertSplitDialogueErrorProperties(EClientKicked.MODIFIED_DATA,					"#STR_modified_data" + VERIFY, "#STR_modified_data");
		InsertSplitDialogueErrorProperties(EClientKicked.UNSTABLE_NETWORK,				"#STR_unstable_network_client", "#STR_unstable_network_server");
		InsertDialogueErrorProperties(EClientKicked.SERVER_SHUTDOWN,					"#STR_server_shutdown");

		InsertDialogueErrorProperties(EClientKicked.NOT_WHITELISTED,					"#multi_server_not_whitelisted_message");
		InsertDialogueErrorProperties(EClientKicked.NO_IDENTITY,						"#STR_no_identity");
		InsertDialogueErrorProperties(EClientKicked.INVALID_UID,						"#STR_invalid_UID");
		InsertSplitDialogueErrorProperties(EClientKicked.BANK_COUNT,					"#STR_bank_count" + VERIFY, "#STR_bank_count");
		InsertDialogueErrorProperties(EClientKicked.ADMIN_KICK,							"#STR_admin_kick");
		InsertDialogueErrorProperties(EClientKicked.INVALID_ID,							"#STR_invalid_ID");
		
		InsertDialogueErrorProperties(EClientKicked.INPUT_HACK,							"#STR_INPUT_HACK");
		
		InsertErrorProperties(EClientKicked.QUIT); // No handling, just register it exists

		// LoginMachine kicks			
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.LOGIN_MACHINE_ERROR,				"#server_browser_error_unknown", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.PLAYER_STATE_TIMEOUT,				"#STR_player_state_timeout", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.IDENTITY_NOT_CREATED,				"#STR_identity_not_created", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DUPLICATE_UID,					"#STR_duplicate_UID", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_PING_RESPONSE,					"#STR_db_ping_response", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_IDENTITY,						"#STR_db_identity", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_TIMEOUT,						"#STR_db_timeout", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_BAD_VERSION,					"#STR_db_bad_version", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_ERROR,							"#STR_db_error", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_APP_ERROR,						"#STR_app_error", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_APP_EXIT,						"#STR_app_exit", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_BAD_DATA,						"#menu_character_check_fail", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.DB_GET_RESPONSE,					"#STR_db_get_response", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NO_PLAYER,						"#STR_no_player", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.CREATE_NEW_CLIENT,				"#STR_create_new_client", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.CREATE_LOADED_CHAR,				"#STR_create_loaded_char", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.LOAD_LOADED_CHAR,					"#STR_load_loaded_char", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NO_NETWORK_INFO,					"#STR_no_network_info", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.CREATE_READY_CLIENT,				"#STR_create_ready_client", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.RECONNECT_INFO,					"#STR_reconnect_info", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.RECONNECT_IDENTITY,				"#STR_reconnect_identity", LOGIN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.RECONNECT_CHAR,					"#STR_reconnect_char", LOGIN_PREFIX);

		// RespawnMachine Kicks			
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.INIT_RESPAWN_IDENTITY,			"#STR_init_respawn_identity", RESPAWN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.RESPAWN_IDENTITY,					"#STR_respawn_identity", RESPAWN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NEW_RESPAWN_NETWORK,				"#STR_new_respawn_network", RESPAWN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NEW_RESPAWN_IDENTITY,				"#STR_new_respawn_identity", RESPAWN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NEW_RESPAWN_INFO,					"#STR_new_respawn_info", RESPAWN_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.RESPAWN_NEW_CLIENT,				"#STR_respawn_new_client", RESPAWN_PREFIX);

		// PlayerVerification Kicks
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.PLAYER_VERIFICATION,			"#server_browser_error_unknown", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.NO_INFO,						"#STR_no_info", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.BROKEN_DATA,					"#STR_broken_data", VE_PREFIX);

		// VerificationError
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VERIFICATION_ERROR,			"#server_browser_error_unknown", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_UNKNOWN,					"#server_browser_error_unknown", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_DECOMPRESS,				"#STR_ve_decompress", VE_PREFIX);
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.VE_MISSING_MOD,			"#STR_ve_missing_mod" + MODS, VE_PREFIX, "#STR_ve_missing_mod");
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.VE_EXTRA_MOD,			"#STR_ve_extra_mod" + MODS, VE_PREFIX, "#STR_ve_extra_mod");
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_CLIENT_CORRUPT,			"#STR_ve_client_corrupt" + VERIFY, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_SERVER_CORRUPT,			"#STR_ve_server_corrupt", VE_PREFIX);
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.VE_UNEXPECTED_MOD_PBO,	"#STR_ve_unexpected_mod_pbo0"+"\n"+"#STR_ve_unexpected_mod_pbo1"+"\n"+"#STR_ve_unexpected_mod_pbo2"+"\n"+"#STR_ve_unexpected_mod_pbo3"+"\n", VE_PREFIX, "#STR_ve_unexpected_mod_pbo0");
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.VE_UM_ROGUE_PBO,			"#STR_ve_unexpected_mod_pbo0"+"\n"+"#STR_ve_unexpected_mod_pbo1"+"\n"+"#STR_ve_unexpected_mod_pbo2"+"\n"+"#STR_ve_unexpected_mod_pbo3"+"\n", VE_PREFIX, "#STR_ve_unexpected_mod_pbo0");
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_UM_CLIENT_UPDATED,			"#STR_ve_um_client_updated" + SERVER_MUST_UPDATE, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_UM_SERVER_UPDATED,			"#STR_ve_um_server_updated" + CLIENT_MUST_UPDATE, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_UNEXPECTED_SOURCE,			"#STR_ve_unexpected_source", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_PATCHED_PBO,				"#STR_ve_patched_pbo" + VERIFY, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_INTEGRITY,					"#STR_ve_integrity" + VERIFY, VE_PREFIX);
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.VE_MISSING_BISIGN,		"#STR_ve_missing_bising" + VERIFY, VE_PREFIX, "#STR_ve_missing_bising");
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_MISSING_PBO,				"#STR_ve_missing_pbo", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_ROGUE_PBO,				"#STR_ve_m_rogue_pbo0"+"\n"+"#STR_ve_m_rogue_pbo1", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_CLIENT_UPDATED,			"#STR_ve_m_client_updated" + SERVER_MUST_UPDATE, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_SERVER_UPDATED,			"#STR_ve_m_server_updated0"+"\n"+"#STR_ve_m_server_updated1" + CLIENT_MUST_UPDATE, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_SERVER_CORRUPT,			"#STR_ve_m_server_corrupt0"+"\n"+"#STR_ve_m_server_corrupt1"+"\n", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_CLIENT_CORRUPT,			"#STR_ve_m_client_corrupt0"+"\n"+"#STR_ve_m_client_corrupt1" + VERIFY, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_M_UNEXPECTED_SOURCE,		"#STR_ve_m_unexpected_source", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.VE_GPROJ,						VERIFY + " (.gproj)", VE_PREFIX);
		
		// PBO Mismatch
		InsertExtendedPrefixSplitDialogueErrorProperties(EClientKicked.PBO_MISMATCH,			"#STR_pbo_mismatch" + VERIFY, VE_PREFIX, "#STR_pbo_mismatch");
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.PBO_VERSION_MISMATCH,			"#STR_pbo_version_mismatch", VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.PBO_CLIENT_UPDATED,			"#STR_pbo_client_updated" + SERVER_MUST_UPDATE, VE_PREFIX);
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.PBO_SERVER_UPDATED,			"#STR_pbo_server_updated" + CLIENT_MUST_UPDATE, VE_PREFIX);
		
		// AuthError Kicks
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_ERROR,					"#server_browser_error_unknown", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_NO_USER,					"#STR_auth_no_user", AUTH_PREFIX);

		// Steam Validate
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_STEAM_VALIDATE,	"#server_browser_error_unknown", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_NO_SERVER,				"#STR_auth_no_server", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_TICKET,			"#STR_auth_invalid_ticket", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_DUPLICATE_REQUEST,		"#STR_auth_duplicate_request", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_VERSION,			"#STR_auth_invalid_version", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_GAME_MISMATCH,			"#STR_auth_game_mismatch", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_EXPIRED_TICKET,			"#STR_auth_expired_ticket", AUTH_PREFIX);

		// Steam OnValidate
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_STEAM_ONVALIDATE,"#server_browser_error_unknown", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_USER_NOT_CONNECTED,		"#STR_auth_user_not_connected", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_LICENCE,					"#STR_auth_licence", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_SERVICE_BAN,				"#STR_auth_service_ban", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_LOGGED_IN_ELSEWHERE,		"#STR_auth_logged_in_elsewhere", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_TIMEOUT,					"#STR_auth_timeout", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_CANCELED,				"#STR_auth_cancelled", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_ALREADY_USED,			"#STR_auth_already_used", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_TICKET_INVALID,			"#STR_auth_ticket_invalid", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_PUBLISHER_BAN,			"#STR_auth_publisher_ban", AUTH_PREFIX);

		// Bios Validate
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_BIOS_VALIDATE,	"#server_browser_error_unknown", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_BIOS_REQUEST_CREATE,		"#STR_auth_bios_request_create", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_BIOS_REQUEST_SEND,		"#STR_auth_bios_request_send", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_BIOS_REQUEST_WAIT,		"#STR_auth_bios_request_wait", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_UNKNOWN_BIOS_VALIDATE,	"#server_browser_error_unknown", AUTH_PREFIX);

		// Bios OnValidate
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_INVALID_BIOS_ONVALIDATE,	"#server_browser_error_unknown", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_BIOS_WRONG_RESPONSE,		"#STR_auth_bios_wrong_response", AUTH_PREFIX);
		InsertPrefixDialogueErrorProperties(EClientKicked.AUTH_UNKNOWN_BIOS_ONVALIDATE,	"#server_browser_error_unknown", AUTH_PREFIX);

		// BattlEye Kicks
		InsertExtendedPrefixDialogueErrorProperties(EClientKicked.BATTLEYE,	"", "BattlEye\n");
		
		// Unauthorized Network Message
		InsertDialogueErrorProperties(EClientKicked.UNM1, UNM_MESSAGE);
		InsertDialogueErrorProperties(EClientKicked.UNM2, UNM_MESSAGE);
		InsertDialogueErrorProperties(EClientKicked.UNM3, UNM_MESSAGE);
		InsertDialogueErrorProperties(EClientKicked.UNM4, UNM_MESSAGE);
		InsertDialogueErrorProperties(EClientKicked.UNM5, UNM_MESSAGE);
		InsertDialogueErrorProperties(EClientKicked.UNM6, UNM_MESSAGE);
	}
}

/*class ClientKickedModuleUI : UIScriptedMenu
{
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		super.OnModalResult(w, x, y, code, result);
		
		int error = ErrorModuleHandler.GetCodeFromError(code);		
		switch ( error )
		{
			default:
				break;
		}
		
		return true;
	}
}*/
