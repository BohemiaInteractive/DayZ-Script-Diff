class ActionCarHornBase
{
	static bool ActionCondition(PlayerBase player)
	{
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();

		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				if (!PlayerIsDriver(trans, player))
				{
					return false;
				}
				
				CarScript car;
				if (Class.CastTo(car, trans))
				{
					return BatteryIsVital(car.GetBattery());
				}
			}
		}
		
		return false;
	}
	
	static void SetCarHornState(CarHornActionData pActionData, int pState)
	{
		if (!pActionData.m_Player)
			return;
		
		if (!pActionData.m_Car)
			return;
		
		pActionData.m_Car.SetCarHornState(pState);
	}
	
	protected static bool PlayerIsDriver(Transport trans, PlayerBase player)
	{
		return trans.GetSeatAnimationType(trans.CrewMemberIndex(player)) == DayZPlayerConstants.VEHICLESEAT_DRIVER;
	}

	protected static bool BatteryIsVital(ItemBase battery)
	{
		return battery && battery.GetHealthLevel("Health") <= GameConstants.STATE_DAMAGED;
	}
}

class CarHornActionReceiveData : ActionReciveData
{
	bool 		m_ActionNeedEnd;
	CarScript 	m_Car;
	
}

class CarHornActionData : ActionData
{
	bool 		m_ActionNeedEnd;
	CarScript 	m_Car;
}

class ActionCarHornShortCB : ActionSingleUseBaseCB {}

class ActionCarHornShort : ActionSingleUseBase
{
	void ActionCarHornShort()
	{
		m_CallbackClass	= ActionCarHornShortCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_TOOTHORN;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ALL;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return false;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}
		
	override typename GetInputType()
	{
		return CarHornShortActionInput;
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new CarHornActionData();
		return action_data;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		return ActionCarHornBase.ActionCondition(player);
	}
	
	//! special case - manually ends the action on exec event
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		if (carHornData.m_WasExecuted && carHornData.m_ActionNeedEnd)
		{
			carHornData.m_Callback.SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			carHornData.m_ActionNeedEnd = false;
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);
	
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.OFF);
	}
	
	//! sets the m_ActionNeeded flag on both sides
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		CarHornActionData carHornData 	= CarHornActionData.Cast(action_data);
		carHornData.m_ActionNeedEnd  	= true;
		carHornData.m_Car			 	= CarScript.Cast(action_data.m_Player.GetCommand_Vehicle().GetTransport());
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.SHORT);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.OFF);
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		
		ctx.Write(carHornData.m_ActionNeedEnd);
		ctx.Write(carHornData.m_Car);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
		{
			action_recive_data = new CarHornActionReceiveData();
		}
		
		super.ReadFromContext(ctx, action_recive_data);

		bool needUpdate;		
		if (!ctx.Read(needUpdate))
			return false;
		
		CarScript carScript;		
		if (!ctx.Read(carScript))
			return false;
		
		CarHornActionReceiveData carHornReceivedData 	= CarHornActionReceiveData.Cast(action_recive_data);
		carHornReceivedData.m_ActionNeedEnd 			= needUpdate;
		carHornReceivedData.m_Car			 			= carScript;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		CarHornActionReceiveData carHornReceivedData 	= CarHornActionReceiveData.Cast(action_recive_data);
		CarHornActionData carHornData					= CarHornActionData.Cast(action_data);
		carHornData.m_ActionNeedEnd 					= carHornReceivedData.m_ActionNeedEnd;
		carHornData.m_Car			 					= carHornReceivedData.m_Car;
	}
}

class ActionCarHornLong : ActionContinuousBase
{
	void ActionCarHornLong()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_TOOTHORN;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ALL;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return false;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}

	override typename GetInputType()
	{
		return CarHornLongActionInput;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new CarHornActionData();
		return action_data;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		return ActionCarHornBase.ActionCondition(player);
	}
	
	//! special case - manually ends the action on exec event
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		if (carHornData.m_ReciveEndInput && carHornData.m_ActionNeedEnd)
		{
			carHornData.m_Callback.SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			carHornData.m_ActionNeedEnd = false;
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);

		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.OFF);
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		CarHornActionData carHornData 	= CarHornActionData.Cast(action_data);
		carHornData.m_ActionNeedEnd  	= true;
		carHornData.m_Car			 	= CarScript.Cast(action_data.m_Player.GetCommand_Vehicle().GetTransport());
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		carHornData.m_ActionNeedEnd = true;
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.LONG);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		ActionCarHornBase.SetCarHornState(carHornData, ECarHornState.OFF);
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		CarHornActionData carHornData = CarHornActionData.Cast(action_data);
		
		ctx.Write(carHornData.m_ActionNeedEnd);
		ctx.Write(carHornData.m_Car);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
		{
			action_recive_data = new CarHornActionReceiveData();
		}
		
		super.ReadFromContext(ctx, action_recive_data);

		bool needUpdate;		
		if (!ctx.Read(needUpdate))
			return false;
		
		CarScript carScript;		
		if (!ctx.Read(carScript))
			return false;
		
		CarHornActionReceiveData carHornReceivedData 	= CarHornActionReceiveData.Cast(action_recive_data);
		carHornReceivedData.m_ActionNeedEnd 			= needUpdate;
		carHornReceivedData.m_Car			 			= carScript;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		CarHornActionReceiveData carHornReceivedData 	= CarHornActionReceiveData.Cast(action_recive_data);
		CarHornActionData carHornData					= CarHornActionData.Cast(action_data);
		carHornData.m_ActionNeedEnd 					= carHornReceivedData.m_ActionNeedEnd;
		carHornData.m_Car			 					= carHornReceivedData.m_Car;
	}
}
