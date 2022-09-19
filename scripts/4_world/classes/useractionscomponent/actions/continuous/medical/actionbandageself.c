class ActionBandageSelfCB : ActionContinuousBaseCB
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

class ActionBandageSelf: ActionBandageBase
{	
	void ActionBandageSelf()
	{
		m_CallbackClass = ActionBandageSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BANDAGE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#treat_wound";
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
/*
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return player.IsBleeding();
	}*/
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase target = PlayerBase.Cast(action_data.m_Player);
		if(action_data.m_MainItem && target)
		{
			ApplyBandage( action_data.m_MainItem, target );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
};
