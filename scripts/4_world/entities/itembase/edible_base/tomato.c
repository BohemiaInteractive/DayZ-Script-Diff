class Tomato : Edible_Base
{
	override bool CanBeCookedOnStick()
	{
		return true;
	}

	override bool CanBeCooked()
	{
		return true;
	}	
	
	override bool IsFruit()
	{
		return true;
	}

	override bool CanDecay()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatFruit);
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
	}
	
	override void EEOnCECreate()
	{
		int rand = Math.RandomInt(0,10);

		if ( rand > 6 )
		{
			ChangeFoodStage( FoodStageType.ROTTEN );
			SetHealth( "", "", GetMaxHealth()*0.1 );
		}
		else if ( rand > 2 )
		{
			ChangeFoodStage( FoodStageType.DRIED );
			SetHealth( "", "", GetMaxHealth()*0.4 );
		}
	}
}
