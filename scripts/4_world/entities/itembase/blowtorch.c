class Blowtorch extends ItemBase
{
	const string TEXTURE_FLAME 		= "dz\\gear\\cooking\\data\\flame_butane_ca.paa";
	const string ANIM_PHASE_FLAME	= "FlameHide";
	const string SOUND_BURNING 		= "Blowtorch_Loop_SoundSet";
	
	protected BlowtorchLight	m_Light;
	
	protected EffectSound 		m_SoundBurningLoop;

	override void OnWorkStart()
	{
		super.OnWorkStart();

		#ifndef SERVER
		m_Light = BlowtorchLight.Cast(ScriptedLightBase.CreateLight(BlowtorchLight, "0 0 0"));
		m_Light.AttachOnMemoryPoint(this, "light");
		#endif

		RefreshFlameVisual(true);
		SoundBurningStart();
	}
	
	override void OnWorkStop()
	{
		#ifndef SERVER
		if (m_Light)
		{
			m_Light.FadeOut();
		}
		#endif
		
		RefreshFlameVisual(false);
		SoundBurningStop();
	}
	
	protected void RefreshFlameVisual(bool working = false)
	{
	    if (working)
	    {
	        SetObjectTexture(0, TEXTURE_FLAME);
	        SetAnimationPhase(ANIM_PHASE_FLAME, 0.0);
	    }
	    else
	    {
	        SetObjectTexture(0, "");
	        SetAnimationPhase(ANIM_PHASE_FLAME, 1.0);
	    }
	}
	
	protected void SoundBurningStart()
	{
		PlaySoundSetLoop(m_SoundBurningLoop, SOUND_BURNING, 0.1, 0.0);
	}
	
	protected void SoundBurningStop()
	{
		StopSoundSet(m_SoundBurningLoop);
	}	
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}

		return !GetCompEM().IsSwitchedOn();
	}
	
	override bool CanRemoveFromCargo(EntityAI parent)
	{
		return true;
	}
	
	override bool IsIgnited()
	{
		return GetCompEM().IsWorking();
	}
	
	override void OnIgnitedTarget(EntityAI ignited_item)
	{
		if (GetGame().IsServer())
		{
			if (GetGasCanister())
			{
				ComponentEnergyManager canisterEM = GetGasCanister().GetCompEM();
				if (canisterEM)
					canisterEM.AddEnergy(-1 * (GetCompEM().GetEnergyUsage() * UATimeSpent.FIREPLACE_IGNITE));
			}
		}
	}
	
	override bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return ignite_target.CanBeIgnitedBy(this);
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionLightItemOnFireWithBlowtorch);
		AddAction(ActionRepairCarEngineWithBlowtorch);
		AddAction(ActionRepairCarChassisWithBlowtorch);
		AddAction(ActionRepairCarPartWithBlowtorch);
		AddAction(ActionRepairItemWithBlowtorch);
	}
	
	protected EntityAI GetGasCanister()
	{
		if (GetInventory().AttachmentCount() != 0)
		{
			return GetInventory().GetAttachmentFromIndex(0);
		}
		
		return null;
	}
	
	bool HasEnoughEnergyForRepair(float pTime)
	{
		if (GetGasCanister())
		{
			ComponentEnergyManager canisterEM = GetGasCanister().GetCompEM();
			if (canisterEM)
			{
				return canisterEM.GetEnergy() > GetCompEM().GetEnergyUsage() * pTime;
			}
		}
		
		return false;
	}
	
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("LargeGasCanister");
	}
}