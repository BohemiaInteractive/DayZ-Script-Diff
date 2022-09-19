class ActionEmptyCookingPot: ActionEmptyBottleBase
{
	void ActionEmptyCookingPot()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CallbackClass = ActionEmptyBottleBaseCB;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
	}
};