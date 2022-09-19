class RidersJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class RidersJacket_Black extends RidersJacket_ColorBase {};