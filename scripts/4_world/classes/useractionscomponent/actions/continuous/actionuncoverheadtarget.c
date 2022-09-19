class ActionUncoverHeadTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COVER_HEAD);
	}
};

class ActionUncoverHeadTarget: ActionUncoverHeadBase
{
	void ActionUncoverHeadTarget()
	{
		m_CallbackClass = ActionUncoverHeadTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#uncover_players_head";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT,false);
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		PlayerBase targetPlayer;
		if( Class.CastTo(targetPlayer, target.GetObject()) )
		{
			if ( IsWearingBurlap(targetPlayer) )
			{
				return true;
			}
		}
		return false;
	}

	
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		UncoverHead(PlayerBase.Cast( action_data.m_Target.GetObject()),action_data.m_Player);
	}
	
	
	bool IsWearingBurlap( PlayerBase player )
	{
		EntityAI attachment;
		Class.CastTo(attachment, player.GetInventory().FindAttachment(InventorySlots.HEADGEAR));
		if ( attachment && attachment.GetType() == "BurlapSackCover" )
		{
			return true;
		}
		return false;
	}
};