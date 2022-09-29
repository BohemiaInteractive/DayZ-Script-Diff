class CanisterGasoline extends Bottle_Base
{
	void CanisterGasoline()
	{
	}
	
	void ~CanisterGasoline()
	{
	}
	
	override bool IsContainer()
	{
		return true;
	}
	
	override string GetPouringSoundset()
	{
		return "emptyVessle_CanisterGasoline_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetHard()
	{
		return "pour_HardGround_GasolineCanister_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetSoft()
	{
		return "pour_SoftGround_GasolineCanister_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetWater()
	{
		return "pour_Water_GasolineCanister_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetHard()
	{
		return "pour_End_HardGround_GasolineCanister_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetSoft()
	{
		return "pour_End_SoftGround_GasolineCanister_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetWater()
	{
		return "pour_End_Water_GasolineCanister_SoundSet";
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_GASOLINECANISTER;
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}	
		if ( parent && (parent.IsKindOf("CanisterGasoline"))/* && !(parent.IsKindOf("Container_Base"))*/)
		{
			return false;
		}
		
		return true;
	}
	
	override bool IsOpen()
	{
		return true;
	}
}