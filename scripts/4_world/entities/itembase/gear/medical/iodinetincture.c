class IodineTincture : Edible_Base
{
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDisinfectTarget);
		AddAction(ActionDisinfectSelf);;
		AddAction(ActionWashHandsItem);
		//AddAction(ActionForceDrinkDisinfectant);
		//AddAction(ActionDrinkDisinfectant);
	}
	
	override float GetDisinfectQuantity(int system = 0, Param param1 = null)
	{
		return (GetQuantityMax() * 0.05);
	}
}