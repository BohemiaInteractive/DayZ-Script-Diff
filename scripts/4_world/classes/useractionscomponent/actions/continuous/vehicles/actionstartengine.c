class ActionStartCarCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.START_ENGINE);
	}
}

class ActionStartEngine : ActionContinuousBase
{	
	void ActionStartEngine()
	{
		m_CallbackClass 	= ActionStartCarCB;
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
		
		auto vehicle = CarScript.Cast(vehCommand.GetTransport());
		if (!vehicle || vehicle.EngineIsOn())
			return false;
		
		return vehicle.CrewDriver() == player;
	}
	
	override void OnFinishProgress(ActionData action_data)
	{
		super.OnFinishProgress(action_data);

		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCommand)
			return;
		
		auto vehicle = CarScript.Cast(vehCommand.GetTransport());
		if (!vehicle)
			return;

		if (vehicle.GetNetworkMoveStrategy() == NetworkMoveStrategy.PHYSICS)
		{
			// Only perform on clients (or robos), validation is performed in C++ with respect to scripted 'Car.OnBeforeEngineStart'
			if (action_data.m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
			{
				return;
			}
		}
		else
		{
			if (g_Game.IsClient())
				return;
		}

		vehicle.EngineStart();
	}
	
	override void OnExecute(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCommand)
			return;
		
		auto vehicle = CarScript.Cast(vehCommand.GetTransport());
		if (vehicle)
			vehicle.OnIgnition();
	}
		
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool UseMainItem()
	{
		return false;
	}

	//! DEPRECATED
	private const int SOUND_IGNITION_DELAY = 700; // ms, delay for ignition sound to match animation of hand touching the key
	private const float ROUGH_SPECIALTY_WEIGHT	= 0.5;
	static const float MINIMUM_BATTERY_ENERGY	= 5.0;
	bool m_BatteryCon	= false;
	bool m_SparkCon		= false;
	bool m_BeltCon		= false;
	bool m_FuelCon		= false;
}
