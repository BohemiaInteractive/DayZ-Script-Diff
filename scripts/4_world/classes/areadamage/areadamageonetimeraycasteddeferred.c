// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageOneTimeRaycastedDeferred extends AreaDamageBase
{	
	void AreaDamageOneTimeRaycastedDeferred(EntityAI parent)
	{

	}

	void ~AreaDamageOneTimeRaycastedDeferred() {}
	
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

		EvaluateDamage_Defer(object);
	}
}