// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageRegularRaycasted extends AreaDamageBase
{	
	void AreaDamageRegularRaycasted(EntityAI parent)
	{

	}

	void ~AreaDamageRegularRaycasted() {}

	override void SetLoopInterval(float time)
	{
		m_LoopInterval = time;
	}

	override void SetRaycastSources( array<string> raycast_sources )
	{
		m_RaycastSources = raycast_sources;
	}
	
	override void OnEnterServer(Object object)
	{
		super.OnEnterServer(object);
		
		EvaluateDamage_Loop(object);
	}
}