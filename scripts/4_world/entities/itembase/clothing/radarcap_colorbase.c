class RadarCap_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class RadarCap_Black extends RadarCap_ColorBase {};
class RadarCap_Blue extends RadarCap_ColorBase {};
class RadarCap_Brown extends RadarCap_ColorBase {};
class RadarCap_Green extends RadarCap_ColorBase {};
class RadarCap_Red extends RadarCap_ColorBase {};