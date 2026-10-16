class LinenVest_ColorBase extends Vest_Base {
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class LinenVest_DarkGrey extends LinenVest_ColorBase {};
class LinenVest_Grey extends LinenVest_ColorBase {};
class LinenVest_Black extends LinenVest_ColorBase {};
class LinenVest_Green extends LinenVest_ColorBase {};
class LinenVest_Brown extends LinenVest_ColorBase {};
class LinenVest_NavyBlue extends LinenVest_ColorBase {};