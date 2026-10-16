class OBRCap_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class OBRCap_Beige extends OBRCap_ColorBase {};
class OBRCap_Olive extends OBRCap_ColorBase {};
