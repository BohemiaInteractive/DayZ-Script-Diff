class ActionEatSnowContinuousCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.DRINK_POND);
	}
}

class ActionEatSnowContinuous: ActionContinuousBase
{
	void ActionEatSnowContinuous()
	{
		m_CallbackClass 	= ActionEatSnowContinuousCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_EATING_SNOW;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 				= "#eat";
	}
	
	override bool IsDrink()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTWaterSurfaceEx(UAMaxDistances.DEFAULT,LIQUID_SNOW);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		if (item && item.IsHeavyBehaviour())
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
			PlayerConsumeData consumeData = new PlayerConsumeData();
			consumeData.m_Type = EConsumeType.ENVIRO_SNOW;
			consumeData.m_Amount = UAQuantityConsumed.EAT_NORMAL;
			consumeData.m_Source = null;
			consumeData.m_Agents = action_data.m_Player.GetBloodyHandsPenaltyAgents();
			consumeData.m_LiquidType = LIQUID_SNOW;
			
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
}
