class DlcDataLoader
{
	protected static const string JSON_FILE_PATH = "Scripts/Data/DlcInfo.json";
	
	static JsonDataDLCList GetData()
	{
		ref JsonDataDLCList data;
		JsonFileLoader<ref JsonDataDLCList>.JsonLoadFile( JSON_FILE_PATH, data );
		
		return data;
	}
};

