class ActionFillCoolantCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.075;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillCoolant( UAQuantityConsumed.FUEL, TIME_TO_REPEAT );
	}
};

class ActionFillCoolant: ActionContinuousBase
{
	const string RADIATOR_SELECTION_NAME = "radiator";

	void ActionFillCoolant()
	{
		m_CallbackClass = ActionFillCoolantCB;
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
		m_ConditionTarget = new CCTParent(10);
	}

	override bool IsUsingProxies()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !target )
			return false;

		if( item.GetQuantity() <= 0 )
			return false;

		if( item.GetLiquidType() != LIQUID_WATER && item.GetLiquidType() != LIQUID_RIVERWATER )
			return false;

		Car car = Car.Cast( target.GetParent() );
		if( !car )
			return false;
		
		if( car.GetFluidFraction( CarFluid.COOLANT ) >= 0.95 )
			return false;

		array<string> selections = new array<string>;
		target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

		CarScript carS = CarScript.Cast(car);
		
		if( carS )
		{
			EntityAI radiator = null;
			EntityAI carAI;
			
			if ( CastTo(carAI, car) )
			{
				radiator = carAI.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("CarRadiator"));
				if ( radiator && !radiator.IsRuined() )
				{
					for (int s = 0; s < selections.Count(); s++)
					{
						if ( selections[s] == carS.GetActionCompNameCoolant() )
						{
							float dist = vector.Distance( carS.GetCoolantPtcPosWS(), player.GetPosition() );

							if ( dist < carS.GetActionDistanceCoolant() )
								return true;
						}
					}
				}
			}
		}

		return false;
	}
};