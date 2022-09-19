class ActionUngagTarget: ActionContinuousBase
{
	void ActionUngagTarget()
	{
		m_CallbackClass = ActionUncoverHeadTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_TARGET;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#ungag_person";
	}

	override void CreateConditionComponents()  
	{
	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
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
			if ( IsWearingGag(targetPlayer) && null == player.GetHumanInventory().GetEntityInHands()) 
			{
				return true;
			}
		}
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		
		EntityAI attachment;
		Class.CastTo(attachment, ntarget.GetInventory().FindAttachment(InventorySlots.MASK));
		
		if ( attachment && attachment.GetType() == "MouthRag" )
		{
			UngagSelfLambda lamb = new UngagSelfLambda(attachment, "Rag", action_data.m_Player);
			lamb.SetTransferParams(true, true, true, false, 1);
			action_data.m_Player.ServerReplaceItemElsewhereWithNewInHands(lamb);
			//action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
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