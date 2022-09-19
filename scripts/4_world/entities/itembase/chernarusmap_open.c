class ChernarusMap extends ItemMap
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnfoldMap);
	}
	
	override bool GetActionWidgetOverride(out typename name)
	{
		if ( GetMapStateAnimation() ) //is map in the 'opened' state
		{
			name = ActionFoldMap;
			return true;
		}
		return false;
	}
}
class ChernarusMap_Open extends ItemMap {} //unused