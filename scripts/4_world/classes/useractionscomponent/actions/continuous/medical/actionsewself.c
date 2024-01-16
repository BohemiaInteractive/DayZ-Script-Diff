class ActionSewSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.SEW_CUTS);
	}
}

class ActionSewSelf : ActionBandageBase
{
	void ActionSewSelf()
	{
		m_CallbackClass 	= ActionSewSelfCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_STITCHUPSELF;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#sew_cuts";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTSelf();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

		return player.IsBleeding();
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase player = PlayerBase.Cast(action_data.m_Player);
		if (action_data.m_MainItem && player)
			ApplyBandage(action_data.m_MainItem, player);
	}
	
	override void ApplyBandage(ItemBase item, PlayerBase player)
	{	
		if (player.GetBleedingManagerServer())
			player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(item);	
		
		PluginTransmissionAgents transmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		transmissionAgents.TransmitAgents(item, player, AGT_ITEM_TO_FLESH);
		
		if (item.HasQuantity())
			item.AddQuantity(-20, true);
		else
			item.Delete();
	}
}
