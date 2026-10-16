class ActionWashHeadWellCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
}

class ActionWashHeadWell : ActionWashHeadWettingClothesBase
{
	void ActionWashHeadWell()
	{
		m_CallbackClass	= ActionWashHeadWellCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WASHFACEPOND;
		m_FullBody		= true;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#STR_Wash_Head";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem		= new CCINone();
		m_ConditionTarget 	= new CCTLiquid(LIQUID_GROUP_WATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
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
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override protected float GetEyeIrritationReduction() { return 50.0; }
}
