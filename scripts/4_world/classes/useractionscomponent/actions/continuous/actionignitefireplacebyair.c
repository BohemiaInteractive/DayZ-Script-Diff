class ActionIgniteFireplaceByAirCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.FIREPLACE_IGNITE );
	}
};

class ActionIgniteFireplaceByAir: ActionContinuousBase
{	
	void ActionIgniteFireplaceByAir()
	{
		m_CallbackClass = ActionIgniteFireplaceByAirCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BLOWFIREPLACE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;	
		m_Text = "#blow_air_into_fireplace";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINotPresent;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
		
	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
		
		if ( target_object && target_object.IsFireplace() )
		{
			if ( fireplace_target.IsBaseFireplace() || fireplace_target.IsFireplaceIndoor() )
			{				
				if ( !fireplace_target.IsBurning() && fireplace_target.GetTemperature() >= fireplace_target.PARAM_MIN_TEMP_TO_REIGNITE )
				{
					return true;
				}
			}
		}
		
		return false;
	}
		
	override void OnFinishProgressServer( ActionData action_data )
	{
		FireplaceBase fireplace_target = FireplaceBase.Cast( action_data.m_Target.GetObject() );

		//start fire
		fireplace_target.StartFire();
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	bool SkipKindlingCheck()
	{
		return false;
	}
}