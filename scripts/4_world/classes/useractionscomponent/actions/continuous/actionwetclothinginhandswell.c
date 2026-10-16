class ActionWetClothingInHandsWellCB : ActionWetClothingInHandsBaseCB {};

class ActionWetClothingInHandsWell : ActionWetClothingInHandsBase
{
	void ActionWetClothingInHandsWell()
	{
		m_CallbackClass = ActionWetClothingInHandsWellCB;
		m_StanceMask    = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WETCLOTH;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem   = new CCINonRuined;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!item || !item.IsClothing())
			return false;

		if (item.GetWet() >= item.GetWetMax())
			return false;

		Object targetObject = target.GetObject();
		if (targetObject)
		{
			TIntArray componentIndices = new TIntArray();
			targetObject.GetActionComponentsForSelectionName(targetObject.GetViewGeometryLevel(), UAMisc.LIQUID_SOURCE_SELECTION, componentIndices);

			if (componentIndices.Count() > 0 && componentIndices.Find(target.GetComponentIndex()) == -1)
				return false;
		}

		return true;
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}
}
