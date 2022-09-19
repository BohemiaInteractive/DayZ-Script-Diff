class ActionSetAlarmClockCB : ActionContinuousBaseCB
{
	const float TIME_TO_COMPLETE = 24.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(TIME_TO_COMPLETE);
		AlarmClock_ColorBase alarm = AlarmClock_ColorBase.Cast(m_ActionData.m_MainItem);
		CAContinuousRepeat.Cast(m_ActionData.m_ActionComponent).SetProgress(alarm.GetAnimationPhase("ClockAlarm") * TIME_TO_COMPLETE);
	}
};

class ActionSetAlarmClock: ActionContinuousBase
{
	void ActionSetAlarmClock()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_SET_ALARM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_SET_ALARM;
		m_SpecialtyWeight = 0;
		
		m_CallbackClass = ActionSetAlarmClockCB;
		m_Text = "#STR_SetAlarm0";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTSelf;
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}	
	
	override void OnStart(ActionData action_data)
	{
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return true;
	}
	
	override void OnEndServer(ActionData action_data)
	{
		float progress = action_data.m_Callback.GetActionComponentProgress();
		AlarmClock_ColorBase clock = AlarmClock_ColorBase.Cast(action_data.m_MainItem);
		clock.SetAlarmTimeServer(progress);
	}
	
	override void OnUpdate(ActionData action_data)
	{
		if (action_data.m_Callback)
		{
			float progress = action_data.m_Callback.GetActionComponentProgress();
			AlarmClock_ColorBase alarm = AlarmClock_ColorBase.Cast(action_data.m_MainItem);
			alarm.SetAnimationPhaseNow("ClockAlarm", progress);
		}
	}
	
};