class ActionTestBloodSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.TEST_BLOOD);
	}
}

class ActionTestBloodSelf: ActionContinuousBase
{
	void ActionTestBloodSelf()
	{
		m_CallbackClass	= ActionTestBloodSelfCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_BLOODTEST;
		m_FullBody 		= false;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;

		m_Text = "#test_blood";
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

	override void OnFinishProgressServer(ActionData action_data)
	{	
		PluginLifespan lifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		int bloodType = action_data.m_Player.GetStatBloodType().Get();
		
		lifespan.UpdateBloodType(action_data.m_Player, bloodType);
		lifespan.UpdateBloodTypeVisibility(action_data.m_Player, true);
		
		action_data.m_MainItem.Delete();
	}
}
