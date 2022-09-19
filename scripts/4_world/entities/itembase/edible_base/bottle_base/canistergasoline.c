class CanisterGasoline extends Bottle_Base
{
	void CanisterGasoline()
	{
		m_LiquidEmptyRate = 2000;
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
/*
	override void SetActions()
	{
//		super.SetActions();

		AddAction(ActionWorldLiquidActionSwitch);
		AddAction(ActionFillCoolant);
		AddAction(ActionFillFuel);
		AddAction(ActionFillGeneratorTank);
		AddAction(ActionExtinguishFireplaceByLiquid);
		AddAction(ActionFillBottleBase);
		AddAction(ActionWaterGardenSlot);
		AddAction(ActionWaterPlant);
		AddAction(ActionForceDrink);
		AddAction(ActionEmptyBottleBase);
		AddAction(ActionDrink);
		
	}
*/
}