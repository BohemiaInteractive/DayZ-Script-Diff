class ActionExtinguishFireplaceByLiquidCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT 				= 0.5;
	private const float WETNESS_GAIN_MULTIPLIER 	= 1.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityExtinguish( UAQuantityConsumed.FIREPLACE_LIQUID, TIME_TO_REPEAT, WETNESS_GAIN_MULTIPLIER );
	}
};

class ActionExtinguishFireplaceByLiquid: ActionContinuousBase
{
	void ActionExtinguishFireplaceByLiquid()
	{
		m_CallbackClass = ActionExtinguishFireplaceByLiquidCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#extinguish";
	}
	
	override void CreateConditionComponents()  
	{
		
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTDummy;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object target_object = target.GetObject();
		if ( target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.CanExtinguishFire() && !item.IsDamageDestroyed() && (item.GetLiquidType() & (GROUP_LIQUID_BLOOD | LIQUID_WATER | LIQUID_RIVERWATER | LIQUID_BEER)) ) 
			{
				return true;
			}		
		}
		
		return false;
	}
	
	override void OnEndServer( ActionData action_data )
	{	
		Object target_object = action_data.m_Target.GetObject();
		FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );

		//reset fire state
		fireplace_target.RefreshFireState();
	}	
}