class StaticObj_Wreck_Mi24_TK extends CrashBase
{
	void StaticObj_Wreck_Mi24_TK()
	{
		if ( !g_Game.IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, GetMemoryPointPos("emitter"), Vector(0,0,0), true);
		}
	}
}
class StaticObj_Wreck_Mi24_TK_DE : StaticObj_Wreck_Mi24_TK {}