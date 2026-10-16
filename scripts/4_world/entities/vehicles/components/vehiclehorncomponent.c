class VehicleHornMode
{
	const int OFF 	= 0;
	const int SHORT = 1;
	const int LONG	= 2;
	
	const int COUNT = 3;
}

class VehicleHornComponent : Managed
{
	private EntityAI m_Parent;
	
	protected int m_Mode;
	protected bool m_NoiseSystemInitialized;
	
	protected ref NoiseParams m_NoisePar;
	protected NoiseSystem m_NoiseSystem;

	protected ref EffectSound m_SoundEffect;

	protected ref map<int, string> m_SoundToModeMap;

	void VehicleHornComponent(notnull EntityAI parent)
	{
		m_Parent = parent;
		
		m_SoundToModeMap = new map<int, string>();
	}

	void ~VehicleHornComponent()
	{
		Cleanup(m_SoundEffect);
	}
	
	void RegisterSound(int mode, string soundSet)
	{
		m_SoundToModeMap.Set(mode, soundSet);
	}

	void SetMode(int mode)
	{
		m_Mode = mode;

		if (g_Game.IsServer())
		{
			m_Parent.SetSynchDirty();
			if (m_NoiseSystemInitialized)
				GenerateHornAINoise(mode);
			else
				m_NoiseSystemInitialized = InitializeNoiseSystem();
		}
	}
	
	void HandleSound()
	{
		switch (m_Mode)
		{
			case VehicleHornMode.SHORT:
				m_Parent.PlaySoundSet(m_SoundEffect, m_SoundToModeMap[VehicleHornMode.SHORT], 0, 0, false);
				break;

			case VehicleHornMode.LONG:
				m_Parent.PlaySoundSet(m_SoundEffect, m_SoundToModeMap[VehicleHornMode.LONG], 0, 0, true);
				break;

			default:
				Cleanup(m_SoundEffect);
				break;
		}
	}
	
	void Cleanup(EffectSound sound)
	{
		SEffectManager.DestroyEffect(sound);
	}
		
	protected bool InitializeNoiseSystem()
	{
		if (g_Game.IsServer())
		{
			m_NoiseSystem = g_Game.GetNoiseSystem();
			if (m_NoiseSystem && !m_NoisePar)
			{
				m_NoisePar = new NoiseParams();
				m_NoisePar.LoadFromPath("cfgVehicles " + m_Parent.GetType() + " NoiseCarHorn");
				
				return true;
			}
		}
		
		return false;
	}	
	
	protected void GenerateHornAINoise(int hornType)
	{
		if (hornType != VehicleHornMode.OFF)
		{
			if (m_NoiseSystem && m_NoisePar)
			{
				float noiseMultiplier = 1.0;
				if (hornType == VehicleHornMode.LONG)
					noiseMultiplier = 2.0;

				// weather dampening is now done engine-side
				
				m_NoiseSystem.AddNoiseTarget(m_Parent.GetPosition(), 5, m_NoisePar, noiseMultiplier);
			}
		}
	}
}
