class ActionCollectSampleSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COLLECT_SAMPLE);
	}
};

class ActionCollectSampleSelf: ActionContinuousBase
{
	void ActionCollectSampleSelf()
	{
		m_CallbackClass = ActionCollectSampleSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_COLLECTBLOODSELF;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#collect_sample";
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

	override void OnEndServer( ActionData action_data )
	{
		Param1<float> nacdata;
		Class.CastTo(nacdata,  action_data.m_ActionComponent.GetACData() );
		float delta = 0;
		if(nacdata)
			delta = (nacdata.param1 / UATimeSpent.COLLECT_SAMPLE);
		
		if(delta > 0)
		{
			ActionCollectBloodTargetLambda lambda = new ActionCollectBloodTargetLambda(action_data.m_MainItem, "BloodSyringe", action_data.m_Player, m_SpecialtyWeight, action_data.m_Player, delta);
			action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);
		}
	}
};

