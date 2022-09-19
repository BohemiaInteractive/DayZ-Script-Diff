class WoolCoat_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class WoolCoat_Black extends WoolCoat_ColorBase {};
class WoolCoat_Red extends WoolCoat_ColorBase {};
class WoolCoat_Blue extends WoolCoat_ColorBase {};
class WoolCoat_Green extends WoolCoat_ColorBase {};
class WoolCoat_Beige extends WoolCoat_ColorBase {};
class WoolCoat_RedCheck extends WoolCoat_ColorBase {};
class WoolCoat_BlackCheck extends WoolCoat_ColorBase {};
class WoolCoat_BlueCheck extends WoolCoat_ColorBase {};
class WoolCoat_GreyCheck extends WoolCoat_ColorBase {};
class WoolCoat_BrownCheck extends WoolCoat_ColorBase {};