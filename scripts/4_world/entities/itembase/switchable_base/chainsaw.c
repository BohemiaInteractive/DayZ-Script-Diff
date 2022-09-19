class Chainsaw extends Switchable_Base
{
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}
		if( !GetCompEM().IsSwitchedOn() )
		{
			return true;
		}
		return false;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSawPlanks);
		AddAction(ActionDestroyPart);
	}
}