class ActionExtinguishFireplaceByLiquidCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT 			= 0.5;
	private const float WETNESS_GAIN_MULTIPLIER	= 1.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityExtinguish(UAQuantityConsumed.FIREPLACE_LIQUID, TIME_TO_REPEAT, WETNESS_GAIN_MULTIPLIER);
		CAContinuousQuantityExtinguish.Cast(m_ActionData.m_ActionComponent).MultiplyQuantityUsed(m_ActionData.m_MainItem.GetLiquidThroughputCoef());
	}
}

class ActionExtinguishFireplaceByLiquid : ActionContinuousBase
{
	void ActionExtinguishFireplaceByLiquid()
	{
		m_CallbackClass 	= ActionExtinguishFireplaceByLiquidCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_MEDIUM;
		m_FullBody 			= true;

		m_Text 		= "#extinguish";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINotRuinedAndEmpty();
		m_ConditionTarget 	= new CCTObject(UAMaxDistances.SMALL);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		
		if (targetObject.IsFireplace())
		{
			FireplaceBase fireplace = FireplaceBase.Cast(targetObject);
			
			if (fireplace.CanExtinguishFire() && !item.IsDamageDestroyed() && (item.GetLiquidType() & (GROUP_LIQUID_BLOOD | LIQUID_WATER | LIQUID_BEER)))
			{
				return !item.GetIsFrozen();
			}
		}
		
		return false;
	}
	
	override void OnStartAnimationLoop(ActionData action_data)
	{
		super.OnStartAnimationLoop(action_data);
		
		FireplaceBase fireplace = FireplaceBase.Cast(action_data.m_Target.GetObject());
		fireplace.SetExtinguishingState();
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		FireplaceBase fireplace = FireplaceBase.Cast(action_data.m_Target.GetObject());		
		fireplace.RefreshFireState();
	}
}
