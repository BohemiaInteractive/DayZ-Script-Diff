class IodineTincture : Edible_Base
{
	
	override void InitItemVariables()
	{
		super.InitItemVariables();

		can_this_be_combined 	= true;
		m_VarLiquidType 		= GetLiquidTypeInit();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDisinfectTarget);
		AddAction(ActionDisinfectSelf);
		AddAction(ActionWashHandsItemContinuous);
	}
	
	override float GetDisinfectQuantity(int system = 0, Param param1 = null)
	{
		return (GetQuantityMax() * 0.05);
	}
}
