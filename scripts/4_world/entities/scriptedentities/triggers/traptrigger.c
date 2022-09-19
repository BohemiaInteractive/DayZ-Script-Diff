//! Trigger used by traps
class TrapTrigger : Trigger
{
	TrapBase m_ParentObj;
	
	#ifdef DEVELOPER
	bool m_CanSendDbg = true;
	#endif
	
	void SetParentObject( TrapBase obj )
	{
		if (GetGame().IsServer())
		{
			m_ParentObj = obj;
		}
	}
	
	override protected bool CanAddObjectAsInsider(Object object)
	{
		return object.IsInherited(EntityAI) && m_ParentObj && m_ParentObj.IsActive();
	}
	
	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		#ifdef DEVELOPER
		m_CanSendDbg = false;
		#endif
		
		if (m_ParentObj && m_ParentObj.IsActive())
		{
			m_ParentObj.SnapOnObject(EntityAI.Cast(insider.GetObject()));
		}
	}
	
	override protected void OnLeaveServerEvent(TriggerInsider insider)
	{
		if (m_ParentObj && m_ParentObj.IsActive())
		{
			m_ParentObj.RemoveFromObject(EntityAI.Cast(insider.GetObject()));
		}
	}
	
	#ifdef DEVELOPER
	
	override void DebugSendDmgTrigger()
	{
		if ( m_CanSendDbg )
			super.DebugSendDmgTrigger();
	}
	
	#endif
}

// Used for tripwire type traps, where we want to allow players to go through
class TripWireTrigger : TrapTrigger
{
	override protected void OnEnterServerEvent( TriggerInsider insider )
	{
		PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
		
		if (playerInsider)
		{
			// If the player is jogging / sprinting while standing with or without hands raised, tripwire will trigger
			bool isErect = ( playerInsider.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT || playerInsider.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT || playerInsider.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH ); 
			bool isJogging = ( playerInsider.m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_RUN || playerInsider.m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT );
			if (isJogging && isErect) 
			{
				super.OnEnterServerEvent(insider);
				return;
			}
			
			// Special handling of crouch sprint
			if ( playerInsider.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH && playerInsider.m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT )
			{
				super.OnEnterServerEvent(insider);
				return;
			}
		}
		else
		{
			super.OnEnterServerEvent(insider);
		}
	}
}