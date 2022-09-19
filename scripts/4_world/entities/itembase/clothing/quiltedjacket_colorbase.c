class QuiltedJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class QuiltedJacket_Black extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Green extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Blue extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Red extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Grey extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Orange extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Yellow extends QuiltedJacket_ColorBase {};
class QuiltedJacket_Violet extends QuiltedJacket_ColorBase {};