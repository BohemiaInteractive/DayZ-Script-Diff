class ActionSawPlanksCB : ActionContinuousBaseCB
{
	static const float TIME_SAW_HANDSAW = 1.5;
	static const float TIME_SAW_HACKSAW = 3.0;
	static const float TIME_AXES = 1.2;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(GetDefaultTime());
	}
	
	float GetDefaultTime()
	{
		string item_type = m_ActionData.m_MainItem.GetType();
		
		switch(item_type)
		{
			case "Hacksaw": 
				return TIME_SAW_HACKSAW;
			break;
		
			case "HandSaw": 
				return TIME_SAW_HANDSAW;
			break;
		
			default: // axes
				return TIME_AXES;
			break
		}
		Print("ActionSawPlanksCB | Item detection error, assigning negative time");
		return -1;
	}
};

class ActionSawPlanks: ActionContinuousBase
{
	static const int DECREASE_HEALTH_OF_TOOL_DEFAULT = 10; // this constant is not use anymore see ActionConstants.c UADamageApplied
	//static const int DECREASE_HEALTH_OF_TOOL_AXE = 20; // axes
	//static const int DECREASE_FUEL_OF_CHAINSAW = 20; // chainsaw fuel in ml
	
	static const int YIELD = 3;
	ItemBase m_Planks;
	ref InventoryLocation m_PlanksLocation = new InventoryLocation;
	
	void ActionSawPlanks()
	{
		m_CallbackClass = ActionSawPlanksCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#saw_planks";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_O = target.GetObject();
		
		if ( item  &&  target_O.IsInherited(PileOfWoodenPlanks))
		{
			string item_type = item.GetType();
			
			switch(item_type)
			{
				case "Chainsaw":
					if ( item.HasEnergyManager()  &&  item.GetCompEM().CanWork() )
					{
						return true;
					}
					else
					{
						return false;
					}
				break;
			}
			
			return true;
		}
		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PileOfWoodenPlanks item_POWP = PileOfWoodenPlanks.Cast( action_data.m_Target.GetObject() );
		item_POWP.RemovePlanks(YIELD);
		
		vector pos = action_data.m_Player.GetPosition();
		
		InventoryLocation currentLoc = new InventoryLocation;
		if (m_Planks)
			m_Planks.GetInventory().GetCurrentInventoryLocation(currentLoc);
		
		if (!m_Planks || !currentLoc.CompareLocationOnly(m_PlanksLocation))
		{
			m_Planks = ItemBase.Cast( GetGame().CreateObjectEx("WoodenPlank", pos, ECE_PLACE_ON_SURFACE) );
			m_Planks.SetQuantity(YIELD);
			
			m_Planks.GetInventory().GetCurrentInventoryLocation(currentLoc);
			m_PlanksLocation.Copy(currentLoc);
		}
		else if ((m_Planks.GetQuantity() + YIELD) >= m_Planks.GetQuantityMax())
		{
			int remnant = m_Planks.GetQuantity() + YIELD - m_Planks.GetQuantityMax();
			m_Planks.SetQuantity(m_Planks.GetQuantityMax());
			if (remnant > 0)
			{
				m_Planks = ItemBase.Cast( GetGame().CreateObjectEx("WoodenPlank", pos, ECE_PLACE_ON_SURFACE) );
				m_Planks.SetQuantity(remnant);

				m_Planks.GetInventory().GetCurrentInventoryLocation(currentLoc);
				m_PlanksLocation.Copy(currentLoc);
			}
		}
		else
		{
			m_Planks.AddQuantity(YIELD);
		}
		
		ItemBase item = action_data.m_MainItem;
		
		string item_type = item.GetType();
		
		item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( UADamageApplied.SAW_PLANKS, GetSpecialtyWeight() ));
		/*switch(item_type)
		{
			case "WoodAxe": 
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( DECREASE_HEALTH_OF_TOOL_AXE, GetSpecialtyWeight() ));
			break;
		
			case "FirefighterAxe": 
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( DECREASE_HEALTH_OF_TOOL_AXE, GetSpecialtyWeight() ));
			break;
		
			case "FirefighterAxe_Black": 
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( DECREASE_HEALTH_OF_TOOL_AXE, GetSpecialtyWeight() ));
			break;
		
			case "FirefighterAxe_Green": 
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( DECREASE_HEALTH_OF_TOOL_AXE, GetSpecialtyWeight() ));
			break;
		
			case "Hatchet": 
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( DECREASE_HEALTH_OF_TOOL_AXE, GetSpecialtyWeight() ));
			break;
		
			case "Chainsaw":
				if ( item.HasEnergyManager() )
				{
					item.GetCompEM().ConsumeEnergy(DECREASE_FUEL_OF_CHAINSAW);
				}
			break;
		
			default: // Hacksaw and other
				item.DecreaseHealth( "", "", action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( UADamageApplied.SAW_PLANKS, GetSpecialtyWeight() ));
			break;
		}*/
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};