class OBRJacket_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};


class OBRJacket_Beige extends OBRJacket_ColorBase {};
class OBRJacket_Olive extends OBRJacket_ColorBase {};
class OBRJacket_Officer extends OBRJacket_ColorBase {};