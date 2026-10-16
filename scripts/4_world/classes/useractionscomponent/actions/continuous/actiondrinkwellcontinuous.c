class ActionDrinkWellContinuousCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.DRINK_WELL);
	}
}

class ActionDrinkWellContinuous : ActionContinuousBase
{	
	void ActionDrinkWellContinuous()
	{
		m_CallbackClass = ActionDrinkWellContinuousCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_DRINKWELL;
		m_FullBody 		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_CROUCH;

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
	
	override bool CanBeUsedInFreelook()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item && item.IsHeavyBehaviour())
			return false;
		
		if (!player.CanEatAndDrink())
			return false;
		
		Object targetObject = target.GetObject();
		if (targetObject)
		{
			TIntArray componentIndices = new TIntArray();
			targetObject.GetActionComponentsForSelectionName(targetObject.GetViewGeometryLevel(), UAMisc.LIQUID_SOURCE_SELECTION, componentIndices);
			if (componentIndices.Count() > 0 && componentIndices.Find(target.GetComponentIndex()) == -1)
				return false;
		}
		
		return targetObject && (targetObject.GetWaterSourceObjectType() == EWaterSourceObjectType.WELL || targetObject.IsWell());
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
			consumeData.m_Type = EConsumeType.ENVIRO_WELL;
			consumeData.m_Amount = UAQuantityConsumed.DRINK;
			consumeData.m_Source = null;
			consumeData.m_Agents = action_data.m_Player.GetBloodyHandsPenaltyAgents();
			consumeData.m_LiquidType = LIQUID_CLEANWATER;
			
			action_data.m_Player.Consume(consumeData);
		}
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
