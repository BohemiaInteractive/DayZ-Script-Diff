// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageRegularRaycastedDeferred extends AreaDamageBase
{	
	void AreaDamageRegularRaycastedDeferred(EntityAI parent)
	{

	}

	void ~AreaDamageRegularRaycastedDeferred() {}

	override void SetLoopInterval(float time)
	{
		m_LoopInterval = time;
	}

	override void SetDeferDuration(float time)
	{
		m_DeferDuration = time;
	}
	
	override void SetRaycastSources( array<string> raycast_sources )
	{
		m_RaycastSources = raycast_sources;
	}
	
	override void OnEnterServer(Object object)
	{
		super.OnEnterServer(object);
		
		EvaluateDamage_DeferLoop(object);
	}
}