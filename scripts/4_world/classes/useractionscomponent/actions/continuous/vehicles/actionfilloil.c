class ActionFillOilCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.05;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillOil( UAQuantityConsumed.POUR_LIQUID, TIME_TO_REPEAT );
	}
};

class ActionFillOil: ActionContinuousBase
{
	void ActionFillOil()
	{
		m_CallbackClass = ActionFillOilCB;
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
		
		if( car.GetFluidFraction( CarFluid.OIL ) >= 0.98 )
			return false;

		array<string> selections = new array<string>;
		target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

		CarScript carS = CarScript.Cast(car);
		if( carS )
		{
			for (int s = 0; s < selections.Count(); s++)
			{
				if ( selections[s] == carS.GetActionCompNameOil() )
				{
					float dist = vector.Distance( carS.GetRefillPointPosWS(), player.GetPosition() );

					if ( dist < carS.GetActionDistanceOil() )
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