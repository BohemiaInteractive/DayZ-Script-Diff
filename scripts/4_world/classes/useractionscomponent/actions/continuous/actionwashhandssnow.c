class ActionWashHandsSnowCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
}

class ActionWashHandsSnow : ActionWashHandsWater
{
	void ActionWashHandsSnow()
	{
		m_CallbackClass	= ActionWashHandsSnowCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_FullBody		= true;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 			= "#wash_hands";
	}
	
	override void CreateConditionComponents()
	{		
		m_ConditionItem		= new CCINone();
		m_ConditionTarget 	= new CCTWaterSurfaceEx(UAMaxDistances.DEFAULT, LIQUID_SNOW);
	}
}