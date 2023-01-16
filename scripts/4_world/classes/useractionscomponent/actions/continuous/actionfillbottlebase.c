class ActionFillBottleBaseCB : ActionContinuousBaseCB
{
	private int m_liquid_type;
	private float m_BaseFillQuantity;
	
	override void CreateActionComponent()
	{
		m_liquid_type = ActionFillBottleBase.Cast(m_ActionData.m_Action).GetLiquidType(m_ActionData.m_Player, m_ActionData.m_Target, m_ActionData.m_MainItem);
		
		if (m_ActionData.m_Target.GetObject() && m_ActionData.m_Target.GetObject().IsFuelStation())
		{
			m_BaseFillQuantity = UAQuantityConsumed.FUEL;
		}
		else
		{
			m_BaseFillQuantity = UAQuantityConsumed.FILL_LIQUID;
		}
		m_ActionData.m_ActionComponent = new CAContinuousFill(m_BaseFillQuantity, m_liquid_type);
	}
};

class ActionFillBottleBase: ActionContinuousBase
{
	private const float WATER_DEPTH = 0.5;
	private const string ALLOWED_WATER_SURFACES = string.Format("%1|%2", UAWaterType.FRESH, UAWaterType.STILL);
	
	void ActionFillBottleBase()
	{
		m_CallbackClass		= ActionFillBottleBaseCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEPOND;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text				= "#fill";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone()
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item.IsFullQuantity())
			return false;

		Object targetObject = target.GetObject();
		if (targetObject)
		{
			if (vector.DistanceSq(player.GetPosition(), targetObject.GetPosition()) > UAMaxDistances.DEFAULT * UAMaxDistances.DEFAULT)
				return false;
			
			if (targetObject.IsWell() || targetObject.IsFuelStation())
			{
				return GetLiquidType(player,target,item) != -1;
			}
		}

		CCTWaterSurface waterCheck = new CCTWaterSurface(UAMaxDistances.DEFAULT, ALLOWED_WATER_SURFACES);
		if (waterCheck.Can(player, target))
			return GetLiquidType(player, target, item);

		return false;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return action_data.m_MainItem.GetQuantity() < action_data.m_MainItem.GetQuantityMax();
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{	
		SetupStance(player);
	
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			if (action_data.m_Target.GetObject())
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEWELL;
			
			return true;
		}

		return false;
	}
	
	int GetLiquidType(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target.GetObject() && target.GetObject().IsFuelStation())
		{
			if (Liquid.CanFillContainer(item, LIQUID_GASOLINE))
				return LIQUID_GASOLINE;
		}
		else if ((!target.GetObject() || target.GetObject().IsWell()) && Liquid.CanFillContainer(item, LIQUID_WATER))
		{
			return LIQUID_WATER;
		}
		
		return -1;
	}
	
	void SetupStance(PlayerBase player)
	{
		//returns in format (totalWaterDepth, characterDepht, 0)
		vector water_info = HumanCommandSwim.WaterLevelCheck(player, player.GetPosition());
		if (water_info[1] > WATER_DEPTH)
		{
			m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		}
		else
		{
			m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		}
	}
}
