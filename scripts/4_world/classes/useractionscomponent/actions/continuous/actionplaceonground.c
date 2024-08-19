class ActionPlaceOnGroundCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_PLACE);
	}
};

class ActionPlaceOnGround: ActionContinuousBase
{
	void ActionPlaceOnGround()
	{
		m_CallbackClass = ActionPlaceOnGroundCB;
		m_Text = "#place_object";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionTarget = new CCTNone;
		m_ConditionItem = new CCIDummy;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( player && player.IsAlive() && !item.IsDamageDestroyed() ) //&& IsConscious && IsNotCaptured
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		action_data.m_Player.DropItem(action_data.m_MainItem);
	}
};