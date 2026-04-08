class WinterMilitaryCoat_ColorBase : Top_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class WinterMilitaryCoat_Grey : WinterMilitaryCoat_ColorBase {};
class WinterMilitaryCoat_Brown : WinterMilitaryCoat_ColorBase {};
class WinterMilitaryCoat_DarkGrey : WinterMilitaryCoat_ColorBase {};