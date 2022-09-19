class ActionGagSelf: ActionContinuousBase
{	
	void ActionGagSelf()
	{
		m_CallbackClass = ActionCoverHeadSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_SELF;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#gag_self";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item.GetQuantity() > 1)
			return false;
		
		if ( !IsWearingMask(player) ) 
		{
			ItemBase headgear = ItemBase.Cast(player.FindAttachmentBySlotName( "Headgear" ));
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
		action_data.m_Player.GetInventory().CreateInInventory("MouthRag");
		//This is specific to rags, would need something better to scale if we want more gag types
		MouthRag m_Gag = MouthRag.Cast(action_data.m_Player.GetItemOnSlot("Mask"));
		if (m_Gag)
		{
			m_Gag.SetHealth01("", "", action_data.m_MainItem.GetHealth01("", ""));
			action_data.m_Player.CheckForGag();
		}
		
		
		action_data.m_MainItem.TransferModifiers(action_data.m_Player);
		action_data.m_MainItem.Delete();
		//action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
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