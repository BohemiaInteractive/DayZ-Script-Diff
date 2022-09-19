class BroomBase : FlammableBase
{
	override void Init()
	{
		super.Init();
		m_DecraftResult = "LongWoodenStick";
		m_ParticleLocalPos = Vector(0, 1.2, 0);
		//GetCompEM().SetEnergyUsage(20);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionBreakLongWoodenStick);
	}
	
	override bool CanReceiveUpgrade()
	{
		return false;
	}
	
	override void OnWorkStart()
	{
		super.OnWorkStart();
		CalculateQuantity();
	}
	
	override void ApplyResultModifications(ItemBase result)
	{
		result.SetHealth(result.GetHealthLevelValue(2,""));
		result.SetQuantity(1);
	}
	
	override bool CanTransformIntoStick()
	{
		if ( GetGame().IsServer() && !IsIgnited() && GetEnergy() < 1 && !IsSetForDeletion())
			return true;
		else
			return false;
	}
	
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		GetCompEM().SetEnergy0To1(GetHealth01("",""));
	}
	
	
	override void CalculateQuantity()
	{
		if (GetGame().IsServer())
		{
			float currentHealth01 = GetHealth01();
			float currentEnergy01 = GetCompEM().GetEnergy0To1();
		
			//Health needs to copy internal energy, but only if that means it will be going down from its current value(the item can't heal itself after obtaining damage just because it has full internal energy)
			SetHealth01("","",Math.Min(currentHealth01,currentEnergy01));
			//Energy needs to copy health, but only if it is higher(in 01 terms) than health, that means if an item with full energy gets damaged and lit, the internal energy needs to be adjusted to follow the health. Situations where internal energy is lower than health are handled on the line above
			GetCompEM().SetEnergy0To1(GetHealth01());
		}
	}
	
	override void UpdateParticle()
	{
		float normalizedQuant = GetQuantity() / GetQuantityMax();

		if (!m_FireParticle)
		{
			m_FireParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.BROOM_TORCH_T1, this, m_ParticleLocalPos);
		}
		m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY, 1.0);
		m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY_RND, 1.0);	
		if (m_FireParticle)
		{
			float scale = Math.Max(normalizedQuant * 2.4, 0.4);
			
			m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.SIZE, scale);
		}
	}
	
	// DEBUG BELLOW
	void DebugSetHealthAndEnergy(float time)
	{
		float max_energy = GetCompEM().GetEnergyMaxPristine();
		float health01 = Math.InverseLerp(0, max_energy, time);
		SetHealth01("","", health01);
		GetCompEM().SetEnergy( time );
	}
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "SetBurnTimeTo15Secs";
		button2 = "SetBurnTimeTo1Min";
		button3 = "SetBurnTimeTo10Min";
		button4 = "SetBurnTimeToMax";
	}
	
	
	override void OnDebugButtonPressServer(int button_index)
	{
		if (button_index == 1)
		{
			DebugSetHealthAndEnergy(15);
			OnIgnitedThis(null);
		}
		
		if (button_index == 2)
		{
			DebugSetHealthAndEnergy(60);
			OnIgnitedThis(null);
		}
		
		if (button_index == 3)
		{
			DebugSetHealthAndEnergy(600);
			OnIgnitedThis(null);
		}
		
		if (button_index == 4)
		{
			DebugSetHealthAndEnergy(GetCompEM().GetEnergyMaxPristine());
			OnIgnitedThis(null);
		}
	}
	
}

class Broom: BroomBase
{
	override string GetBurningMaterial()
	{
		return "DZ\\gear\\tools\\data\\broom_emissive.rvmat";
	}
	
	override string GetBurntMaterial()
	{
		return "DZ\\gear\\tools\\data\\broom_burn.rvmat";
	}
};