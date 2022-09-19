class SodaCan_ColorBase : Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionWashHandsItem);
		AddAction(ActionForceDrink);
		AddAction(ActionDrinkCan);
	}
};
