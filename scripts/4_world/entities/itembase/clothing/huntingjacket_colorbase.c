class HuntingJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class HuntingJacket_Autumn extends HuntingJacket_ColorBase {};
class HuntingJacket_Brown extends HuntingJacket_ColorBase {};
class HuntingJacket_Spring extends HuntingJacket_ColorBase {};
class HuntingJacket_Summer extends HuntingJacket_ColorBase {};
class HuntingJacket_Winter extends HuntingJacket_ColorBase {};