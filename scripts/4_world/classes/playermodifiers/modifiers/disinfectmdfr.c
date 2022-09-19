class DisinfectionMdfr: ModifierBase
{
	const int LIFETIME = 100;
	//const int EFFECT_TIME = 100;
	PluginTransmissionAgents m_PluginTransmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
	float m_WoundAgentInvasibility;
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiers.MDF_DISINFECTION;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		//m_WoundAgentInvasibility = m_PluginTransmissionAgents.GetAgentInvasibility(eAgents.WOUND_AGENT);
		DisableActivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate( player );
	}
	
	override string GetDebugText()
	{
		return ( LIFETIME - GetAttachedTime() ).ToString();
	}
	
	override string GetDebugTextSimple()
	{
		return ( LIFETIME - GetAttachedTime() ).ToString();
	}

	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();

	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		
		if(player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION1))
		{
			player.m_AgentPool.AddAgent(eAgents.WOUND_AGENT, -10000);
		}

	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		
		if ( attached_time >= LIFETIME )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		if(player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION2))
		{
			player.m_AgentPool.AddAgent(eAgents.WOUND_AGENT, -m_PluginTransmissionAgents.GetAgentInvasibility(eAgents.WOUND_AGENT) * deltaT);
		}
		else
		{
			player.m_AgentPool.AddAgent(eAgents.WOUND_AGENT, -m_PluginTransmissionAgents.GetAgentInvasibility(eAgents.WOUND_AGENT) * 2 * deltaT);
		}
	}
};