class WitchHood extends Clothing //old name or typo? Leaving it for purposes of backwards compatibility
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class WitchHood_ColorBase : HeadGear_Base {};
class WitchHood_Black : WitchHood_ColorBase {};
class WitchHood_Brown : WitchHood_ColorBase {};
class WitchHood_Red : WitchHood_ColorBase {};
