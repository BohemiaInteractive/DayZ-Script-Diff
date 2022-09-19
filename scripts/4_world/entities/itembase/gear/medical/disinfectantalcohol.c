class DisinfectantAlcohol: Edible_Base
{
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
	
	override float GetDisinfectQuantity(int system = 0, Param param1 = null)
	{
		return (GetQuantityMax() * 0.1);
	}
	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDisinfectTarget);
		AddAction(ActionDisinfectSelf);
		AddAction(ActionFillGeneratorTank);
		AddAction(ActionWashHandsItem);
		//AddAction(ActionForceDrinkAlcohol);
		//AddAction(ActionDrinkAlcohol);
	}
};
