class ActionBurnSewSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SEW_WOUNDS);
	}
};

//!DEPRECATED
class ActionBurnSewSelf: ActionContinuousBase
{
	void ActionBurnSewSelf()
	{
		m_CallbackClass = ActionBurnSewSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
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

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( item.GetTemperature() > 80 ) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		const float ITEM_DAMAGE = 5;
		const float SHOCK_AMOUNT = 1000;
		
		if (action_data.m_Player.GetBleedingManagerServer() )
		{
			action_data.m_Player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(action_data.m_MainItem);	
		}		
		//OlD_SHOCK//action_data.m_Player.GetStatShock().Add( action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( SHOCK_AMOUNT, this.GetSpecialtyWeight() ) );
		action_data.m_MainItem.DecreaseHealth("", "", ITEM_DAMAGE);
	}
};