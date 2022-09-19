class CanvasPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class CanvasPants_Beige extends CanvasPants_ColorBase {};
class CanvasPants_Blue extends CanvasPants_ColorBase {};
class CanvasPants_Grey extends CanvasPants_ColorBase {};
class CanvasPants_Red extends CanvasPants_ColorBase {};
class CanvasPants_Violet extends CanvasPants_ColorBase {};

class CanvasPantsMidi_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class CanvasPantsMidi_Beige extends CanvasPantsMidi_ColorBase {};
class CanvasPantsMidi_Blue extends CanvasPantsMidi_ColorBase {};
class CanvasPantsMidi_Grey extends CanvasPantsMidi_ColorBase {};
class CanvasPantsMidi_Red extends CanvasPantsMidi_ColorBase {};
class CanvasPantsMidi_Violet extends CanvasPantsMidi_ColorBase {};