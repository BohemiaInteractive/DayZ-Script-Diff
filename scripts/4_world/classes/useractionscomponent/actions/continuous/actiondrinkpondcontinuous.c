class ActionDrinkPondContinuousCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.DRINK_POND);
	}
}

class ActionDrinkPondContinuous : ActionContinuousBase
{
	private const float WATER_DRANK_PER_SEC = 35;
	protected const string ALLOWED_WATER_SURFACES = string.Format("%1|%2", UAWaterType.FRESH, UAWaterType.STILL);
	protected int m_AllowedLiquidMask;
	
	void ActionDrinkPondContinuous()
	{
		m_CallbackClass 	= ActionDrinkPondContinuousCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_DRINKPOND;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 				= "#drink";
		
		m_AllowedLiquidMask = LIQUID_GROUP_DRINKWATER;
		m_AllowedLiquidMask &= ~LIQUID_SNOW;
		m_AllowedLiquidMask &= ~LIQUID_HOTWATER;
	}
	
	override bool IsDrink()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool CanBeUsedInFreelook()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTWaterSurfaceEx(UAMaxDistances.DEFAULT, m_AllowedLiquidMask);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		if (item && item.IsHeavyBehaviour())
			return false;
		
		int liquidType = LIQUID_NONE;
		liquidType = target.GetSurfaceLiquidType();

		return liquidType & m_AllowedLiquidMask && player.CanEatAndDrink();
	}

	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		action_data.m_Player.TryHideItemInHands(true);
	}

	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		Param1<float> nacdata = Param1<float>.Cast(action_data.m_ActionComponent.GetACData());
		if (nacdata)
		{
			EConsumeType consumeType;
			
			CCTWaterSurfaceEx waterCheck = CCTWaterSurfaceEx.Cast(m_ConditionTarget);
			if (!waterCheck)
				return;
			
			int liquidSource = waterCheck.GetLiquidType();
			switch (liquidSource)
			{
				case LIQUID_CLEANWATER:
					consumeType = EConsumeType.ENVIRO_WELL;
					break;
			
				default:
					consumeType = EConsumeType.ENVIRO_POND;
					break;
			}

			PlayerConsumeData consumeData = new PlayerConsumeData();

			consumeData.m_Type 		= consumeType;
			consumeData.m_Amount 	= UAQuantityConsumed.DRINK;
			consumeData.m_Source 	= null;
			consumeData.m_Agents 	= action_data.m_Player.GetBloodyHandsPenaltyAgents();
			consumeData.m_LiquidType = liquidSource;
			
			action_data.m_Player.Consume(consumeData);
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		if (HasTarget())
		{
			ctx.Write(action_data.m_Target.GetCursorHitPos());
			return;
		}
		
		ctx.Write(vector.Zero);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{		
		super.ReadFromContext(ctx, action_recive_data);
		
		if (HasTarget())
		{
			vector cursorPosition;
			if (!ctx.Read(cursorPosition))
				return false;

			action_recive_data.m_Target.SetCursorHitPos(cursorPosition);
		}

		return true;
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
	
	// DEPRECATED
	[Obsolete("CCTWaterSurfaceEx::GetSurfaceLiquidType can be used instead")]
	protected int GetLiquidSource(ActionTarget target)
	{
		vector hitPosition = target.GetCursorHitPos();
		string surfaceType;
		int liquidSource;
		
		g_Game.SurfaceGetType3D(hitPosition[0], hitPosition[1], hitPosition[2], surfaceType);
		string path = "CfgSurfaces " + surfaceType + " liquidType";
		liquidSource = GetGame().ConfigGetInt(path);
		
		return liquidSource;
	}
}
