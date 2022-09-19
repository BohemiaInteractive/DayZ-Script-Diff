class ActionStartCarCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.START_ENGINE);
	}
};

class ActionStartEngine: ActionContinuousBase
{
	private const float ROUGH_SPECIALTY_WEIGHT	= 0.5;
	static const float MINIMUM_BATTERY_ENERGY	= 5.0;	//! DEPRECATED
	
	bool m_BatteryCon	= false; //! DEPRECATED
	bool m_SparkCon		= false; //! DEPRECATED
	bool m_BeltCon		= false; //! DEPRECATED
	bool m_FuelCon		= false; //! DEPRECATED
	
	void ActionStartEngine()
	{
		m_CallbackClass 	= ActionStartCarCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_STARTENGINE;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ALL;
		m_SpecialtyWeight	= ROUGH_SPECIALTY_WEIGHT;
		m_LockTargetOnUse	= false;
		m_Text = "#start_the_car";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionTarget	= new CCTNone;
		m_ConditionItem		= new CCINone;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				Car car;
				if (Class.CastTo(car, trans) && !car.EngineIsOn())
				{
					if (car.GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
					{
						return false;
					}
					
					if (car.CrewMemberIndex(player) == DayZPlayerConstants.VEHICLESEAT_DRIVER)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				CarScript car;
				if (Class.CastTo(car, trans))
				{
					car.EngineStart();
				}	
			}
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				CarScript car;
				if (Class.CastTo(car, trans))
				{
					car.OnBeforeEngineStart();
				}
			}
		}
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
};
