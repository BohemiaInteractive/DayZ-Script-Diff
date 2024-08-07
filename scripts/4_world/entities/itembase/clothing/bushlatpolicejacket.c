class BushlatPoliceJacket_ColorBase : Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class BushlatPoliceJacket_Blue : BushlatPoliceJacket_ColorBase{};