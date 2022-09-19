// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageRegularDeferred extends AreaDamageRegular
{	
	void AreaDamageRegularDeferred(EntityAI parent)
	{

	}

	void ~AreaDamageRegularDeferred() {}

	override void SetLoopInterval(float time)
	{
		m_LoopInterval = time;
	}

	override void SetDeferDuration(float time)
	{
		m_DeferDuration = time;
	}

	override void SetHitZones( array<string> hitzones )
	{
		m_HitZones = hitzones;
	}
	
	override void OnEnterServer(Object object)
	{
		super.OnEnterServer(object);
		
		EvaluateDamage_DeferLoop(object);
	}
}