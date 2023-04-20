class USMCJacket_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class USMCJacket_Desert extends USMCJacket_ColorBase {};
class USMCJacket_Woodland extends USMCJacket_ColorBase {};