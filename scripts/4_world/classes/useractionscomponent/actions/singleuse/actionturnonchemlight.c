class ActionTurnOnChemlight: ActionTurnOnWhileInHands
{
	void ActionTurnOnChemlight()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT;
		m_Text = "#activate";
	}
};