class ActionFillFuelCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.5;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillFuel(UAQuantityConsumed.FUEL, TIME_TO_REPEAT);
	}
}

class ActionFillFuel : ActionContinuousBase
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;
		
		Transport transport = Transport.Cast(target.GetObject());
		if (!transport)
			return false;
		
		if (transport.IsDamageDestroyed())
			return false;

		if (item.GetQuantity() <= 0)
			return false;

		if (item.GetLiquidType() != LIQUID_GASOLINE)
			return false;

		if (transport.GetFluidFractionScript(ETransportFluid.FUEL) >= 0.98)
			return false;

		TStringArray selections = new TStringArray();
		transport.GetActionComponentNameList(target.GetComponentIndex(), selections);
		
		foreach (string selection : selections)
		{
			if (selection == transport.GetActionCompNameFuel())
			{
				float dist = vector.DistanceSq(transport.GetRefillPointPosWS(), player.GetPosition());

				if (dist < transport.GetActionDistanceFuel() * transport.GetActionDistanceFuel())
					return true;
			}
		}

		return false;
	}
}
