class ActionBurnSewTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SEW_WOUNDS);
	}
};

class ActionBurnSewTarget: ActionContinuousBase
{	
	void ActionBurnSewTarget()
	{
		m_CallbackClass = ActionBurnSewTargetCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#treat_persons_wound";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
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
		PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (ntarget.GetBleedingManagerServer() )
		{
			ntarget.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(action_data.m_MainItem);
		}
		//OlD_SHOCK//ntarget.GetStatShock().Add(1000);
		action_data.m_MainItem.DecreaseHealth ( "", "", 5 );

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};