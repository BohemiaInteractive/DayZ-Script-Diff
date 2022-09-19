/**
  \brief ErrorCategory - To decide what ErrorHandlerModule needs to be called and easily identify where it came from
*/
enum ErrorCategory
{
	Unknown/* = -1*/,
	//! Generic error group
	Generic,
	//! Error group for when something went wrong while trying to connect to the server
	ConnectErrorClient,
	//! Error group for when the Client did connect to the Server, but the server rejected the connection
	ConnectErrorServer,
	//! Error group for connect errors thrown from Script
	ConnectErrorScript,
	//! Error group for when Client is kicked from server
	ClientKicked,
	//! Error group for BIOS errors
	BIOSError,
};

/**
  \brief The error handler itself, for managing and distributing errors to modules
	* Manages the ErrorHandlerModule instances inserted in Init.
	* API comes with several functions to Create, Throw and extract data from error codes.
	* The format used is an int which is made up of two shorts, one that holds the category and one that holds the code.
	* Therefore when looking at an error code, it is much easier to identify when looking at the hex value.
*/
class ErrorModuleHandler
{
	/**
	\brief Creates and throws the error code, sending it to the handler of the category.
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p int The full error code
		@code
			int errorCode = ErrorModuleHandler.ThrowError( ErrorCategory.ConnectErrorClient, -1 );
			Print( errorCode );

			>> errorCode = 196607
		@endcode
	*/	
	static proto int 						ThrowError(ErrorCategory category, int code, string additionalInfo = "");
	
	/**
	\brief Throws the error code and sends it to the handler of the category.
		\param errorCode \p int The full error code
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p int The full error code
		@code
			int errorCode = ErrorModuleHandler.ThrowErrorCode( 0x0002FFFF );
			Print( errorCode );

			>> errorCode = 196607
		@endcode
	*/
	static proto int						ThrowErrorCode(int errorCode, string additionalInfo = "");
	
	/**
	\brief Creates full error code.
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\return \p int The full error code
		@code
			int errorCode = ErrorModuleHandler.CreateError( ErrorCategory.ConnectErrorClient, -1 );
			Print( errorCode );

			>> errorCode = 196607
		@endcode
	*/
	static proto int 						CreateError(ErrorCategory category, int code);
	
	/**
	\brief Returns the category the error was thrown from.
		\param errorCode \p int The full error code
		\return \p ErrorCategory The ErrorCategory the error was thrown from
		@code
			ErrorCategory category = ErrorModuleHandler.GetCategoryFromError( 0x0002FFFF );
			Print( category );

			>> category = 2
		@endcode
	*/
	static proto ErrorCategory 				GetCategoryFromError(int errorCode);
	
	/**
	\brief Returns the code of the error.
		\param errorCode \p int The full error code
		\return \p int The code of the error
		@code
			int code = ErrorModuleHandler.GetCodeFromError( 0x0002FFFF );
			Print( code );

			>> code = -1
		@endcode
	*/
	static proto int 						GetCodeFromError(int errorCode);
	
	/**
	\brief Returns a formatted string of the error code.
		\param errorCode \p int The full error code
		\return \p string A formatted string of the error code
		@code
			string formattedCode = ErrorModuleHandler.GetErrorHex( 196607 );
			Print( formattedCode );

			>> formattedCode = '0x0002FFFF'
		@endcode
	*/
	static proto owned string				GetErrorHex(int errorCode);
	
	
	
	/**
	\brief Adds a module handler to the ErrorModuleHandler.
		\param category \p ErrorCategory Category the module is for
		\param errorModule \p ErrorHandlerModule The class containing the information and codes for the category.
		\return \p bool Whether the adding of the module was successful or not
	*/
	static proto bool						AddModule(ErrorCategory category, notnull ErrorHandlerModule errorModule);
	
	/**
	\brief Removes a module handler from the ErrorModuleHandler.
		\param category \p ErrorCategory Category the module is for
		\return \p bool Whether the removing of the module was successful or not
	*/
	static proto bool						RemoveModule(ErrorCategory category);
	
	
	
	/**
	\brief Gets the Client Message for specified error
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p string The message which would appear on Client
	*/	
	static proto string						GetClientMessage(ErrorCategory category, int code, string additionalInfo = "");
	
	/**
	\brief Gets the Client Message for specified error
		\param errorCode \p int The full error code
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p string The message which would appear on Client
	*/
	static proto string						GetClientMessageByCode(int errorCode, string additionalInfo = "");
	
	/**
	\brief Gets the Client Message for specified error, while attempting to restore information on the most recent error
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\return \p string The message which would appear on Client
	*/	
	static proto string						GetLastClientMessage(ErrorCategory category, int code);
	
	/**
	\brief Gets the Client Message for specified error, while attempting to restore information on the most recent error
		\param errorCode \p int The full error code
		\return \p string The message which would appear on Client
	*/
	static proto string						GetLastClientMessageByCode(int errorCode);
	
	/**
	\brief Gets the Server Message for specified error
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p string The message which would appear on Server
	*/	
	static proto string						GetServerMessage(ErrorCategory category, int code, string additionalInfo = "");
	
	/**
	\brief Gets the Server Message for specified error
		\param errorCode \p int The full error code
		\param additionalInfo \p string Any additional info regarding the error, usually data
		\return \p string The message which would appear on Server
	*/
	static proto string						GetServerMessageByCode(int errorCode, string additionalInfo = "");
	
	/**
	\brief Gets the Server Message for specified error, while attempting to restore information on the most recent error
		\param category \p ErrorCategory Category the error is thrown from
		\param code \p int The code that the error belongs to inside the category between [-32768, 32767]
		\return \p string The message which would appear on Server
	*/	
	static proto string						GetLastServerMessage(ErrorCategory category, int code);
	
	/**
	\brief Gets the Server Message for specified error, while attempting to restore information on the most recent error
		\param errorCode \p int The full error code
		\return \p string The message which would appear on Server
	*/
	static proto string						GetLastServerMessageByCode(int errorCode);
	
	
	
	/**
	\brief Gets the EMH Instance
		\return \p ErrorModuleHandler The ErrorModuleHandler Instance
	*/
	static proto native ErrorModuleHandler	GetInstance();
	
	static proto void						GetErrorModules(notnull out array<ErrorHandlerModule> errorModules);
	
	
	
	/**
	\brief Wrapper for AddModule to give feedback whether it succeeded or not.
		\param errorModule \p ErrorHandlerModule The ErrorHandlerModule to add
	*/
	void SafeAddModule(notnull ErrorHandlerModule errorModule)
	{
		if ( !AddModule(errorModule.GetCategory(), errorModule) )
			Error(string.Format("[EMH] Adding %1 failed. (Category: %2)", errorModule, errorModule.GetCategory()));
	}
	
	/**
	\brief Wrapper for RemoveModule to give feedback whether it succeeded or not.
		\param errorModule \p ErrorHandlerModule The ErrorHandlerModule to add
	*/
	void SafeRemoveModule(notnull ErrorHandlerModule errorModule)
	{
		if ( !RemoveModule(errorModule.GetCategory()) )
			Error(string.Format("[EMH] Removing %1 failed. (Category: %2)", errorModule, errorModule.GetCategory()));
	}
	
	/**
	\brief Wrapper for RemoveModule to give feedback whether it succeeded or not.
		\param category \p ErrorCategory Category to remove
	*/
	void SafeRemoveModule(ErrorCategory category)
	{
		if ( !RemoveModule(category) )
			Error(string.Format("[EMH] Removing %1 failed.", category));
	}
	
	/**
	\brief Gets called shortly after creation of ErrorModuleHandler.
	*/
	private void Init()
	{
		if (!g_Game.IsDedicatedServer())
		{
			SafeAddModule(new ConnectErrorClientModule);
			SafeAddModule(new ConnectErrorServerModule);		
			SafeAddModule(new ConnectErrorScriptModule);
		}
		SafeAddModule(new ClientKickedModule);
		SafeAddModule(new BIOSErrorModule);
	}
	
	/**
	\brief is called by DayZGame to pass Events.
	*/
	void OnEvent(EventType eventTypeId, Param params)
	{
		array<ErrorHandlerModule> errorModules = new array<ErrorHandlerModule>;
		GetErrorModules(errorModules);
		
		foreach (ErrorHandlerModule module : errorModules)
		{
			module.OnEvent(eventTypeId, params);
		}
	}
}