//!ammo pile base
class Ammunition_Base: Magazine_Base
{
	static ref map<string, float> m_AmmoWeightByBulletType = new map<string, float>();
	
	static float GetAmmoWeightByBulletType(string bulletType)
	{
		if (m_AmmoWeightByBulletType.Contains(bulletType))
		{
			return m_AmmoWeightByBulletType.Get(bulletType);
		}
		else
		{
			float ammoWeight;
			string ammoTypeName;
			g_Game.ConfigGetText( string.Format("CfgAmmo %1 spawnPileType", bulletType) , ammoTypeName);
			if (ammoTypeName)
				ammoWeight = g_Game.ConfigGetFloat(string.Format("CfgMagazines %1 weight", ammoTypeName));
			else
				ErrorEx("empty 'spawnPileType' for bullet type:" + bulletType);
			if (ammoWeight)
				m_AmmoWeightByBulletType.Insert(bulletType, ammoWeight);
			return ammoWeight;
		}
	}
		
	override bool IsAmmoPile()
	{
		return true;
	}
	
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		#ifdef DEVELOPER
		if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
		{
			WeightDebugData data = WeightDebug.GetWeightDebug(this);
			data.SetCalcDetails("TAmmo: ("+GetAmmoCount()+"(Ammo count) * "+ GetConfigWeightModifiedDebugText());
		}
		#endif
		
		return GetAmmoCount() * GetConfigWeightModified();
	}
	
	override void SetQuantityToMinimum()
	{
		ServerSetAmmoCount(1);
	}
	
	override void SetFromProjectile(ProjectileStoppedInfo info)
	{
		float dmgPerUse = g_Game.ConfigGetFloat("cfgAmmo " + info.GetAmmoType() + " dmgPerUse");
		float totalDmg = info.GetProjectileDamage() + dmgPerUse;
		float health = Math.Max(1 - totalDmg, 0);
		
		SetQuantityToMinimum();
		SetHealth01("","", health);
		
		// SetCartridgeDamageAtIndex() MUST be called AFTER SetHealth01()!!
		// otherwise, decreasing health by less than an entire health level get ignored
		SetCartridgeDamageAtIndex(0, totalDmg);
	}
};

class Ammo_45ACP: Ammunition_Base {};
class Ammo_308Win: Ammunition_Base {};
class Ammo_308WinTracer: Ammunition_Base {};
class Ammo_9x19: Ammunition_Base {};
class Ammo_380: Ammunition_Base {};
class Ammo_556x45: Ammunition_Base {};
class Ammo_556x45Tracer: Ammunition_Base {};
class Ammo_762x54: Ammunition_Base {};
class Ammo_762x54Tracer: Ammunition_Base {};
class Ammo_762x39: Ammunition_Base {};
class Ammo_762x39Tracer: Ammunition_Base {};
class Ammo_9x39: Ammunition_Base {};
class Ammo_22: Ammunition_Base {};
class Ammo_12gaPellets: Ammunition_Base {};
class Ammo_12gaSlug: Ammunition_Base {};
class Ammo_357: Ammunition_Base {};
class Ammo_545x39: Ammunition_Base {};
class Ammo_545x39Tracer: Ammunition_Base {};
class Bolt_Base: Ammunition_Base
{
	override bool IsInventoryVisible()
	{
		//! omitted super call is intended
		return CanBeActionTarget();
	}

	override bool CanBeActionTarget()
	{
		if (super.CanBeActionTarget())
		{
			EntityAI parent = EntityAI.Cast(GetParent());
			if (parent)
			{
				return !parent.IsManagingArrows();
			}
		}
		return true;
	}
	
	override void EEParentedTo(EntityAI parent)
	{
		if (!parent)
			return;

		ArrowManagerBase arrowManager = parent.GetArrowManager();
		if (arrowManager)
		{
			arrowManager.AddArrow(this);	
		}
	}
	
	override void EEParentedFrom(EntityAI parent)
	{
		if (!parent)
			return;

		ArrowManagerBase arrowManager = parent.GetArrowManager(); 
		if (arrowManager)
		{
			arrowManager.RemoveArrow(this);	
		}
	}
}

class Ammo_DartSyringe: Ammunition_Base {};
class Ammo_Flare: Ammunition_Base {};
class Ammo_RPG7_HE: Ammunition_Base {};
class Ammo_RPG7_AP: Ammunition_Base {};
class Ammo_LAW_HE: Ammunition_Base {};
class Ammo_GrenadeM4 : Ammunition_Base {};

//bolts
class Ammo_HuntingBolt : Bolt_Base {}

class Ammo_ImprovisedBolt_1 : Bolt_Base 
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCraftBoltsFeather);
	}
}

class Ammo_ImprovisedBolt_2 : Bolt_Base {}
class Ammo_CupidsBolt : Bolt_Base
{
	override void EEParentedTo(EntityAI parent)
	{
		Delete();
	}
	
	override void EEParentedFrom(EntityAI parent);
	
	static void PlayOnHitParticle(vector position)
	{
		ParticleManager.GetInstance().PlayInWorld(ParticleList.BOLT_CUPID_HIT, position);
	}
}

// 40mm

class Ammo_40mm_Base: Ammunition_Base
{
	override bool IsTakeable()
	{
		return GetAnimationPhase("Visibility") == 0;
	}
	
	override bool IsInventoryVisible()
	{
		if (!super.IsInventoryVisible())
		{
			return false;
		}
		
		return IsTakeable();
	}
};

class Ammo_40mm_Explosive: Ammo_40mm_Base
{
	override bool ShootsExplosiveAmmo()
	{
		return true;
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (g_Game.IsServer())
		{
			DamageSystem.ExplosionDamage(this, null, "Explosion_40mm_Ammo", item.GetPosition(), DamageType.EXPLOSION);
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		DamageSystem.ExplosionDamage(this, null, "Explosion_40mm_Ammo", GetPosition(), DamageType.EXPLOSION);
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 1000, false);
	}
	
	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		#ifndef SERVER
		ClearFlags(EntityFlags.VISIBLE, false);
		#endif
	}
}
//class Ammo_40mm_Grenade_Gas: Ammo_40mm_Base {};
class Ammo_40mm_ChemGas: Ammo_40mm_Base
{
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (g_Game.IsServer())
		{
			g_Game.CreateObject("ContaminatedArea_Local", item.GetPosition());
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		g_Game.CreateObject("ContaminatedArea_Local", GetPosition());
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 1000, false);
	}
	
	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		
		#ifndef SERVER
		ClearFlags(EntityFlags.VISIBLE, false);
		ParticleManager.GetInstance().PlayInWorld(ParticleList.GRENADE_CHEM_BREAK, GetPosition());
		#endif
	}
	
	
}

class Ammo_40mm_Smoke_ColorBase: Ammo_40mm_Base
{
	protected Particle		m_ParticleSmoke;
	protected float 		m_ParticleLifetime;
	protected int			m_ParticleId;
	protected bool			m_Activated;
	
	void Ammo_40mm_Smoke_ColorBase()
	{
		RegisterNetSyncVariableBool("m_Activated");
	}
	
 	override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

		if (m_Activated)
		{
			#ifndef SERVER
			string particleStrIdentifier = g_Game.ConfigGetTextOut(string.Format("CfgMagazines %1 particleStrIdentifier", GetType()));
			m_ParticleId = ParticleList.GetParticleIDByName(particleStrIdentifier);
			if (m_ParticleId > 0)
			{
				m_ParticleSmoke = ParticleManager.GetInstance().PlayOnObject(m_ParticleId, this);
				m_ParticleSmoke.SetWiggle(7, 0.3);
			}
			#endif
		}
	}
	
	protected void Activate()
	{
		m_ParticleLifetime = g_Game.ConfigGetFloat(string.Format("CfgMagazines %1 particleLifeTime", GetType()));		
		m_Activated = true;
		SetSynchDirty();
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, m_ParticleLifetime * 1000);
	}

	//! special behaviour - do not call super
	override void EEKilled(Object killer)	
	{
		//analytics (behaviour from EntityAI)
		g_Game.GetAnalyticsServer().OnEntityKilled(killer, this);
	}

	override void EEDelete(EntityAI parent)	
	{
		#ifndef SERVER	
		if (m_ParticleSmoke)
		{
			m_ParticleSmoke.Stop();
		}
		#endif
		
		super.EEDelete(parent);
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		return !m_Activated;
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		SetHealth("", "", 0.0);
		Activate();
	}
}

class Ammo_40mm_Smoke_Red: Ammo_40mm_Smoke_ColorBase {};
class Ammo_40mm_Smoke_Green: Ammo_40mm_Smoke_ColorBase {};
class Ammo_40mm_Smoke_White: Ammo_40mm_Smoke_ColorBase {};
class Ammo_40mm_Smoke_Black: Ammo_40mm_Smoke_ColorBase {};