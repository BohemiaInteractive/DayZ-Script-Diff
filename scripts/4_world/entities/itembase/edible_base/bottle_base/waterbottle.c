class WaterBottle extends Bottle_Base
{
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
	
	override void EEOnCECreate()
	{
		super.EEOnCECreate();

		WorldData data = GetGame().GetMission().GetWorldData();
		if (data)
		{
			float chance = data.GetAgentSpawnChance(eAgents.CHOLERA);
			int rand = Math.RandomFloat(0, 100);
			
			if (rand < chance)
				InsertAgent(eAgents.CHOLERA, 1);
		}
	}
	
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		InsertAgent(eAgents.CHOLERA, 1);
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
}