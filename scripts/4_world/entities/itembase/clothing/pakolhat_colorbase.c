class PakolHat_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class PakolHat_Beige extends PakolHat_ColorBase {};
class PakolHat_Black extends PakolHat_ColorBase {};
class PakolHat_Brown extends PakolHat_ColorBase {};
class PakolHat_Grey extends PakolHat_ColorBase {};
class PakolHat_White extends PakolHat_ColorBase {};
