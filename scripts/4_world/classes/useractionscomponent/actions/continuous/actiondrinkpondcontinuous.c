class ActionDrinkPondContinuousCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.DRINK_POND);
	}
};

class ActionDrinkPondContinuous: ActionContinuousBase
{
	private const float WATER_DRANK_PER_SEC = 35;
	
	void ActionDrinkPondContinuous()
	{
		m_CallbackClass = ActionDrinkPondContinuousCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DRINKPOND;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#drink";
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
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTWaterSurface(UAMaxDistances.DEFAULT, UAWaterType.FRESH);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		// Other conditions are in CCTWaterSurface
		// Note: using SurfaceIsPond makes you unable to drink under bridges
		//return true /*g_Game.SurfaceIsPond(pos_cursor[0], pos_cursor[2])*/;
		
		if ( item && item.IsHeavyBehaviour() )
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
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		//Print("OnFinishProgressServer");
		Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
		float amount = UAQuantityConsumed.DRINK;
		action_data.m_Player.Consume(NULL, amount, EConsumeType.ENVIRO_POND);
	}

	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		//Print("OnEndAnimationLoopServer");
		if ( action_data.m_Player.HasBloodyHands() && !action_data.m_Player.GetInventory().FindAttachment( InventorySlots.GLOVES ) )
		{
			action_data.m_Player.SetBloodyHandsPenalty();
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		if( HasTarget() )
		{
			ctx.Write(action_data.m_Target.GetCursorHitPos());
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{		
		super.ReadFromContext(ctx, action_recive_data);
		
		if( HasTarget() )
		{
			vector cursor_position;
			if ( !ctx.Read(cursor_position) )
				return false;
			action_recive_data.m_Target.SetCursorHitPos(cursor_position);
		}
		return true;
	}
};