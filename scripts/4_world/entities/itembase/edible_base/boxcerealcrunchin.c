class BoxCerealCrunchin extends Edible_Base
{
	void BoxCerealCrunchin()
	{
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEat);
	}
}

