class ManSuit_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class ManSuit_Beige extends ManSuit_ColorBase {};
class ManSuit_Black extends ManSuit_ColorBase {};
class ManSuit_Blue extends ManSuit_ColorBase {};
class ManSuit_Brown extends ManSuit_ColorBase {};
class ManSuit_DarkGrey extends ManSuit_ColorBase {};
class ManSuit_Khaki extends ManSuit_ColorBase {};
class ManSuit_LightGrey extends ManSuit_ColorBase {};
class ManSuit_White extends ManSuit_ColorBase {};