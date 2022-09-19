class ActionFillBrakesCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.05;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillBrakes( UAQuantityConsumed.POUR_LIQUID, TIME_TO_REPEAT );
	}
};

class ActionFillBrakes: ActionContinuousBase
{
	void ActionFillBrakes()
	{
		m_CallbackClass = ActionFillBrakesCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_LockTargetOnUse = false;
		m_Text = "#refill_car";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !target || !IsTransport(target) )
			return false;

		if( item.GetQuantity() <= 0 )
			return false;

		Car car = Car.Cast( target.GetObject() );
		if( !car )
			return false;
		
		if( car.GetFluidFraction( CarFluid.BRAKE ) >= 0.98 )
			return false;

		float distance = Math.AbsFloat(vector.Distance(car.GetPosition(), player.GetPosition()));

		CarScript carS = CarScript.Cast(car);
		if( distance <= carS.GetActionDistanceFuel() )
		{
			array<string> selections = new array<string>;
			target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				if ( selections[s] == carS.GetActionCompNameBrakes() )
				{
					return true;
				}
			}
		}
		return false;
	}

	override void OnEndAnimationLoopServer( ActionData action_data )
	{	
		if ( action_data.m_MainItem && action_data.m_MainItem.GetQuantity() <= 0.01 )
		{
			action_data.m_MainItem.SetQuantity(0);
		}
	}
};