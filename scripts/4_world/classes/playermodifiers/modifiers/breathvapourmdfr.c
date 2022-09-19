const int BREATH_VAPOUR_LEVEL_MAX = 3;

class BreathVapourMdfr extends ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiers.MDF_BREATH_VAPOUR;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 6;
		DisableDeactivateCheck();

	}

	override bool ActivateCondition(PlayerBase player)
	{
		return true;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnReconnect(PlayerBase player)
	{

	}
	
	override string GetDebugText()
	{
		//return m_Player.GetStatHeatComfort().Get().ToString();
	}
	
	override string GetDebugTextSimple()
	{
		//return m_Player.GetStatHeatComfort().Get().ToString();
	}

	
	override void OnTick(PlayerBase player, float deltaT)
	{
		if( !player.m_Environment.IsTemperatureSet() )
			return;
		float air_temperature = player.m_Environment.GetTemperature();
		int level = 0;//default
		if( MiscGameplayFunctions.IsValueInRange( air_temperature, PlayerConstants.BREATH_VAPOUR_THRESHOLD_HIGH, PlayerConstants.BREATH_VAPOUR_THRESHOLD_LOW) )
		{
			float value = Math.InverseLerp( PlayerConstants.BREATH_VAPOUR_THRESHOLD_LOW, PlayerConstants.BREATH_VAPOUR_THRESHOLD_HIGH,air_temperature);
			value = Math.Clamp(value,0,1);
			level = Math.Round(Math.Lerp(1,BREATH_VAPOUR_LEVEL_MAX,value));
		}
		else if(air_temperature < PlayerConstants.BREATH_VAPOUR_THRESHOLD_HIGH)
		{
			level = BREATH_VAPOUR_LEVEL_MAX;
		}
		if( level != player.m_BreathVapour )
		{
			player.m_BreathVapour = level;
			player.SetSynchDirty();
		}
	}
};
