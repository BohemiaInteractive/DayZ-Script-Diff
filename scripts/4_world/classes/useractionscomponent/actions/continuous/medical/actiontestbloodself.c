class ActionTestBloodSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.TEST_BLOOD);
	}
};

class ActionTestBloodSelf: ActionContinuousBase
{
	void ActionTestBloodSelf()
	{
		m_CallbackClass = ActionTestBloodSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#test_blood";
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

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		int blood_type = action_data.m_Player.GetStatBloodType().Get();
		
		module_lifespan.UpdateBloodType( action_data.m_Player, blood_type );
		module_lifespan.UpdateBloodTypeVisibility( action_data.m_Player, true );
		
		action_data.m_MainItem.Delete();
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};