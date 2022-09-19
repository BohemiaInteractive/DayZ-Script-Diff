class ActionEatTetracyclineAntibiotics: ActionConsume
{
	void ActionEatTetracyclineAntibiotics()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EAT;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EAT;
	}
	
	override int IsEat()
	{
		return true;
	}

	override void ApplyModifiers( ActionData action_data )
	{
		//action_data.m_Player.m_ModifiersManager.ActivateModifier(eModifiers.MDF_CHEMICALPOISON);
	}
};