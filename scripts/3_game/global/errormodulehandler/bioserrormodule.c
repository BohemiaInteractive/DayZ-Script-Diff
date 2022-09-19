/**
  \brief Possible Error codes for bios API.
	* This is the list of errors that can be returned from bios API.
	* Errors after UNKNOWN are extended errors that are not present in the native API.	
*/
enum EBiosError
{
	//! 0 - No error. Can be returned from any call.
	OK,
	//! 1 - The operation was cancelled. See individual calls for details.
	CANCEL
	//! 2 - Atleast one of the input parameters is incorrect
	BAD_PARAMETER,
	//! 3 - The call could not find some data. See individual calls for details.
	NOT_FOUND,
	//! 4 - The call is not supported for the current platform. Can be returned from any call.
	NOT_IMPLEMENTED,
	//! 5 - [XB] The action cannot be performed because the user does not own the content.
	PURCHASE_REQUIRED,
	//! 6 - The action is not allowed. (restricted, not enough permission)
	NOT_ALLOWED,
	//! 7 - [XB] The user is banned from some action.
	BANNED,
	//! 8 - API usage error. For example, usage of objects whos native lifetime ended. Can be returned from any call.
	LOGICAL,
	
	//! 9 - Response was of unexpected format
	WRONG_RESPONSE_DATA,		
	
	//! 10 - Server is no longer registered
	SERVER_HEARTBEAT_SERVER_NOT_FOUND,
	//! 11 - Server was already registered
	SERVER_REGISTER_ALREADY_EXISTS,		
	
	//! 12 - Failed to open the request
	REQUEST_CREATE_FAIL_XHR,
	//! 13 - Failed to send the data
	REQUEST_SEND_FAIL,
	//! 14 - Failed to wait for the operation to end - only in a synchronous call
	REQUEST_WAIT_FAIL,
	
	//! 15 - Response was expected to be in JSON format, but it was not
	NON_JSON_RESPONSE,
	
	// RESERVED SPACE OF REDUNDANT ERRORS

	//! 21 - [PS] Mandatory update exist and is required.	
	UPDATE_REQUIRED,
	//! 22 - [PS] Mandatory update exist and is ready to install.
	UPDATE_REQUIRED_AND_DOWNLOADED,
	
	//! 23 - Resource is unreachable or unavailable, can be 404 or no internet connection (etc.)
	COMUNICATION_ERROR,

	//! 24 - Any other error. Can be returned from any call.
	UNKNOWN,
	
	//! 25 - Extended error. Script not properly linked with engine. May be removed in future.
	BAD_SCRIPT,
};

class BIOSErrorModule : ErrorHandlerModuleScript
{
	void BIOSErrorModule()
	{
		SetCategory(ErrorCategory.BIOSError);
	}
	
	override void InitOptionalVariables()
	{
		super.InitOptionalVariables();
		
		m_Header = "#STR_BIOS_BIOSError";
	}
	
	override void FillErrorDataMap()
	{
		super.FillErrorDataMap();
		
		InsertErrorProperties(EBiosError.OK); // Not an error, so no message
		InsertBIOSError(EBiosError.CANCEL								, "#STR_BIOS_OperationCancelled");
		InsertBIOSError(EBiosError.BAD_PARAMETER						, "#STR_BIOS_BadParameter");
		InsertBIOSError(EBiosError.NOT_FOUND							, "#STR_BIOS_NotFound");
		InsertBIOSError(EBiosError.NOT_IMPLEMENTED						, "#STR_BIOS_NotImplemented");
		InsertBIOSError(EBiosError.PURCHASE_REQUIRED					, "#STR_BIOS_PurchaseRequired");
		InsertBIOSError(EBiosError.NOT_ALLOWED							, "#STR_BIOS_ActionRestricted");
		InsertBIOSError(EBiosError.BANNED								, "#STR_auth_publisher_ban");
		InsertBIOSError(EBiosError.LOGICAL								, "#STR_BIOS_APILogicError");
		
		InsertBIOSError(EBiosError.WRONG_RESPONSE_DATA					, "#STR_BIOS_UnexpectedFormat");

		InsertBIOSError(EBiosError.SERVER_HEARTBEAT_SERVER_NOT_FOUND	, "#STR_BIOS_ServerNotRegistered");
		InsertBIOSError(EBiosError.SERVER_REGISTER_ALREADY_EXISTS		, "#STR_BIOS_ServerAlreadyRegistered");

		InsertBIOSError(EBiosError.REQUEST_CREATE_FAIL_XHR				, "#STR_BIOS_FailedToOpenRequest");
		InsertBIOSError(EBiosError.REQUEST_SEND_FAIL					, "#STR_BIOS_FailedToSendData");
		InsertBIOSError(EBiosError.REQUEST_WAIT_FAIL					, "#STR_BIOS_FailedToWaitOperationEnd");

		InsertBIOSError(EBiosError.NON_JSON_RESPONSE					, "#STR_BIOS_UnexpectedFormat");

		InsertBIOSError(EBiosError.UPDATE_REQUIRED						, "#STR_BIOS_UpdateRequired");
		InsertBIOSError(EBiosError.UPDATE_REQUIRED_AND_DOWNLOADED		, "#STR_BIOS_UpdateRequiredAndDownloaded");

		InsertBIOSError(EBiosError.COMUNICATION_ERROR					, "#STR_BIOS_CommunicationError");

		InsertBIOSError(EBiosError.UNKNOWN								, "#server_browser_error_unknown");
		InsertBIOSError(EBiosError.BAD_SCRIPT							, "#STR_BIOS_LinkageError");
	}
	
	void InsertBIOSError(int code, string message)
	{
		#ifdef PLATFORM_CONSOLE
		InsertDialogueErrorProperties(code, message);
		#else
		InsertErrorProperties(code, message);
		#endif
	}
}