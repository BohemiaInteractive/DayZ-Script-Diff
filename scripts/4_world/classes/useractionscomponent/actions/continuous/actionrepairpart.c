
/*
TODO:
No pairing with part and tool similar to 'CanUseToolToBuildPart' exists here, since only one type of part is damage-able at this point.
This should be handled by adding "repair_action_type" array to basebuilding and tool configs, this would allow for independent pairing of repair action.
*/

class RepairPartActionReciveData : ActionReciveData
{
	int m_ComponentIndexRecived; //DEPRECATED, target is fully synced anyway
}

class RepairPartActionData : ActionData
{
	int m_ComponentIndex; //DEPRECATED, target is fully synced anyway
}

class ActionRepairPartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = SetCallbackDuration(m_ActionData.m_MainItem);
		m_ActionData.m_ActionComponent = new CAContinuousRepeat( time );
	}
	
	float SetCallbackDuration( ItemBase item )
	{
		/*switch( item.Type() )
		{
			case WoodAxe:
				return UATimeSpent.BASEBUILDING_REPAIR_SLOW;
			default:
				return UATimeSpent.BASEBUILDING_REPAIR_FAST;
		}*/
		return UATimeSpent.BASEBUILDING_REPAIR_FAST;
	}
};

class ActionRepairPart: ActionDismantlePart
{
	void ActionRepairPart()
	{
		m_CallbackClass = ActionRepairPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;	
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_LockTargetOnUse 	= false;
		m_Text = "#repair";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor();
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		m_Text = "#repair " + construction_action_data.GetTargetPart().GetName();
	}
	
	override string GetText()
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		if ( player )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			ConstructionPart constrution_part = construction_action_data.GetTargetPart();
			
			if ( constrution_part )
			{
				return "#repair" + " " + constrution_part.GetName();
			}
		}
		
		return "";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		return RepairCondition( player, target, item, true );
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{	
		return RepairCondition( action_data.m_Player, action_data.m_Target, action_data.m_MainItem , false );
	}	
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		array<string> compNames = new array<string>();
		ent.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), compNames);
		string partName;
		foreach (string testName : compNames)
		{
			if (construction.IsPartConstructed(testName))
			{
				partName = testName;
				break;
			}
		}
		
		if (partName == "")
		{
			ErrorEx("Invalid part name on component idx: " + action_data.m_Target.GetComponentIndex());
			return;
		}
		
		string zoneName;
		PluginRepairing moduleRrepairing = PluginRepairing.Cast(GetPlugin(PluginRepairing));
		DamageSystem.GetDamageZoneFromComponentName(ent, partName, zoneName);
		moduleRrepairing.Repair(action_data.m_Player, action_data.m_MainItem, ent,m_SpecialtyWeight, zoneName);
		
		//consume materials
		construction.TakeMaterialsServer(partName, true);
		
		//add damage to tool
		action_data.m_MainItem.DecreaseHealth(UADamageApplied.REPAIR, false);
	}
	
	protected override void SetBuildingAnimation( ItemBase item )
	{
		switch( item.Type() )
		{
			case Pickaxe:
			case Shovel:
			case FarmingHoe:
			case FieldShovel:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIG;
				break;
			case Pliers:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
				break;				
			default:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
				break;
		}
	}
	
	override ActionData CreateActionData()
	{
		RepairPartActionData action_data = new RepairPartActionData();
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairPartActionData repair_action_data;
		
		if( HasTarget() && Class.CastTo(repair_action_data,action_data) )
		{
			repair_action_data.m_ComponentIndex = action_data.m_Target.GetComponentIndex();
			ctx.Write(repair_action_data.m_ComponentIndex);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new RepairPartActionReciveData();
		}
		RepairPartActionReciveData recive_data_repair = RepairPartActionReciveData.Cast(action_recive_data);
		
		super.ReadFromContext(ctx, action_recive_data);
		
		if( HasTarget() )
		{
			int component_index;
			if ( !ctx.Read(component_index) )
				return false;
			
			recive_data_repair.m_ComponentIndexRecived = component_index;
		}
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		RepairPartActionReciveData recive_data_repair = RepairPartActionReciveData.Cast(action_recive_data);
		RepairPartActionData.Cast(action_data).m_ComponentIndex = recive_data_repair.m_ComponentIndexRecived;
	}
	
	protected bool RepairCondition( PlayerBase player, ActionTarget target, ItemBase item, bool camera_check )
	{
		string zone_name;
		EntityAI ent = EntityAI.Cast(target.GetObject());
		
		if (ent && ent.CanUseConstruction())
		{
			ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
			array<string> compNames = new array<string>();
			ent.GetActionComponentNameList(target.GetComponentIndex(), compNames);
			foreach (string partName : compNames)
			{
				if (construction.IsPartConstructed(partName))
					return construction.CanRepairPart(partName, item, player, camera_check);
			}
		}
		
		return false;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return " repaired " + action_data.m_Target.GetObject().GetDisplayName() + " with " + action_data.m_MainItem.GetDisplayName();
	}
}