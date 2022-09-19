class GorkaPants_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class GorkaPants_Summer extends GorkaPants_ColorBase {};
class GorkaPants_Autumn extends GorkaPants_ColorBase {};
class GorkaPants_Flat extends GorkaPants_ColorBase {};
class GorkaPants_PautRev extends GorkaPants_ColorBase {};