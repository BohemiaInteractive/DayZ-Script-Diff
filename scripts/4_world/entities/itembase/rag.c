class Rag extends ItemBase
{
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		if (!super.CanSwapEntities(otherItem, otherDestination, destination))
		{
			return false;
		}
		if (Torch.Cast(GetHierarchyParent()) && otherItem.IsInherited(Rag))
		{
			return false;
		}
		return true;
	}
	
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent))
		{
			return false;
		}
		if (GetQuantity() > 1 && PlayerBase.Cast(parent))
		{
			return false;
		}
		return true;
	}
	
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
	
	override bool CanBeCombined( EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		if (!super.CanBeCombined(other_item, reservation_check, stack_max_limit))
		{
			return false;
		}
		return Torch.Cast(other_item.GetHierarchyParent()) == null;//when the other rag is attached to the torch, disallow this action
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{
		Fireplace.IgniteEntityAsFireplace( this, fire_source );
	}

	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		return Fireplace.CanIgniteEntityAsFireplace( this );
	}
	
	override bool CanAssignToQuickbar()
	{
		return (!GetInventory().IsAttachment());
	}
	
	override bool CanBeDisinfected()
	{
		return true;
	}
	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBandageTarget);
		AddAction(ActionBandageSelf);
		AddAction(ActionGagTarget);
		AddAction(ActionGagSelf);
		
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
		
		AddAction(ActionCraftImprovisedHandsCover);
		AddAction(ActionCraftImprovisedHeadCover);
		AddAction(ActionCraftImprovisedFeetCover);
		AddAction(ActionCraftImprovisedFaceCover);
		AddAction(ActionCraftImprovisedTorsoCover);
		AddAction(ActionCraftImprovisedLegsCover);
		AddAction(ActionCraftImprovisedEyePatch);
	}
	
	override float GetBandagingEffectivity()
	{
		return 0.5;
	};
	
	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if(m_Cleanness == 1)
		{
			return 0;
		}
		else
		{
			return 0.15;
		}
	}
	
	override void OnCombine(ItemBase other_item)
	{
		if(m_Cleanness == 1 && other_item.m_Cleanness == 0)
			SetCleanness(0);
	}
	
}