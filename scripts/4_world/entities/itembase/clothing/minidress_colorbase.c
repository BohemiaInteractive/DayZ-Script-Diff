class MiniDress_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class MiniDress_Pink extends MiniDress_ColorBase {};
class MiniDress_PinkChecker extends MiniDress_ColorBase {};
class MiniDress_RedChecker extends MiniDress_ColorBase {};
class MiniDress_WhiteChecker extends MiniDress_ColorBase {};
class MiniDress_GreenChecker extends MiniDress_ColorBase {};
class MiniDress_BrownChecker extends MiniDress_ColorBase {};
class MiniDress_BlueChecker extends MiniDress_ColorBase {};
class MiniDress_BlueWithDots extends MiniDress_ColorBase {};