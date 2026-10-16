class StaticObjectType : EntityType
{
	//may not contain every soundset info, defaults from DayZPlayerType used in that case
	protected ref map<int, ref SoundObjectBuilder> m_CustomVegetationSoundsMap; //<stepEventID, SoundObjectBuilder>
	
	void StaticObjectType()
	{
		LoadCustomStepSounds();
	}
	
	protected void LoadCustomStepSounds()
	{
		string pathBase = "CfgNonAIVehicles " + GetName() + " VegetationSounds";
		if (!g_Game.ConfigIsExisting(pathBase))
			return;
		
		m_CustomVegetationSoundsMap = new map<int, ref SoundObjectBuilder>();
		
		int vegSoundsCount = g_Game.ConfigGetChildrenCount(pathBase);
		for (int i = 0; i < vegSoundsCount; ++i)
		{
			string subclass;
			g_Game.ConfigGetChildName(pathBase, i, subclass);
			
			string soundSet = "";
			g_Game.ConfigGetText(pathBase + " " + subclass + " soundSet", soundSet);
			
			TIntArray animEventIds = new TIntArray();
			g_Game.ConfigGetIntArray(pathBase + " " + subclass + " animEventIds", animEventIds);
			
			SoundParams soundParams = new SoundParams(soundSet);
			
			if (soundParams.IsValid() && animEventIds.Count() > 0)
			{
				SoundObjectBuilder builder = new SoundObjectBuilder(soundParams);
				foreach (int eventID : animEventIds)
				{
					m_CustomVegetationSoundsMap.Set(eventID, builder);
				}
			}
		}
	}
	
	SoundObjectBuilder GetVegetationSoundBuilderFromEventID(int stepEventID)
	{
		if (m_CustomVegetationSoundsMap)
			return m_CustomVegetationSoundsMap.Get(stepEventID);
		
		return null;
	}
}