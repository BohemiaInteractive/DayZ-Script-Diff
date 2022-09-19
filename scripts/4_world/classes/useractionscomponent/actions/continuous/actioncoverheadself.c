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
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
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
		if ( IsWearingHeadgear(player) ) 
			return false;

		return true;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		//setaperture will be called from here, or from statemachine
		//GetGame().GetWorld().SetAperture(10000);
		ItemBase new_item = ItemBase.Cast(action_data.m_Player.GetInventory().CreateInInventory("BurlapSackCover"));
		if( new_item )
		{
			MiscGameplayFunctions.TransferItemProperties(action_data.m_MainItem,new_item,true,false,true);
			action_data.m_MainItem.Delete();
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}


	bool IsWearingHeadgear( PlayerBase player )
	{
		if ( player.GetInventory().FindAttachment(InventorySlots.HEADGEAR) )
		{
			return true;
		}
		return false;		
	}
};