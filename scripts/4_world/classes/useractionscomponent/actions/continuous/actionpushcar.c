class ActionPushCarData : ActionPushObjectData
{
	int m_PushDirection = -1;
	float m_HorizontalDirectionRandom 	= 1.0;
	float m_VerticalDirectionRandom 	= 1.0;
	
	// deprecated
	CarScript m_Car;
}

class CAContinuousRepeatPushCar : CAContinuousRepeatPushObject
{
	override float GetProgressWidgetMultiplier()
	{
		return 2.5;
	}
}

class ActionPushCarCB : ActionPushObjectCB
{	
	protected float PUSH_FORCE_IMPULSE_INCREMENT = 200.0;
	
	private ActionPushCarData m_ActionDataPushCar;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeatPushCar(UATimeSpent.PUSH_CAR);
	}
	
	override protected void ApplyForce(ActionPushObjectData actionData)
	{	
		m_ActionDataPushCar = ActionPushCarData.Cast(m_ActionData);
		CarScript car = CarScript.Cast(actionData.m_Object);
		
		m_ActionDataPushCar.m_HorizontalDirectionRandom = Math.RandomFloat(1.0, 3.5);
		m_ActionDataPushCar.m_VerticalDirectionRandom = Math.RandomFloat(-0.5, 0.5);
		
		float bodyMass = dBodyGetMass(car);
		float invBodyMass = 1.0 / bodyMass;
		float force = bodyMass * PUSH_FORCE_IMPULSE_INCREMENT * car.GetPushForceCoefficientMultiplier();
		
		float easedProgress = Easing.EaseInOutSine(actionData.m_ActionComponent.GetProgress());
		
		vector impulse 	= car.GetDirection() * force * (float)m_ActionDataPushCar.m_PushDirection;
		impulse 		= impulse * invBodyMass;
		impulse[0]	 	= impulse[0] * m_ActionDataPushCar.m_HorizontalDirectionRandom;
		impulse[1] 		= impulse[1] * m_ActionDataPushCar.m_VerticalDirectionRandom;
		impulse 		= impulse * easedProgress;
		
		actionData.m_Player.DepleteStamina(EStaminaModifiers.PUSH_CAR);

		dBodyApplyImpulseAt(car, impulse, car.ModelToWorld(car.GetEnginePos()));	
	}
}

class ActionPushCar : ActionPushObject
{
	void ActionPushCar()
	{
		m_CallbackClass	= ActionPushCarCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_VEHICLE_PUSH;
		m_FullBody 		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT;

		m_Text = "#push_car";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		if (!IsInReach(player, target, UAMaxDistances.SMALL))
			return false;
		
		return true;	
	}
		
	override ActionData CreateActionData()
	{
		ActionPushObjectData data = new ActionPushCarData();
		return data;
	}
		
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		ActionPushCarData data = ActionPushCarData.Cast(action_data);
		Car carObj = Car.Cast(data.m_Object);
		if (carObj)
		{
			carObj.SetBrake(0.0);
			carObj.SetHandbrake(0.0);
			carObj.SetBrakesActivateWithoutDriver(false);
		}
		
		data.m_PushDirection = -1;
		if (GetDirectionDot(data.m_Object, data.m_Player) > 0)
			data.m_PushDirection = 1;
		
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ActionPushObjectData data = ActionPushObjectData.Cast(action_data);
		Car carObj = Car.Cast(data.m_Object);
		if (carObj)
			carObj.SetBrakesActivateWithoutDriver(true);
	}
	
	protected float GetDirectionDot(Object target, EntityAI player)
	{
		return vector.Dot(target.GetDirection(), player.GetDirection());
	}
	
	//! DEPRECATED
	private CarScript GetCar(ActionTarget target)
	{
		return CarScript.Cast(GetPushObject(target));
	}
}

// deprecated
class ActionPushCarDataReceiveData : ActionPushObjectDataReceiveData
{
	CarScript m_Car; 
}
