
/** @file */

// -------------------------------------------------------------------------
// error codes for handle processing
// defined in C++
enum EJsonApiError
{
	ETJSON_UNKNOWN,			// invalid code

	ETJSON_OK,				// all fine
	ETJSON_COMMSEND,		// error during send
	ETJSON_COMMRECV,		// error during receive
	ETJSON_PARSERERROR,		// error during parsing
	ETJSON_PACKNOSTART,		// error - cannot start packing (invalid state)
	ETJSON_TIMEOUT,			// failed to send/ store handle due to timeout
	ETJSON_NOBUFFERS,		// not enough buffers available
	ETJSON_FAILFILELOAD,	// failed to load file
	ETJSON_FAILFILESAVE,	// failed to save file
	ETJSON_NOTARRAY,		// object is not array (ie. attempt to provide different or none object as array)
};


// -------------------------------------------------------------------------
// JsonApi Handle is container encapsulating real JSON data being sent or recieved via. RESTful/ other service
class JsonApiHandle
{
	private void JsonApiHandle() {}
	private void ~JsonApiHandle() {}
	
	/**
	\brief Length of JSON
	*/
	proto native int Size();

	/**
	\brief Return as string
	*/
	proto native owned string AsString();

	/**
	\brief Invalidate handle and schedule removal
	*/
	proto native void Invalidate();

};


// -------------------------------------------------------------------------
// parent class for handling JsonApi Struct objects
//
//
class JsonApi
{
	private void JsonApi() {}
	private void ~JsonApi() {}

	/**
	\brief Override number of concurrent buffers used (depending project requirements, DEFAULT = 16, MIN = 4)
	*/
	proto native void SetBuffers( int iBufferCount );

	/**
	\brief Actual number of total buffers active + free
	*/
	proto native int BufferCount();

	/**
	\brief Maximum number of buffers used at once!
	*/
	proto native int BufferMax();

	/**
	\brief List of all currently active handles
	*/
	proto native void DebugList();

};


// -------------------------------------------------------------------------
// JsonApi access methods
proto native JsonApi CreateJsonApi();
proto native void DestroyJsonApi();
proto native JsonApi GetJsonApi();

