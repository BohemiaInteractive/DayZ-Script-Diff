class FlatCap_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class FlatCap_Black extends FlatCap_ColorBase {};
class FlatCap_Blue extends FlatCap_ColorBase {};
class FlatCap_Red extends FlatCap_ColorBase {};
class FlatCap_Brown extends FlatCap_ColorBase {};
class FlatCap_Grey extends FlatCap_ColorBase {};
class FlatCap_BrownCheck extends FlatCap_ColorBase {};
class FlatCap_GreyCheck extends FlatCap_ColorBase {};
class FlatCap_BlackCheck extends FlatCap_ColorBase {};