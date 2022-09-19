class StuffedStomachMdfr: ModifierBase
{
	protected const int	 	STUFFED_TRESHOLD = PlayerConstants.BT_STOMACH_VOLUME_LVL3;
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_STUFFED;
		m_TickIntervalInactive 	= 1;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override void OnTick(PlayerBase player, float deltaT)
	{

	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		float stomach = player.m_PlayerStomach.GetStomachVolume();
		if ( stomach >= STUFFED_TRESHOLD )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnActivate(PlayerBase player)
	{
		if( player.GetNotifiersManager() )
		{
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_STUFFED);
		}
	}

	override void OnDeactivate(PlayerBase player)
	{
		if( player.GetNotifiersManager() )
		{
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_STUFFED);
		}
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

};