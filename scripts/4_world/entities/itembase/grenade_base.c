enum EGrenadeType
{
	FRAGMENTATION = 0,
	CHEMICAL,
	ILLUMINATING,
	NON_LETHAL
}

//! For backward compatibility
class GrenadeLight : ExplosiveLight {}

class FlashGrenadeLight : PointLightBase
{
	protected static float m_DefaultBrightness	= 50;
	protected static float m_DefaultRadius		= 20;
	
	void FlashGrenadeLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 1.0, 1.0);
		SetDiffuseColor(1.0, 1.0, 1.0);
		SetLifetime(0.35);
		SetDisableShadowsWithinRadius(-1);
	}
}

class Grenade_Base : ExplosivesBase
{
	protected const float DEFAULT_FUSE_DELAY 	= 10;

	protected ref Timer		m_FuseTimer;
	protected float			m_FuseDelay;
	protected float 		m_RemainingFuseTime;
	
	protected bool			m_Pinned;
	protected bool			m_Pinnable;
	//protected bool		m_Explodable;	//! DEPRECATED; not used anywhere
	
	protected EGrenadeType	m_GrenadeType;

	void Pin()
	{
		if (!m_Pinned && m_Pinnable)
		{
			OnPin();
		}
	}
	
	void Unpin()
	{
		if (m_Pinned)
		{
			OnUnpin();
		}
	}		
	
	//! DEPRECATED use OnActivatedByItem
	override void OnActivatedByTripWire();
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (item == this)
		{
			OnActivateFinished();
			return;
		}

		Unpin();
	}
	
	bool IsPinned()
	{
		return m_Pinned;
	}
	
	bool IsPinnable()
	{
		//! cannot be pinned once the fuse has started
		if (m_FuseTimer.IsRunning())
		{
			return false;	
		}
		
		return m_Pinnable;
	}
	
	void ActivateImmediate()
	{
		OnActivateImmediate();
	}
	
	void ActivateRandomTime()
	{
		float delay = Math.RandomFloat(1, 20);
		delay *= 1000; //! to millis
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ActivateImmediate, delay, false);
	}
	
	void SetPinnable(bool state)
	{
		m_Pinnable = state;
	}
	
	void SetFuseDelay(float delay)
	{
		m_FuseDelay = delay;
	}

	void SetGrenadeType(EGrenadeType type)
	{
		m_GrenadeType = type;
	}
	
	EGrenadeType GetGrenadeType()
	{
		return m_GrenadeType;
	}
	
	protected void Activate()
	{
		if (!m_FuseTimer.IsRunning())
		{
			//! run only the remaining part (already unpinned and pinned)
			if (m_RemainingFuseTime > 0)
			{
				//Debug.Log(string.Format("Grenade activated num of seconds to explosion: %1", m_RemainingFuseTime));
				m_FuseTimer.Run(m_RemainingFuseTime, this, "OnActivateFinished");
			}
			else
			{
				//Debug.Log(string.Format("Grenade activated num of seconds to explosion: %1", m_FuseDelay));
				m_FuseTimer.Run(m_FuseDelay, this, "OnActivateFinished");
			}

		}
	}

	protected void Deactivate()
	{
		if (m_FuseTimer.IsRunning())
		{
			m_RemainingFuseTime = m_FuseTimer.GetRemaining();
			m_FuseTimer.Stop();
			OnDeactivate();
		}
	}
	
	protected override void InitiateExplosion()
	{
		switch (GetGrenadeType())
		{
			case EGrenadeType.FRAGMENTATION:
			case EGrenadeType.ILLUMINATING:
				for (int i = 0; i < m_AmmoTypes.Count(); i++)
				{
					Explode(DamageType.EXPLOSION, m_AmmoTypes[i]);
				}
			break;
			case EGrenadeType.CHEMICAL:
			case EGrenadeType.NON_LETHAL:
			break;
		}
		
		OnExplode();	
	}

	//! DEPRECATED - for backward compatibility only
	protected void ExplodeGrenade(EGrenadeType grenade_type)
	{
		InitiateExplosion();
	}

	protected void OnPin()
	{
		m_Pinned = true;
		if (GetGame().IsServer())
		{
			SetSynchDirty();
		}

		Deactivate();
	}

	protected void OnUnpin()
	{
		m_Pinned = false;
		if (GetGame().IsServer())
		{
			SetSynchDirty();
		}

		OnActivateStarted();
	}

	protected void OnActivateStarted();
	protected void OnActivateFinished()
	{
		if (GetGame().IsServer())
		{
			SetHealth("", "", 0.0); // causes explosion when grenade is destroyed
			SetTakeable(false);
		}
	}
	
	protected void OnActivateImmediate()
	{
		if (GetGame().IsServer())
		{
			SetHealth("", "", 0.0); // causes explosion when grenade is destroyed
			SetTakeable(false);
		}
	}
	
	protected void OnDeactivate();
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		if (GetGame().SaveVersion() >= 107)
		{
			ctx.Write(m_Pinned);
		}
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		bool pinned;
		if (version >= 107)
		{
			if (!ctx.Read(pinned))
			{
				return false;
			}
			
			m_Pinned = pinned;
		}

		return true;
	}
	
	override bool CanBeArmed()
	{
		return false;
	}
	
	override bool CanBeDisarmed()
	{
		return false;
	}

	override bool CanExplodeInFire()
	{
		return true;
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionUnpin);
		AddAction(ActionPin);
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		//! activate grenade when it leaves player hands (safety handle released)
		if (newLoc.GetType() != InventoryLocationType.HANDS && !IsPinned())
		{
			Activate();
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if (parent.IsAnyInherited({TrapBase,ImprovisedExplosive}))
		{
			Deactivate();
		}
	}
	
	void Grenade_Base()
	{
		m_Pinned			= true;
		m_FuseTimer			= new Timer;
		m_RemainingFuseTime = -1;

		SetPinnable(true);
		SetFuseDelay(DEFAULT_FUSE_DELAY);
		SetGrenadeType(EGrenadeType.FRAGMENTATION);
		
		RegisterNetSyncVariableBool("m_Pinned");
	}
}