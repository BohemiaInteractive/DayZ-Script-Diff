class Pen_ColorBase: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionWritePaper);
	}
};
class Pen_Black: Pen_ColorBase {};
class Pen_Red: Pen_ColorBase {};
class Pen_Green: Pen_ColorBase {};
class Pen_Blue: Pen_ColorBase {};