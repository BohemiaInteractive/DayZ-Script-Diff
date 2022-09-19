enum EConnectErrorServer
{
	UNKNOWN = -1,			// -1 must always be UNKNOWN
	OK = 0,					// 0 must always be OK

	WRONG_PASSWORD,			// User entered wrong password
	WRONG_VERSION,			// Server and client do not have the same build while "forceSameBuild" is true
	CONNECT_ERROR,			// Unknown error
	SESSION_FULL,			// Server is full
	FILE_PATCHING,			// Bad file patching

	ALREADY_CONNECTED,		// Client is already connected
	HOST_ADDRESS,			// Was unable to resolve host address
	NET_CHANNEL,			// Client failed to create NetChannel
	CONNECT,				// Connecting itself has failed
	REQUEST_PLAYER,			// Failed to request player
	CREATE_PLAYER,			// Failed to create player
	TIMEOUT,				// Packages timed out
	UID,					// Failed to find uid	
	NET_CHANNEL_SERVER,		// Server failed to create NetChannel	
	ACTIVATE,				// Failed to activate session
	SERVER_LOCKED,			// Server is locked by admin
	SERVER_SHUTDOWN,		// Server is shutting down soon
	BUILD_SMALLER,			// Client has a build smaller than the Server
	BUILD_GREATER,			// Client has a build greater than the Server
	VERSION_SMALLER,		// Client has a version smaller than the Server
	VERSION_GREATER,		// Client has a version greater than the Server
	EQUAL_MOD,				// Server and Client do not have equal mods while "equalModRequired" is true
	CLIENT_DIAG,			// Client is using Diag exe while Server is not
	SERVER_DIAG,			// Server is using Diag exe while Client is not
};

class ConnectErrorServerModule : ErrorHandlerModuleScript
{
	void ConnectErrorServerModule()
	{
		SetCategory(ErrorCategory.ConnectErrorServer);
	}
	
	override void InitOptionalVariables()
	{
		super.InitOptionalVariables();
		
		m_Header = "#server_browser_connecting_failed";
	}
	
	override void FillErrorDataMap()
	{
		super.FillErrorDataMap();
			
		InsertDialogueErrorProperties(EConnectErrorServer.WRONG_PASSWORD, 		"#server_browser_wrong_password");
		InsertDialogueErrorProperties(EConnectErrorServer.WRONG_VERSION, 		"#server_browser_wrong_version");
		InsertDialogueErrorProperties(EConnectErrorServer.CONNECT_ERROR,		"#server_browser_wrong_connect_error");
		InsertDialogueErrorProperties(EConnectErrorServer.SESSION_FULL,			"#server_browser_session_full");
		InsertDialogueErrorProperties(EConnectErrorServer.FILE_PATCHING,		"#server_browser_bad_file_patching");

		InsertDialogueErrorProperties(EConnectErrorServer.ALREADY_CONNECTED, 	"#STR_already_connected");
		InsertDialogueErrorProperties(EConnectErrorServer.HOST_ADDRESS,			"#STR_host_address");
		InsertDialogueErrorProperties(EConnectErrorServer.NET_CHANNEL,			"#STR_net_channel");
		InsertDialogueErrorProperties(EConnectErrorServer.CONNECT,				"#STR_connect");
		InsertDialogueErrorProperties(EConnectErrorServer.REQUEST_PLAYER,		"#STR_request_player");
		InsertDialogueErrorProperties(EConnectErrorServer.CREATE_PLAYER,		"#STR_create_player");
		InsertDialogueErrorProperties(EConnectErrorServer.TIMEOUT,				"#STR_timeout");
		InsertDialogueErrorProperties(EConnectErrorServer.UID,					"#STR_UID");
		InsertDialogueErrorProperties(EConnectErrorServer.NET_CHANNEL_SERVER, 	"#STR_net_channel_server");
		InsertDialogueErrorProperties(EConnectErrorServer.ACTIVATE,				"#STR_activate");
		InsertDialogueErrorProperties(EConnectErrorServer.SERVER_LOCKED,		"#STR_MP_SESSION_LOCKED");
		InsertDialogueErrorProperties(EConnectErrorServer.SERVER_SHUTDOWN,		"#STR_shutdown_server");
		InsertDialogueErrorProperties(EConnectErrorServer.BUILD_SMALLER,		"#STR_build_smaller"+"\n"+"#STR_PLEASE_UPDATE");
		InsertDialogueErrorProperties(EConnectErrorServer.BUILD_GREATER,		"#STR_build_greater"+"\n"+"#STR_PLEASE_WAIT");
		InsertDialogueErrorProperties(EConnectErrorServer.VERSION_SMALLER,		"#STR_version_smaller"+"\n"+"#STR_PLEASE_UPDATE");
		InsertDialogueErrorProperties(EConnectErrorServer.VERSION_GREATER,		"#STR_version_greater"+"\n"+"#STR_PLEASE_WAIT");
		InsertDialogueErrorProperties(EConnectErrorServer.EQUAL_MOD,			"#STR_equal_mod");
		InsertDialogueErrorProperties(EConnectErrorServer.CLIENT_DIAG,			"#STR_client_diag");
		InsertDialogueErrorProperties(EConnectErrorServer.SERVER_DIAG,			"#STR_server_diag");
	}
	
	override void OnEvent(EventType eventTypeId, Param params)
	{
		switch (eventTypeId)
		{
			case MPSessionPlayerReadyEventTypeID:
#ifndef NO_GUI
				g_Game.GetUIManager().CloseSpecificDialog(m_LastErrorThrown);
#endif
				break;
			
			default:
				break;
		}
	}
}

/*class ConnectErrorServerModuleUI : UIScriptedMenu
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