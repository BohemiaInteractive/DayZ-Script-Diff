class RepairTentActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairTentActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairTentCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
};

class ActionRepairTent: ActionContinuousBase
{
	typename m_LastValidType; //legacy stuff
	string m_CurrentDamageZone = "";
	int m_LastValidComponentIndex = -1; //legacy stuff
	
	void ActionRepairTent()
	{
		m_CallbackClass = ActionRepairTentCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#repair";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined; //?
		m_ConditionTarget = new CCTCursorParent(UAMaxDistances.SMALL);
	}
	
	override bool IsUsingProxies()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		Object targetParent = target.GetParent();
		if ( !targetParent || !targetParent.IsInherited(TentBase) )
			return false;

		if ( player && targetObject && targetParent )
		{
			array<string> selections = new array<string>;
			PluginRepairing module_repairing;
			Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
			targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections, "view");
			TentBase tent = TentBase.Cast( targetParent );
			
			string damageZone = "";
			
			for (int s = 0; s < selections.Count(); s++)
			{
				if ( DamageSystem.GetDamageZoneFromComponentName(tent, selections[s], damageZone) ) //NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
				{
					//Print("selections[s]: " + selections[s] + " | damageZone: " + damageZone);
					break;
				}
			}
			
			if ( damageZone != "" )
			{
				if (module_repairing.CanRepair(item,tent,damageZone))
				{
					m_CurrentDamageZone = damageZone;
					return true;
				}
			}
		}
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		Object targetParent = action_data.m_Target.GetParent();
		
		string damageZone = RepairTentActionData.Cast(action_data).m_DamageZone;
		if (!GetGame().IsMultiplayer())
			damageZone = m_CurrentDamageZone;
		
		if ( targetParent && targetParent.IsInherited(TentBase) && damageZone != "" ) 
		{
			TentBase tent = TentBase.Cast( targetParent );
			PluginRepairing module_repairing;
			Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
			
			RepairDamageTransfer(action_data.m_Player,action_data.m_MainItem,tent,m_SpecialtyWeight,damageZone);
			module_repairing.Repair(action_data.m_Player,action_data.m_MainItem,tent,m_SpecialtyWeight,damageZone);
		}
	}
	
	override ActionData CreateActionData()
	{
		RepairTentActionData action_data = new RepairTentActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairTentActionData repair_action_data;
		
		if( HasTarget() && Class.CastTo(repair_action_data,action_data) )
		{
			repair_action_data.m_DamageZone = m_CurrentDamageZone;
			ctx.Write(repair_action_data.m_DamageZone);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new RepairTentActionReciveData;
		}
		super.ReadFromContext(ctx, action_recive_data);
		RepairTentActionReciveData recive_data_repair = RepairTentActionReciveData.Cast(action_recive_data);
		
		if( HasTarget() )
		{
			string zone;
			if ( !ctx.Read(zone) )
				return false;
			
			recive_data_repair.m_DamageZoneRecived = zone;
		}
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		RepairTentActionReciveData recive_data_repair = RepairTentActionReciveData.Cast(action_recive_data);
		RepairTentActionData.Cast(action_data).m_DamageZone = recive_data_repair.m_DamageZoneRecived;
	}
	
	void RepairDamageTransfer(PlayerBase player, ItemBase repair_kit, ItemBase item, float specialty_weight, string damage_zone = "") //hack; mirrors current config setup, replace with either native DamageSystem methods, or script-side DamageSystem systemic solution
	{
		float transfer_to_global_coef = 0;
		array<string> transfer_zones = new array<string>;
		string path = "" + CFG_VEHICLESPATH + " " + item.GetType() + " DamageSystem DamageZones " + damage_zone;
		PluginRepairing module_repairing;
		Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
		
		GetGame().ConfigGetTextArray("" + path + " transferToZonesNames", transfer_zones);
		
		for (int i = 0; i < transfer_zones.Count(); i++)
		{
			transfer_to_global_coef += GetGame().ConfigGetFloat("" + path + " Health transferToGlobalCoef");
			if (transfer_zones.Get(i) == damage_zone)
				continue;
			
			module_repairing.Repair(player,repair_kit,item,specialty_weight,transfer_zones.Get(i),false);
		}
		
		//finally, repairs global
		if (transfer_to_global_coef > 0)
		{
			module_repairing.Repair(player,repair_kit,item,specialty_weight,"",false);
		}
	}
};