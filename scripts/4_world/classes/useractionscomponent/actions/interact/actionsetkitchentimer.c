class ActionSetKitchenTimerCB : ActionContinuousBaseCB
{
	const float TIME_TO_COMPLETE = 24.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(TIME_TO_COMPLETE);
		KitchenTimer clock = KitchenTimer.Cast(m_ActionData.m_MainItem);
		CAContinuousRepeat.Cast(m_ActionData.m_ActionComponent).SetProgress(clock.GetAnimationPhase("ClockAlarm") * TIME_TO_COMPLETE);
	}
};

class ActionSetKitchenTimer: ActionContinuousBase
{
	void ActionSetKitchenTimer()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_SET_KITCHENTIMER;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_SET_KITCHENTIMER;
		m_SpecialtyWeight = 0;
		
		m_CallbackClass = ActionSetKitchenTimerCB;
		m_Text = "#STR_SetTimer0";
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
	
	override void OnStartServer(ActionData action_data)
	{
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return true;
	}
	
	override void OnEndServer(ActionData action_data)
	{
		float progress = action_data.m_Callback.GetActionComponentProgress();
		KitchenTimer clock = KitchenTimer.Cast(action_data.m_MainItem);
		clock.SetAlarmTimeServer(progress);
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{

	}
	
	override void OnUpdate(ActionData action_data)
	{
		float progress = action_data.m_Callback.GetActionComponentProgress();
		KitchenTimer clock = KitchenTimer.Cast(action_data.m_MainItem);
		clock.SetAnimationPhaseNow("ClockAlarm", progress);
	}
	
};