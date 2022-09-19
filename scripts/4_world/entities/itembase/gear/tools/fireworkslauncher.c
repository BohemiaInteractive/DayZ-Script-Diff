class ExplosionLight : PointLightBase
{
	void ExplosionLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(80);
		SetBrightnessTo(0.05);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 1.0, 1.0);
		SetDiffuseColor(1.0, 1.0, 1.0);
		SetLifetime(2.1);
		SetDisableShadowsWithinRadius(-1);
		SetFadeOutTime(1);
		m_FadeInTime = 0.25;
		SetFlickerSpeed(7);
		//SetFlickerAmplitude(0.5);
		SetFlickerAmplitudeMax(3);
		SetFlickerAmplitudeMin(0);
	}
}

class FireworksLauncherClientEventBase
{
	void OnFired();
}

class FireworksLauncherClientEvent : FireworksLauncherClientEventBase
{
	protected ref Timer 			m_Timer = new Timer();
	protected FireworksLauncher 	m_Item;
	protected int 					m_Index;
	protected vector				m_ExplosionPos;
	protected EffectSound 			m_FireSound;
	protected EffectSound 			m_ExplosionSound;
	protected ParticleSource 		m_ParticleShot;
	protected ParticleSource 		m_ParticleExplosion;
	protected vector				m_ShotDir;
	protected ExplosionLight		m_ExplosionLight;
	protected ParticleSource 		m_ParticleAfterBurnEnd;
	protected string				m_Color;
	protected int 					m_RemainingExplosions = GetSecondaryExplosionCount();
	protected ref array<ref FireworksLauncherClientEventBase> m_Events = new ref array<ref FireworksLauncherClientEventBase>;
	#ifdef DEVELOPER
	Shape 							m_ShotTrajectory;
	#endif
	
	
	// -----------------------------
	// ------- Tweaking Start-------
	// -----------------------------
	
	protected int GetSoundDelay()
	{
		return 0;
	}
	
	protected int GetLightDelay()
	{
		return 0;
	}
	
	protected float GetExplosionDistance()
	{
		return Math.RandomFloatInclusive(35,40);
	}
	
	protected float GetShotDispersionAngle()
	{
		return Math.RandomFloatInclusive(-25,25);//this is rotated 360 degrees around the UP vector too
	}
	
	protected string GetExplosionSoundSet()
	{
		return "FireworksLauncher_Explosion_SoundSet";
	}
	
	protected float GetExplosionDelay()
	{
		return Math.RandomFloatInclusive(1.5,1.75);
	}
	
	// -----------------------------
	// ------- Tweaking End --------
	// -----------------------------

	
	void FireworksLauncherClientEvent(FireworksLauncher item, int index)
	{
		m_Item = item;
		m_Index = index;
		
		int colorSequenceLastIndex = m_Item.GetColorSequence().Length() - 1;
		int coloreSequenceIndex = m_Index - 1;//shot index(m_Index) starts at 1
		
		if (colorSequenceLastIndex >= coloreSequenceIndex)
		{
			m_Color = m_Item.GetColorSequence().Get(coloreSequenceIndex);
		}
		else
		{
			Debug.LogError("Failed to obtain color from color sequence");
			m_Color = "B";
		}
	}
	
	void ~FireworksLauncherClientEvent()
	{
		SEffectManager.DestroyEffect(m_FireSound);
		SEffectManager.DestroyEffect(m_ExplosionSound);
		if (m_ParticleAfterBurnEnd)
		{
			m_ParticleAfterBurnEnd.StopParticle();
		}
	}

	override protected void OnFired()
	{
		m_Timer.Run(GetExplosionDelay(), this, "OnExplode", null);
		m_Item.PlaySoundSet( m_FireSound, "FireworksLauncher_Shot_SoundSet", 0, 0 );
		m_ParticleShot = ParticleManager.GetInstance().PlayInWorld(ParticleList.FIREWORKS_SHOT, GetShotPos());
		m_ParticleShot.SetYawPitchRoll(Vector(Math.RandomFloatInclusive(0,360),GetShotDispersionAngle(),0));
		
		m_ParticleAfterBurnEnd = ParticleManager.GetInstance().PlayInWorld(ParticleList.FIREWORKS_AFTERBURN_END, GetShotPos());
		if (m_ParticleAfterBurnEnd)
			m_ParticleAfterBurnEnd.SetOwner(this);
		m_ShotDir = m_ParticleShot.GetDirectionUp();
		
		#ifdef DEVELOPER
		vector pts[2];
		pts[0] = GetShotPos();
		pts[1] = GetShotPos() + m_ShotDir * GetExplosionDistance();
		//m_ShotTrajectory = Shape.CreateLines(COLOR_RED, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.NOZBUFFER, pts, 2);
		#endif
	}
	
	protected vector GetShotPos()
	{
		vector memPos; 
		string memName = "Shot_" + m_Index;
		
		if (m_Item.MemoryPointExists(memName))
		{
			memPos = m_Item.GetMemoryPointPos(memName);
			memPos = m_Item.ModelToWorld(memPos);
		}
		else
		{
			memPos = m_Item.GetPosition();//fallback to item's location
			Debug.LogError("Missing shot memory point on Fireworks");
		}
		return memPos;
	}
	
	protected void CalculateExplosionPosition()
	{
		m_ExplosionPos = GetShotPos() + m_ShotDir * GetExplosionDistance();
	}
	
	protected vector GetExplosionPosition()
	{
		if (!m_ExplosionPos)
		{
			CalculateExplosionPosition();
		}
		return m_ExplosionPos;
	}
	
	int GetSecondaryExplosionCount()
	{
		return 4;
	}
	
	
	protected void OnExplode()
	{
		#ifdef DEVELOPER
		if (m_ShotTrajectory)
		{
			m_ShotTrajectory.Destroy();
			m_ShotTrajectory = null;
		}
		#endif
		m_ParticleAfterBurnEnd.Stop();

		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayExplosionSound, GetSoundDelay(), false);
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SpawnLight, GetLightDelay(), false);
		m_ParticleExplosion = ParticleManager.GetInstance().PlayInWorld(GetExplParticleFromSequence(), GetExplosionPosition());
		
		RequestSecondaryExplosion();
		
	}
	
	protected void RequestSecondaryExplosion()
	{
		if (m_RemainingExplosions> 0)
		{
			m_RemainingExplosions--;
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SpawnSecondaryExplosion, GetSecondaryExplosionDelay(), false);
		}
	}
	
	protected void SpawnSecondaryExplosion()
	{
		FireworksLauncherClientEventSecondary evnt = new FireworksLauncherClientEventSecondary(m_Item,m_Index);
		evnt.Init(GetExplosionPosition());
		evnt.OnExplode();
		m_Events.Insert(evnt);
		RequestSecondaryExplosion();
	}
	
	protected int GetSecondaryExplosionDelay()
	{
		return Math.RandomIntInclusive(100,250);
	}
	
	protected int GetExplParticleFromSequence()
	{
		switch (m_Color)
		{
			case "R":
				return ParticleList.FIREWORKS_EXPLOSION_RED;
			case "G":
				return ParticleList.FIREWORKS_EXPLOSION_GREEN;
			case "B":
				return ParticleList.FIREWORKS_EXPLOSION_BLUE;
			case "Y":
				return ParticleList.FIREWORKS_EXPLOSION_YELLOW;
			case "P":
				return ParticleList.FIREWORKS_EXPLOSION_PINK;
			default:
				ErrorEx("Incorrect explosion particle color in the sequence");
		}
		return ParticleList.FIREWORKS_EXPLOSION_RED;
	}
	
	protected void SetupLight(PointLightBase light)
	{
		switch (m_Color)
		{
			case "R":
				light.SetDiffuseColor(255,51,51);
				light.SetAmbientColor(255,51,51);
				
				break;
			case "G":
				light.SetDiffuseColor(0,255,128);
				light.SetAmbientColor(0,255,128);
				break;
			case "B":
				light.SetDiffuseColor(51,153,255);
				light.SetAmbientColor(51,153,255);
				break;
			case "Y":
				light.SetDiffuseColor(255,255,51);
				light.SetAmbientColor(255,255,51);
				break;
			case "P":
				light.SetDiffuseColor(255,102,255);
				light.SetAmbientColor(255,102,255);
				break;
			default:
				ErrorEx("Incorrect explosion particle color in the sequence");
		}
	}
	
	protected void SpawnLight()
	{
		m_ExplosionLight = ExplosionLight.Cast( ScriptedLightBase.CreateLight( ExplosionLight, GetExplosionPosition()) );
		SetupLight(m_ExplosionLight);
	}
	
	protected void PlayExplosionSound()
	{
		if (m_FireSound)
		{
			m_FireSound.Stop();
		}
		m_Item.PlaySoundSet( m_ExplosionSound, GetExplosionSoundSet(), 0, 0 );
	}
}

class FireworksLauncherClientEventSecondary : FireworksLauncherClientEvent
{
	protected vector m_ShotPos;
	
	override protected vector GetExplosionPosition()
	{
		return GetShotPos() + m_ShotDir * GetExplosionDistance();
	}
	
	void Init(vector pos)
	{
		m_ShotPos = pos;
		m_ShotDir[0] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir[1] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir[2] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir.Normalize();
	}
	override protected vector GetShotPos()
	{
		return m_ShotPos;
	}
	
	override protected float GetExplosionDistance()
	{
		return Math.RandomFloatInclusive(10,15);
	}
	
	override void OnExplode()
	{
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayExplosionSound, GetSoundDelay(), false);
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SpawnLight, GetLightDelay(), false);
		m_ParticleExplosion = ParticleManager.GetInstance().PlayInWorld(GetExplParticleFromSequence(), GetExplosionPosition());
	}
}

class FireworksLauncher: FireworksBase
{
	int m_Index;//counts the shots
	int m_IndexPrev;//counts the shots
	int m_ColorSequenceIndex;
	ref Timer m_TimerFuse;
	ref array<ref FireworksLauncherClientEventBase> m_Events;
	EffectSound 		m_FuseSoundStart;
	EffectSound 		m_FuseSound;
	ParticleSource 		m_ParticleFuse;
	ParticleSource 		m_ParticleAfterBurnEnd;
	ref array<string>				m_ColorSequence;
	
	void FireworksLauncher()
	{
		m_ColorSequence = new array<string>();
		SetupColorSequences();
		RegisterNetSyncVariableInt("m_State", 0, EnumTools.GetLastEnumValue(EFireworksState));
		RegisterNetSyncVariableInt("m_Index", 0, GetMaxShots());
		RegisterNetSyncVariableInt("m_RandomSeed", 0, 1023);
		int lastIndex = m_ColorSequence.Count() - 1;
		RegisterNetSyncVariableInt("m_ColorSequenceIndex", 0, lastIndex);
		
		m_RandomSeed = Math.Randomize(Math.RandomInt(0,1023));
		m_ColorSequenceIndex = Math.RandomIntInclusive(0, lastIndex);
	}
	
	void ~FireworksLauncher()
	{
		SEffectManager.DestroyEffect(m_FuseSound);
		SEffectManager.DestroyEffect(m_FuseSoundStart);
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	//!how long it takes to deploy this item in seconds
	override float GetDeployTime()
	{
		return 2;
	}
	
	protected void SetupColorSequences()
	{
		m_ColorSequence.Insert("RGBYPBRGBRGBYPBRGBRGBYPBRGBPBRGBRGBY");
		m_ColorSequence.Insert("PGPYPBYPYPBYYPBRPYPBYYPBRGBPBRGRGBRB");
		m_ColorSequence.Insert("GRPBRGBRYPBYYPBRPYPBYYPBRGBPBRGRGBRB");
		m_ColorSequence.Insert("YRBGPRYPGRYBGRGRGBRBBYPYPBYRYPGRYGRP");
		m_ColorSequence.Insert("BGRYPYRPBYYPYRBGPRYPGBYPBRGBPBRGBRGB");
		m_ColorSequence.Insert("RYGRPBRGBYPBRRPBRGBBRBBYPYPRGBRGBRPY");
		m_ColorSequence.Insert("GBRGBYRGBYPBRRPBRBYRYPGPYPRGBRGBRPYG");
		m_ColorSequence.Insert("RYPBYYPBRGBYPBRGBRBGBPBRGRGBRBGRYPYR");
	}
	
	string GetColorSequence()
	{
		if (m_ColorSequence.IsValidIndex(m_ColorSequenceIndex))
		{
			return m_ColorSequence.Get(m_ColorSequenceIndex);
		}
		else
		{
			ErrorEx("Failed to obtain color sequence");
			return "RYPBYYPBRGBYPBRGBRBGBPBRGRGBRBGRYPYR";
		}
	}

	
	override protected void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		if (GetGame().IsServer())
		{
			if (GetState() == EFireworksState.DEFAULT)
			{
				SetState(EFireworksState.PLACED);
			}
		}
	}
	
	override protected float GetMaxAllowedWetness()
	{
		return GameConstants.STATE_WET;
	}
	
	
	override protected float GetEventDelay()
	{
		return Math.RandomFloatInclusive(3,3.35);
	}
	
	protected string GetFuseSoundSet()
	{
		return "FireworksLauncher_Ignition_Loop_SoundSet";
	}
	
	protected int GetMaxShots()
	{
		return 16;
	}
	
	protected float GetFuseDelay()
	{
		return 3;
	}
	
	protected string GetAmmoType()
	{
		return "Fireworks_Ammo";
	}
	
	protected int GetDamageType()
	{
		return DamageType.EXPLOSION;
	}
		
	
	override protected void OnStateChangedServer(EFireworksState currentState)
	{
		switch (currentState)
		{
			case EFireworksState.PLACED:
				break
			case EFireworksState.IGNITED:
				OnFuseIgnitedServer();
				break
			case EFireworksState.FIRING:
				HideSelection("cover");
				OnFiringStartServer();
				break
			case EFireworksState.FINISHED:
				HideSelection("cover");//when loading from storage
				break
			default: {};
		}
	}
	
	override protected void OnStateChangedClient(EFireworksState currentState)
	{
		switch (currentState)
		{
			case EFireworksState.IGNITED:
				OnFuseIgnitedClient();
				break
			case EFireworksState.PLACED:
				break
			case EFireworksState.FIRING:
				OnFiringStartClient();
				break
			default: {};
		}
	}
	
	//! Executed on Server when some item ignited this one
	override protected void OnIgnitedThis( EntityAI fire_source)
	{
		super.OnIgnitedThis(fire_source);
		if (m_Events)
		{
			m_Events.Clear();
		}
		m_Index = 0;
		
		if (m_TimerEvent)
		{
			m_TimerEvent.Stop();
		}
	}
	
	override protected bool CanPutInCargo( EntityAI parent )
	{
		return (GetState() == EFireworksState.DEFAULT || (GetState() == EFireworksState.PLACED) || (GetState() == EFireworksState.FINISHED);
	}
	
	override protected bool CanPutIntoHands( EntityAI parent )
	{
		return (GetState() == EFireworksState.DEFAULT || (GetState() == EFireworksState.PLACED) || (GetState() == EFireworksState.FINISHED);
	}
	
	protected void OnFuseIgnitedServer()
	{
		int state = EFireworksState.FIRING;
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SetState, GetFuseDelay()*1000, false, state);
	}
	
	protected void OnFuseIgnitedClient()
	{
		m_ParticleFuse = ParticleManager.GetInstance().PlayInWorld(ParticleList.FIREWORKS_FUSE, GetPosition() + "0 0.15 0");
		if (m_ParticleFuse)
			m_ParticleFuse.SetOwner(this);
		PlaySoundSet( m_FuseSoundStart, "FireworksLauncher_Ignition_Start_SoundSet", 0, 0 );
		vector fuseStart;
		vector fuseEnd;
		
		if (MemoryPointExists("Fuse_Start"))
		{
			fuseStart = GetMemoryPointPos("Fuse_Start");
			fuseStart = ModelToWorld(fuseStart);
		}
		if (MemoryPointExists("Fuse_End"))
		{
			fuseEnd = GetMemoryPointPos("Fuse_End");
			fuseEnd = ModelToWorld(fuseEnd);
		}
		
		vector fuseDir = fuseEnd - fuseStart;
		vector fuseOrientation[4];
		vector ori = fuseDir.VectorToAngles();
		m_ParticleFuse.SetOrientation(ori);
		m_ParticleFuse.SetPosition(fuseStart);

		PlaySoundSetLoop( m_FuseSound, GetFuseSoundSet(), 0, 0 );
	}
	
	protected void OnFiringStartServer()
	{
		OnEventServer(0);
	}
	
	protected void OnFiringStartClient()
	{
		if (m_ParticleFuse)
		{
			m_ParticleFuse.StopParticle(StopParticleFlags.IMMEDIATE);
		}
		if (m_FuseSound)
		{
			m_FuseSound.Stop();
		}
	}
	
	protected void OnFiringStop()
	{
		SetHealth01("","",0);
	}
	
	//!Starts event timer
	protected void RestartEventTimer()
	{
		if (!m_TimerEvent)
		{
			m_TimerEvent = new Timer();
		}
		m_TimerEvent.Run(GetEventDelay(), this, "OnEventServer", new Param1<int>(0));
	}
	
	
	override protected bool IsIgnited()
	{
		return GetState()==EFireworksState.IGNITED;
	}
	
	override protected bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return false;
	}
	
	
	
	//!Called periodically but only after the entity gets ignited
	override protected void OnEventServer(int type)
	{
		m_Index++;
		DamageSystem.ExplosionDamage(this, NULL, GetAmmoType(), GetPosition(), GetDamageType());
		
		SetSynchDirty();
		if (m_Index > GetMaxShots())
		{
			m_Index = GetMaxShots();
			m_TimerEvent = null;
			SetState(EFireworksState.FINISHED);
		}
		else
		{
			RestartEventTimer();
		}
	}
	
	protected void OnIndexChangedClient()
	{
		if (!m_Events)
		{
			m_Events = new array<ref FireworksLauncherClientEventBase>;
		}
		if (m_Index != 0 && m_State == EFireworksState.FIRING)//can only be true when restarting the device during debug calls
		{
			FireworksLauncherClientEventBase fireEvent = SpawnEvent();
			m_Events.Insert(fireEvent);
		}
	}
	
	protected FireworksLauncherClientEventBase SpawnEvent()
	{
		FireworksLauncherClientEventBase evnt = new FireworksLauncherClientEvent(this,m_Index);
		evnt.OnFired();
		return evnt;
	}
	
	override protected void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		//Print("index: " + m_Index);
		if (m_Index != m_IndexPrev)
		{
			Math.Randomize(m_RandomSeed+m_Index);
			OnIndexChangedClient();
			m_IndexPrev = m_Index;
		}
	}
	
	override protected void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_Index);
		ctx.Write(m_State);
	}
	
	
	override protected bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if (!super.OnStoreLoad(ctx, version))
		{
			return false;
		}
		
		if (version >= 130)
		{
			if (!ctx.Read(m_Index))
			{
				return false;
			}
		
			if (!ctx.Read(m_State))
			{
				return false;
			}
			
			SetState(m_State);
		}
		return true;
	}
	
	#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output;
		
		if( GetGame().IsDedicatedServer())
		{
			debug_output+= EnumTools.EnumToString(EFireworksState, m_State) +"\n";
			debug_output+= "m_Index:" + m_Index +"\n";
		}
		else
		{
			
		}
		return debug_output;
	}
	
	#endif
}