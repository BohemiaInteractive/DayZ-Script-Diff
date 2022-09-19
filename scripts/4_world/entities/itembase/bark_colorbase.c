class Bark_ColorBase extends ItemBase
{
	//================================================================
	// IGNITION ACTION
	//================================================================
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy( EntityAI igniter = NULL )
	{
		if ( GetHierarchyParent() ) return false;
		
		return true;
	}
	
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		return false;
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{
		Fireplace.IgniteEntityAsFireplace( this, fire_source );
	}

	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		return Fireplace.CanIgniteEntityAsFireplace( this );
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
}