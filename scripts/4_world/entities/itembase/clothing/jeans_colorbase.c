class Jeans_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Jeans_Black extends Jeans_ColorBase {};
class Jeans_Blue extends Jeans_ColorBase {};
class Jeans_Brown extends Jeans_ColorBase {};
class Jeans_Green extends Jeans_ColorBase {};
class Jeans_Grey extends Jeans_ColorBase {};
class Jeans_BlueDark extends Jeans_ColorBase {};

class ShortJeans_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class ShortJeans_Black extends ShortJeans_ColorBase {};
class ShortJeans_Blue extends ShortJeans_ColorBase {};
class ShortJeans_Brown extends ShortJeans_ColorBase {};
class ShortJeans_Darkblue extends ShortJeans_ColorBase {};
class ShortJeans_Green extends ShortJeans_ColorBase {};
class ShortJeans_Red extends ShortJeans_ColorBase {};