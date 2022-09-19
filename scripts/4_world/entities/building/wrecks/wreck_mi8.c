//New russian helicopter crash site
class Wreck_Mi8_Crashed extends CrashBase
{
	void Wreck_Mi8_Crashed()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, Vector(2, 0, -5));
		}
	}
}

//Old Russian helicopter crash site
class Wreck_Mi8 extends CrashBase
{

}