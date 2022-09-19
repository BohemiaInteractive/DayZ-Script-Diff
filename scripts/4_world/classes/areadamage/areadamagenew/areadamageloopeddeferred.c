//! Deferred version of AreaDamageLooped
class AreaDamageLoopedDeferred : AreaDamageLooped
{
	//! Time the damage should be deferred in seconds
	protected float m_DeferDuration = 1.0;

	override void SetDeferDuration(float time)
	{
		m_DeferDuration = time;
	}
	
	override protected float CalculateDamageScale(TriggerInsider insider, float deltaTime)
	{
		if ( m_DeferDuration > 0 && (insider.lastUpdated - insider.timeEntered) >= m_DeferDuration )
			return super.CalculateDamageScale(insider, deltaTime);
		
		return 0;
	}
}