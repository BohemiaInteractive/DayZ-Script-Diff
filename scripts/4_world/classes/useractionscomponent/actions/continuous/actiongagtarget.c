class ActionGagTarget: ActionContinuousBase
{	
	void ActionGagTarget()
	{
		m_CallbackClass = ActionCoverHeadTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_TARGET;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#gag_person";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);		
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item.GetQuantity() > 1)
			return false;
		
		PlayerBase targetPlayer;
		Class.CastTo(targetPlayer, target.GetObject());
		if ( !IsWearingMask(targetPlayer) ) 
		{
			ItemBase headgear = ItemBase.Cast(targetPlayer.FindAttachmentBySlotName( "Headgear" ));
			if ( headgear )
			{
				bool headgear_restricted;
				headgear_restricted = headgear.ConfigGetBool( "noMask" );
				if (headgear_restricted)
				{
					return false;
				}
			}
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase ntarget;
		Class.CastTo(ntarget, action_data.m_Target.GetObject());
		
		if (CanReceiveAction(action_data.m_Target))
		{
			ItemBase m_Gag;
			if (Class.CastTo(m_Gag,ntarget.GetInventory().CreateAttachmentEx("MouthRag",InventorySlots.MASK)))
			{
				m_Gag.SetHealth01("", "", action_data.m_MainItem.GetHealth01("", ""));
				
				action_data.m_MainItem.TransferModifiers(ntarget);
				action_data.m_MainItem.Delete();
			}
		}
	}
	
	bool IsWearingMask( PlayerBase player)
	{
		if ( player.GetInventory().FindAttachment(InventorySlots.MASK) )
		{
			return true;
		}
		return false;
	}
};