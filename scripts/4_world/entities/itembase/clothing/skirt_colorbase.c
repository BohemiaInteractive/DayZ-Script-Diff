class Skirt_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Skirt_Blue extends Skirt_ColorBase {};
class Skirt_Red extends Skirt_ColorBase {};
class Skirt_White extends Skirt_ColorBase {};
class Skirt_Yellow extends Skirt_ColorBase {};