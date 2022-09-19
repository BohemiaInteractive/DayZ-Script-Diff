class ActionBiteCharcoalTablets: ActionConsumeSingle
{
	void ActionBiteCharcoalTablets()
	{
		//m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_LICKBATTERY;
	}

	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_Player.m_ModifiersManager.DeactivateModifier(eModifiers.MDF_POISONING);
	}
};