class Candycane_Colorbase extends Edible_Base
{
	void Candycane_Colorbase()
	{
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionEat);
		AddAction(ActionForceFeed);
	}
};
class Candycane_Green extends Candycane_Colorbase {};
class Candycane_Red extends Candycane_Colorbase {};
class Candycane_RedGreen extends Candycane_Colorbase {};
class Candycane_Yellow extends Candycane_Colorbase {};

