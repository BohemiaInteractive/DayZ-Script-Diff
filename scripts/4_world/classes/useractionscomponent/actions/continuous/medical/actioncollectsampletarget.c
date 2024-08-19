class ActionCollectSampleTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COLLECT_SAMPLE);
	}
};

class ActionCollectSampleTarget : ActionContinuousBase
{
	void ActionCollectSampleTarget()
	{
		m_CallbackClass = ActionCollectSampleTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#collect_persons_sample";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override void OnEndServer( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		Param1<float> nacdata;
		Class.CastTo(nacdata,  action_data.m_ActionComponent.GetACData() );
		
		float delta = 0;
		if(nacdata)
			delta = (nacdata.param1 / UATimeSpent.COLLECT_SAMPLE);
		
		if(delta > 0)
		{
			ActionCollectBloodTargetLambda lambda = new ActionCollectBloodTargetLambda(action_data.m_MainItem, "BloodSyringe", action_data.m_Player, m_SpecialtyWeight, ntarget, delta);
			action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);
		}
	}
};

