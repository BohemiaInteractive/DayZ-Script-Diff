class RidersJacket_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class RidersJacket_Black extends RidersJacket_ColorBase {};