class ActionShaveTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SHAVE);
	}
};

class ActionShaveTarget: ActionContinuousBase
{
	void ActionShaveTarget()
	{
		m_CallbackClass = ActionShaveTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#shave_target";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		PlayerBase man;
		Class.CastTo(man,  target.GetObject() );
		
		if ( man.CanShave() )
		{		
			return true;
		}

		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase man;
		if ( action_data.m_Target && Class.CastTo(man, action_data.m_Target.GetObject()) )
		{
			man.ShavePlayer();
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
};