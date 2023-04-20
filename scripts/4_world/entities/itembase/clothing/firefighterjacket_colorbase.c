class FirefighterJacket_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class FirefighterJacket_Beige extends FirefighterJacket_ColorBase {};
class FirefighterJacket_Black extends FirefighterJacket_ColorBase {};