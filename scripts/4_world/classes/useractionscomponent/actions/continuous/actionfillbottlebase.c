class ActionFillBottleBaseCB : ActionContinuousBaseCB
{
	private int m_liquid_type;
	private float m_BaseFillQuantity;
	
	override void CreateActionComponent()
	{
		m_liquid_type = ActionFillBottleBase.Cast(m_ActionData.m_Action).GetLiquidType(m_ActionData.m_Player, m_ActionData.m_Target, m_ActionData.m_MainItem);
		
		if (m_liquid_type == LIQUID_GASOLINE)
		{
			m_BaseFillQuantity = UAQuantityConsumed.FUEL;
		}
		else if (m_liquid_type == LIQUID_SNOW)
		{
			m_BaseFillQuantity = UAQuantityConsumed.FILL_SNOW;
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
	private const string ALLOWED_WATER_SURFACES = string.Format("%1|%2|%3", UAWaterType.FRESH, UAWaterType.STILL, UAWaterType.SNOW);
	protected int m_AllowedLiquidMask;
	
	void ActionFillBottleBase()
	{
		m_CallbackClass		= ActionFillBottleBaseCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEPOND;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text				= "#fill";
		
		m_AllowedLiquidMask = LIQUID_GROUP_DRINKWATER | LIQUID_GASOLINE;
		m_AllowedLiquidMask &= ~LIQUID_SNOW;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone()
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		if (targetObject)
		{
			if (vector.DistanceSq(player.GetPosition(), targetObject.GetPosition()) > UAMaxDistances.DEFAULT * UAMaxDistances.DEFAULT)
				return false;
		}
		else
		{
			CCTWaterSurfaceEx waterCheck = new CCTWaterSurfaceEx(UAMaxDistances.DEFAULT, m_AllowedLiquidMask);
			if (!waterCheck.Can(player, target))
				return false;
		}
		
		int liquidType = GetLiquidType(player, target, item);
		
		if (item.GetQuantity() > item.GetQuantityMin())
			liquidType = Liquid.TranslateLiquidType(liquidType);
		
		return liquidType != LIQUID_NONE && Liquid.CanFillContainer(item,liquidType);
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return action_data.m_MainItem.GetQuantity() < action_data.m_MainItem.GetQuantityMax();
	}

	override protected int GetStanceMask(PlayerBase player)
	{
		vector water_info = HumanCommandSwim.WaterLevelCheck(player, player.GetPosition());
		if (water_info[1] > WATER_DEPTH)
		{
			return DayZPlayerConstants.STANCEMASK_ERECT;
		}
		else
		{
			return DayZPlayerConstants.STANCEMASK_CROUCH;
		}
	}
	
	override protected int GetActionCommandEx(ActionData actionData)
	{
		int commandUID = DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEPOND;
		if (actionData.m_Target.GetObject())
		{
			commandUID = DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEWELL;
		}
		
		return commandUID;
	}
		
	int GetLiquidType(PlayerBase player, ActionTarget target, ItemBase item)
	{
		int liquidType = LIQUID_NONE;
		if ( target.GetObject() )
		{
			liquidType = target.GetObject().GetLiquidSourceType();
		}
		else
		{
			string surfaceType;
			vector hit_pos = target.GetCursorHitPos();
			g_Game.SurfaceGetType3D(hit_pos[0], hit_pos[1], hit_pos[2], surfaceType);
			if (surfaceType == "")
			{
				if ( hit_pos[1] <= g_Game.SurfaceGetSeaLevel() + 0.001 )
				{
					liquidType = LIQUID_SALTWATER;
				}
			}
			else
			{
				liquidType = SurfaceInfo.GetByName(surfaceType).GetLiquidType();
			}
		}
		
		return liquidType & m_AllowedLiquidMask;
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
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
	
	// deprecated
	private const int ALLOWED_LIQUID;
}
