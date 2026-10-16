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
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	static protected ref map<string, ref map<string, int>> m_TypeToSurfaceParticleIDMap;
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	static protected ref map<string, ref ExplosionEffectsData> m_SurfaceToExplosionEffectsData;
	
	void ExplosivesBase()
	{
		m_DeleteTimer 	= new Timer();
		m_AmmoTypes 	= new array<string>();

		SetAmmoType(DEFAULT_AMMO_TYPE);	
		SetParticlePosition(WorldToModel(GetPosition()));
		SetParticleOrientation(vector.Zero);
		
		RegisterNetSyncVariableBool("m_Armed");
		RegisterNetSyncVariableBool("m_Defused");
		
		Init();
	}
	
	
	override bool IsExplosive()
	{
		return true;
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		super.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
		CreateLight();
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		//! should be called only here to avoid multiple explosion calculations, call SetHealth("","",0.0) instead
	 	InitiateExplosion();
		
		UnpairRemote();	
	}
	
	override void OnCEIterate(float currentTime, float elapsedTime)
	{
		super.OnCEIterate(currentTime, elapsedTime);
		
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
	
	override void UnpairRemote()
	{
		if (GetRemotelyActivatedItemBehaviour())
		{
			if (GetPairDevice())
			{
				GetPairDevice().UnpairRemote();
			}
			GetRemotelyActivatedItemBehaviour().Unpair();
		}
			
	}

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (g_Game.IsServer())
		{
			SetOrientation(orientation);
			SetPosition(position);
			PlaceOnSurface();
		}
	}
	
	string GetArmSoundset()
	{
		return string.Empty;
	}
	
	string GetDisarmSoundset()
	{
		return string.Empty;
	}
	
	string GetDisarmSoundsetUnpaired()
	{
		return string.Empty;
	}
	
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		
		if (GetArmSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_EXPLOSIVE_ARM, GetArmSoundset());
	
		if (GetDisarmSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_EXPLOSIVE_DISARM, GetDisarmSoundset());
		
		if (GetDisarmSoundsetUnpaired() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_EXPLOSIVE_DISARM_UNPAIRED, GetDisarmSoundsetUnpaired());
	}
	
	protected void CreateLight()
	{
		m_Light = ExplosiveLight.Cast(ScriptedLightBase.CreateLight(ExplosiveLight, GetPosition()));
	}
	
	protected void DestroyParticle(Particle p)
	{
		#ifndef SERVER
		#ifndef NO_GUI
		if (p != null)
		{
			p.Stop();
		}
		#endif
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
		if (g_Game.IsServer())
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

	void OnBeforeDisarm();
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
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}

		return IsTakeable();
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}

		return IsTakeable();
	}

	override bool CanRemoveFromHands(EntityAI parent)
	{
		return IsTakeable();
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
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
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
	
	void LockExplosivesSlots();
	void UnlockExplosivesSlots();
	
	void Init()
	{
	}
	
	static void Cleanup()
	{
	}
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	void InitSpecificsExplosionEffectForSurface()
	{
		//Here add in override call AddExplosionEffectForSurface for specific explosion on surface 
	}
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	void AddExplosionEffectForSurface(ExplosionEffectsData data)
	{
		ExplosionEffectsData explosionEffectData;
		if (!m_SurfaceToExplosionEffectsData.Find(data.m_Surface, explosionEffectData))
			m_SurfaceToExplosionEffectsData.Insert(data.m_Surface, data);
	}
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	int GetParticleExplosionID(string surface)
	{
		int particleID;
		
		// kept for backward compatibility - looked up first
		map<string, int> extraSurfaceEffect;
		m_TypeToSurfaceParticleIDMap.Find(GetName(), extraSurfaceEffect);
		
		if (extraSurfaceEffect)
		{
			if (extraSurfaceEffect.Find(surface, particleID))
				return particleID;
		}
		
		ExplosionEffectsData explosionEffectData;
		if (m_SurfaceToExplosionEffectsData.Find(surface, explosionEffectData))
		{
			if (explosionEffectData)
				return explosionEffectData.m_ParticleId;
		}

		return m_ParticleExplosionId;
	}
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	string GetExplosionSoundSetName(string surface)
	{
		ExplosionEffectsData explosionEffectData;
		if (m_SurfaceToExplosionEffectsData.Find(surface, explosionEffectData))
		{
			if (explosionEffectData)
				return explosionEffectData.m_SoundSetName;
		}
		
		return "";
	}
	
	[Obsolete("1.30: Handled in AmmoTypes instead")]
	void AddExplosionEffectForSurface(string surface, int effectID)
	{
		map<string, int> extraSurfaceEffect;
		m_TypeToSurfaceParticleIDMap.Find(GetName(), extraSurfaceEffect);
		extraSurfaceEffect.Insert(surface, effectID);
	}
}

//[Obsolete("1.30: Handled in AmmoTypes instead")]
class ExplosionEffectsData
{
	string m_Surface;
	int m_ParticleId;
	string m_SoundSetName;
	
	void ExplosionEffectsData(string surface, int particleId, string soundSetName = "")
	{
		m_Surface 		= surface;
		m_ParticleId 	= particleId;
		m_SoundSetName	= soundSetName;
	}
}