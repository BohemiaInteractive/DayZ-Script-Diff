class Canteen extends Bottle_Base
{
	void Canteen()
	{

	}
	
	void ~Canteen()
	{
		
	}
	
	override string GetPouringSoundset()
	{
		return "emptyVessle_Canteen_SoundSet";
	}
	
		override string GetEmptyingLoopSoundsetHard()
	{
		return "pour_HardGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetSoft()
	{
		return "pour_SoftGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetWater()
	{
		return "pour_Water_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetHard()
	{
		return "pour_End_HardGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetSoft()
	{
		return "pour_End_SoftGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetWater()
	{
		return "pour_End_Water_Canteen_SoundSet";
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo(parent) ) {return false;}	
		if ( parent && (parent.IsKindOf("Canteen"))/* && !(parent.IsKindOf("Container_Base"))*/)
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
}