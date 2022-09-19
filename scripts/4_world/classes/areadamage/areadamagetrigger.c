// DEPRECATED: Left for backwards compatibility with mods
// This file is NOT AN EXAMPLE of how to use triggers
// As overriding UpdateInsiders should ideally no longer be done
// Look at "AreaDamageTriggerBase.c" instead and use that as base
class AreaDamageTrigger extends AreaDamageTriggerBase
{
	protected ref AreaDamageBase	m_AreaDamageType;
	
	protected vector				m_ExtentMin;
	protected vector				m_ExtentMax;	
	protected float					m_InnerDistance;
	
	protected int					m_TriggerUpdateMs;
	
	void AreaDamageTrigger()
	{
		// Backwards compat, revert the flags back to what they used to be
		ClearEventMask(EntityEvent.ENTER | EntityEvent.LEAVE);
		SetEventMask(EntityEvent.TOUCH);
		
		m_AreaDamageType = null;
		
		m_ExtentMin = vector.Zero;
		m_ExtentMax = vector.Zero;	
		m_InnerDistance = 0;
		
		m_TriggerUpdateMs = 100;
	}
	
	// Backwards compat
	override void EOnTouch(IEntity other, int extra)
	{
		Object obj;
		if (Class.CastTo(obj, other))
			AddInsider(obj);
	}
	
	override void SetExtents( vector mins, vector maxs )
	{
		m_ExtentMax = maxs;
		m_ExtentMin = mins;
		
		m_InnerDistance = (GetRadius(m_ExtentMin, m_ExtentMax) * 0.5) + 0.2;

		super.SetExtents(mins, maxs);
	}
	
	override void SetAreaDamageManager( AreaDamageManager adType )
	{
		super.SetAreaDamageManager( adType );
		
		m_AreaDamageType = AreaDamageBase.Cast(adType);
	}
		
	void SetAreaDamageType( AreaDamageBase adType )
	{
		super.SetAreaDamageManager( adType );
		
		m_AreaDamageType = adType;
	}
	
	override void OnEnter( Object obj )
	{
		super.OnEnter( obj );
		
		if ( GetGame().IsServer() && m_AreaDamageType )
			 m_AreaDamageType.OnEnter( obj );
	}
	
	override void OnLeave( Object obj )
	{
		super.OnLeave( obj );
		
		if ( GetGame().IsServer() && m_AreaDamageType )
			 m_AreaDamageType.OnLeave( obj );
	}
	
	override protected void UpdateInsiders(int timeout)
	{
		if ( GetGame().IsServer() )
		{
			for (int n = 0; n < m_insiders.Count(); )
			{
				TriggerInsider ins = m_insiders.Get(n);
				if ( ins.GetObject() == null )
				{
					//object has been deleted. Remove it
					m_insiders.Remove(n);
					continue;
				}
	
				Object insObj = ins.GetObject();
				if ( insObj && ( !insObj.IsAlive() || vector.DistanceSq(insObj.GetPosition(), GetPosition()) > (m_InnerDistance * m_InnerDistance) ) )
				{
					int timeDiff = g_Game.GetTime() - ins.timeStamp;
					if (timeDiff > 500)
					{
						//object left. Remove it
						OnLeave(ins.GetObject());
						m_insiders.Remove(n);
						
						continue;
					}
					else
					{
						//Print("" + this + " :: " + insObj + " :: " + timeDiff);
					}
				}
				
				++n;
			}
			
			//!DEBUG
			#ifdef DEVELOPER
			DebugSendDmgTrigger();
			#endif
		}
	}
}