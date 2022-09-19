class MeleeAttackSoundEvents extends PlayerSoundEventBase
{
	void MeleeAttackSoundEvents()
	{
		m_Type = EPlayerSoundEventType.MELEE;
		m_HasPriorityOverTypes = -1;
	}
	
	override bool CanPlay(PlayerBase player)
	{
		return true;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		if( type_other == EPlayerSoundEventType.DAMAGE )
		{
			return false;
		}
		return true;
	}
	
	override void OnEnd()
	{
		super.OnEnd();
		if(m_Player)
			StaminaSoundHandlerClient.Cast(m_Player.m_PlayerSoundManagerClient.GetHandler(eSoundHandlers.STAMINA)).PostponeStamina(800);
	}
}

class MeleeAttackLightEvent extends MeleeAttackSoundEvents
{
	void MeleeAttackLightEvent()
	{
		m_ID = EPlayerSoundEventID.MELEE_ATTACK_LIGHT;
		m_SoundVoiceAnimEventClassID = 16;
	}
}

class MeleeAttackHeavyEvent extends MeleeAttackSoundEvents
{
	void MeleeAttackHeavyEvent()
	{
		m_ID = EPlayerSoundEventID.MELEE_ATTACK_HEAVY;
		m_SoundVoiceAnimEventClassID = 17;
	}
	
}