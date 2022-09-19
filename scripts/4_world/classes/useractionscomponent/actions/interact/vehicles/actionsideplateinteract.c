class ActionSidePlateInteract: ActionAnimateCarSelection
{
	void ActionSidePlateInteract()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ALL;
	}
};