class ShockDamageMdfr: ModifierBase
{
	const int UNCONSIOUSS_COOLDOWN_TIME = 60;//in s
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_SHOCK_DAMAGE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 0.5;
	}

	override void OnActivate(PlayerBase player)
	{
		
	}

	override void OnReconnect(PlayerBase player)
	{

	}

	override bool ActivateCondition(PlayerBase player)
	{
		if( !player.IsUnconscious() && player.GetHealth("","Blood") <= PlayerConstants.SHOCK_DAMAGE_BLOOD_THRESHOLD_HIGH && GetGame().GetTime() > (player.m_UnconsciousEndTime + UNCONSIOUSS_COOLDOWN_TIME) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}
	// ------------------------------------------------------------------------------

	override void OnTick(PlayerBase player, float deltaT)
	{
		float blood =  player.GetHealth("","Blood");
		float value = Math.InverseLerp( PlayerConstants.SHOCK_DAMAGE_BLOOD_THRESHOLD_HIGH, PlayerConstants.SHOCK_DAMAGE_BLOOD_THRESHOLD_LOW, blood );
		value = Math.Clamp(value,0,1);
		float dmg = Math.Lerp( PlayerConstants.SHOCK_DAMAGE_HIGH, PlayerConstants.SHOCK_DAMAGE_LOW, value);
		float damage =  -dmg * deltaT;
		player.AddHealth("","Shock", damage);
		//PrintString(damage.ToString());
	}
};