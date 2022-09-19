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
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BANDAGETARGET;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#test_targets_blood";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override void OnFinishProgressClient( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );

		if( ntarget )
		{
			string blood_type_name, blood_name;
			bool positive;
			blood_type_name = BloodTypes.GetBloodTypeName( ntarget.GetBloodType(), blood_name, positive );
	
			ntarget.SetLastUAMessage(blood_type_name);
		}
	}

	override void OnStartServer( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		int blood_type = ntarget.GetStatBloodType().Get();
		
		module_lifespan.UpdateBloodType( ntarget, blood_type );			
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		
		module_lifespan.UpdateBloodTypeVisibility( ntarget, true );
		
		action_data.m_MainItem.Delete();
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};