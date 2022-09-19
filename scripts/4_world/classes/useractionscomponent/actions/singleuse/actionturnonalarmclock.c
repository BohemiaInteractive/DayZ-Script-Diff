class ActionTurnOnAlarmClockCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseTurnOnPlugged;
	}
}

class ActionTurnOnAlarmClock: ActionTurnOnWhileInHands
{
	void ActionTurnOnAlarmClock()
	{
		m_CallbackClass = ActionTurnOnAlarmClockCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_STOP_ALARM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_STOP_ALARM;
		m_Text = "#turn_on";
	}
	
	override bool HasProneException()
	{
		return true;
	}
		
	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		AlarmClock_ColorBase alarm = AlarmClock_ColorBase.Cast(item);
		return (!alarm.IsAlarmOn() && !alarm.IsRinging());
	}
		
	/*
	override void OnExecuteClient( ActionData action_data )
	{
		AlarmClock_ColorBase alarm = AlarmClock_ColorBase.Cast(action_data.m_MainItem);
		if (alarm)
			alarm.TurnOnClient();
	}
	*/
	
	override void OnExecuteServer( ActionData action_data )
	{
		AlarmClock_ColorBase alarm = AlarmClock_ColorBase.Cast(action_data.m_MainItem);
		if (alarm)
			alarm.TurnOn();
	}
}
