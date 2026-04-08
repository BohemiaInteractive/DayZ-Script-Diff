class ActionRepairBoatEngineCB : ActionRepairVehiclePartCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM);
	}
}

class ActionRepairBoatEngine : ActionRepairVehiclePartBase
{
	void ActionRepairBoatEngine()
	{
		m_CallbackClass 			= ActionRepairBoatEngineCB;	
		m_CommandUID 				= DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR;
		m_StanceMask				= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 					= true;
		m_LockTargetOnUse			= false;
		m_Text 						= "#repair";
	}
		
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Blowtorch bt = Blowtorch.Cast(item);
		if (!bt.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM))
			return false;
		
		BoatScript boat = BoatScript.Cast(target.GetObject());
		if (!boat || boat.EngineIsOn())
			return false;
	
		if (g_Game.IsDedicatedServer())
			return true;
			
		string damageZone = "";
		array<string> selections = new array<string>();
		boat.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);

		foreach (string selection : selections)
		{
			if (boat && DamageSystem.GetDamageZoneFromComponentName(boat, selection, damageZone))
			{
				if (damageZone == "Engine")
				{
					int zoneHP = boat.GetHealthLevel(damageZone);
					if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
					{
						m_CurrentDamageZone = damageZone;	
						return true;
					}
				}
			}
		}
				
		return false;
	}
	
	override void AdjustVehicleHealthServer(ActionData action_data)
	{
		super.AdjustVehicleHealthServer(action_data);
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, UADamageApplied.BUILD);
	}
	
	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		super.OnStartAnimationLoopServer(action_data);

		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}