class ActionUngagSelf: ActionContinuousBase
{
	void ActionUngagSelf()
	{
		m_CallbackClass = ActionUncoverHeadSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_SELF;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#ungag";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( IsWearingGag(player) && null == player.GetHumanInventory().GetEntityInHands()) 
			return true;

		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{		
		EntityAI attachment;
		Class.CastTo(attachment, action_data.m_Player.GetInventory().FindAttachment(InventorySlots.MASK));
		if ( attachment && attachment.GetType() == "MouthRag" )
		{
			UngagSelfLambda lamb = new UngagSelfLambda(attachment, "Rag", action_data.m_Player);
			lamb.SetTransferParams(true, true, true, false, 1);
			action_data.m_Player.ServerReplaceItemElsewhereWithNewInHands(lamb);
		}
		//action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	bool IsWearingGag( PlayerBase player )
	{
		EntityAI attachment;
		Class.CastTo(attachment, player.GetInventory().FindAttachment(InventorySlots.MASK));
		if ( attachment && attachment.GetType() == "MouthRag" )
		{
			return true;
		}
		return false;
	}
};

class UngagSelfLambda : TurnItemIntoItemLambda
{
	MouthRag gag;
	
	void UngagSelfLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		gag = MouthRag.Cast(m_OldItem);
		if (gag)
			gag.SetIncomingLambaBool(true);
		InventoryLocation targetHnd = new InventoryLocation;
		targetHnd.SetHands(player, null);
		OverrideNewLocation(targetHnd);
	}
	
	/*override void OnSuccess (EntityAI new_item)
	{
		
	}
	*/
	override void OnAbort ()
	{
		if (gag)
			gag.SetIncomingLambaBool(false);
	}
};