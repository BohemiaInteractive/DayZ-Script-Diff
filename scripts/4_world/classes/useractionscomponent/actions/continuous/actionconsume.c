class ActionConsumeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.DEFAULT,UATimeSpent.DEFAULT);
	}
};

class ActionConsume: ActionContinuousBase
{
	void ActionConsume()
	{
		m_CallbackClass = ActionConsumeCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EAT;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EAT;
		m_Text = "#eat";	
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnEndServer( ActionData action_data )
	{	
		ItemBase item = action_data.m_MainItem;
		
		if ( item && item.GetQuantity() <= 0.01 )
		{
			item.SetQuantity(0);
		}
		
		else if (item && GetProgress(action_data) > 0)
		{
			// we don't want to inject an agent into an empty container
			PlayerBase player = action_data.m_Player;
			PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast( GetPlugin(PluginTransmissionAgents) );
			plugin.TransmitAgents(player, item, AGT_UACTION_TO_ITEM);
		}
	}
};