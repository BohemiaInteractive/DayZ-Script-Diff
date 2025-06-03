class UniversalTemperatureSourceLambdaBase
{
	protected float m_LastExecuteTime = -1;
	protected float m_ExecuteInterval = -1;
	protected bool m_AffectsPlayer;
	
	void UniversalTemperatureSourceLambdaBase()
	{
		m_AffectsPlayer = true;
	}

	void ~UniversalTemperatureSourceLambdaBase();
	void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues);
	void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings);
	void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<EntityAI> nearestObjects);
	void WarmAndCoolItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<EntityAI> nearestObjects);
	
	void OnBeforeExecute()
	{
		float time = g_Game.GetTickTime();
		
		if (m_LastExecuteTime == -1)
			m_LastExecuteTime += time; //time is at least 1
		
		m_ExecuteInterval = time - m_LastExecuteTime;
	}
	
	void OnAfterExecute()
	{
		m_LastExecuteTime = g_Game.GetTickTime();
	}
	
	void OnUTSActivate()
	{
		m_LastExecuteTime = -1;
	}
	
	void OnUTSDeactivate()
	{
		m_LastExecuteTime = -1;
	}
	
	bool AffectsPlayer()
	{
		return m_AffectsPlayer;
	}
	
	//! --------------------------------------
	//! DEPRECATED
	const float HEAT_THROUGH_AIR_COEF = 0.003;
}
