class FlagTendencyActionReciveData : ActionReciveData
{
	bool m_TendencyRecived;
}

class FlagTendencyActionData : ActionData
{
	bool m_Tendency;
};

class ActionWorldFlagActionSwitch: ActionSingleUseBase
{
	bool 	m_switch_to;
	void ActionWorldFlagActionSwitch()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	/*override string GetText()
	{
		if (!m_switch_to)
			return  "#switch_to_flag_raise";
		return "#switch_to_flag_lower";
	}*/
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		TerritoryFlag totem = TerritoryFlag.Cast( target.GetObject() );
		if (!totem)
			return false;
		
		float state = totem.GetAnimationPhase("flag_mast");
		
		if ( totem && totem.FindAttachmentBySlotName("Material_FPole_Flag") )
		{
			if ( player.GetFlagTendencyRaise() && state < 1 )
			{
				m_switch_to = true;
				return true;
			}
			else if ( !player.GetFlagTendencyRaise() && state > 0 )
			{
				m_switch_to = false;
				return true;
			}
		}
		return false;
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		
		bool state = action_data.m_Player.GetFlagTendencyRaise();
		action_data.m_Player.SetFlagTendencyRaise(!state);
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	void SetFlagTendencyOnCurrentActionInvalid(bool state)
	{
		
	}
};