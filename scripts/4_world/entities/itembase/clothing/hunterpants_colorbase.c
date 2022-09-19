class HunterPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class HunterPants_Autumn extends HunterPants_ColorBase {};
class HunterPants_Brown extends HunterPants_ColorBase {};
class HunterPants_Spring extends HunterPants_ColorBase {};
class HunterPants_Summer extends HunterPants_ColorBase {};
class HunterPants_Winter extends HunterPants_ColorBase {};