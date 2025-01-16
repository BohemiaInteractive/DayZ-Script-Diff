class ActionStartCarCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.START_ENGINE);
	}
}

class ActionStartEngine : ActionContinuousBase
{
	private const int SOUND_IGNITION_DELAY = 700; // ms, delay for ignition sound to match animation of hand touching the key
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
					
					return car.CrewMemberIndex(player) == DayZPlayerConstants.VEHICLESEAT_DRIVER);
				}
			}
		}

		return false;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCommand)
			return;
		
		CarScript car = CarScript.Cast(vehCommand.GetTransport());
		if (!car)
			return;
		
		if (car.CheckOperationalState())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(car.SetCarEngineSoundState, SOUND_IGNITION_DELAY, false, CarEngineSoundState.STARTING);
		
	}
		
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCommand)
			return;
				
		Transport trans = vehCommand.GetTransport();
		if (!trans)
			return;
		
		CarScript car;
		if (Class.CastTo(car, trans))
			car.SetCarEngineSoundState(CarEngineSoundState.NONE);
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
	
	override void OnExecuteServer(ActionData action_data)
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
	
	override bool HasTarget()
	{
		return false;
	}
}
