class ActionVehicleHornBase
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
					return false;
				
				if (trans.NeedElectricitySourceDevice())
				{
					EntityAI battery = trans.GetElectricitySourceDevice();
					return battery && (battery.GetHealthLevel("Health") <= GameConstants.STATE_DAMAGED);
				}
				else
					return true;
			}
		}
		
		return false;
	}
	
	static void SetHornState(VehicleHornActionData pActionData, int pMode)
	{
		if (!pActionData.m_Player)
			return;
		
		if (!pActionData.m_Transport)
			return;
		
		VehicleHornComponent horn = VehicleHornComponent.Cast(pActionData.m_Transport.GetVehicleHornComponent());
		horn.SetMode(pMode);
	}
	
	protected static bool PlayerIsDriver(Transport trans, PlayerBase player)
	{
		return trans.GetSeatAnimationType(trans.CrewMemberIndex(player)) == DayZPlayerConstants.VEHICLESEAT_DRIVER;
	}
}

class VehicleHornActionReceiveData : ActionReciveData
{
	bool 		m_ActionNeedEnd;
	Transport 	m_Transport;
}

class VehicleHornActionData : ActionData
{
	bool 		m_ActionNeedEnd;
	Transport 	m_Transport;
}

class ActionVehicleHornShortCB : ActionSingleUseBaseCB {}

class ActionVehicleHornShort : ActionSingleUseBase
{
	void ActionVehicleHornShort()
	{
		m_CallbackClass	= ActionVehicleHornShortCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_TOOTHORN;
		//m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
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
		return VehicleHornShortActionInput;
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new VehicleHornActionData();
		return action_data;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		return ActionVehicleHornBase.ActionCondition(player);
	}
	
	//! special case - manually ends the action on exec event
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		if (vehicleHornData.m_WasExecuted && vehicleHornData.m_ActionNeedEnd)
		{
			vehicleHornData.m_Callback.SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			vehicleHornData.m_ActionNeedEnd = false;
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);
	
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.OFF);
	}
	
	//! sets the m_ActionNeeded flag on both sides
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		VehicleHornActionData vehicleHornData 	= VehicleHornActionData.Cast(action_data);
		vehicleHornData.m_ActionNeedEnd  		= true;
		vehicleHornData.m_Transport			 	= action_data.m_Player.GetCommand_Vehicle().GetTransport();
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.SHORT);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.OFF);
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		
		ctx.Write(vehicleHornData.m_ActionNeedEnd);
		ctx.Write(vehicleHornData.m_Transport);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new VehicleHornActionReceiveData();
		
		super.ReadFromContext(ctx, action_recive_data);

		bool needUpdate;		
		if (!ctx.Read(needUpdate))
			return false;
		
		Transport transport;		
		if (!ctx.Read(transport))
			return false;
		
		VehicleHornActionReceiveData vehicleHornReceivedData 	= VehicleHornActionReceiveData.Cast(action_recive_data);
		vehicleHornReceivedData.m_ActionNeedEnd 				= needUpdate;
		vehicleHornReceivedData.m_Transport			 			= transport;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		VehicleHornActionReceiveData vehicleHornReceivedData 	= VehicleHornActionReceiveData.Cast(action_recive_data);
		VehicleHornActionData vehicleHornData				= VehicleHornActionData.Cast(action_data);
		vehicleHornData.m_ActionNeedEnd 					= vehicleHornReceivedData.m_ActionNeedEnd;
		vehicleHornData.m_Transport			 				= vehicleHornReceivedData.m_Transport;
	}
}

class ActionVehicleHornLong : ActionContinuousBase
{
	void ActionVehicleHornLong()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_TOOTHORN;
		//m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
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
		return VehicleHornLongActionInput;
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
		ActionData action_data = new VehicleHornActionData();
		return action_data;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		return ActionVehicleHornBase.ActionCondition(player);
	}
	
	//! special case - manually ends the action on exec event
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		if (vehicleHornData.m_ReciveEndInput && vehicleHornData.m_ActionNeedEnd)
		{
			vehicleHornData.m_Callback.SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			vehicleHornData.m_ActionNeedEnd = false;
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);

		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.OFF);
		
		ActionContinuousBaseCB callback;
		if (Class.CastTo(callback, action_data.m_Callback))
		{
			vehicleHornData.m_State = UA_FINISHED;
			callback.EndActionComponent();
		}
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		VehicleHornActionData vehicleHornData 	= VehicleHornActionData.Cast(action_data);
		vehicleHornData.m_ActionNeedEnd  		= true;
		vehicleHornData.m_Transport			 	= action_data.m_Player.GetCommand_Vehicle().GetTransport();
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		vehicleHornData.m_ActionNeedEnd = true;
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.LONG);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		ActionVehicleHornBase.SetHornState(vehicleHornData, VehicleHornMode.OFF);
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		VehicleHornActionData vehicleHornData = VehicleHornActionData.Cast(action_data);
		
		ctx.Write(vehicleHornData.m_ActionNeedEnd);
		ctx.Write(vehicleHornData.m_Transport);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
		{
			action_recive_data = new VehicleHornActionReceiveData();
		}
		
		super.ReadFromContext(ctx, action_recive_data);

		bool needUpdate;		
		if (!ctx.Read(needUpdate))
			return false;
		
		Transport transport;		
		if (!ctx.Read(transport))
			return false;
		
		VehicleHornActionReceiveData vehicleHornReceivedData 	= VehicleHornActionReceiveData.Cast(action_recive_data);
		vehicleHornReceivedData.m_ActionNeedEnd 				= needUpdate;
		vehicleHornReceivedData.m_Transport			 			= transport;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		VehicleHornActionReceiveData vehicleHornReceivedData 	= VehicleHornActionReceiveData.Cast(action_recive_data);
		VehicleHornActionData vehicleHornData					= VehicleHornActionData.Cast(action_data);
		vehicleHornData.m_ActionNeedEnd 						= vehicleHornReceivedData.m_ActionNeedEnd;
		vehicleHornData.m_Transport			 					= vehicleHornReceivedData.m_Transport;
	}
}
