class StaminaSoundEventBase extends PlayerSoundEventBase
{
	const int MAX_VOLUME = 4;
	void StaminaSoundEventBase()
	{
		m_Type = EPlayerSoundEventType.STAMINA;
		m_HasPriorityOverTypes = EPlayerSoundEventType.STAMINA;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		return false;
	}
	
	override void OnPlay(PlayerBase player)
	{
		super.OnPlay(player);
		
		MaskBase mask = MaskBase.Cast(player.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Mask")));
		
		float rel_volume = 1;
		if(mask)
		{
			if(mask.IsExternalFilterAttached() || mask.HasIntegratedFilter())
			{
				rel_volume = Math.Lerp(MAX_VOLUME,1,mask.GetFilterQuantity01());
			}
		}
		
		if(m_SoundSetCallback)
			m_SoundSetCallback.SetVolumeRelative(rel_volume);
		#ifdef DEVELOPER
		if(!PluginDiagMenu.ENABLE_BREATH_VAPOR)//disabled in debug, do not spawn particles
			return;
		#endif
		if( player.CanSpawnBreathVaporEffect() ) 
			player.SpawnBreathVaporEffect();
		
		
		//!!! beware of the ifdef above , most likely you want to put your code above it, otherwise it might not get executed !!!
		
	}
	

}
class StaminaDownLight extends StaminaSoundEventBase
{
	void StaminaDownLight()
	{
		m_ID = EPlayerSoundEventID.STAMINA_DOWN_LIGHT;
		m_SoundVoiceAnimEventClassID = 1;
	}
}

class StaminaDownHeavy extends StaminaSoundEventBase
{
	void StaminaDownHeavy()
	{
		m_ID = EPlayerSoundEventID.STAMINA_DOWN_HEAVY;
		m_SoundVoiceAnimEventClassID = 2;
	}
}

class StaminaUpHeavy extends StaminaSoundEventBase
{
	void StaminaUpHeavy()
	{
		m_ID = EPlayerSoundEventID.STAMINA_UP_HEAVY;
		m_SoundVoiceAnimEventClassID = 3;
	}
}

class StaminaUpLight extends StaminaSoundEventBase
{
	void StaminaUpLight()
	{
		m_ID = EPlayerSoundEventID.STAMINA_UP_LIGHT;
		m_SoundVoiceAnimEventClassID = 4;
	}
}

class StaminaUpEnd extends StaminaSoundEventBase
{
	void StaminaUpEnd()
	{
		m_ID = EPlayerSoundEventID.STAMINA_UP_END;
		m_SoundVoiceAnimEventClassID = 26;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		return false;
	}
}

class StaminaNormalDummy extends StaminaSoundEventBase
{
	void StaminaNormalDummy()
	{
		m_HasPriorityOverTypes = 0;
		m_Type = EPlayerSoundEventType.DUMMY;
		m_ID = EPlayerSoundEventID.STAMINA_NORMAL_DUMMY;
		m_IsDummyType = true;
		m_DummySoundLength = 3000;//in ms
	}
}


// Low filter
class StaminaLowFilterBase extends StaminaSoundEventBase
{
	void StaminaLowFilterBase()
	{
		m_ProcessPlaybackEvent = true;
	}
}

class StaminaLowFilterUpper extends StaminaLowFilterBase
{
	void StaminaLowFilterUpper()
	{
		m_ID = EPlayerSoundEventID.STAMINA_LOW_FILTER_UPPER;
		m_SoundVoiceAnimEventClassID = 33;
	}
}

class StaminaLowFilterMid extends StaminaLowFilterBase
{
	void StaminaLowFilterMid()
	{
		m_ID = EPlayerSoundEventID.STAMINA_LOW_FILTER_MID;
		m_SoundVoiceAnimEventClassID = 32;
	}
}

class StaminaLowFilterLower extends StaminaLowFilterBase
{
	void StaminaLowFilterLower()
	{
		m_ID = EPlayerSoundEventID.STAMINA_LOW_FILTER_LOWER;
		m_SoundVoiceAnimEventClassID = 31;
	}
}
