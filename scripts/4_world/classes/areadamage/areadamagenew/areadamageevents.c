//! Events used across AreaDamage API, calling of these needs to be handled by parent
class AreaDamageEvents
{
	///@{
	//! Called when a trigger is created
	void OnTriggerCreated() {}
	//! Called when a trigger is destroyed
	void OnTriggerDestroyed() {}	
	///@}
	
	///@{
	//! Called when an object enters the trigger
	void OnEnterBeginEvent(TriggerInsider insider) {}
	void OnEnterServerEvent(TriggerInsider insider) {}
	void OnEnterClientEvent(TriggerInsider insider) {}
	void OnEnterEndEvent(TriggerInsider insider) {}
	///@}
	
	///@{
	//! Called at the beginning of an update loop
	void OnStayStartBeginEvent(int nrOfInsiders) {}
	void OnStayStartServerEvent(int nrOfInsiders) {}
	void OnStayStartClientEvent(int nrOfInsiders) {}
	void OnStayStartEndEvent(int nrOfInsiders) {}
	///@}
	
	///@{
	//! Called in the update loop
	void OnStayBeginEvent(TriggerInsider insider, float deltaTime) {}
	void OnStayServerEvent(TriggerInsider insider, float deltaTime) {}
	void OnStayClientEvent(TriggerInsider insider, float deltaTime) {}
	void OnStayEndEvent(TriggerInsider insider, float deltaTime) {}
	///@}
	
	///@{
	//! Called at the end of an update loop
	void OnStayFinishBeginEvent() {}
	void OnStayFinishServerEvent() {}
	void OnStayFinishClientEvent() {}
	void OnStayFinishEndEvent() {}
	///@}
	
	///@{
	//! Called when an object leaves the trigger
	void OnLeaveBeginEvent(TriggerInsider insider) {}
	void OnLeaveServerEvent(TriggerInsider insider) {}
	void OnLeaveClientEvent(TriggerInsider insider) {}
	void OnLeaveEndEvent(TriggerInsider insider) {}
	///@}
	
	///@{
	//! Called when an object gets evaluated for taking damage
	protected bool OnEvaluateDamage(TriggerInsider insider, float damageCoef) { return false; }
	///@}
	
	///@{
	//! Called before an object is damaged (only when it actually damaged an object)
	protected void PreDamageActions() {}
	//! Called after an object was damaged (only when it actually damaged an object)
	protected void PostDamageActions() {}
	///@}
}