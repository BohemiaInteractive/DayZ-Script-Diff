class HikingJacket_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class HikingJacket_Black extends HikingJacket_ColorBase {};
class HikingJacket_Blue extends HikingJacket_ColorBase {};
class HikingJacket_Red extends HikingJacket_ColorBase {};
class HikingJacket_Green extends HikingJacket_ColorBase {};