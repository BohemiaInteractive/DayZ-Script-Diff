//!DEPRECATED
class ActionHandcuffTarget: ActionSingleUseBase
{
	void ActionHandcuffTarget()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_HANDCUFFTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#restrain";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{	
		PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());
		action_data.m_MainItem.Delete();
	}
	
	override void OnExecuteClient( ActionData action_data )
	{	
		GetGame().GetAnalyticsClient().OnActionRestrain();
	}
};