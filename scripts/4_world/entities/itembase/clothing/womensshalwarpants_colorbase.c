class WomensShalwarPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class WomensShalwarPants_Red extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_Pink extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_NavyBlue extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_Olive extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_Orange extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_Purple extends WomensShalwarPants_ColorBase {};
class WomensShalwarPants_SkyBlue extends WomensShalwarPants_ColorBase {};