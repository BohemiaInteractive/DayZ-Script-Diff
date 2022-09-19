class USMCJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class USMCJacket_Desert extends USMCJacket_ColorBase {};
class USMCJacket_Woodland extends USMCJacket_ColorBase {};