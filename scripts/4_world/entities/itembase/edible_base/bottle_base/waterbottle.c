class WaterBottle extends Bottle_Base
{
	const float DAMAGE_OVERHEAT_PER_S = 0.1;
	const float DAMAGE_ENVIRO_LIQUID_COEF_MIN = 1;
	const float DAMAGE_ENVIRO_LIQUID_COEF_MAX = 2;
	const float DAMAGE_ENVIRO_TEMPDIFF_MIN = 80; //min damage at this demperature diff
	const float DAMAGE_ENVIRO_TEMPDIFF_MAX = 10; //maximum damage at this demperature diff
	
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
		float liquidTemperature = GetGame().GetMission().GetWorldData().GetLiquidTypeEnviroTemperature(liquid_type);
		if (liquidTemperature >= GetItemOverheatThreshold())
		{
			float temperatureDiff = liquidTemperature - GetTemperature();
			float tTime = Math.Clamp(Math.InverseLerp(DAMAGE_ENVIRO_TEMPDIFF_MIN,DAMAGE_ENVIRO_TEMPDIFF_MAX,temperatureDiff),0,1);
			float temperatureDiffCoef = Math.Lerp(DAMAGE_ENVIRO_LIQUID_COEF_MIN,DAMAGE_ENVIRO_LIQUID_COEF_MAX,tTime);
			float damageVal = GetMaxHealth("","Health") / GetQuantityMax();
			DecreaseHealth(amount * damageVal * temperatureDiffCoef,false);
		}
	}
	
	override void AffectLiquidContainerOnTransfer(int liquidType, float amount, float sourceLiquidTemperature)
	{
		//does damage if receiving scalding liquid
		if (sourceLiquidTemperature >= GetItemOverheatThreshold())
		{
			float temperatureDiff = sourceLiquidTemperature - GetTemperature();
			float tTime = Math.Clamp(Math.InverseLerp(DAMAGE_ENVIRO_TEMPDIFF_MIN,DAMAGE_ENVIRO_TEMPDIFF_MAX,temperatureDiff),0,1);
			float temperatureDiffCoef = Math.Lerp(DAMAGE_ENVIRO_LIQUID_COEF_MIN,DAMAGE_ENVIRO_LIQUID_COEF_MAX,tTime);
			float damageVal = GetMaxHealth("","Health") / GetQuantityMax();
			DecreaseHealth(amount * damageVal * temperatureDiffCoef,false);
		}
	}
	
	//! disregards liquid boil threshold if filled
	override float GetItemOverheatThreshold()
	{
		return GetTemperatureMax();
	}
}