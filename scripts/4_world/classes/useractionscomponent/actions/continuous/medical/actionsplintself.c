class ActionSplintSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.APPLY_SPLINT);
	}
};

class ActionSplintSelf: ActionContinuousBase	
{
	void ActionSplintSelf()
	{
		m_CallbackClass = ActionSplintSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#apply_splint";
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
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		action_data.m_MainItem.TransferModifiers(action_data.m_Player);
		action_data.m_Player.ApplySplint();
		
		//Double check to not enter splinted state if legs are not broken
		if (action_data.m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			action_data.m_Player.SetBrokenLegs(eBrokenLegs.BROKEN_LEGS_SPLINT);
	
			ItemBase new_item = ItemBase.Cast(action_data.m_Player.GetInventory().CreateInInventory("Splint_Applied"));
			if ( new_item )
			{
				MiscGameplayFunctions.TransferItemProperties(action_data.m_MainItem,new_item,true,false,true);
				action_data.m_MainItem.Delete();
				action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
			}
		}
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() != eBrokenLegs.BROKEN_LEGS || IsWearingSplint(player))
		{
			return false;
		}
		return super.ActionCondition(player, target, item);
	
	}
	
	bool IsWearingSplint( PlayerBase player )
	{
		if ( player.GetItemOnSlot("Splint_Right") )
		{
			return true;
		}
		return false;		
	}
};