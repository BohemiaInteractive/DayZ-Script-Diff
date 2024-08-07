class ActionCoverHeadTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COVER_HEAD);
	}
};

class ActionCoverHeadTarget: ActionContinuousBase
{	
	void ActionCoverHeadTarget()
	{
		m_CallbackClass = ActionCoverHeadTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_TARGET;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		//m_Animation = "INJECTEPIPENS";
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#put_on_targets_head";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);		
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		PlayerBase targetPlayer;
		Class.CastTo(targetPlayer, target.GetObject());
		if ( !IsWearingHeadgear(targetPlayer) ) 
		{
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase ntarget;
		
		if (Class.CastTo(ntarget, action_data.m_Target.GetObject()) && CanReceiveAction(action_data.m_Target))
		{
			CoverHeadOfTargetPlayerLambda lambda = new CoverHeadOfTargetPlayerLambda(action_data.m_MainItem, "BurlapSackCover", ntarget);
			action_data.m_Player.ServerReplaceItemInHandsWithNewElsewhere(lambda);
		}
	}
	
	bool IsWearingHeadgear( PlayerBase player)
	{
		if ( player.GetInventory().FindAttachment(InventorySlots.HEADGEAR) )
		{
			return true;
		}
		return false;
	}
};

class CoverHeadOfTargetPlayerLambda : TurnItemIntoItemLambda
{
	PlayerBase m_TargetPlayer;

	void CoverHeadOfTargetPlayerLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		m_TargetPlayer = player;
		
		InventoryLocation targetAtt = new InventoryLocation;
		targetAtt.SetAttachment(m_TargetPlayer, NULL, InventorySlots.HEADGEAR);
		OverrideNewLocation(targetAtt);
	}
};