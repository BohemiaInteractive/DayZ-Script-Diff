class ActionBandageSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float effectivity 		= m_ActionData.m_MainItem.GetBandagingEffectivity();
		float adjustedTimeSpent = 0;

		if (effectivity > 0)
			adjustedTimeSpent = UATimeSpent.BANDAGE / effectivity;

		m_ActionData.m_ActionComponent = new CAContinuousRepeat(adjustedTimeSpent);
	}
}

class ActionBandageSelf : ActionBandageBase
{	
	void ActionBandageSelf()
	{
		m_CallbackClass 	= ActionBandageSelfCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_BANDAGE;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#treat_wound";
	}

	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTSelf();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player.IsBleeding();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		PlayerBase target = PlayerBase.Cast(action_data.m_Player);
		if (action_data.m_MainItem && target)
				ApplyBandage(action_data.m_MainItem, target);
	}
}
