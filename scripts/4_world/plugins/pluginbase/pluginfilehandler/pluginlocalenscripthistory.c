class PluginLocalEnscriptHistory extends PluginLocalHistoryBase
{	
	void PluginLocalEnscriptHistory()
	{
	}
		
	void ~PluginLocalEnscriptHistory()
	{
	}

	override string GetFileName()
	{
		return CFG_FILE_ENS_HISTORY;
	}
}

class PluginLocalEnscriptHistoryServer extends PluginLocalHistoryBase
{	
	void PluginLocalEnscriptHistoryServer()
	{
	}
		
	void ~PluginLocalEnscriptHistoryServer()
	{
	}

	override string GetFileName()
	{
		return CFG_FILE_ENS_HISTORY_SERVER;
	}
}

