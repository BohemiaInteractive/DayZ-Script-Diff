class FilteringBottle: Bottle_Base
{
	static const float DAMAGE_CONSUME_PER_ML = 0.01;
	static const float DAMAGE_OVERHEAT_PER_S = 1;
	
	override string GetPouringSoundset()
	{
		return "emptyVessle_WaterBottle_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetHard()
	{
		return "pour_HardGround_WatterBottle_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetSoft()
	{
		return "pour_SoftGround_WatterBottle_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetWater()
	{
		return "pour_Water_WatterBottle_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetHard()
	{
		return "pour_End_HardGround_WatterBottle_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetSoft()
	{
		return "pour_End_SoftGround_WatterBottle_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetWater()
	{
		return "pour_End_Water_WatterBottle_SoundSet";
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}	
		if ( parent && (parent.IsKindOf("WatterBottle"))/* && !(parent.IsKindOf("Container_Base"))*/)
		{
			return false;
		}
		
		return true;
	}
	
	override bool IsOpen()
	{
		return true;
	}
	
	override int FilterAgents(int agentsIn)
	{
		int agentsOut = agentsIn & (~eAgents.HEAVYMETAL) & (~eAgents.CHOLERA);
		
		return agentsOut;
	}
	
	override void OnConsume(float amount, PlayerBase consumer)
	{
		super.OnConsume(amount,consumer);
		
		DamageBottleConsume(amount,consumer);
	}
	
	protected void DamageBottleConsume(float amount, PlayerBase consumer)
	{
		DecreaseHealth(amount * DAMAGE_CONSUME_PER_ML,false);
	}
	
	override void AffectLiquidContainerOnFill(int liquid_type, float amount)
	{
		if (liquid_type == LIQUID_HOTWATER)
		{
			float damageVal = GetMaxHealth("","Health") / GetQuantityMax();
			DecreaseHealth(amount * damageVal,false);
		}
	}
	
	//! disregards liquid boil threshold if filled
	override float GetItemOverheatThreshold()
	{
		return GetTemperatureMax();
	}
	
	override void OnItemOverheat(float deltaTime)
	{
		float damageVal = deltaTime * DAMAGE_OVERHEAT_PER_S;
		DecreaseHealth(damageVal,false);
	}
};
