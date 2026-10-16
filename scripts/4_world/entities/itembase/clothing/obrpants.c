class OBRPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};


class OBRPants_Beige extends OBRPants_ColorBase {};
class OBRPants_Olive extends OBRPants_ColorBase {};