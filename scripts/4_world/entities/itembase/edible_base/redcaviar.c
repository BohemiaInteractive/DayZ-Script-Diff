class RedCaviar extends Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEat);
	}
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
}
