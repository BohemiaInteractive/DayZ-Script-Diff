class MensShalwarPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class MensShalwarPants_Black extends MensShalwarPants_ColorBase {};
class MensShalwarPants_White extends MensShalwarPants_ColorBase {};
class MensShalwarPants_NavyBlue extends MensShalwarPants_ColorBase {};
class MensShalwarPants_Brown extends MensShalwarPants_ColorBase {};
class MensShalwarPants_Beige extends MensShalwarPants_ColorBase {};
class MensShalwarPants_Grey extends MensShalwarPants_ColorBase {};
class MensShalwarPants_SkyBlue extends MensShalwarPants_ColorBase {};