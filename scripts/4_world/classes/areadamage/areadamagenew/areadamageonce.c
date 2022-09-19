class AreaDamageOnce : AreaDamageManager
{	
	override void OnEnterServerEvent(TriggerInsider insider)
	{
		super.OnEnterServerEvent(insider);
		
		OnEvaluateDamageEx(insider, 1);
	}
}

class AreaDamageOnceDeferred : AreaDamageOnce
{
	//! Time the damage should be deferred in seconds
	protected float m_DeferDuration = 1.0;

	override void SetDeferDuration(float time)
	{
		m_DeferDuration = time;
	}
	
	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		super.OnStayServerEvent(insider, deltaTime);
		
		AreaDamageTriggerInsider dInsider;
		if ( CastTo( dInsider, insider ) && dInsider.lastDamaged == 0 )
			OnEvaluateDamageEx(insider, 1);
	}
	
	override protected float CalculateDamageScale(TriggerInsider insider, float deltaTime)
	{
		if ( m_DeferDuration > 0 && (insider.lastUpdated - insider.timeEntered) >= m_DeferDuration )
			return super.CalculateDamageScale(insider, deltaTime);
		
		return 0;
	}
}