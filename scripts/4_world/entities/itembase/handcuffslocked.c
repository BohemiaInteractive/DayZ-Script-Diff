class RestrainingToolLocked extends ItemBase
{
	void ~RestrainingToolLocked()
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if ( player && player.IsRestrained() )
		{
			player.SetRestrained(false);
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if ( GetGame().IsServer() )
		{
			if ( oldLoc.IsValid() && oldLoc.GetParent() )
			{
				PlayerBase old_player = PlayerBase.Cast(oldLoc.GetParent().GetHierarchyRootPlayer());
				if (old_player && old_player.IsRestrained())
				{
					old_player.SetRestrained(false);
				}
			}
		}
		
		if ( newLoc.IsValid() )
		{
			if (newLoc.GetParent())
			{
				PlayerBase player = PlayerBase.Cast(newLoc.GetParent().GetHierarchyRootPlayer());
			
				if ( player )
				{
					if ( newLoc.GetType() == InventoryLocationType.HANDS )
					{
						if ( !player.IsRestrained() )
						{
							player.SetRestrained(true);
							player.OnItemInHandsChanged();
						}
						
						if(player.IsControlledPlayer())
							player.OnRestrainStart();
					}
				}
			}
		}
		
		if ( GetGame().IsServer() )
		{
			if ( newLoc.GetType() != InventoryLocationType.HANDS )
			{
				if (oldLoc.GetParent())
				{
					PlayerBase old_p = PlayerBase.Cast(oldLoc.GetParent().GetHierarchyRootPlayer());
					if (old_p)
					{
						MiscGameplayFunctions.TransformRestrainItem(this, null, old_p, old_p);
						return;
					}
				}
				
				Delete();
			}
		}	
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainSelf);
	}
}

class HandcuffsLocked extends RestrainingToolLocked
{
}

class RopeLocked extends RestrainingToolLocked
{
}

class DuctTapeLocked extends RestrainingToolLocked
{
}

class MetalWireLocked extends RestrainingToolLocked
{
}

class BarbedWireLocked extends RestrainingToolLocked
{
}