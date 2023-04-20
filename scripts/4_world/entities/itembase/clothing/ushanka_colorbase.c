class Ushanka_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Ushanka_Black extends Ushanka_ColorBase {};
class Ushanka_Blue extends Ushanka_ColorBase {};
class Ushanka_Green extends Ushanka_ColorBase {};
