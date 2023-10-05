class PluginLocalEnscriptHistory extends PluginLocalHistoryBase
{	
	override string GetSubFolderName()
	{
		return CFG_FILE_DEBUG_DIR;
	}
	
	override string GetFileName()
	{
		if (!FileExist(GetSubFolderName()))//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
			return "$profile:script_enscript.history";//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
		
		return GetSubFolderName() + CFG_FILE_ENS_HISTORY;
	}
}

class PluginLocalEnscriptHistoryServer extends PluginLocalHistoryBase
{	

	override string GetSubFolderName()
	{
		return CFG_FILE_DEBUG_DIR;
	}
	
	override string GetFileName()
	{
		if (!FileExist(GetSubFolderName()))//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
			return "$profile:script_enscriptServer.history";//---ALL LINES CONTAINING THIS COMMENT CAN BE REMOVED IN 1.23
		
		return GetSubFolderName() + CFG_FILE_ENS_HISTORY_SERVER;
	}

}

