// WIP

class Explosion
{
	void SpawnEffect( vector position, Effect eff, vector pos, vector ori)
	{
		SEffectManager.PlayInWorld(eff, pos);
	}
}

class ExplosionTest : House
{
	ref Timer m_Delay;
	
	void ExplosionTest()
	{
		m_Delay = new Timer;
		m_Delay.Run(1, this, "ExplodeNow", null, false);
	}
	
	void ExplodeNow()
	{
		Explode(DT_EXPLOSION, "Explosion_NonLethal");
	}
}
