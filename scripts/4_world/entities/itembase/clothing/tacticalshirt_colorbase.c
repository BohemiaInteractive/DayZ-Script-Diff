class TacticalShirt_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TacticalShirt_Grey extends TacticalShirt_ColorBase {};
class TacticalShirt_Black extends TacticalShirt_ColorBase {};
class TacticalShirt_Olive extends TacticalShirt_ColorBase {};
class TacticalShirt_Tan extends TacticalShirt_ColorBase {};