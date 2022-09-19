// deprecated
class ActionHandsPartSwitch: ActionSingleUseBase
{
	void ActionHandsPartSwitch()
	{
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone; //'CCINotPresent' to perfom with empty hands only
		m_ConditionTarget = new CCTCursor;
		m_Text = "#switch_to_the_next_part";
	}
		
	override bool IsInstant()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool UseAcknowledgment()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return false;
	}
	
	//-------------------------------------------------
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient( action_data );
		
		SetNextIndex(action_data);
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer( action_data );
		
		if (!GetGame().IsMultiplayer())
			SetNextIndex(action_data);
	}
	
	void SetNextIndex(ActionData action_data)
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		construction_action_data.SetNextIndex();
	}
}