class ActionEatPillFromBottle : ActionConsumeSingle
{
	void ActionEatPillFromBottle()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_EAT_PILL;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_EAT_PILL;

		m_Text = "#eat";
	}
}
