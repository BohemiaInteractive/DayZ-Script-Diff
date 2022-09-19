class DrowningMdfr: ModifierBase
{
	const int 	TICK_FREQUENCY = 1;
	static const float STAMINA_RECOVERY_MULTIPLIER = 0;
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_DROWNING;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= TICK_FREQUENCY;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_DROWNING;
	}	

	bool CheckIsDrowning(PlayerBase player)
	{
		int index = player.GetBoneIndexByName("head");
		vector pos = player.GetBonePositionWS(index);
		float depth = g_Game.GetWaterDepth(pos);
		if (player.IsSwimming())
		{
			return depth > PlayerConstants.DROWNING_SWIMMING_THRESHOLD;
		}
		else if (player.IsUnconscious())
		{
			return depth > PlayerConstants.DROWNING_UNCONSCIOUS_THRESHOLD;
		}
		return depth > PlayerConstants.DROWNING_DEFAULT_THRESHOLD;
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return CheckIsDrowning(player);
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return !CheckIsDrowning(player);
	}

	override void OnTick(PlayerBase player, float deltaT)
	{	
		if (player.GetStaminaHandler().GetStamina() <= 0)
		{
			player.AddHealth("","",deltaT * -CfgGameplayHandler.GetHealthDepletionSpeed());
			player.AddHealth("","Shock",deltaT * -CfgGameplayHandler.GetShockDepletionSpeed());
			player.RequestSoundEventEx(EPlayerSoundEventID.DROWNING_PAIN, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
		}
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate(player);
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.SetDrowning(true);
		player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.DROWNING);
		player.RequestSoundEventEx(EPlayerSoundEventID.DROWNING_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayDrowningSound, 3000 ,false, 1);
	}
	
	
	void PlayDrowningSound(int index)
	{
		if (m_Player.m_IsDrowning)//check if we are still drowning after the delay
		{
			if (index == 0)
			{
				m_Player.RequestSoundEventEx(EPlayerSoundEventID.EXHAUSTED_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
			}
			else if (index == 1)
			{
				m_Player.RequestSoundEventEx(EPlayerSoundEventID.DROWNING_BREATH, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
			}
		}
	}
		
	override void OnDeactivate(PlayerBase player)
	{
		player.SetDrowning(false);
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DROWNING);
	}
};