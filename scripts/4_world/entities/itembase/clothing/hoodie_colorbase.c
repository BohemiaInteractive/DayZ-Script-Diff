class Hoodie_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Hoodie_Blue extends Hoodie_ColorBase {};
class Hoodie_Black extends Hoodie_ColorBase {};
class Hoodie_Brown extends Hoodie_ColorBase {};
class Hoodie_Green extends Hoodie_ColorBase {};
class Hoodie_Grey extends Hoodie_ColorBase {};
class Hoodie_Red extends Hoodie_ColorBase {};