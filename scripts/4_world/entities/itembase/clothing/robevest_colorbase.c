class RobeVest_ColorBase : Vest_Base {
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class RobeVest_Beige : RobeVest_ColorBase {};
class RobeVest_Brown : RobeVest_ColorBase {};
class RobeVest_Green : RobeVest_ColorBase {};
class RobeVest_Grey : RobeVest_ColorBase {};
class RobeVest_Navy : RobeVest_ColorBase {};