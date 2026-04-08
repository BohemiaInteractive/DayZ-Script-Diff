class EntityType
{
	protected void EntityType()
	{
	}

	protected void ~EntityType()
	{
	}

	proto native owned string GetName();

	// config class API	
	
	proto string ConfigGetString(string entryName);

	/**
	\brief Get a raw strings from config entry.
	@param entryName 
	\return value output string
	\note use 'FormatRawConfigStringKeys' method to change localization keys to script-friendly
	*/
	proto string ConfigGetStringRaw(string entryName);
	proto int ConfigGetInt(string entryName);
	bool ConfigGetBool(string entryName)
	{
		return (ConfigGetInt(entryName) == 1);
	}

	proto float ConfigGetFloat(string entryName);
	proto vector ConfigGetVector(string entryName);

	/**
	\brief Get array of strings from config entry.
	@param entryName 
	@param value output
	*/
	proto native void		ConfigGetTextArray(string entryName, out TStringArray values);

	/**
	\brief Get array of raw strings from config entry.
	@param entryName 
	@param value output in raw format (localization keys '$STR_' are not translated).
	\note use 'FormatRawConfigStringKeys' method to change localization keys to script-friendly
	*/
	proto native void		ConfigGetTextArrayRaw(string entryName, out TStringArray values);
	
	/**
	\brief Get array of floats from config entry.
	@param entryName 
	@param value output
	*/
	proto native void		ConfigGetFloatArray(string entryName, out TFloatArray values);
	
	/**
	\brief Get array of integers from config entry.
	@param entryName 
	@param value output
	*/
	proto native void		ConfigGetIntArray(string entryName, out TIntArray values);

	/**
	\brief Checks if given entry exists.
	@param entryName 
	*/
	proto native bool		ConfigIsExisting(string entryName);
};
