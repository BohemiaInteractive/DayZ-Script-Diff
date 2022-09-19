class USMCPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class USMCPants_Desert extends USMCPants_ColorBase {};
class USMCPants_Woodland extends USMCPants_ColorBase {};