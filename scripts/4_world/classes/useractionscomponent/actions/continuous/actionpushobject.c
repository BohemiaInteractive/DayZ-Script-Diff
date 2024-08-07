class ActionPushObjectDataReceiveData : ActionReciveData
{
	Object m_PushTarget;
}

class ActionPushObjectData : ActionData
{
	bool m_NeedUpdate;
	int m_EndType = 0;
	vector m_Origin = "0 0 0";
	
	Object m_Object;
}

class CAContinuousRepeatPushObject : CAContinuousRepeat
{
	void CAContinuousRepeatPushObject(float time_to_complete_action)
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

class ActionPushObjectCB : ActionContinuousBaseCB
{
	protected float PUSH_FORCE_INCREMENT = 3000.0;
	protected ActionPushObjectData m_ActionDataPushObject;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeatPushObject(10);
	}
	
	override void ProgressActionComponent()
	{
		super.ProgressActionComponent();
		
		ActionPushObjectData data = ActionPushObjectData.Cast(m_ActionData);
		ApplyForce(data);
	}
	
	override void EndActionComponent()
	{
		m_ActionDataPushObject = ActionPushObjectData.Cast(m_ActionData);
		
		if (!m_ActionDataPushObject)
		{
			super.EndActionComponent();
			return;
		}
		
		if (m_ActionDataPushObject.m_State == UA_FINISHED)
		{
			if (m_ActionDataPushObject.m_EndType == 1)
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			else if (m_ActionDataPushObject.m_EndType == 0)
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		else if (m_ActionDataPushObject.m_State == UA_CANCEL)
		{
			ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionDataPushObject.m_Action);
			if (action.HasAlternativeInterrupt())
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			else
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);

			return;
		}

		m_ActionDataPushObject.m_State = UA_FINISHED;
	}
	
	protected void ApplyForce(ActionPushObjectData actionData)
	{	
		Object entity = actionData.m_Object;
				
		float bodyMass = dBodyGetMass(entity);
		float invBodyMass = 1.0 / bodyMass;
		float force = bodyMass * PUSH_FORCE_INCREMENT;
		float easedProgress = Easing.EaseInOutSine(actionData.m_ActionComponent.GetProgress());
		
		vector impulse 	= force * actionData.m_Player.GetDirection() * invBodyMass * easedProgress;
		
		actionData.m_Player.DepleteStamina(EStaminaModifiers.PUSH_CAR);

		dBodyApplyImpulseAt(entity, impulse, entity.ModelToWorld(vector.Zero));	
	}
}

class ActionPushObject : ActionContinuousBase
{

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
		ActionPushObjectData data = new ActionPushObjectData();
		return data;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT|DayZPlayerConstants.STANCEMASK_CROUCH))
			return false;
		
		if (!player.GetSurfaceType())
			return false;

		if (!player.CanConsumeStamina(EStaminaConsumers.PUSH))
			return false;
		
		return true;
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		ActionPushObjectData data = ActionPushObjectData.Cast(action_data);		
		if (data.m_NeedUpdate)
		{
			if (!data.m_Player.CanConsumeStamina(EStaminaConsumers.PUSH))
			{
				data.m_State 		= UA_CANCEL;
				data.m_NeedUpdate 	= false;

				return;
			}
	
			if (vector.Distance(data.m_Object.GetPosition(), data.m_Origin) > 0.5)
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
		
		ActionPushObjectData data 	= ActionPushObjectData.Cast(action_data);
		data.m_Object 			= GetPushObject(action_data.m_Target);
		data.m_Origin 			= data.m_Object.GetPosition();
		data.m_NeedUpdate 		= true;
	}
			
	override void OnEndInput(ActionData action_data)
	{
		super.OnEndInput(action_data);
		
		ActionPushObjectData data 	= ActionPushObjectData.Cast(action_data);
		data.m_State 			= UA_CANCEL;
		data.m_EndType 			= 1;
		data.m_NeedUpdate 		= false;
	}
	
	protected Object GetPushObject(ActionTarget target)
	{
		if (target.IsProxy())
			return target.GetParent();
		
		return target.GetObject();
	}
}