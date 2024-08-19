class ActionShaveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SHAVE);
	}
};

class ActionShave: ActionContinuousBase
{
	void ActionShave()
	{
		m_CallbackClass = ActionShaveCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_SHAVE;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#shave_myself";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( player.CanShave() )
		{		
			return true;
		}
			
		return false;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool IsShaveSelf()
	{
		return true;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		action_data.m_Player.ShavePlayer();
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		super.OnFinishProgressClient( action_data );
		
		GetGame().GetAnalyticsClient().OnActionFinishedShaveSelf();
	}
};