class Grenade_ChemGas : Grenade_Base
{
	protected bool 					m_Exploded;
	protected ParticleSource 		m_ParticleExploded;
	protected EffectSound 			m_ExplosionSound;
	
	
	void Grenade_ChemGas()
	{
		SetParticleExplosion(ParticleList.RGD5);
		SetGrenadeType(EGrenadeType.CHEMICAL);
		m_Pinned = false;
		SetPinnable(false);
		Arm();
	}

	void ~Grenade_ChemGas();

	override protected void OnExplode()
	{
		m_Exploded = true;
		
		if (GetGame().IsServer())
		{
			GetGame().CreateObject("ContaminatedArea_Local", GetPosition());
		}
	}
	
	protected string GetExplosionSoundSet()
	{
		return "Grenade_detonation_SoundSet";
	}
	
	override void EOnContact(IEntity other, Contact extra)
	{
		super.EOnContact(other, extra);
		
		if (GetGame().IsServer())
		{
			if (!m_Exploded)
			{
				OnActivateFinished();
			}
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
	}
	
	override void OnDamageDestroyed(int oldLevel)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Delete, 1000);
		}
		#ifndef SERVER
		ClearFlags(EntityFlags.VISIBLE, false);
		m_ParticleExploded = ParticleManager.GetInstance().PlayInWorld(ParticleList.GRENADE_CHEM_BREAK, GetPosition());
		PlaySoundSet( m_ExplosionSound, GetExplosionSoundSet(), 0, 0 );
		#endif

	}
	
	override protected void Activate()
	{
		//we don't want base functionality here
	}
	
}