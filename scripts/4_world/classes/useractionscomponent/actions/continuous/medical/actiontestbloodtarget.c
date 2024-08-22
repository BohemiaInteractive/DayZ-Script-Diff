class ActionTestBloodTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.TEST_BLOOD);
	}
};

class ActionTestBloodTarget: ActionContinuousBase
{	
	void ActionTestBloodTarget()
	{
		m_CallbackClass = ActionTestBloodTargetCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_BLOODTESTOTHER;
		m_FullBody		= false;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;

		m_Text = "#test_targets_blood";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTMan(UAMaxDistances.DEFAULT);
	}

	override void OnFinishProgressClient(ActionData action_data)
	{
		PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());

		if (ntarget)
		{
			string bloodTypeName, bloodName;
			bool positive;
			bloodTypeName = BloodTypes.GetBloodTypeName(ntarget.GetBloodType(), bloodName, positive);
	
			ntarget.SetLastUAMessage(bloodTypeName);
		}
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());
		PluginLifespan lifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		int bloodType = ntarget.GetStatBloodType().Get();
		
		lifespan.UpdateBloodType(ntarget, bloodType);			
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());
		PluginLifespan lifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		lifespan.UpdateBloodTypeVisibility(ntarget, true);
		
		action_data.m_MainItem.Delete();
	}
}
