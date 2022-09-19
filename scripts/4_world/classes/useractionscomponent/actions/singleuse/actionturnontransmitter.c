class ActionTurnOnTransmitterCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseTurnOnPlugged();
	}
}

class ActionTurnOnTransmitter : ActionTurnOnWhileInHands
{
	void ActionTurnOnTransmitter()
	{
		m_CallbackClass 	= ActionTurnOnTransmitterCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text 				= "#turn_on";
	}
		
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item.IsTransmitter())
		{
			return item.HasEnergyManager() && !item.GetCompEM().IsWorking();
		}
		
		return false;
	}
		
	override void OnExecuteServer(ActionData action_data)
	{
		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
}
