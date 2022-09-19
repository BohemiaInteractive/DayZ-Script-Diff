//!ammo pile base
class Ammunition_Base: Magazine_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionSortAmmoPile);
	}
	
	override bool IsAmmoPile()
	{
		return true;
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
class Ammo_ArrowComposite: Ammunition_Base {};
class Ammo_SharpStick: Ammunition_Base {};
class Ammo_ArrowPrimitive: Ammunition_Base {};
class Ammo_ArrowBoned: Ammunition_Base {};
class Ammo_ArrowBolt: Ammunition_Base {};
class Ammo_DartSyringe: Ammunition_Base {};
class Ammo_Flare: Ammunition_Base {};
class Ammo_RPG7_HE: Ammunition_Base {};
class Ammo_RPG7_AP: Ammunition_Base {};
class Ammo_LAW_HE: Ammunition_Base {};
class Ammo_GrenadeM4 : Ammunition_Base {};
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
		if (GetGame().IsServer())
		{
			DamageSystem.ExplosionDamage(this, null, "Explosion_40mm_Ammo", item.GetPosition(), DamageType.EXPLOSION);
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		DamageSystem.ExplosionDamage(this, null, "Explosion_40mm_Ammo", GetPosition(), DamageType.EXPLOSION);
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 1000, false);
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
		if (GetGame().IsServer())
		{
			GetGame().CreateObject("ContaminatedArea_Local", item.GetPosition());
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		GetGame().CreateObject("ContaminatedArea_Local", GetPosition());
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DeleteSafe, 1000, false);
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
			string particleStrIdentifier = GetGame().ConfigGetTextOut(string.Format("CfgMagazines %1 particleStrIdentifier", GetType()));
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
		m_ParticleLifetime = GetGame().ConfigGetFloat(string.Format("CfgMagazines %1 particleLifeTime", GetType()));		
		m_Activated = true;
		SetSynchDirty();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, m_ParticleLifetime * 1000);
	}

	//! special behaviour - do not call super
	override void EEKilled(Object killer)	
	{
		//analytics (behaviour from EntityAI)
		GetGame().GetAnalyticsServer().OnEntityKilled(killer, this);
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