class StaticObj_Wreck_Chinook extends CrashBase
{	
	void StaticObj_Wreck_Chinook()
	{
		
		if ( !g_Game.IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, GetMemoryPointPos("emitter"), Vector(0,0,0), true);
		}
	}
	override string GetSoundSet()
	{
		int x = Math.RandomInt(0, 2);		
		if (x == 0)
		{
			return "HeliCrash_big_SoundSet";
		}
		else
		{
			return "HeliCrash_big_no_explosions_SoundSet";
		}
	}
}
class StaticObj_Wreck_Chinook_DE : StaticObj_Wreck_Chinook {}