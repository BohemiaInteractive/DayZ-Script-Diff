class Sweater_ColorBase extends Top_Base 
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class Sweater_Gray extends Sweater_ColorBase {};
class Sweater_Blue extends Sweater_ColorBase {};
class Sweater_Green extends Sweater_ColorBase {};
class Sweater_Red extends Sweater_ColorBase {};
class Sweater_ChristmasBlue extends Sweater_ColorBase {};
class Sweater_ChristmasGreen extends Sweater_ColorBase {};
class Sweater_ChristmasRed extends Sweater_ColorBase {};