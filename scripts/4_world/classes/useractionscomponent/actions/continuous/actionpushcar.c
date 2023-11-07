class ActionPushCarDataReceiveData : ActionReciveData
{
	CarScript m_Car;
}

class ActionPushCarData : ActionData
{
	int m_PushDirection = -1;
	int m_EndType 		= 0;

	float m_HorizontalDirectionRandom 	= 1.0;
	float m_VerticalDirectionRandom 	= 1.0;

	vector m_Origin = "0 0 0";

	bool m_NeedUpdate;
	CarScript m_Car;
}

class CAContinuousRepeatPushCar : CAContinuousRepeat
{
	void CAContinuousRepeatPushCar(float time_to_complete_action)
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override int Execute(ActionData action_data)
	{
		if (!action_data.m_Player)
			return UA_ERROR;

		if (m_TimeElpased < m_TimeToComplete)
		{
			m_TimeElpased += action_data.m_Player.GetDeltaT();
			m_TotalTimeElpased += action_data.m_Player.GetDeltaT();
			return UA_PROCESSING;
		}
		else
		{
			m_SpentUnits.param1 = m_TimeElpased;
			SetACData(m_SpentUnits);
			m_TimeElpased = m_TimeToComplete - m_TimeElpased;
			OnCompletePogress(action_data);

			return UA_CANCEL;
		}
		
		return UA_FINISHED;
	}
}

class ActionPushCarCB : ActionContinuousBaseCB
{
	private static float PUSH_FORCE_IMPULSE_INCREMENT = 200.0;
	private ActionPushCarData m_ActionDataPushCar;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeatPushCar(10.0);
	}
	
	override void ProgressActionComponent()
	{
		super.ProgressActionComponent();
		
		ActionPushCarData data = ActionPushCarData.Cast(m_ActionData);
		ApplyForce(data);
	}
	
	override void EndActionComponent()
	{
		m_ActionDataPushCar = ActionPushCarData.Cast(m_ActionData);
		
		if (!m_ActionDataPushCar)
		{
			super.EndActionComponent();
			return;
		}
		
		if (m_ActionDataPushCar.m_State == UA_FINISHED)
		{
			if (m_ActionDataPushCar.m_EndType == 1)
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			else if (m_ActionDataPushCar.m_EndType == 0)
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		else if (m_ActionDataPushCar.m_State == UA_CANCEL)
		{
			ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionDataPushCar.m_Action);
			if (action.HasAlternativeInterrupt())
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			else
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);

			return;
		}

		m_ActionDataPushCar.m_State = UA_FINISHED;
	}
	
	protected void ApplyForce(ActionPushCarData actionData)
	{	
		CarScript car = actionData.m_Car;
		
		actionData.m_HorizontalDirectionRandom = Math.RandomFloat(1.0, 3.5);
		actionData.m_VerticalDirectionRandom = Math.RandomFloat(-0.5, 0.5);
		
		float bodyMass = dBodyGetMass(car);
		float invBodyMass = 1.0 / bodyMass;
		float force = dBodyGetMass(car) * PUSH_FORCE_IMPULSE_INCREMENT * car.GetPushForceCoefficientMultiplier();
		
		float easedProgress = Easing.EaseInOutSine(actionData.m_ActionComponent.GetProgress());
		
		vector impulse 	= car.GetDirection() * force * (float)actionData.m_PushDirection;
		impulse 		= impulse * invBodyMass;
		impulse[0]	 	= impulse[0] * actionData.m_HorizontalDirectionRandom;
		impulse[1] 		= impulse[1] * actionData.m_VerticalDirectionRandom;
		impulse 		= impulse * easedProgress;
		
		actionData.m_Player.DepleteStamina(EStaminaModifiers.PUSH_CAR);

		dBodyApplyImpulseAt(car, impulse, car.ModelToWorld(car.GetEnginePos()));	
	}
}

class ActionPushCar : ActionContinuousBase
{
	void ActionPushCar()
	{
		m_CallbackClass	= ActionPushCarCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_VEHICLE_PUSH;
		m_FullBody 		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT;

		m_Text = "#push_car";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem		= new CCINotPresent();
		m_ConditionTarget	= new CCTCursorNoRuinCheck(UAMaxDistances.DEFAULT);
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override ActionData CreateActionData()
	{
		ActionPushCarData data = new ActionPushCarData();
		return data;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT|DayZPlayerConstants.STANCEMASK_CROUCH))
			return false;
		
		if (!player.GetSurfaceType())
			return false;

		if (!IsInReach(player, target, UAMaxDistances.SMALL))
			return false;
		
		if (!player.CanConsumeStamina(EStaminaConsumers.PUSH))
			return false;

		CarScript car = GetCar(target);
		if (car)
		{
			if (Math.AbsFloat(GetDirectionDot(car, player)) < 0.85)
				return false;

			if (car.EngineIsOn() && car.GetGear() != CarGear.NEUTRAL)
				return false;

			return true;
		}
		
		return false;
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		ActionPushCarData data = ActionPushCarData.Cast(action_data);		
		if (data.m_NeedUpdate)
		{
			if (!data.m_Player.CanConsumeStamina(EStaminaConsumers.PUSH))
			{
				data.m_State 		= UA_CANCEL;
				data.m_NeedUpdate 	= false;

				return;
			}
	
			if (vector.Distance(data.m_Car.GetPosition(), data.m_Origin) > 0.5)
			{
				data.m_State 		= UA_FINISHED;
				data.m_EndType 		= 0;
				data.m_NeedUpdate 	= false;
			}
		}
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		ActionPushCarData data 	= ActionPushCarData.Cast(action_data);
		data.m_Car 				= GetCar(action_data.m_Target);
		data.m_Origin 			= data.m_Car.GetPosition();
		data.m_NeedUpdate 		= true;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		ActionPushCarData data = ActionPushCarData.Cast(action_data);

		if (data.m_Car)
		{
			data.m_Car.SetBrake(0.0);
			data.m_Car.SetHandbrake(0.0);
			data.m_Car.SetBrakesActivateWithoutDriver(false);
		}
		
		data.m_PushDirection = -1;
		if (GetDirectionDot(data.m_Car, data.m_Player) > 0)
			data.m_PushDirection = 1;
		
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ActionPushCarData data = ActionPushCarData.Cast(action_data);
		if (data.m_Car)
		{
			data.m_Car.SetBrakesActivateWithoutDriver(true);
			data.m_Car.SetBrake(1.0);
			data.m_Car.SetHandbrake(1.0);
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);
		
		ActionPushCarData data 	= ActionPushCarData.Cast(action_data);
		data.m_State 			= UA_CANCEL;
		data.m_EndType 			= 1;
		data.m_NeedUpdate 		= false;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		ActionPushCarData data = ActionPushCarData.Cast(action_data);
		
		ctx.Write(data.m_Car);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new ActionPushCarDataReceiveData();
		
		super.ReadFromContext(ctx, action_recive_data);
		
		CarScript car;
		if (!ctx.Read(car))
			return false;
		
		ActionPushCarDataReceiveData receiveData 	= ActionPushCarDataReceiveData.Cast(action_recive_data);
		receiveData.m_Car			 				= car;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		ActionPushCarDataReceiveData receiveData 	= ActionPushCarDataReceiveData.Cast(action_recive_data);
		receiveData.m_Car			 				= receiveData.m_Car;
	}
	
	private float GetDirectionDot(Object target, EntityAI player)
	{
		return vector.Dot(target.GetDirection(), player.GetDirection());
	}
	
	private CarScript GetCar(ActionTarget target)
	{
		if (target.IsProxy())
			return CarScript.Cast(target.GetParent());
		
		return CarScript.Cast(target.GetObject());
	}
}
