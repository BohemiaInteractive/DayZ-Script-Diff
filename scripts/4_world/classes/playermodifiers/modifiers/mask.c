class MaskMdfr: ModifierBase
{
	const float IN_AREA_CONSUME_FILTER_QUANTITY_PER_SEC = 0.3;
	const float OUT_AREA_CONSUME_FILTER_QUANTITY_PER_SEC = 0.03;
	
	const float STAMINA_RECOVERY_MODIFIER = 0.5;
	const float STAMINA_DEPLETION_MODIFIER = 1.25;
	
	const float LOW_FILTER_SOUND_EVENT_MIN = 3;
	const float LOW_FILTER_SOUND_EVENT_MAX = 9;
	
	const float LOW_FILTER_SOUND_THRESHOLD = 0.2;//[0..1] , what's the filter quantity between 0 and 1 at which low filter sounds begin playing ?
	
	float m_SoundTimeAccu1;
	float m_NextSoundEventTime;
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_MASK;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
	}	

	override bool ActivateCondition(PlayerBase player)
	{
		return MaskBase.Cast(player.GetInventory().FindAttachment(InventorySlots.MASK)) != null;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition( player);
	}

	override void OnTick(PlayerBase player, float deltaT)
	{	
		MaskBase mask = MaskBase.Cast(player.GetInventory().FindAttachment(InventorySlots.MASK));
		
		float consume_quantity = OUT_AREA_CONSUME_FILTER_QUANTITY_PER_SEC;
		
		bool inside_area = m_Manager.IsModifierActive(eModifiers.MDF_AREAEXPOSURE);
		
		if(inside_area)
		{
			consume_quantity = IN_AREA_CONSUME_FILTER_QUANTITY_PER_SEC;
		}

		if ( mask && !mask.IsDamageDestroyed())
		{
			bool consuming_filter = mask.ConsumeQuantity(consume_quantity * deltaT, player);
			StaminaSoundHandlerServer.Cast(player.GetPlayerSoundManagerServer().GetHandler(eSoundHandlers.STAMINA)).SetStaminaZoneOffset(1);

			if (mask.HasValidFilter())
			{
				player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.MASK);
				player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.MASK);
				
			}
			else
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.MASK);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.MASK);
			}
			HandleSounds(player, deltaT, mask);
		}
	}
	
	void HandleSounds(PlayerBase player, float deltaT, MaskBase mask)
	{
		float filter_quantity_01 = mask.GetFilterQuantity01();
		
		if(mask.CanHaveExternalFilter() && !mask.IsExternalFilterAttached())
			return;
		
		if(filter_quantity_01 > LOW_FILTER_SOUND_THRESHOLD)
			return;
		
		float low_quantity_norm = Math.InverseLerp(0, LOW_FILTER_SOUND_THRESHOLD, filter_quantity_01);//should give us 0..1 based on normalized quantity between 0 and LOW_FILTER_SOUND_THRESHOLD
		
		m_SoundTimeAccu1 += deltaT;
		if( m_SoundTimeAccu1 >= m_NextSoundEventTime)
		{
			m_NextSoundEventTime = Math.Lerp(LOW_FILTER_SOUND_EVENT_MIN, LOW_FILTER_SOUND_EVENT_MAX,low_quantity_norm);
			
			EPlayerSoundEventID event_type = EPlayerSoundEventID.STAMINA_LOW_FILTER_UPPER;
			//under 50% of low filter quantity (not overall filter quantity!!)
			if (low_quantity_norm < 0.5)
			{
				event_type = EPlayerSoundEventID.STAMINA_LOW_FILTER_MID;
			}
			if (low_quantity_norm < 0.25)
			{
				event_type = EPlayerSoundEventID.STAMINA_LOW_FILTER_LOWER;
			}
			player.RequestSoundEventEx(event_type, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
			m_SoundTimeAccu1 = 0;
		} 
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate(player);
	}
	
	override void OnActivate(PlayerBase player)
	{
		//m_NextSoundEventTime = Math.RandomFloat(LOW_FILTER_SOUND_EVENT_MIN, LOW_FILTER_SOUND_EVENT_MAX);
	}	
	
	override void OnDeactivate(PlayerBase player)
	{
		StaminaSoundHandlerServer.Cast(player.GetPlayerSoundManagerServer().GetHandler(eSoundHandlers.STAMINA)).SetStaminaZoneOffset(0);
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.MASK);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.MASK);
	}
	
	
	
};