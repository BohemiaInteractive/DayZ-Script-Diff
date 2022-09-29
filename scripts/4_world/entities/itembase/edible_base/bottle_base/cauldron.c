class Cauldron extends Bottle_Base
{
	void Cauldron()
	{

	}
	
	void ~Cauldron()
	{

	}
	
	override bool IsContainer()
	{
		return true;
	}
	
	override string GetPouringSoundset()
	{
		return "emptyVessle_Pot_SoundSet";
	}
	
		override string GetEmptyingLoopSoundsetHard()
	{
		return "pour_HardGround_Pot_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetSoft()
	{
		return "pour_SoftGround_Pot_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetWater()
	{
		return "pour_Water_Pot_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetHard()
	{
		return "pour_End_HardGround_Pot_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetSoft()
	{
		return "pour_End_SoftGround_Pot_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetWater()
	{
		return "pour_End_Water_Pot_SoundSet";
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo( parent ) )
			return false;
		
		if ( parent && IsCargoException4x3( parent ) )
			return false;
		
		return true;
	}
	
	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if ( !super.CanReceiveItemIntoCargo( item ) )
			return false;

		if ( IsCargoException4x3( item ) )
			return false;

		return true;
	}
	
	override bool CanLoadItemIntoCargo(EntityAI item)
	{
		if ( !super.CanLoadItemIntoCargo( item ) )
			return false;

		if ( IsCargoException4x3( item ) )
			return false;

		return true;
	}
	
	override bool IsOpen()
	{
		return true;
	}
	
	override bool CanHaveWetness()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionDrink);
		RemoveAction(ActionEmptyBottleBase);
		AddAction(ActionFillBottleBase);
		AddAction(ActionWaterGardenSlot);
		AddAction(ActionWaterPlant);
		AddAction(ActionForceDrink);
		AddAction(ActionDrainLiquid);
		AddAction(ActionPourLiquid);
		//AddAction(ActionTransferLiquid);
		AddAction(ActionEmptyCookingPot);
		AddAction(ActionDrinkCookingPot);
	}
}