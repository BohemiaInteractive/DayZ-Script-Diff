class ObtainLiquidActionReciveData : ActionReciveData
{
	int m_LiquidType = LIQUID_NONE;
}

class ObtainLiquidActionData : ActionData
{
	int m_LiquidType = LIQUID_NONE;
};

class ActionObtainLiquidBaseCB : ActionContinuousBaseCB
{
	private int m_LiquidType;
	private float m_BaseFillQuantity;
	
	override void CreateActionComponent()
	{
		m_LiquidType = ObtainLiquidActionData.Cast(m_ActionData).m_LiquidType;
		
		if (m_LiquidType == LIQUID_GASOLINE)
		{
			m_BaseFillQuantity = UAQuantityConsumed.FUEL;
		}
		else if (m_LiquidType == LIQUID_SNOW)
		{
			m_BaseFillQuantity = UAQuantityConsumed.FILL_SNOW;
		}
		else
		{
			m_BaseFillQuantity = UAQuantityConsumed.FILL_LIQUID;
		}
		m_ActionData.m_ActionComponent = new CAContinuousFill(m_BaseFillQuantity, m_LiquidType);
	}
};

class ActionObtainLiquidBase: ActionContinuousBase
{
	private const float WATER_DEPTH = 0.5;
	protected int m_AllowedLiquidMask;
	
	void ActionObtainLiquidBase()
	{
		m_CallbackClass		= ActionObtainLiquidBaseCB;
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
		m_ConditionTarget 	= new CCTLiquid(m_AllowedLiquidMask, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}
	
	override ActionData CreateActionData()
	{
		ObtainLiquidActionData action_data = new ObtainLiquidActionData();
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		ObtainLiquidActionData actionDataOL;
		
		if (Class.CastTo(actionDataOL, action_data))
			ctx.Write(actionDataOL.m_LiquidType);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new ObtainLiquidActionReciveData();
		
		super.ReadFromContext(ctx, action_recive_data);
		ObtainLiquidActionReciveData receiveDataOL = ObtainLiquidActionReciveData.Cast(action_recive_data);
		
		int liquidType;
		if (!ctx.Read(liquidType))
			return false;
		
		receiveDataOL.m_LiquidType = liquidType;
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		ObtainLiquidActionReciveData receiveDataOL = ObtainLiquidActionReciveData.Cast(action_recive_data);
		ObtainLiquidActionData.Cast(action_data).m_LiquidType = receiveDataOL.m_LiquidType;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			#ifndef SERVER
			ObtainLiquidActionData actionDataOL = ObtainLiquidActionData.Cast(action_data);
			actionDataOL.m_LiquidType = CCTLiquid.Cast(m_ConditionTarget).GetLiquidType();
			#endif
			
			return true;
		}
		
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//! Other conditions are handled in CCTLiquid
		int liquidType = CCTLiquid.Cast(m_ConditionTarget).GetLiquidType();
		if (liquidType == LIQUID_NONE)
			return false;
		
		if (item.GetQuantity() > item.GetQuantityMin())
			liquidType = Liquid.TranslateLiquidType(liquidType);
		
		return Liquid.CanFillContainer(item, liquidType);
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return action_data.m_MainItem.GetQuantity() < action_data.m_MainItem.GetQuantityMax();
	}

	override protected int GetStanceMask(PlayerBase player)
	{
		vector waterInfo = HumanCommandSwim.WaterLevelCheck(player, player.GetPosition());
		if (waterInfo[1] > WATER_DEPTH)
		{
			return DayZPlayerConstants.STANCEMASK_ERECT;
		}
		else
		{
			return DayZPlayerConstants.STANCEMASK_CROUCH;
		}
	}
			
	void SetupStance(PlayerBase player)
	{
		// Returns in format (totalWaterDepth, characterDepht, 0)
		vector waterInfo = HumanCommandSwim.WaterLevelCheck(player, player.GetPosition());
		if (waterInfo[1] > WATER_DEPTH)
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
}