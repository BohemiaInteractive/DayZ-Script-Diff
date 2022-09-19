class Container_Base extends ItemBase
{
	override bool IsContainer()
	{
		return true;
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}	
		if ( parent && ( parent == this || ( this.GetType() == parent.GetType() ) ) )
		{
			return false;
		}
		
		return true;
	}
}

class DeployableContainer_Base extends Container_Base
{
	protected vector m_HalfExtents; // The Y value contains a heightoffset and not the halfextent !!!
	
	void DeployableContainer_Base()
	{
		m_HalfExtents = vector.Zero;
		
		ProcessInvulnerabilityCheck(GetInvulnerabilityTypeString());
	}
	
	override string GetInvulnerabilityTypeString()
	{
		return "disableContainerDamage";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
	}
	
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanLoadAttachment( EntityAI attachment )
	{
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;
		
		return super.CanLoadAttachment(attachment);
	}

	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		return super.CanReceiveItemIntoCargo( item );
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if (!super.CanLoadItemIntoCargo( item ))
			return false;
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		return true;
	}
	
	/*
	override bool CanReceiveItemIntoCargo (EntityAI item)
	{
		super.CanReceiveItemIntoCargo( item );
		
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;
		
		return true;
	}
	*/
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if ( newLevel == GameConstants.STATE_RUINED )
			MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, m_HalfExtents);
	}
}