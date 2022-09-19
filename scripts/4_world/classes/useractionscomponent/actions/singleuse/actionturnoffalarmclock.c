class ActionTurnOffAlarmClockCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseTurnOnPlugged;
	}
}

class ActionTurnOffAlarmClock: ActionTurnOffWhileInHands
{
	void ActionTurnOffAlarmClock()
	{
		m_CallbackClass = ActionTurnOffAlarmClockCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_STOP_ALARM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_STOP_ALARM;
		m_Text = "#turn_off";
	}
	
	override bool HasProneException()
	{
		return true;
	}
		
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
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