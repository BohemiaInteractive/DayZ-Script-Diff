class ActionBandageTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float effectivity = m_ActionData.m_MainItem.GetBandagingEffectivity();
		float adjusted_time_spent;
		if (effectivity > 0)
			adjusted_time_spent = UATimeSpent.BANDAGE / effectivity;
		m_ActionData.m_ActionComponent = new CAContinuousTime(adjusted_time_spent);
	}
};

class ActionBandageTarget: ActionBandageBase
{
	void ActionBandageTarget()
	{
		m_CallbackClass = ActionBandageTargetCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BANDAGETARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#treat_persons_wound";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}
/*
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase other_player = PlayerBase.Cast(target.GetObject());
		return other_player.IsBleeding();
	}
	*/
	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		
		if(action_data.m_MainItem && target)
		{
			ApplyBandage( action_data.m_MainItem, target );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		GetGame().GetAnalyticsClient().OnActionBandageTarget();
	}
};
