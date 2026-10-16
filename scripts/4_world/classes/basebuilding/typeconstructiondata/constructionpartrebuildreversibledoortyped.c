class ConstructionPartRebuildReversibleDoorTypeData : ConstructionPartRebuildReversibleBaseTypeData
{
	protected string m_SourceNameDefault;
	protected string m_SourceNameReversed;
	
	override protected void InitCustomPartTypeProperties(PartTypeInitData initData)
	{
		m_SourceNameDefault = "";
		m_SourceNameReversed = "";
		string path = initData.m_PartPath + " CustomPartTypeProperties";
		if (!g_Game.ConfigIsExisting(path))
		{
			ErrorEx("class CustomPartTypeProperties undefined for " + initData.m_PartPath);
			return;
		}
		else
		{
			if (!g_Game.ConfigIsExisting(path + " door_board_default"))
			{
				ErrorConfigUndefined("door_board_default", path);
				return;
			}
			g_Game.ConfigGetText(path + " door_board_default", m_SourceNameDefault);
			
			if (!g_Game.ConfigIsExisting(path + " door_board_reversed"))
			{
				ErrorConfigUndefined("door_board_reversed", path);
				return;
			}
			g_Game.ConfigGetText(path + " door_board_reversed", m_SourceNameReversed);
		}
	}
	
	string GetDefaultSourceName()
	{
		return m_SourceNameDefault;
	}
	
	string GetReversedSourceName()
	{
		return m_SourceNameReversed;
	}
}