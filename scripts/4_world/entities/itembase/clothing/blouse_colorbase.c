class Blouse_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Blouse_Blue extends Blouse_ColorBase {};
class Blouse_Green extends Blouse_ColorBase {};
class Blouse_Violet extends Blouse_ColorBase {};
class Blouse_White extends Blouse_ColorBase {};