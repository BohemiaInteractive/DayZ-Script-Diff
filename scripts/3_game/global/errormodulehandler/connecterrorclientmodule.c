enum EConnectErrorClient
{
	UNKNOWN = -1,				// -1 must always be UNKNOWN
	OK = 0,						// 0 must always be OK

	SERVER_UNREACHABLE,			// Could not connect to server because it is not reachable (offline, restarting, ..)
	ALREADY_CONNECTING,			// Client is already attempting to join a server
	ERROR_MSG_SHOWED,			// Indicates there is an error on screen that has yet to be confirmed
	INCORRECT_CLIENT_STATE,		// The client is already connected or is connecting to a server
	INVALID_SESSION,			// The guid of the session is empty

	VERSION_MISMATCH,			// Mismatch between server and client version
	VERSION_MISMATCH_RB,		// Client build is lower than "requiredBuild" (server config)
	VERSION_MISMATCH_AB,		// Client build is higher than "allowedBuild" (server config)

	DLC_CHECK_FAILED,			// Client does not have required DLC
	EMPTY_PASSWORD,				// Player input an empty password
	
	PASSWORD,					// Server is password protected
	BE_LICENCE,					// Server is using BE and it has not yet been agreed to
};

class ConnectErrorClientModule : ErrorHandlerModuleScript
{
	void ConnectErrorClientModule()
	{
		SetCategory(ErrorCategory.ConnectErrorClient);
	}
	
	override void InitOptionalVariables()
	{
		super.InitOptionalVariables();
		
		m_Header = "#server_browser_connecting_failed";
	}
	
	override void FillErrorDataMap()
	{
		super.FillErrorDataMap();
		
		InsertDialogueErrorProperties(EConnectErrorClient.SERVER_UNREACHABLE,		"#STR_server_unreachable");
		InsertDialogueErrorProperties(EConnectErrorClient.ALREADY_CONNECTING,		"#STR_already_connecting");
		InsertDialogueErrorProperties(EConnectErrorClient.ERROR_MSG_SHOWED,			"#STR_error_msg_showed");
		InsertDialogueErrorProperties(EConnectErrorClient.INCORRECT_CLIENT_STATE, 	"#STR_incorrect_client_state");
		InsertDialogueErrorProperties(EConnectErrorClient.INVALID_SESSION,			"#STR_invalid_session");
		
		InsertDialogueErrorProperties(EConnectErrorClient.VERSION_MISMATCH,			"#multi_server_not_compatible_message");
		InsertDialogueErrorProperties(EConnectErrorClient.VERSION_MISMATCH_RB,		"#multi_server_not_compatible_message");
		InsertDialogueErrorProperties(EConnectErrorClient.VERSION_MISMATCH_AB,		"#multi_server_not_compatible_message");
		
		InsertDialogueErrorProperties(EConnectErrorClient.DLC_CHECK_FAILED,			"#mod_detail_info_warning");
		InsertDialogueErrorProperties(EConnectErrorClient.EMPTY_PASSWORD,			"#STR_empty_password");
		
		InsertErrorProperties(EConnectErrorClient.PASSWORD);
		InsertErrorProperties(EConnectErrorClient.BE_LICENCE);
	}

#ifndef NO_GUI	
	override void OnEvent(EventType eventTypeId, Param params)
	{
		switch (eventTypeId)
		{
			case MPSessionPlayerReadyEventTypeID:
				g_Game.GetUIManager().CloseSpecificDialog(m_LastErrorThrown);
				break;
			
			default:
				break;
		}
	}
#endif
}

/*class ConnectErrorClientModuleUI : UIScriptedMenu
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