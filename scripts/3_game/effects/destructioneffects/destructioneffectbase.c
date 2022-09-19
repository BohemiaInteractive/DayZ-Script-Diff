class DestructionEffectBase
{
	EntityAI 		m_Entity;
	
	bool 			m_EntityIsTakeable;
	
	ParticleSource 	m_POneTime;
	ParticleSource 	m_PPersistent;
	
	int 			m_ParticleOneTime;
	int 			m_ParticlePersistent;
	
	EffectSound 	m_SOneTime;
	EffectSound 	m_SPersistent;
	
	string 			m_SoundSetOneTime;
	string 			m_SoundSetPersistent;

	bool 			m_KeepHealthOnReplace;
	string 			m_ReplaceWithEntity;
	int 			m_ReplaceDelay;
		
	bool 			m_HasExplosionDamage;
	DamageType		m_DamageType;
	string 			m_AmmoType;
	
	
	void ~DestructionEffectBase()
	{
		if (m_POneTime)
		{
			m_POneTime.Stop();
		}
		if (m_PPersistent)
		{
			m_PPersistent.Stop();
		}
		
		SEffectManager.DestroyEffect(m_SOneTime);
		SEffectManager.DestroyEffect(m_SPersistent);
	}
	
	private void Init();
	
	bool HasExplosionDamage()
	{
		return (m_HasExplosionDamage && m_AmmoType);
	}
	
	private void DealExplosionDamage()
	{
		DamageSystem.ExplosionDamage(m_Entity, null, m_AmmoType, m_Entity.GetPosition(), m_DamageType);
	}
	
	void OnHealthLevelChanged(notnull EntityAI entity, int oldLevel, int newLevel, string zone)
	{
		m_Entity = entity;
		Init();
		
		if (GetGame().IsServer())
		{
			entity.SetTakeable(m_EntityIsTakeable);
			
			if (oldLevel != -1 || entity.m_Initialized)
			{
				if (m_ReplaceWithEntity)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ReplaceEntityServer, m_ReplaceDelay, false);
				}
				if (HasExplosionDamage())
				{
					DealExplosionDamage();
				}
				OnEntityDestroyedOneTimeServer(entity, oldLevel, zone);
			}
			OnEntityDestroyedPersistentServer(entity, zone);
		}
		#ifndef SERVER//client OR single
		{
			if (oldLevel != -1 || entity.m_Initialized)//one time
			{
				m_POneTime = PlayParticle(m_ParticleOneTime);
				
				if (m_POneTime)
				{
					m_POneTime.SetOwner(this);
				}
								
				OnEntityDestroyedOneTimeClient(entity, oldLevel, zone);
				
				m_Entity.PlaySoundSet(m_SOneTime, m_SoundSetOneTime, 0, 0 );
				m_Entity.PlaySoundSetLoop(m_SPersistent, m_SoundSetPersistent, 0, 0 );
			}
			else//Persistent
			{
				OnEntityDestroyedPersistentClient(entity, zone);
				m_Entity.PlaySoundSetLoop(m_SPersistent, m_SoundSetPersistent, 0, 0 );
			}
			
			m_PPersistent = PlayParticle(m_ParticlePersistent, true);
			
			if (m_PPersistent)
			{
				m_PPersistent.SetOwner(this);
			}
		}
		#endif
	}
	
	private void ReplaceEntityServer()
	{
		EntityAI dead_entity = EntityAI.Cast(GetGame().CreateObjectEx(m_ReplaceWithEntity, m_Entity.GetPosition(), ECE_OBJECT_SWAP, RF_ORIGINAL));
		dead_entity.SetOrientation(m_Entity.GetOrientation());
		if (m_KeepHealthOnReplace)
		{
			dead_entity.SetHealth(m_Entity.GetHealth());
		}
		m_Entity.Delete();
	}
	
	private ParticleSource PlayParticle(int particleType, bool attach = false)
	{
		if (!m_Entity)
		{
			ErrorEx("Missing entity - something went wrong");
			return null;
		}
		if (particleType)
		{
			ParticleSource p = ParticleManager.GetInstance().PlayInWorld(particleType, m_Entity.GetPosition());
			if (attach && p)
			{
				p.AddAsChild(m_Entity);//Note: it's also possible to directly play on object: Particle.PlayOnObject
			}
			return p;
		}
		return null;
	}
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!! Override methods bellow !!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// ! Client Event called the moment the entity is destroyed, best for explosion and other one-time effects
	void OnEntityDestroyedOneTimeClient(EntityAI entity, int oldLevel,string zone);

	// ! Server Event called the moment the entity is destroyed
	void OnEntityDestroyedOneTimeServer(EntityAI entity, int oldLevel, string zone);

	// ! Client Event called at the same moment as the one-time event, but also when the entity is loaded/spawned on client, typically as the player connects to the server or moves close enough so that previously non-existent client entity is now spawned in
	void OnEntityDestroyedPersistentClient(EntityAI entity, string zone);

	// ! Server Event called at the same moment as the one time event, but also when the entity is loaded/spawned on Server, typically as the server is starting up
	void OnEntityDestroyedPersistentServer(EntityAI entity, string zone);

	// !Relayed from entity when explosion happens
	void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType);

}

