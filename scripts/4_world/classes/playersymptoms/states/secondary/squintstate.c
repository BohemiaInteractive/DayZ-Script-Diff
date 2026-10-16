class SquintSymptom : SymptomBase
{
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_Priority 				= 0;
		m_SymptomType 			= SymptomTypes.SECONDARY;
		m_ID 					= SymptomIDs.SYMPTOM_SQUINT;
		m_DestroyOnAnimFinish 	= true;
		m_IsPersistent 			= false;
		m_ServerUpdateInterval	= 5.0;
		m_SyncToClient			= true;
	}
	
	override bool AllowInUnconscious()
	{
		return true;
	}
	
	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		if (player.IsUnconscious())
			return;

		CachedEquipmentStorageQuery query = new CachedEquipmentStorageQuery();
		query.m_Category = ECachedEquipmentItemCategory.EYEWEAR|ECachedEquipmentItemCategory.MASK;
		query.m_Placement = ECachedEquipmentPlacement.ATTACHMENT;
		query.m_MaximumDepth = 2;

		float protectionLevel = 0.0;
		ItemBase coverItem = null;
		array<Entity> coverItems = player.GetCachedEquipment().GetEntitiesByCategory(query);
		int coverItemsCount = coverItems.Count();
		for (int i = 0; i < coverItemsCount; ++i)
		{
			coverItem = ItemBase.Cast(coverItems[i]);
			protectionLevel = coverItem.GetProtectionLevel(DEF_DUST_PARTICLE_EYES);
			if (protectionLevel > 0.0)
				break;
		}

		float sandstormIntensity;
		ScriptedSandstormController sandstorm = ScriptedSandstormController.Cast(g_Game.GetWeather().GetSandstorm());
		if (sandstorm)
			sandstormIntensity = sandstorm.GetIntensityForPlayer(player);

		float squintIntensity = Math.Clamp(sandstormIntensity - protectionLevel, 0.0, 1.0);
		
		Param1<float> params = new Param1<float>(squintIntensity);
		g_Game.GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.SQUINTING, deltatime, params, EffectWidgetHandles.SQUINTING);
	}

	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	override void OnGetActivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
		
		g_Game.GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.SQUINTING});
	}

	override void OnGetDeactivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}
	
	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());

		g_Game.GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.SQUINTING});
	}
	
}
