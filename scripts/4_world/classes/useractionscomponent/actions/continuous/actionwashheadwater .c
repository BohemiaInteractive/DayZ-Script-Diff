class ActionWashHeadWaterCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
}

class ActionWashHeadWater : ActionWashHeadWettingClothesBase
{
	void ActionWashHeadWater()
	{
		m_CallbackClass	= ActionWashHeadWaterCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WASHFACEPOND;
		m_FullBody		= true;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 			= "#STR_Wash_Head";
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
