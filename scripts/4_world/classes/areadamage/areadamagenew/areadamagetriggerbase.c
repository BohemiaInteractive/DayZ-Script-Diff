//! Extended TriggerInsider for AreaDamageTriggerBase
class AreaDamageTriggerInsider : TriggerInsider
{
	//! Last time the object was damaged in seconds
	float lastDamaged;
	
	void AreaDamageTriggerInsider(Object obj)
	{
	}
}

//! Managed by AreaDamageManager and its derivatives
class AreaDamageTriggerBase : Trigger
{
	//! AreaDamageManager managing this instance
	protected ref AreaDamageManager		m_AreaDamageManager;
	
	//! ctor
	void AreaDamageTriggerBase()
	{
		m_AreaDamageManager = null;
	}

	//! dtor
	void ~AreaDamageTriggerBase()
	{
		//! call Leave for all insiders when removing trigger
		int nr = m_insiders.Count();
		for (int n = 0; n < nr; ++n)
		{
			Object insObj = m_insiders[n].GetObject();
			if ( insObj )
				Leave(m_insiders[n]);
		}
	}
	
	//! AreaDamageTriggerBase configuration
	//@{
	//! Set the manager instance
	void SetAreaDamageManager( AreaDamageManager adType )
	{
		m_AreaDamageManager = adType;
		
		#ifdef DEVELOPER
		m_DebugAreaType = m_AreaDamageManager.GetAmmoName();
		#endif
	}
	//@}
	
	
	//! TriggerEvents, pass them on to manager
	//@{
	//! OnEnter
	override protected void OnEnterBeginEvent(TriggerInsider insider)
	{
		super.OnEnterBeginEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnEnterBeginEvent(insider);
	}
	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		super.OnEnterServerEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnEnterServerEvent(insider);
	}
	override protected void OnEnterClientEvent(TriggerInsider insider)
	{
		super.OnEnterClientEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnEnterClientEvent(insider);
	}
	override protected void OnEnterEndEvent(TriggerInsider insider)
	{
		super.OnEnterEndEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnEnterEndEvent(insider);
	}

	//! OnStayStart
	override protected void OnStayStartBeginEvent(int nrOfInsiders)
	{
		super.OnStayStartBeginEvent(nrOfInsiders);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayStartBeginEvent(nrOfInsiders);
	}
	override protected void OnStayStartServerEvent(int nrOfInsiders)
	{
		super.OnStayStartServerEvent(nrOfInsiders);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayStartServerEvent(nrOfInsiders);
	}
	override protected void OnStayStartClientEvent(int nrOfInsiders)
	{
		super.OnStayStartClientEvent(nrOfInsiders);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayStartClientEvent(nrOfInsiders);
	}
	override protected void OnStayStartEndEvent(int nrOfInsiders)
	{
		super.OnStayStartEndEvent(nrOfInsiders);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayStartEndEvent(nrOfInsiders);
	}

	//!OnStay
	override protected void OnStayBeginEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayBeginEvent(insider, deltaTime);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayBeginEvent(insider, deltaTime);
	}
	override protected void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayServerEvent(insider, deltaTime);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayServerEvent(insider, deltaTime);
	}
	override protected void OnStayClientEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayClientEvent(insider, deltaTime);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayClientEvent(insider, deltaTime);
	}
	override protected void OnStayEndEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayEndEvent(insider, deltaTime);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayEndEvent(insider, deltaTime);
	}

	//! OnStayFinish
	override protected void OnStayFinishBeginEvent()
	{
		super.OnStayFinishBeginEvent();
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayFinishBeginEvent();
	}
	override protected void OnStayFinishServerEvent()
	{
		super.OnStayFinishServerEvent();
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayFinishServerEvent();
	}
	override protected void OnStayFinishClientEvent()
	{
		super.OnStayFinishClientEvent();
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayFinishClientEvent();
	}
	override protected void OnStayFinishEndEvent()
	{
		super.OnStayFinishEndEvent();
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnStayFinishEndEvent();
	}
	
	//! OnLeave
	override protected void OnLeaveBeginEvent(TriggerInsider insider)
	{
		super.OnLeaveBeginEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnLeaveBeginEvent(insider);
	}
	override protected void OnLeaveServerEvent(TriggerInsider insider)
	{
		super.OnLeaveServerEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnLeaveServerEvent(insider);
	}
	override protected void OnLeaveClientEvent(TriggerInsider insider)
	{
		super.OnLeaveClientEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnLeaveClientEvent(insider);
	}
	override protected void OnLeaveEndEvent(TriggerInsider insider)
	{
		super.OnLeaveEndEvent(insider);
		
		if ( m_AreaDamageManager )
			m_AreaDamageManager.OnLeaveEndEvent(insider);
	}
	//@}
	
	
	//! TriggerInsider conditions
	//@{
	//! Condition whether a TriggerInsider should still be updated or not (checked in update loop and before adding)
	override protected bool ShouldRemoveInsider(TriggerInsider insider)
	{
		return !insider.GetObject().IsAlive();
	}
	//@}
	
	
	//! TriggerInsider processing
	//@{
	//! Override the TriggerInsider type to be AreaDamageTriggerInsider
	override protected TriggerInsider CreateInsider(Object obj)
	{
		return new AreaDamageTriggerInsider(obj);		
	}
	
	//! Adding of new TriggerInsider
	override protected void AddInsider(Object obj)
	{
		if ( !GetGame().IsServer() )
			return;
		
		super.AddInsider(obj);
	}
	
	//! Removing of TriggerInsider
	override protected void RemoveInsider(TriggerInsider insider, int index = -1)
	{
		if ( !GetGame().IsServer() )
			return;
		
		super.RemoveInsider(insider, index);
	}
	
	//! Update the current TriggerInsider inside the Trigger
	override protected void UpdateInsiders(int timeout)
	{
		if ( !GetGame().IsServer() )
			return;
	
		super.UpdateInsiders(timeout);
	}
	//@}
}