class Honey: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmall);
	}
};

class Zagorky_ColorBase: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmall);
	}
};

class Snack_ColorBase: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmall);
	}
};
