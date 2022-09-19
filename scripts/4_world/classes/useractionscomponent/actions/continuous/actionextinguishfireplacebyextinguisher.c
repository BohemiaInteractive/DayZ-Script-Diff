class ActionExtinguishFireplaceByExtinguisherCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT				= 0.5;
	private const float WETNESS_GAIN_MULTIPLIER 	= 0.5;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityExtinguish( UAQuantityConsumed.FIREPLACE_EXTINGUISHER, TIME_TO_REPEAT, WETNESS_GAIN_MULTIPLIER );
	}
};

class ActionExtinguishFireplaceByExtinguisher: ActionContinuousBase
{
	void ActionExtinguishFireplaceByExtinguisher()
	{
		m_CallbackClass = ActionExtinguishFireplaceByExtinguisherCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FIREESTINGUISHER;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#extinguish";
	}
	
	override void CreateConditionComponents()  
	{
		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTDummy;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object target_object = target.GetObject();
		if ( target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.CanExtinguishFire() && !item.IsDamageDestroyed() ) 
			{
				return true;
			}		
		}
		
		return false;
	}
	
	//TODO Fire extinguisher is not in BETA (no animations, no sounds)
	//If something will change in the future, extinguishing action needs to be updated to reflect the fire extinguisher usage
	/*
	override void OnCancelServer( ActionData action_data )
	{	
		Object target_object = action_data.m_Target.GetObject();
		FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );

		//reset fire state
		fireplace_target.RefreshFireState();
	}
	*/	
}