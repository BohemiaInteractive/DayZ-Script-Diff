//checks for critical blood level and kills the character if bellow
class BleedingCheckMdfr: ModifierBase
{
	protected const float		BLOOD_DECREMENT_PER_SEC = -10;
	PluginAdminLog 				m_AdminLog;
		
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_BLEEDING;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		
		if( GetGame().IsServer() )
		{
			m_AdminLog 				= PluginAdminLog.Cast( GetPlugin(PluginAdminLog) );
		}
		
		DisableDeactivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		float blood = player.GetHealth("","Blood");
		
		if( blood < PlayerConstants.BLOOD_THRESHOLD_FATAL )
		{
			return true;
		}
		return false;
		
	}

	override void OnActivate(PlayerBase player)
	{
		player.SetHealth("","",-1000);
		
		if ( m_AdminLog )
		{
			m_AdminLog.BleedingOut( player );
		}
	}

	override void OnDeactivate(PlayerBase player)
	{
		
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		
	}
};