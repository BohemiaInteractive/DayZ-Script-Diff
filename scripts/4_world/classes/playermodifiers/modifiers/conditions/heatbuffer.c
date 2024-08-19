class HeatBufferMdfr: ModifierBase
{
	protected const float HEATBUFFER_SHOW = 25.0;
	protected const float HEATBUFFER_HIDE = 20.0;

	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_HEATBUFFER;
		m_TickIntervalInactive 	= 1;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override void OnTick(PlayerBase player, float deltaT)
	{

	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		float heatbuffer = player.GetStatHeatBuffer().Get();
		if ( heatbuffer >= HEATBUFFER_SHOW )
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
			//player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_HEATBUFFER);
			player.ToggleHeatBufferVisibility( true );
		}
	}

	override void OnDeactivate(PlayerBase player)
	{
		if( player.GetNotifiersManager() )
		{
			//player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_HEATBUFFER);
			player.ToggleHeatBufferVisibility( false );
		}
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		float heatbuffer = player.GetStatHeatBuffer().Get();
		if ( heatbuffer < HEATBUFFER_HIDE )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};