/*class ActionToggleFishing: ActionSingleUseBase
{
	void ActionToggleFishing()
	{
		m_Sound = "CastingRod";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override string GetText()
	{
		return "TODO";
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
		
		vector pos_cursor = target.GetCursorHitPos();
		float distance = Math.AbsInt(vector.Distance(pos_cursor,player.GetPosition()));
		if ( distance <= 2 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnExecuteServer( ActionData action_data )
	{
		FishingRod_Base nitem = FishingRod_Base.Cast( action_data.m_MainItem );
		if( action_data.m_MainItem.GetAnimationPhase("OpenRod") )
		{
			//nitem.DeactivateFishing();
			action_data.m_MainItem.SetAnimationPhase("CloseRod",1);
			action_data.m_MainItem.SetAnimationPhase("OpenRod",0);
		}
		else
		{
			//nitem.ActivateFishing();
			action_data.m_MainItem.SetAnimationPhase("CloseRod",0);
			action_data.m_MainItem.SetAnimationPhase("OpenRod",1);
		}
	}
};*/