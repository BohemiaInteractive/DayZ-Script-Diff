class ActionCoverHeadSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COVER_HEAD);
	}
};

class ActionCoverHeadSelf: ActionContinuousBase
{	
	void ActionCoverHeadSelf()
	{
		m_CallbackClass = ActionCoverHeadSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_SELF;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE;
		//m_Animation = "INJECTEPIPENS";
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#put_on_head";
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
		if (IsWearingHeadgear(player)) 
			return false;

		return true;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		ItemBase new_item;
		if (Class.CastTo(new_item,action_data.m_Player.GetInventory().CreateAttachmentEx("BurlapSackCover",InventorySlots.HEADGEAR)))
		{
			MiscGameplayFunctions.TransferItemProperties(action_data.m_MainItem,new_item,true,false,true);
			action_data.m_MainItem.Delete();
		}
	}


	bool IsWearingHeadgear( PlayerBase player )
	{
		if (player.GetInventory().FindAttachment(InventorySlots.HEADGEAR))
		{
			return true;
		}
		return false;		
	}
};