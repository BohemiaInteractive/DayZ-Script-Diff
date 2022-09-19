class BaseballCap_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};


class BaseballCap_Blue extends BaseballCap_ColorBase {};
class BaseballCap_Beige extends BaseballCap_ColorBase {};
class BaseballCap_Black extends BaseballCap_ColorBase {};
class BaseballCap_Olive extends BaseballCap_ColorBase {};
class BaseballCap_Pink extends BaseballCap_ColorBase {};
class BaseballCap_Red extends BaseballCap_ColorBase {};
class BaseballCap_Camo extends BaseballCap_ColorBase {};
class BaseballCap_CMMG_Black extends BaseballCap_ColorBase {};
class BaseballCap_CMMG_Pink extends BaseballCap_ColorBase {};