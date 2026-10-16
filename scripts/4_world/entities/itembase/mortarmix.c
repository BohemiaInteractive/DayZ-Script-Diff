class MortarMix extends ItemBase
{	
	override void SetActions()
	{
		super.SetActions();
        AddAction(ActionMixMortarAtWater);
		AddAction(ActionMixMortarAtWell);
	}
	
}