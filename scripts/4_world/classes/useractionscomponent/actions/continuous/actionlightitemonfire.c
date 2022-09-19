class ActionLightItemOnFireCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.FIREPLACE_IGNITE );
	}
}

class ActionLightItemOnFire: ActionContinuousBase
{
	void ActionLightItemOnFire()
	{
		m_CallbackClass = ActionLightItemOnFireCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#ignite";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINonRuined;
	}

	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
	
	// Removed as a fix for igniting Torch by Matchbox through drag&drop in inventory
	/*override bool CanBePerformedFromInventory()
	{
		return true;
	}*/
	
	// Check if ignite candidate is in cargo of something or not.
	bool IsItemInCargoOfSomething(ItemBase item)
	{
		if ( item.GetInventory() )
		{
			InventoryLocation loc = new InventoryLocation;
			item.GetInventory().GetCurrentInventoryLocation(loc);
			
			if ( loc.GetIdx() > -1 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		FireworksBase fireworks = FireworksBase.Cast( action_data.m_Target.GetObject() );
		
		if (fireworks)
		{
			return true;//something might begin occluding the item after the ignition began, we don't want that to cancel the ignition process, not the prettiest solution, but it works
		}
		else 
		{
			return ActionCondition(action_data.m_Player, action_data.m_Target,action_data.m_MainItem);
		}
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		ItemBase target_item = ItemBase.Cast(target.GetObject());
		
		if (target_item && item)
		{
			// when igniting item on the ground with igniter in hands
			if (!target_item.IsIgnited() && !IsItemInCargoOfSomething(target_item) && item.CanIgniteItem(target_item) && target_item.CanBeIgnitedBy(item))
			{
				// oven stage of standard fireplace
				if (target_item.IsKindOf("Fireplace"))
				{
					if (Fireplace.Cast(target_item).IsOven())
					{
						return true;
					}
					
					if (Fireplace.CanIgniteEntityAsFireplace(target_item))
					{
						return true;
					}

					return false;
				}
				
				return true;
			}
			// when igniting item in hands from something on ground
			else if (!item.IsIgnited() && !IsItemInCargoOfSomething(item) && target_item.CanIgniteItem(item) && item.CanBeIgnitedBy(target_item))
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		ItemBase target_item = ItemBase.Cast( action_data.m_Target.GetObject() );
		ItemBase item = action_data.m_MainItem;
		
		bool is_ignition_successful;
		ItemBase ignited_item;				//item that was lit on fire
		ItemBase fire_source_item;			//item that was the source of fire
		
		if ( item.CanIgniteItem( target_item ) )
		{
			is_ignition_successful = target_item.IsThisIgnitionSuccessful( item );
			ignited_item = target_item;
			fire_source_item = item;
		}
		else if ( item.CanBeIgnitedBy( target_item ) )
		{
			is_ignition_successful = target_item.IsTargetIgnitionSuccessful( item );
			ignited_item = item;
			fire_source_item = target_item;			
		}
		
		if ( is_ignition_successful )
		{
			fire_source_item.OnIgnitedTarget( ignited_item );
			ignited_item.OnIgnitedThis( fire_source_item );			
		}
		else
		{
			fire_source_item.OnIgnitedTargetFailed( ignited_item );
			ignited_item.OnIgnitedThisFailed( fire_source_item );			
		}
	}
	
	//setup
	override bool SetupAction( PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{	
		if ( super.SetupAction( player, target, item, action_data, extra_data ) )
		{
			ItemBase target_item = ItemBase.Cast( target.GetObject() );
			if ( target_item )
			{
				SetIgnitingAnimation( target_item );
			}
			
			return true;
		}
		
		return false;
	}
	
	void SetIgnitingAnimation( ItemBase target_item )
	{
		if ( target_item.HasFlammableMaterial() )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE;
		}
		else
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		}
	}
}