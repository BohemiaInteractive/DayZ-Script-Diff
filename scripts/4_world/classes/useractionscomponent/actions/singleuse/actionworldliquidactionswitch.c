//can be eventually extended to allow switching for multiple action types?
class ActionWorldLiquidActionSwitch: ActionSingleUseBase
{
	bool 	m_switch_to;
	void ActionWorldLiquidActionSwitch()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	/*override string GetText()
	{
		//return "#switch_to" + " " + m_switch_to;
		if (!m_switch_to)
			return  "#switch_to_liquid_drain";
		return "#switch_to_liquid_pour";
	}*/
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase target_item = ItemBase.Cast(target.GetObject());
		if (Barrel_ColorBase.Cast(target_item) && !target_item.IsOpen())
			return false;
		
		if ( target_item && item && Liquid.CanFillContainer( item, target_item.GetLiquidType(),true ) && Liquid.CanFillContainer( target_item, item.GetLiquidType(), true ) && !GetGame().IsInventoryOpen() ) //TODO find better condition than IsKindOf
		{
			if ( target_item.GetQuantity() > target_item.GetQuantityMin() && item.GetQuantity() < item.GetQuantityMax() && !player.GetLiquidTendencyDrain() && Liquid.CanFillContainer( item, target_item.GetLiquidType() ) )
			{
				if ( Liquid.CanFillContainer( target_item, item.GetLiquidType() ) )
				{
					m_switch_to = 0;//"#liquid_drain";
					return true;
				}
				else
				{
					player.SetLiquidTendencyDrain(true);
					return false;
				}
			}
			if ( item.GetQuantity() > item.GetQuantityMin() && target_item.GetQuantity() < target_item.GetQuantityMax() && player.GetLiquidTendencyDrain() && Liquid.CanFillContainer( target_item, item.GetLiquidType() ) )
			{
				if ( Liquid.CanFillContainer( item, target_item.GetLiquidType() ) )
				{
					m_switch_to = 1;//"#liquid_pour";
					return true;
				}
				else
				{
					player.SetLiquidTendencyDrain(false);
					return false;
				}
			}
		}
		return false;
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		bool state;
		state = action_data.m_Player.GetLiquidTendencyDrain();
		action_data.m_Player.SetLiquidTendencyDrain(!state);
		
		//Print(action_data.m_Player.GetLiquidTendencyDrain());
	}
	
	override bool IsLocal()
	{
		return true;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
};