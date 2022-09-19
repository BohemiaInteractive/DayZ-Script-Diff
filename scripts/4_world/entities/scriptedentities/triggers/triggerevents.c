//! Events API for triggers, keep in sync with AreaDamageEvents for consistency
//! NOTE: The deltaTime passed in is the one for the Insider specifically.
class TriggerEvents : ScriptedEntity
{	
	/** \name OnEnter
 		Called when an object enters the trigger
	*/
	///@{
	protected void Enter(TriggerInsider insider)
	{
		#ifdef TRIGGER_DEBUG_BASIC
		Debug.TriggerLog(GetDebugName(insider.GetObject()), "TriggerEvents", "", "Enter", GetDebugName(this));
		#endif
		
		OnEnterBeginEvent(insider);
		
		if ( GetGame().IsServer() )
			OnEnterServerEvent(insider);
		else
			OnEnterClientEvent(insider);
		
		OnEnterEndEvent(insider);
	}	
	protected void OnEnterBeginEvent(TriggerInsider insider) {}
	protected void OnEnterServerEvent(TriggerInsider insider) {}
	protected void OnEnterClientEvent(TriggerInsider insider) {}
	protected void OnEnterEndEvent(TriggerInsider insider) {}
	///@}
	
	/** \name OnStayStart
 		Called at the beginning of an update loop
	*/
	///@{
	protected void StayStart(int nrOfInsiders)
	{
		#ifdef TRIGGER_DEBUG_SPAM
		Debug.TriggerLog(nrOfInsiders.ToString(), "TriggerEvents", "", "StayStart", GetDebugName(this));
		#endif
		
		OnStayStartBeginEvent(nrOfInsiders);
		
		if ( GetGame().IsServer() )
			OnStayStartServerEvent(nrOfInsiders);
		else
			OnStayStartClientEvent(nrOfInsiders);
		
		OnStayStartEndEvent(nrOfInsiders);
	}	
	protected void OnStayStartBeginEvent(int nrOfInsiders) {}
	protected void OnStayStartServerEvent(int nrOfInsiders) {}
	protected void OnStayStartClientEvent(int nrOfInsiders) {}
	protected void OnStayStartEndEvent(int nrOfInsiders) {}
	///@}
	
	/** \name OnStay
 		Called in the update loop
	*/
	///@{
	protected void Stay(TriggerInsider insider, float deltaTime)
	{
		#ifdef TRIGGER_DEBUG_SPAM
		Debug.TriggerLog(GetDebugName(insider.GetObject()), "TriggerEvents", "", "Stay", GetDebugName(this));
		#endif
		
		OnStayBeginEvent(insider, deltaTime);
		
		if ( GetGame().IsServer() )
			OnStayServerEvent(insider, deltaTime);
		else
			OnStayClientEvent(insider, deltaTime);
		
		OnStayEndEvent(insider, deltaTime);
	}	
	protected void OnStayBeginEvent(TriggerInsider insider, float deltaTime) {}
	protected void OnStayServerEvent(TriggerInsider insider, float deltaTime) {}
	protected void OnStayClientEvent(TriggerInsider insider, float deltaTime) {}
	protected void OnStayEndEvent(TriggerInsider insider, float deltaTime) {}
	///@}
	
	/** \name OnStayFinish
 		Called at the end of an update loop
	*/
	///@{
	protected void StayFinish()
	{
		#ifdef TRIGGER_DEBUG_SPAM
		Debug.TriggerLog("", "TriggerEvents", "", "StayFinish", GetDebugName(this));
		#endif
		
		OnStayFinishBeginEvent();
		
		if ( GetGame().IsServer() )
			OnStayFinishServerEvent();
		else
			OnStayFinishClientEvent();
		
		OnStayFinishEndEvent();
	}	
	protected void OnStayFinishBeginEvent() {}
	protected void OnStayFinishServerEvent() {}
	protected void OnStayFinishClientEvent() {}
	protected void OnStayFinishEndEvent() {}
	///@}
	
	/** \name OnLeave
 		Called when an object leaves the trigger
	*/
	///@{
	protected void Leave(TriggerInsider insider)
	{
		#ifdef TRIGGER_DEBUG_BASIC
		Debug.TriggerLog(GetDebugName(insider.GetObject()), "TriggerEvents", "", "Leave", GetDebugName(this));
		#endif
		
		OnLeaveBeginEvent(insider);
		
		if ( GetGame() && GetGame().IsServer() )
			OnLeaveServerEvent(insider);
		else
			OnLeaveClientEvent(insider);
		
		OnLeaveEndEvent(insider);
	}	
	protected void OnLeaveBeginEvent(TriggerInsider insider) {}
	protected void OnLeaveServerEvent(TriggerInsider insider) {}
	protected void OnLeaveClientEvent(TriggerInsider insider) {}
	protected void OnLeaveEndEvent(TriggerInsider insider) {}
	///@}
}