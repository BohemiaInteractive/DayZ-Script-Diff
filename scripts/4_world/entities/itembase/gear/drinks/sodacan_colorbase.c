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
class SodaCan_Cola: SodaCan_ColorBase{};
class SodaCan_Fronta: SodaCan_ColorBase{};
class SodaCan_Kvass: SodaCan_ColorBase{};
class SodaCan_Pipsi: SodaCan_ColorBase{};
class SodaCan_Spite: SodaCan_ColorBase{};
class SodaCan_Takola: SodaCan_ColorBase{};
class SodaCan_ZumZam: SodaCan_ColorBase{};
class SmallGlassBottle_ColorBase: SodaCan_ColorBase{};
class SmallGlassBottle_Afza: SmallGlassBottle_ColorBase{};
class SmallGlassBottle_Jallab: SmallGlassBottle_ColorBase{};
