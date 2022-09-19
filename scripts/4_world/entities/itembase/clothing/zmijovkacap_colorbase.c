class ZmijovkaCap_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class ZmijovkaCap_Black extends ZmijovkaCap_ColorBase {};
class ZmijovkaCap_Blue extends ZmijovkaCap_ColorBase {};
class ZmijovkaCap_Brown extends ZmijovkaCap_ColorBase {};
class ZmijovkaCap_Green extends ZmijovkaCap_ColorBase {};
class ZmijovkaCap_Red extends ZmijovkaCap_ColorBase {};