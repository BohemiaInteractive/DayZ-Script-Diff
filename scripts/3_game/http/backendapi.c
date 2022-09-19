
/** @file */


	//! Backend error
	enum EBackendError
	{
		EBERR_OK,					// all OK
		EBERR_UNKNOWN,				// unknown error
		EBERR_DISABLED,				// backend is disabled
		EBERR_INVALID_STATE,		// called request from state where it is not possible (ie. reading data before logon and such)
		EBERR_BUSY,					// no request can be called - login/ auth in process
		EBERR_ALREADY_OFFLINE,		// state is already active
		EBERR_ALREADY_ONLINE,		// state is already active
		EBERR_ALREADY_REQUESTED,	// state already requested once!
		EBERR_LOGIN_FAILED,			// failed to logon
		EBERR_AUTH_FAILED,			// failed to authenticate
		EBERR_LOGIN_SUCCESS,		// logon successfull
		EBERR_AUTH_SUCCESS,			// authenticate successfull
		EBERR_CONFIGURATION_GET,	// configuration received
		EBERR_CEPROFILE_GET,		// CE profile configuration received
		EBERR_CHARACTER_GET,		// character data receieved
		EBERR_CHARACTER_UPDATE,		// character update done
	};
	
	//! Backend request
	enum EBackendRequest
	{
/*		// game api
		EBREQ_GAME_Test,					// environment test - dummy data read
		EBREQ_GAME_World,					// static world configuration read
		EBREQ_GAME_CEProfile,
		EBREQ_GAME_CharacterGet,			// character data read
		EBREQ_GAME_CharacterUpdate,			// character data update
//		#if BACKENDAPI_DEV_CHARACTER
		EBREQ_GAME_DevCharacterGet,			// dev character data read
		EBREQ_GAME_DevCharacterUpdate,		// dev character data update
//		#endif
		EBREQ_GAME_Heartbeat,*/
		// user api request <> response
		EBREQ_USER_Login,
		EBREQ_USER_Auth,
/*		// lobby api request <> response
		EBREQ_LOBBY_RoomsRegister,
		EBREQ_LOBBY_RoomsJoin,
		EBREQ_LOBBY_RoomsAcceptPlayer,
		EBREQ_LOBBY_RoomsHeartBeat,
		EBREQ_LOBBY_RoomsUpdate,
		EBREQ_LOBBY_RoomsRemovePlayer,
		EBREQ_LOBBY_RoomsSearch,
		EBREQ_LOBBY_RoomsGetByIds,
		EBREQ_LOBBY_RoomsGetByHostIds,
		EBREQ_LOBBY_GetActiveScenarios,
		// storage api request <> response
		EBREQ_STORAGE_GetFileTempURL,
		EBREQ_STORAGE_GetFileTempURLS2S,
		EBREQ_STORAGE_GetFile,
		EBREQ_STORAGE_DeleteFile,
		EBREQ_STORAGE_GetFileS2S,
		EBREQ_STORAGE_DeleteFileS2S,
		EBREQ_STORAGE_PatchFileS2S,
		EBREQ_STORAGE_Upload,
		EBREQ_STORAGE_UploadS2S,
		EBREQ_STORAGE_UploadZip,
		EBREQ_STORAGE_UploadZipS2S,
		EBREQ_STORAGE_Limits,
		EBREQ_STORAGE_LimitsS2S,
		// feedback request <> response
//		#if ONLINE_SLACKAPI
		EBREQ_SLACKAPI_PostMessage,
//		#endif*/
	};	
	
	//! Credential parameters
	enum EBackendCredentials
	{
		EBCRED_NAME,
		EBCRED_PWD,
		EBCRED_BASEURI,
	};


// -------------------------------------------------------------------------
// Callback interface for backend - must exist for the duration of request!
class BackendCallback : Managed
{
	/**
	\brief Called when data were recieved, you can ignore it when using callback to JsonStruct object with expand feature
	\param data Contain received data, may be JSON, plain text, XML or image
	\param size 
	*/
	void OnDataReceive( string data, int size )
	{
		Print("[BackendCallback] Data received, size=" + size);
		Print(data);
	}

	/**
	\brief Request finished with error result
	\param code Error code is type of EBackendError
	*/
	void OnError( int code )
	{
		Print("[BackendCallback] OnError: " + GetBackendApi().GetErrorCode(code));
	}

	/**
	\brief Request finished with success result
	\param code Code is type of EBackendRequest
	*/
	void OnSuccess( int code )
	{
		Print("[BackendCallback] OnSuccess()");
	}

	/**
	\brief Request not finished due to timeout
	*/
	void OnTimeout()
	{
		Print("[BackendCallback] OnTimeout");
	}

};


// -------------------------------------------------------------------------
// Backend API access
class BackendApi
{
	private void BackendApi() {}
	private void ~BackendApi() {}
	
	/**
	\brief Initiate backend - request processing
	*/
	proto native bool Initiate();
	/**
	\brief Shutdown backend - request processing
	*/
	proto native bool Shutdown();


	/**
	\brief Backend offline - authentication may be initiated
	*/
	proto native bool IsDisconnected();
	/**
	\brief Backend fully working and initialized
	*/
	proto native bool IsRuntime();
	/**
	\brief Backend is busy - authentication in process
	*/
	proto native bool IsBusy();
	
	
	//! Error code to string
	string GetErrorCode( int code )
	{
		string result;
	
		if ( code == EBackendError.EBERR_OK )
			result = "OK";
		else if ( code == EBackendError.EBERR_UNKNOWN )
			result = "Offline";
		else if ( code == EBackendError.EBERR_DISABLED )
			result = "Communication Disabled";
		else if ( code == EBackendError.EBERR_INVALID_STATE )
			result = "Cannot be called from current state";
		else if ( code == EBackendError.EBERR_BUSY )
			result = "Busy processing requests";
		else if ( code == EBackendError.EBERR_ALREADY_OFFLINE )
			result = "Already disconnected";
		else if ( code == EBackendError.EBERR_ALREADY_ONLINE )
			result = "Already connected";
		else if ( code == EBackendError.EBERR_LOGIN_FAILED )
			result = "Failed to logon";
		else if ( code == EBackendError.EBERR_AUTH_FAILED )
			result = "Failed to Authenticate";
		else
			result = "*";
			
		return result;
	}
	
	/**
	\brief Called when initiate cannot be called
	*/
	void OnCannotInitiate( int code )
	{
		Print("!!! [Backend] Cannot Initiate: "+ GetErrorCode(code));
	}
	
	/**
	\brief Called when shutdown cannot be proceeded
	*/
	void OnCannotShutdown( int code )
	{
		Print("!!! [Backend] Cannot Shutdown: "+ GetErrorCode(code));
	}

	/**
	\brief Called when step was successfully proceeded
	*/
	void OnSuccess( string step )
	{
		Print( "[Backend] Successfully Solicited: " + step );
	}
	
	/**
	\brief Called when step failed
	*/
	void OnFail( string step )
	{
		Print( "[Backend] Failed to Proceed: " + step );
	}

	/**
	\brief Ask specific request with callback result
	\param request Is type of request, which is EBackendRequest
	\param cb Is script callback where you will recieve result/ error or even data when request finsihes
	\param data Is optional callback when request uses or response return Json data and you want to work with object
	*/
	proto native void Request( int request, BackendCallback cb, JsonApiStruct dataObject );

	/**
	\brief Ask player request with callback result from controller (Lobby)
	\param request Is type of request, which is EBackendRequest
	\param cb Is script callback where you will recieve result/ error or even data when request finsihes
	\param data Is optional callback when request uses or response return Json data and you want to work with object
	\param iPlayerId Is Player Id used on player identity
	*/
	proto native void PlayerRequest( int request, BackendCallback cb, JsonApiStruct dataObject, int iPlayerId );

	/**
	\brief Send feedback message and/ or script object with whatever data on it (additionally it is possible to handle callback as well)
	\param cb Is script callback where you will recieve result/ error or even data when request finsihes
	\param data Is optional callback when request uses or response return Json data and you want to work with object
	\param message Is custom
	*/
	proto native void FeedbackMessage( BackendCallback cb, JsonApiStruct dataObject, string message );

	/**
	\brief Set credentials value per item
	\param item Is type of EBackendCredentials parameter you want to set
	\param str Is value itself
	*/
	proto native void SetCredentialsItem( EBackendCredentials item, string str );

	/**
	\brief Get credentials value per item
	\param item Is type of EBackendCredentials parameter you want to read
	*/
	proto native string GetCredentialsItem( EBackendCredentials item );

	/**
	\brief Invoke credentials update (authenticate with new name+password)
	*/
	proto native void VerifyCredentials();

};


proto native BackendApi GetBackendApi();

// -------------------------------------------------------------------------
