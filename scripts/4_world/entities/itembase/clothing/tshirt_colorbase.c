class TShirt_ColorBase extends Clothing 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TShirt_Beige extends TShirt_ColorBase {};
class TShirt_Black extends TShirt_ColorBase {};
class TShirt_Blue extends TShirt_ColorBase {};
class TShirt_Dyed extends TShirt_ColorBase {};
class TShirt_Green extends TShirt_ColorBase {};
class TShirt_OrangeWhiteStripes extends TShirt_ColorBase {};
class TShirt_Red extends TShirt_ColorBase {};
class TShirt_RedBlackStripes extends TShirt_ColorBase {};
class TShirt_White extends TShirt_ColorBase {};
class TShirt_Grey extends TShirt_ColorBase {};
