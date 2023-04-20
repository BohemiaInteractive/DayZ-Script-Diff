class FirefightersPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class FirefightersPants_Beige extends FirefightersPants_ColorBase {};
class FirefightersPants_Black extends FirefightersPants_ColorBase {};