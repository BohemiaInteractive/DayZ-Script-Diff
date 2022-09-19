
/** @file */

// -------------------------------------------------------------------------
// object which allow to parse upon generic JSON structure and format it back
//
//
class JsonApiStruct : Managed
{

	void JsonApiStruct()
	{
	}

	void ~JsonApiStruct()
	{
	}

	/**
	\brief Event when expand (unpack) process starts
	*/
	void OnExpand()
	{
	}

	/**
	\brief Event when pack starts - you will pack your stuff here
	*/
	void OnPack()
	{
		Print( "OnPack() ");
	}

	/**
	\brief Verification event after successfull JSON packing
	*/
	void OnBufferReady()
	{
	}

	/**
	\brief Event called when pending store operation is finished - callback from JsonApiHandle before handle release
	*/
	void OnSuccess( int errorCode )
	{
		// errorCode is EJsonApiError
	}

	/**
	\brief Event called when pending store operation is finished - callback from JsonApiHandle before handle release
	*/
	void OnError( int errorCode )
	{
		// errorCode is EJsonApiError
	}
	
	/**
	\brief Called when parsing object
	*/
	void OnObject( string name )
	{
		Print( "OnObject: " + name );
	}

	/**
	\brief Called when parsing integer value
	*/
	void OnInteger( string name, int value )
	{
		Print( "OnInteger: " + value );
	}

	/**
	\brief Called when parsing float value
	*/
	void OnFloat( string name, float value )
	{
		Print( "OnFloat: " + value );
	}

	/**
	\brief Called when parsing boolean value
	*/
	void OnBoolean( string name, bool value )
	{
		Print( "OnBoolean: " + value );
	}

	/**
	\brief Called when parsing string value
	*/
	void OnString( string name, string value )
	{
		Print( "OnString: " + value );
	}
	
	/**
	\brief Called when parsing vector value
	*/
	void OnVector( string name, vector value )
	{
		Print( "OnVector: " + value );
	}
	
	/**
	\brief Called when parsing array
	*/
	void OnStartArray( string name )
	{
		Print( "OnStartArray: " + name );
	}

	/**
	\brief Called when array end, returns count of items
	*/
	void OnEndArray( int itemCount )
	{
		Print( "OnEndArray: " + itemCount );
	}

	/**
	\brief Called when parsing object
	*/
	void OnItemObject( int index, string name )
	{
		Print( "OnItemObject: " + name );
	}

	/**
	\brief Called when parsing integer value
	*/
	void OnItemInteger( int index, int value )
	{
		Print( "OnItemInteger: " + value );
	}

	/**
	\brief Called when parsing float value
	*/
	void OnItemFloat( int index, float value )
	{
		Print( "OnItemFloat: " + value );
	}

	/**
	\brief Called when parsing boolean value
	*/
	void OnItemBoolean( int index, bool value )
	{
		Print( "OnItemBoolean: " + value );
	}

	/**
	\brief Called when parsing string value from array
	*/
	void OnItemString( int index, string value )
	{
		Print( "OnItemString: " + value );
	}
	
	/**
	\brief Called when parsing vector value from array
	*/
	void OnItemVector( int index, vector value )
	{
		Print( "OnItemVector: " + value );
	}
	
	/**
	\brief Register script variable for auto-feature
	*/
	proto native void RegV( string name );

	/**
	\brief Push object to parse (only during parse operation)
	*/
	proto native void Push( JsonApiStruct obj );	
	
	/**
	\brief Start object at hierarchy - !!! Be cautious and doublecheck results when using this !!!
	*/
	proto native void StartObject( string name );

	/**
	\brief End object at hierarchy - !!! Be cautious and doublecheck results when using this !!!
	*/
	proto native void EndObject();

	/**
	\brief Add scripted object to hierarchy (calls through hierarchy)
	*/
	proto native void StoreObject( string name, JsonApiStruct obj );
	
	/**
	\brief Add float value to hierarchy
	*/
	proto native void StoreFloat( string name, float value );
	
	/**
	\brief Add integer value to hierarchy
	*/
	proto native void StoreInteger( string name, int value );
	
	/**
	\brief Add boolean value to hierarchy
	*/
	proto native void StoreBoolean( string name, bool value );

	/**
	\brief Add string value to hierarchy
	*/
	proto native void StoreString( string name, string value );
	
	/**
	\brief Add vector value to hierarchy
	*/
	proto native void StoreVector( string name, vector value );
	
	/**
	\brief Start array at hierarchy - !!! Be cautious and doublecheck results when using this !!!
	*/
	proto native void StartArray( string name );

	/**
	\brief End array at hierarchy - !!! Be cautious and doublecheck results when using this !!!
	*/
	proto native void EndArray();
	
	/**
	\brief Add scripted unnamed/ array object
	*/
	proto native void ItemObject( JsonApiStruct obj );
	
	/**
	\brief Add unnamed/ array float value
	*/
	proto native void ItemFloat( float value );
	
	/**
	\brief Add unnamed/ array integer value
	*/
	proto native void ItemInteger( int value );
	
	/**
	\brief Add unnamed/ array boolean value
	*/
	proto native void ItemBoolean( bool value );

	/**
	\brief Add unnamed/ array string value
	*/
	proto native void ItemString( string value );

	/**
	\brief Add unnamed/ array vector value
	*/
	proto native void ItemVector( vector value );

	/**
	\brief Call this when you've done packing or unpacking (interrupt operation)
	*/
	proto native void SetDone();

	/**
	\brief Call this when you've done packing or unpacking + want to generate error - prevent to send invalid data etc.
	*/
	proto native void SetFail();
	
	/**
	\brief Start object packing - when it can be done (when sending remote etc.)
	*/
	proto native void Pack();

	/**
	\brief Start object packing now - for use at main thread only!
	*/
	proto native void InstantPack();

	/**
	\brief Start object unpacking from RAW string data
	*/
	proto native void ExpandFromRAW( string data );

	/**
	\brief Get packed JSON as string (!only if you called Pack() first, it may return null)
	*/
	proto native string AsString();

	/**
	\brief Pack() and save JSON to file
	*/
	proto native bool PackToFile( string FileName );
	/**
	\brief Save JSON to file (only If something was loaded or recieved previously!)
	*/
	proto native bool SaveToFile( string FileName );
	/**
	\brief Load JSON from file and Expand
	*/
	proto native bool LoadFromFile( string FileName );

};

