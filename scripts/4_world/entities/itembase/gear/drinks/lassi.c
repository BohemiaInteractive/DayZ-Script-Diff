class Lassi_Colorbase : Edible_Base
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
class Lassi_Mango: Lassi_Colorbase{};
class Lassi_Mint: Lassi_Colorbase{};
class Lassi_Masala: Lassi_Colorbase{};
