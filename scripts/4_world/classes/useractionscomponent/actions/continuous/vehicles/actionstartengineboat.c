class ActionStartEngineBoatCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.START_ENGINE);
	}
}

class ActionStartEngineBoat : ActionContinuousBase
{
	void ActionStartEngineBoat()
	{
		m_CallbackClass 	= ActionStartEngineBoatCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_STARTENGINE;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ALL;
		m_LockTargetOnUse	= false;
		m_Text 				= "#start_the_car";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionTarget	= new CCTNone();
		m_ConditionItem		= new CCINone();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (!vehCommand)
			return false;
		
		BoatScript boat = BoatScript.Cast(vehCommand.GetTransport());
		if (boat && !boat.EngineIsOn())
		{
			if (boat.GetHealthLevel("") >= GameConstants.STATE_RUINED || boat.GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
				return false;
			
			return boat.CrewMemberIndex(player) == DayZPlayerConstants.VEHICLESEAT_DRIVER);
		}
		
		return false;
	}
		
	override void OnFinishProgress(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				Boat boat = Boat.Cast(trans);
				if (boat)
					boat.EngineStart();
			}
		}
	}
	
	override void OnExecute(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				Boat boat = Boat.Cast(trans);
				if (boat)
					boat.OnBeforeEngineStart();
			}
		}
	}
		
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
}