class ShockMdfr: ModifierBase
{
	private const float	 	UNCONSCIOUS_LIMIT = 50;
	private const float 	SHOCK_INCREMENT_PER_SEC = 1;
	
	override void Init()
	{
		m_ID 					= eModifiers.MDF_SHOCK;
		m_TickIntervalInactive 	= 1;
		m_TickIntervalActive 	= 0.35;
		//DisableActivateCheck();
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

	override void OnActivate(PlayerBase player)
	{
	}
	
	override void OnDeactivate(PlayerBase player)
	{
	}
	
	float GetRefillSpeed(PlayerBase player)
	{
		if ( player.m_IsDrowning )
		{
			return 0;
		}
		if ( player.IsUnconscious() )
		{
			return CfgGameplayHandler.GetShockRefillSpeedUnconscious() * player.m_UnconRefillModifier;
		}
		if (player.GetBrokenLegs() != eBrokenLegs.BROKEN_LEGS || (player.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || player.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE))
		{
			return CfgGameplayHandler.GetShockRefillSpeedConscious();
		}
		if ( player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS && (player.IsSwimming() || player.IsClimbingLadder()) )
		{
			return CfgGameplayHandler.GetShockRefillSpeedUnconscious();
		}
		return 0; //Block shock regen when standing with broken legs
	}
	
	override void OnTick(PlayerBase player, float deltaT)
	{	
		//Print("m_UnconRefillModifier" + player.m_UnconRefillModifier);
		if ( GetGame().GetTime() > player.m_LastShockHitTime + PlayerConstants.SHOCK_REFILL_COOLDOWN_AFTER_HIT && player.GetPulseType() == EPulseType.REGULAR)
		{
			if (!player.IsUnconscious())
				player.m_UnconRefillModifier = 1;//reset this UnconRefillModifier some time after the player has been hit(we want the UnconRefillModifier to only apply to uncon induced by a projectile)
			
			if (player.GetHealth01("","Shock") == 1)//no need to continue if the shock is full
				return;
			
			if ( !CfgGameplayHandler.GetAllowRefillSpeedModifier() )
				player.m_UnconRefillModifier = 1;
			
			player.AddHealth("","Shock", deltaT * GetRefillSpeed(player) );
		}
	}
	
	override void OnReconnect(PlayerBase player)
	{

	}
};