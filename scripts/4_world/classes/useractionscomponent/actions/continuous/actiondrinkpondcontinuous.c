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
	
	//protected ref TTypenameArray m_IgnoredObjectTypes = new TTypenameArray();
	
	void ActionDrinkPondContinuous()
	{
		m_CallbackClass 	= ActionDrinkPondContinuousCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_DRINKPOND;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 				= "#drink";
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
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTLiquid(LIQUID_GROUP_DRINKWATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//! Other conditions are handled in CCTLiquid
		//! If object is a specified water source object type we ignore it as it's liquid source handling is handled by a different action and conditions 
		if (target.GetObject() && target.GetObject().GetWaterSourceObjectType() != EWaterSourceObjectType.NONE)
			return false;
		
		if (item && item.IsHeavyBehaviour()) //! Don't allow drinking with hands with heavy items in hands
			return false;
			
		return player.CanEatAndDrink();
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
			int liquidSource = CCTLiquid.Cast(m_ConditionTarget).GetLiquidType();
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
	
	//! DEPRECATED
	[Obsolete("1.30: No replacement. Method is no longer needed as liquid source is now checked and provided by the CCTLiquid target condition.")]
	protected int GetLiquidSource(ActionTarget target)
	{
		vector hitPosition = target.GetCursorHitPos();
		string surfaceType;
		int liquidSource;
		
		g_Game.SurfaceGetType3D(hitPosition[0], hitPosition[1], hitPosition[2], surfaceType);
		string path = "CfgSurfaces " + surfaceType + " liquidType";
		liquidSource = g_Game.ConfigGetInt(path);
		
		return liquidSource;
	}
}
