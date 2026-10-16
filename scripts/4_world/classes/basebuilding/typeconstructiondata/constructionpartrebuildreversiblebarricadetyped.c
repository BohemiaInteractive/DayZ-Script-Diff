class ConstructionPartRebuildReversibleBarricadeTypeData : ConstructionPartRebuildReversibleBaseTypeData
{
	protected string m_SourceNameBarricade;
	
	override protected void InitCustomPartTypeProperties(PartTypeInitData initData)
	{
		m_SourceNameBarricade = "";
		string path = initData.m_PartPath + " CustomPartTypeProperties";
		if (!g_Game.ConfigIsExisting(path))
		{
			ErrorEx("class CustomPartTypeProperties undefined for " + initData.m_PartPath);
			return;
		}
		else
		{
			if (!g_Game.ConfigIsExisting(path + " barricade_animSource"))
			{
				ErrorConfigUndefined("barricade_animSource", path + " barricade_animSource");
				return;
			}
			g_Game.ConfigGetText(path + " barricade_animSource", m_SourceNameBarricade);
		}
	}
	
	string GetSourceNameBarricade()
	{
		return m_SourceNameBarricade;
	}
}