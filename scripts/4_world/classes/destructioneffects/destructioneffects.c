/*
class TestDestructionEffect : DestructionEffectBase
{
	ref AreaDamageManager m_AreaManager;
	override void Init()
	{
		m_EntityIsTakeable = true;
		
		m_ParticlePersistent = ParticleList.GRENADE_M18_GREEN_LOOP;
		m_ParticleOneTime = ParticleList.GRENADE_M84;
		
		m_SoundSetOneTime = "AlarmClock_Destroyed_SoundSet";
		m_SoundSetPersistent= "KitchenTimer_Ticking_Loop_SoundSet";
		
		m_ReplaceWithEntity = "";
		m_KeepHealthOnReplace = true;
		m_ReplaceDelay = 1000;
		
		m_HasExplosionDamage = true;
		m_AmmoType = "";
		m_DamageType = 0;
	}
	
	override void OnEntityDestroyedOneTimeClient(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedOneTimeServer(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentClient(EntityAI entity, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentServer(EntityAI entity, string zone)
	{
		AreaDamageLoopedDeferred_NoVehicle area_damage = new AreaDamageLoopedDeferred_NoVehicle( m_Entity );
		area_damage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		
		vector min, max;
		m_Entity.GetBounds(min, max);
		vector center = m_Entity.GetPosition();

		vector orientation = m_Entity.GetOrientation();
		
		area_damage.SetExtents( "-1 -1 -1", "1 1 1" );
		area_damage.SetAreaPosition( center );
		area_damage.SetAreaOrientation( orientation );
		area_damage.SetLoopInterval( 1.0 );
		area_damage.SetDeferDuration( 0.2 );
		area_damage.SetHitZones( { "Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot" } );
		area_damage.SetAmmoName( "BarbedWireHit" );
		area_damage.Spawn();
		
		m_AreaManager = area_damage;
		
	}
}

//--------------------------------------------------------------------



class TestDestructionEffect2 : DestructionEffectBase
{
	override void Init()
	{
		m_EntityIsTakeable = false;
		
		
		m_ParticlePersistent = ParticleList.GRENADE_M18_GREEN_LOOP;
		m_ParticleOneTime = ParticleList.GRENADE_M84;
		
		m_SoundSetOneTime = "AlarmClock_Destroyed_SoundSet";
		m_SoundSetPersistent= "KitchenTimer_Ticking_Loop_SoundSet";
		
		m_ReplaceWithEntity = "";
		m_KeepHealthOnReplace = true;
		m_ReplaceDelay = 1000;
		
		m_HasExplosionDamage = true;
		m_AmmoType = "LandFuelFeed_Ammo";
		m_DamageType = DamageType.EXPLOSION;
	}
	
	
	override void OnEntityDestroyedOneTimeClient(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedOneTimeServer(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentClient(EntityAI entity, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentServer(EntityAI entity, string zone)
	{
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		if ( !GetGame().IsDedicatedServer() )
		{	
			vector n = surfNormal.VectorToAngles() + "0 90 0";
			Particle p1 = ParticleManager.GetInstance().PlayInWorld(ParticleList.SMOKE_GENERIC_WRECK, pos);
			p1.SetOrientation(n);
		
			Particle p2 = ParticleManager.GetInstance().PlayInWorld(ParticleList.EXPLOSION_LANDMINE, pos);
			p2.SetOrientation(n);

			Particle p3 = ParticleManager.GetInstance().PlayInWorld(ParticleList.IMPACT_METAL_RICOCHET, pos);
			p3.SetOrientation(n);
		
			Particle p4 = ParticleManager.GetInstance().PlayInWorld(ParticleList.IMPACT_GRAVEL_RICOCHET, pos);
			p4.SetOrientation(n);
		}
	}
}
*/
class DestructionEffectGasCanister : DestructionEffectBase
{
	override void Init()
	{
		m_EntityIsTakeable = true;
		
		
		m_ParticlePersistent = ParticleList.NONE;
		m_ParticleOneTime = ParticleList.RGD5;
		
		m_SoundSetOneTime = "Flare_Gun_Shot_SoundSet";
		m_SoundSetPersistent= "";
		
		m_ReplaceWithEntity = "";
		m_KeepHealthOnReplace = true;
		m_ReplaceDelay = 1000;
		
		m_HasExplosionDamage = true;
		m_AmmoType = "GasCanister_Ammo";
		m_DamageType = DamageType.EXPLOSION;
	}
	
	
	override void OnEntityDestroyedOneTimeClient(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedOneTimeServer(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentClient(EntityAI entity, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentServer(EntityAI entity, string zone)
	{
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
	}
}

class DestructionEffectMetalicMechanism : DestructionEffectBase
{
	override void Init()
	{
		m_EntityIsTakeable = true;
		
		
		m_ParticlePersistent = ParticleList.NONE;
		m_ParticleOneTime = ParticleList.NONE;
		
		m_SoundSetOneTime = "AlarmClock_Destroyed_SoundSet";
		m_SoundSetPersistent= "";
		
		m_ReplaceWithEntity = "";
		m_KeepHealthOnReplace = true;
		m_ReplaceDelay = 1000;
		
		m_HasExplosionDamage = false;
		m_AmmoType = "";
		m_DamageType = DamageType.EXPLOSION;
	}
	
	
	override void OnEntityDestroyedOneTimeClient(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedOneTimeServer(EntityAI entity, int oldLevel, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentClient(EntityAI entity, string zone)
	{
	}
	
	override void OnEntityDestroyedPersistentServer(EntityAI entity, string zone)
	{
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
	}
}