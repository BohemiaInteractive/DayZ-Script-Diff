// DEPRECATED: Left for backwards compatibility with mods
class AreaDamageRegular extends AreaDamageBase
{	
	void AreaDamageRegular(EntityAI parent)
	{

	}

	void ~AreaDamageRegular() {}

	override void SetLoopInterval( float time )
	{
		m_LoopInterval = time;
	}

	override void SetHitZones( array<string> hitzones )
	{
		m_HitZones = hitzones;
	}
	
	override void OnEnterServer(Object object)
	{
		super.OnEnterServer(object);

		EvaluateDamage_Loop(object);
	}
}