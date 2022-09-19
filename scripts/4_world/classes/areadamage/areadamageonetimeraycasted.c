// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageOneTimeRaycasted extends AreaDamageBase
{	
	void AreaDamageOneTimeRaycasted(EntityAI parent)
	{

	}

	void ~AreaDamageOneTimeRaycasted() {}

	override void SetRaycastSources( array<string> raycast_sources )
	{
		m_RaycastSources = raycast_sources;
	}
	
	override void OnEnterServer(Object object)
	{
		super.OnEnterServer(object);

		EvaluateDamage(object);
	}
}