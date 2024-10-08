class ActionFillFuelCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.5;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillFuel( UAQuantityConsumed.FUEL, TIME_TO_REPEAT );
	}
};

class ActionFillFuel: ActionContinuousBase
{
	const string FUEL_SELECTION_NAME = "refill";
	
	void ActionFillFuel()
	{
		m_CallbackClass = ActionFillFuelCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_LockTargetOnUse = false;
		m_Text = "#refuel";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;//CCINotRuinedAndEmpty?
		m_ConditionTarget = new CCTNone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( !target || !IsTransport(target) )
			return false;

		if ( item.GetQuantity() <= 0 )
			return false;

		if ( item.GetLiquidType() != LIQUID_GASOLINE )
			return false;

		if (target.GetObject().IsDamageDestroyed())
			return false;
		
		Car car = Car.Cast(target.GetObject());
		Boat boat = Boat.Cast(target.GetObject());
		if (car)
		{
			if (car.GetFluidFraction( CarFluid.FUEL ) >= 0.98)
				return false;
		}
		else if (boat)
		{
			if ( boat.GetFluidFraction( BoatFluid.FUEL ) >= 0.98 )
				return false;
		}
		else 
			return false;

		array<string> selections = new array<string>;
		target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

		Transport vehicle = Transport.Cast(target.GetObject());
		
		if ( vehicle )
		{
			for (int s = 0; s < selections.Count(); s++)
			{
				if ( selections[s] == vehicle.GetActionCompNameFuel() )
				{
					float dist = vector.DistanceSq( vehicle.GetRefillPointPosWS(), player.GetPosition() );

					if ( dist < vehicle.GetActionDistanceFuel() * vehicle.GetActionDistanceFuel() )
						return true;
				}
			}
		}

		return false;
	}
};