class ActionResetKitchenTimerClockCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseTurnOnPlugged;
	}
}

class ActionResetKitchenTimer: ActionTurnOffWhileInHands
{
	void ActionResetKitchenTimer()
	{
		m_CallbackClass = ActionResetKitchenTimerClockCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_RESET_KITCHENTIMER;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_RESET_KITCHENTIMER;
		m_Text = "#turn_off";
	}
	
	override bool HasProneException()
	{
		return true;
	}
		
	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ClockBase alarm = ClockBase.Cast(item);
		return (alarm.IsRinging() || alarm.IsAlarmOn());
	}
		
	override void OnExecuteServer( ActionData action_data )
	{
		ClockBase alarm = ClockBase.Cast(action_data.m_MainItem);
		if (alarm)
			alarm.TurnOff();
	}
}
