class SodaCan_ColorBase : Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceDrink);
		AddAction(ActionDrinkCan);
	}
	
	override int GetConsumptionPenaltyContext()
	{
		//! no penalty while drinking
		return EConsumptionPenaltyContext.NONE;
	}
}
