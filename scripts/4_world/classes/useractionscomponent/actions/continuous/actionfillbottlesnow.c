class ActionFillBottleSnow: ActionFillBottleBase
{
	void ActionFillBottleSnow()
	{
		m_CallbackClass		= ActionFillBottleBaseCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FILLING_CONTAINER_SNOW;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text				= "#fill";
		
		m_AllowedLiquidMask = LIQUID_SNOW;
	}
	
	override protected int GetActionCommandEx(ActionData actionData)
	{
		//skip parents super, can be omitted if parent is split into two actions
		
		int commandOverride = GetCommandOverride(actionData);
		if (commandOverride != -1)
			return commandOverride;
			
		return GetActionCommand(actionData.m_Player);
	}
}