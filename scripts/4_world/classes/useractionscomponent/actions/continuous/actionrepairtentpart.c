class RepairTentPartActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairTentPartActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairTentPartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
};

class ActionRepairTentPart: ActionContinuousBase
{
	typename m_LastValidType;
	string m_CurrentDamageZone = "";
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairTentPart()
	{
		m_CallbackClass = ActionRepairTentPartCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#repair";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined; //To change?
		m_ConditionTarget = new CCTCursorParent(UAMaxDistances.DEFAULT);
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
		//m_CurrentDamageZone = "";
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
			if (m_LastValidType != targetObject.Type() || m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "" || m_CurrentDamageZone == "Body")
			{
				string damageZone = "";
				
				for (int s = 0; s < selections.Count(); s++)
				{
					if ( DamageSystem.GetDamageZoneFromComponentName(tent, selections[s], damageZone) ) //NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
					{
						//Print("#" + s + " damageZone: " + damageZone);
						if (tent.GetHealthLevel("" + damageZone) == GameConstants.STATE_RUINED ) 
						{
							m_CurrentDamageZone = damageZone;
							m_LastValidComponentIndex = target.GetComponentIndex();
							break;
						}else
							continue;
					}
				}
				if ( damageZone != "" && m_CurrentDamageZone != "Body" ) //This may seem like a duplicate but is required to work properly
				{
					m_CurrentDamageZone = damageZone;
					m_LastValidComponentIndex = target.GetComponentIndex();
				}
			}
			
			if ( m_CurrentDamageZone != "" && m_CurrentDamageZone != "Body" && tent.GetHealthLevel("" + damageZone) == GameConstants.STATE_RUINED )
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		Object targetParent = action_data.m_Target.GetParent();
		ItemBase usedItem = action_data.m_MainItem;
		
		string damageZone = RepairTentPartActionData.Cast(action_data).m_DamageZone;
		if (!GetGame().IsMultiplayer())
			damageZone = m_CurrentDamageZone;
		
		if ( targetParent && targetParent.IsInherited(TentBase) && damageZone != "" ) 
		{
			TentBase tent = TentBase.Cast( targetParent );
			float m_RepairedLevel = usedItem.GetHealthLevel();
			
			tent.SetAllowDamage(true);
			targetParent.SetHealth01("" + damageZone, "", targetParent.GetHealthLevelValue(m_RepairedLevel));
			tent.ProcessInvulnerabilityCheck(tent.GetInvulnerabilityTypeString());
			
			if (usedItem.GetQuantity() > 1)
			{
				//Not really clean, but will do for now
				int val = usedItem.GetQuantity();
				val--;
				usedItem.SetQuantity(val);
			}
			else
				usedItem.Delete();
		}
	}
	
	override ActionData CreateActionData()
	{
		RepairTentPartActionData action_data = new RepairTentPartActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairTentPartActionData repair_action_data;
		
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
			action_recive_data = new RepairTentPartActionReciveData;
		}
		super.ReadFromContext(ctx, action_recive_data);
		RepairTentPartActionReciveData recive_data_repair = RepairTentPartActionReciveData.Cast(action_recive_data);
		
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
		
		RepairTentPartActionReciveData recive_data_repair = RepairTentPartActionReciveData.Cast(action_recive_data);
		RepairTentPartActionData.Cast(action_data).m_DamageZone = recive_data_repair.m_DamageZoneRecived;
	}
};