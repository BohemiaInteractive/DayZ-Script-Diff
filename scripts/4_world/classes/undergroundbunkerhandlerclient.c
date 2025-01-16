class UndergroundBunkerHandlerClient : UndergroundHandlerClient
{
	protected bool m_IsDoorOpening;
	protected float m_AnimPhasePrev;
	
	override protected bool CalculateEyeAcco(float timeSlice)
	{
		if (m_IsDoorOpening)
		{
			float adjustedOpen = Math.InverseLerp(0, 1, m_EyeAccoTarget);
			adjustedOpen = Math.Clamp(adjustedOpen,0,1);
			m_EyeAcco = Easing.EaseInOutCubic(adjustedOpen);
		}
		else
		{
			float adjustedClose = Math.InverseLerp(0.07, 1, m_EyeAccoTarget);
			adjustedClose = Math.Clamp(adjustedClose,0,1);
			m_EyeAcco = Easing.EaseOutQuad(adjustedClose);
		}
		
		return true;
	}
	
	override protected void CalculateEyeAccoTarget()
	{
		UndergroundBunkerTrigger trigger = UndergroundBunkerTrigger.Cast(m_TransitionalTrigger);
		if (trigger)
		{
			EntityAI building = EntityAI.Cast(trigger.GetParent());
			if (building)
			{
				float animPhase = building.GetAnimationPhase(trigger.m_LinkedDoorSelection);
				m_IsDoorOpening = animPhase > m_AnimPhasePrev;
				m_AnimPhasePrev = animPhase;
				m_EyeAccoTarget = animPhase;
				
			}
			
		}
	}
	
	override protected void ProcessLighting(float timeSlice)
	{
		//we don't want to change the lighting
	}
	
	override protected void ApplyEyeAcco()
	{
		m_LightingLerp = 0;
		super.ApplyEyeAcco();
	}
	
	override protected void ProcessSound(float timeSlice)
	{
		GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(m_EyeAcco, 0);
	}
	
	override protected void PlayAmbientSound()
	{
		m_Player.PlaySoundSetLoop(m_AmbientSound, "Warhead_Storage_Ambient_SoundSet",3,3);
	}
}