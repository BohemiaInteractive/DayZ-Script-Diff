
/** @file */


// -------------------------------------------------------------------------
// states, (result + error) codes
// defined in C++
enum ERestResultState
{
	EREST_EMPTY,				// not initialized
	EREST_PENDING,				// awaiting processing
	EREST_FEEDING,				// awaiting incoming data
	EREST_SUCCESS,				// result and/ or data are ready (success), awaiting data processing to be finished (no longer blocking queue processing)
	EREST_PROCESSED,			// finished (either successfully or with failure) and eill be removed ASAP

	EREST_ERROR,				// (state >= EREST_ERROR) == error happened
	EREST_ERROR_CLIENTERROR,	//  (EREST_ERROR == EREST_ERROR_CLIENTERROR)
	EREST_ERROR_SERVERERROR,
	EREST_ERROR_APPERROR,
	EREST_ERROR_TIMEOUT,
	EREST_ERROR_NOTIMPLEMENTED,
	EREST_ERROR_UNKNOWN,
};

// -------------------------------------------------------------------------
// options
// defined in C++
enum ERestOption
{
	ERESTOPTION_UNKNOWN,					// invalid option

	ERESTOPTION_READOPERATION,		// read operation timeout (default 10sec)
	ERESTOPTION_CONNECTION,			// connection timeout (default 10sec)
	// note: limit for timeout is between <3 .. 120> seconds, you cannot exceed this value
};



// -------------------------------------------------------------------------
// object to be used from script for result binding
//
//	[Example:]
//
//		RestCallback cbx1 = new RestCallback;
//		RestContext ctx = GetRestApi().GetRestContext("http://somethingsomewhere.com/path/");
//		ctx.GET(cbx1,"RequestPath?Argument=Something");
//
//		Event are then called upon RestCallback()
//
class RestCallback : Managed
{
	/**
	\brief Called in case request failed (ERestResultState) - Note! May be called multiple times in case of (RetryCount > 1)
	*/
	void OnError( int errorCode )
	{
		// override this with your implementation
		Print(" !!! OnError() ");
	};

	/**
	\brief Called in case request timed out or handled improperly (no error, no success, no data)
	*/
	void OnTimeout()
	{
		// override this with your implementation
		Print(" !!! OnTimeout() ");
	};

	/**
	\brief Called when data arrived and/ or response processed successfully
	*/
	void OnSuccess( string data, int dataSize )
	{
		// override this with your implementation
		Print(" !!! OnSuccess() size=" + dataSize );
		if( dataSize > 0 )
			Print(data); // !!! NOTE: Print() will not output string longer than 1024b, check your dataSize !!!
	};

	/**
	\brief Called when data arrived and/ or file created successfully
	*/
	void OnFileCreated( string fileName, int dataSize )
	{
		// override this with your implementation
		Print(" !!! OnFileCreated() file=" + fileName + " size=" + dataSize );
	};

};


// -------------------------------------------------------------------------
// context API and request API
class RestContext
{
	private void RestContext() {}
	private void ~RestContext() {}
	
	/**
	\brief Processes GET request and returns result (ERestResultState) and/ or data (timeout, error) when finished
	*/
	proto native int GET( RestCallback cb, string request );

	/**
	\brief Processes GET request and returns data immediately (thread blocking operation!)
	*/
	proto native string GET_now( string request );

	/**
	\brief Processes GET request and returns result (ERestResultState) and/ or stores data int specified file (timeout, error) when finished
	*/
	proto native int FILE( RestCallback cb, string request, string filename );

	/**
	\brief Processes GET request and returns result (ERestResultState) and/ stores data int specified file immediately (thread blocking operation!)
	*/
	proto native int FILE_now( string request, string filename );

	/**
	\brief Pushes POST request and returns result (ERestResultState) and/ or data (timeout, error) when finished
	*/
	proto native int POST( RestCallback cb, string request, string data );

	/**
	\brief Processes POST request and returns data immediately (thread blocking operation!)
	*/
	proto native string POST_now( string request, string data );

	/**
	\brief Clear all pending requests and buffers
	*/
	proto native void reset();

	/**
	\brief Set Content-Type header (string)
	
	default content type is "application/octet-stream"
	but you can specify whatever you like, for example "application/json" "application/sql" "text/plain"
	*/
	proto native void SetHeader( string value );
};


// -------------------------------------------------------------------------
// RestApi core for context create/ access + debug features
class RestApi
{
	private void RestApi() {}
	private void ~RestApi() {}

	/**
	\brief Get new or existing context for http comm GetRestContext("www.server915.com/interface/")
	*/
	proto native RestContext GetRestContext( string serverURL );
	
	/**
	\brief Get count of registered contexes
	*/
	proto native int GetContextCount();

	/**
	\brief Enable debug output to console (disabled by default)
	*/
	proto native void EnableDebug( bool bEnable );

	/**
	\brief List of all currently active contexes and processed (pending) requests
	*/
	proto native void DebugList();

	/**
	\brief Set specific option (integer)
	*/
	proto native void SetOption( int option, int value );

};

// -------------------------------------------------------------------------
// RestApi create/ access methods out of Hive initialization
proto native RestApi CreateRestApi();
proto native void DestroyRestApi();
proto native RestApi GetRestApi();

