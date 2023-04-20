class SlacksPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class SlacksPants_Beige extends SlacksPants_ColorBase {};
class SlacksPants_Black extends SlacksPants_ColorBase {};
class SlacksPants_Blue extends SlacksPants_ColorBase {};
class SlacksPants_Brown extends SlacksPants_ColorBase {};
class SlacksPants_DarkGrey extends SlacksPants_ColorBase {};
class SlacksPants_Khaki extends SlacksPants_ColorBase {};
class SlacksPants_LightGrey extends SlacksPants_ColorBase {};
class SlacksPants_White extends SlacksPants_ColorBase {};