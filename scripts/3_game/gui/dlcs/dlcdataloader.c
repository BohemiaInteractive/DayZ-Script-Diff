class DlcDataLoader
{
	protected static const string JSON_FILE_PATH = "scripts/data/dlcinfo.json";
	
	static JsonDataDLCList GetData()
	{
		JsonDataDLCList data;
		
		string errorMessage;
		if (!JsonFileLoader<JsonDataDLCList>.LoadFile(JSON_FILE_PATH, data, errorMessage))
			ErrorEx(errorMessage);
		
		return data;
	}
}
