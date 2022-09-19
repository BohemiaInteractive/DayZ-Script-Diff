class ActionTurnValveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT);
	}
}

class ActionTurnValve : ActionContinuousBase
{
	void ActionTurnValve()
	{
		m_CallbackClass = ActionTurnValveCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_TURN_VALVE;
		m_FullBody		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 			= "#STR_USRACT_TURN";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		EntityAI targetEntity;
		if (Class.CastTo(targetEntity, target.GetObject()))
		{
			if (targetEntity.HasTurnableValveBehavior())
			{
				int valveIndex = targetEntity.GetTurnableValveIndex(target.GetComponentIndex());
				if (valveIndex != -1)
				{
					if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
					{
						return false;
					}

					return targetEntity.IsValveTurnable(valveIndex);
				}
			}
		}

		return false;
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		action_data.m_Player.TryHideItemInHands(true);
		
	}

	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);

		super.OnEnd(action_data);		
	}
}
