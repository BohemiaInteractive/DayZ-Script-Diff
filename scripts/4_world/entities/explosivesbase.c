class ExplosiveLight : PointLightBase
{
	protected static float m_DefaultBrightness	= 10;
	protected static float m_DefaultRadius		= 30;
	
	void ExplosiveLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 1.0, 0.3);
		SetDiffuseColor(1.0, 1.0, 0.3);
		SetLifetime(0.15);
		SetDisableShadowsWithinRadius(-1);
	}	
}

class ExplosivesBase : ItemBase
{
	protected const string DEFAULT_AMMO_TYPE 		= "Explosion_NonLethal";
	protected const string ANIM_PHASE_VISIBILITY	= "Visibility";

	protected bool 				m_Armed;
	protected bool				m_Defused;
	protected ref array<string>		m_AmmoTypes;
	
	protected ref Timer			m_DeleteTimer;
	
	//! light	
	protected ExplosiveLight	m_Light;
	
	//! particle
	protected Particle 					m_ParticleExplosion;
	protected ref array<ParticleSource>	m_ParticleExplosionArr = {};
	protected int						m_ParticleExplosionId;
	protected vector					m_ParticlePosition;
	protected vector					m_ParticleOrientation;
	
	override bool IsExplosive()
	{
		return true;
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		super.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);

		if (m_ParticleExplosionId > ParticleList.INVALID)
		{
			EntityAI parent = this;
			if (GetHierarchyParent())
			{
				parent = GetHierarchyParent();
			}

			ParticleSource p = ParticleManager.GetInstance().PlayOnObject(m_ParticleExplosionId, parent, m_ParticlePosition, m_ParticleOrientation);
			m_ParticleExplosionArr.Insert(p);
			m_ParticleExplosion = p;
		}

		CreateLight();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if (m_ParticleExplosion)
		{
			foreach (ParticleSource p : m_ParticleExplosionArr)
			{
				DestroyParticle(p);
			}
		}
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		//! should be called only here to avoid multiple explosion calculations, call SetHealth("","",0.0) instead
	 	InitiateExplosion();
	}
	
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		
		if (!IsRuined() && GetArmed() && GetPairDevice())
		{
			if (vector.DistanceSq(GetPosition(), GetPairDevice().GetPosition()) <= Math.SqrFloat(UAMaxDistances.EXPLOSIVE_REMOTE_ACTIVATION))
			{
				UpdateLED(ERemoteDetonatorLEDState.LIT);
				
				return;
			}
		}
		
		UpdateLED(ERemoteDetonatorLEDState.OFF);
	}
	
	protected void CreateLight()
	{
		m_Light = ExplosiveLight.Cast(ScriptedLightBase.CreateLight(ExplosiveLight, GetPosition()));
	}
	
	protected void DestroyParticle(Particle p)
	{
		#ifndef SERVER
		if (p != null)
		{
			p.Stop();
		}
		#endif
	}
	
	protected void InitiateExplosion()
	{
		int count = m_AmmoTypes.Count();
		for (int i = 0; i < count; i++)
		{
			Explode(DamageType.EXPLOSION, m_AmmoTypes[i]);
		}

		OnExplode();
	}

	protected void OnExplode()
	{
		if (GetGame().IsServer())
		{
			m_DeleteTimer.Run(0.25, this, "DeleteSafe");
		}
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
	
	override bool IsInventoryVisible()
	{
		if (!super.IsInventoryVisible())
		{
			return false;
		}
		
		return GetAnimationPhase("Visibility") == 0;
	}
	
	override bool IsTakeable()
	{
		return super.IsTakeable() && GetAnimationPhase("Visibility") == 0;
	}
	
	bool IsTimerDetonable()
	{
		return false;
	}
	
	void Arm()
	{
		SetArmed(true);
		
		OnArmed();
	}
	
	void OnArmed();
	
	bool CanBeArmed()
	{
		return true;
	}
	
	void Disarm(bool pWithTool = false)
	{
		SetArmed(false);
		
		OnDisarmed(pWithTool);
	}
	
	void OnDisarmed(bool pWithTool);
	
	bool CanBeDisarmed()
	{
		return false;
	}
	
	bool GetArmed()
	{
		return m_Armed;	
	}

	protected void SetArmed(bool state)
	{
		m_Armed = state;
		SetSynchDirty();
	}
	
	bool GetDefused()
	{
		return m_Defused;	
	}

	protected void SetDefused(bool state)
	{
		m_Defused = state;
		SetSynchDirty();
	}
	
	void SetAmmoType(string pAmmoType)
	{
		SetAmmoTypes({pAmmoType});
	}
	
	void SetAmmoTypes(array<string> pAmmoTypes)
	{
		m_AmmoTypes.Clear();
		m_AmmoTypes = pAmmoTypes;
	}
	
	void SetParticleExplosion(int particle)
	{
		m_ParticleExplosionId = particle;
	}

	//! set position for smoke particle - needs to be in Local Space
	void SetParticlePosition(vector local_pos)
	{
		m_ParticlePosition = local_pos;

		if (GetHierarchyParent())
		{
			m_ParticlePosition = GetHierarchyParent().WorldToModel(GetPosition());
		}
	}
	
	void SetParticleOrientation(vector local_ori)
	{
		m_ParticleOrientation = local_ori;

		if (GetHierarchyParent())
		{
			m_ParticleOrientation = GetHierarchyParent().WorldToModel(GetOrientation());
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_Armed);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (version > 129)
		{		
			bool armed = false;
			if (!ctx.Read(armed))
			{
				return false;
			}
		
			SetArmed(armed);
		}

		return true;
	}
	
	//! HELPERS
	void UpdateLED(int pState);
	bool HasLockedTriggerSlots()
	{
		return false;
	}

	void LockTriggerSlots();
	void UnlockTriggerSlots();
	
	void ExplosivesBase()
	{
		m_DeleteTimer 	= new Timer();
		m_AmmoTypes 	= new array<string>();

		SetAmmoType(DEFAULT_AMMO_TYPE);		
		SetParticleExplosion(ParticleList.INVALID); //! no effect
		SetParticlePosition(WorldToModel(GetPosition()));
		SetParticleOrientation(vector.Zero);
		
		RegisterNetSyncVariableBool("m_Armed");
		RegisterNetSyncVariableBool("m_Defused");
	}
}
