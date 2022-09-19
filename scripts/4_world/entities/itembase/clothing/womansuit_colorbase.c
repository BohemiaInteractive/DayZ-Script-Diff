class WomanSuit_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class WomanSuit_Beige extends WomanSuit_ColorBase {};
class WomanSuit_Black extends WomanSuit_ColorBase {};
class WomanSuit_Blue extends WomanSuit_ColorBase {};
class WomanSuit_Brown extends WomanSuit_ColorBase {};
class WomanSuit_DarkGrey extends WomanSuit_ColorBase {};
class WomanSuit_Khaki extends WomanSuit_ColorBase {};
class WomanSuit_LightGrey extends WomanSuit_ColorBase {};
class WomanSuit_White extends WomanSuit_ColorBase {};