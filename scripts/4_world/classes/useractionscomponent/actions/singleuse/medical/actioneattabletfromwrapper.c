class ActionEatTabletFromWrapper : ActionConsumeSingle
{
	void ActionEatTabletFromWrapper()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_EAT_TABLET;

		m_Text = "#eat";
	}
}
