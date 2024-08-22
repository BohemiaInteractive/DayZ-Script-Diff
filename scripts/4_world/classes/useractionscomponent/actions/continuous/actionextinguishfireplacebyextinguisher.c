class ActionExtinguishFireplaceByExtinguisherCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT			= 0.5;
	private const float WETNESS_GAIN_MULTIPLIER = 0.5;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityExtinguish(UAQuantityConsumed.FIREPLACE_EXTINGUISHER, TIME_TO_REPEAT, WETNESS_GAIN_MULTIPLIER);
		CAContinuousQuantityExtinguish.Cast(m_ActionData.m_ActionComponent).MultiplyQuantityUsed(m_ActionData.m_MainItem.GetLiquidThroughputCoef());
	}
}

class ActionExtinguishFireplaceByExtinguisher : ActionContinuousBase
{
	void ActionExtinguishFireplaceByExtinguisher()
	{
		m_CallbackClass 	= ActionExtinguishFireplaceByExtinguisherCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FIREESTINGUISHER;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_MEDIUM;

		m_Text = "#extinguish";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTDummy();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		Object targetObject = target.GetObject();
		if (targetObject.IsFireplace())
		{
			FireplaceBase fireplace = FireplaceBase.Cast(targetObject);
			
			if (fireplace.CanExtinguishFire() && !item.IsDamageDestroyed())
				return true;
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
