class ActionMineTreeBark: ActionMineTree
{			
	void ActionMineTreeBark()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CUTBARK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_HarvestType = EHarvestType.BARK;
	}
};