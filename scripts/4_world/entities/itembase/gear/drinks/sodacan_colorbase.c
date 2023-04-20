class SodaCan_ColorBase : Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionWashHandsItemContinuous);
		AddAction(ActionForceDrink);
		AddAction(ActionDrinkCan);
	}
};
