class WoolGloves_ColorBase extends Gloves_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class WoolGloves_Green extends WoolGloves_ColorBase {};
class WoolGloves_Black extends WoolGloves_ColorBase {};
class WoolGloves_Tan extends WoolGloves_ColorBase {};
class WoolGloves_White extends WoolGloves_ColorBase {};
class WoolGloves_ChristmasBlue extends WoolGloves_ColorBase {};
class WoolGloves_ChristmasRed extends WoolGloves_ColorBase {};
