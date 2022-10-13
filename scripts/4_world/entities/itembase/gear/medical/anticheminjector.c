class AntiChemInjector: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionInjectTarget);
		AddAction(ActionInjectSelf);
	}
	
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;
		player.GiveShock(100);
		bool in_cont_stage3 = player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3);
		
		if ( in_cont_stage3 )
		{
			player.m_AgentPool.SetAgentCount(eAgents.CHEMICAL_POISON, ContaminationStage2Mdfr.AGENT_THRESHOLD_ACTIVATE);//set the agent count to be the same as at the benigning of stage 2
		}
		else
		{
			player.m_AgentPool.SetAgentCount(eAgents.CHEMICAL_POISON, 0);
		}
		
	}
};
