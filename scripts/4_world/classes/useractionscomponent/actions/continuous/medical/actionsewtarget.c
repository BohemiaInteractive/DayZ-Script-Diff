class ActionSewTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.SEW_CUTS);
	}
}

class ActionSewTarget : ActionBandageBase
{
	void ActionSewTarget()
	{
		m_CallbackClass 	= ActionSewTargetCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#sew_targets_cuts";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTMan(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

		PlayerBase otherPlayer = PlayerBase.Cast(target.GetObject());
		return otherPlayer.IsBleeding();
	}

	override void OnFinishProgressServer(ActionData action_data)
	{		
		if (CanReceiveAction(action_data.m_Target))
		{
			PlayerBase player = PlayerBase.Cast(action_data.m_Target.GetObject());
			if (action_data.m_MainItem && player)
				ApplyBandage(action_data.m_MainItem, player);
		}
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
	
	override bool CanTargetBeInVehicle()
	{
		return true;
	}
}
