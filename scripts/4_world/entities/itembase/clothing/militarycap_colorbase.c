class MilitaryCap_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MilitaryCap_BDU extends MilitaryCap_ColorBase {};
class MilitaryCap_Desert extends MilitaryCap_ColorBase {};
class MilitaryCap_Woodland extends MilitaryCap_ColorBase {};
