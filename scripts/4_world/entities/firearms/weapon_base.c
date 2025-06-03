/**@class		AbilityRecord
 * @brief		pair ( action, actionType )
 **/
class AbilityRecord
{
	int m_action; /// corresponds to Human::actions == RELOAD, MECHANISM, ...
	int m_actionType; /// corresponds to Human::actionTypes == CHAMBERING_ONEBULLET_CLOSED, MECHANISM_CLOSED...
	void AbilityRecord (int a, int at) { m_action = a; m_actionType = at; }
};

enum WeaponWithAmmoFlags
{
	//! Attached magazine will be full and no round will be chambered
	NONE = 0,
	//! Chambers bullets
	CHAMBER = 1,
	//! Maybe chambers bullets (sequential rng) example: 1 1 1 0 0 0
	CHAMBER_RNG = 2,
	//! Maybe chambers bullets (full random) example: 0 1 0 0 1 1 
	CHAMBER_RNG_SPORADIC = 4,
	//! Randomizes the quantity of the bullets in the spawned magazine
	QUANTITY_RNG = 8,
	//! Fully randomizes the ammo type instead of picking one random for the entire mag (needs to have type as empty string)
	AMMO_MAG_RNG = 16,
	//! Fully randomizes the ammo type instead of picking one random for all chambers (needs to have type as empty string)
	AMMO_CHAMBER_RNG = 32,
	//! Instead of randomizing when type is empty, it looks for the one which has the highest capacity
	MAX_CAPACITY_MAG = 64,
}

typedef FSMTransition<WeaponStateBase, WeaponEventBase, WeaponActionBase, WeaponGuardBase> WeaponTransition; /// shorthand

/**@class		Weapon_Base
 * @brief		script base for all weapons
 *
 * @NOTE: this class is bound to core-config "Weapon_Base" config class
 **/
class Weapon_Base extends Weapon
{
	//! Full highest capacity magazine + chambered round
	const int SAMF_DEFAULT = WeaponWithAmmoFlags.CHAMBER | WeaponWithAmmoFlags.MAX_CAPACITY_MAG;
	//! Random bullet quantity + maybe chambered round
	const int SAMF_RNG = WeaponWithAmmoFlags.CHAMBER_RNG | WeaponWithAmmoFlags.QUANTITY_RNG;
	//! Validation on client side delay to have time properly synchronize attachments needed for check
	const float VALIDATE_DELAY = 5.0;
	
	protected const float DEFAULT_DAMAGE_ON_SHOT = 0.05;
	protected ref array<ref AbilityRecord> m_abilities = new array<ref AbilityRecord>;		/// weapon abilities
	protected ref WeaponFSM m_fsm;	/// weapon state machine
	protected bool m_isJammed = false;
	protected bool m_LiftWeapon = false;
	protected bool m_BayonetAttached;
	protected bool m_ButtstockAttached;
	protected bool m_Charged = false;
	protected bool m_WeaponOpen = false;
	protected bool m_WasIronSight;
	protected int m_BurstCount;
	protected int m_BayonetAttachmentIdx;
	protected int m_ButtstockAttachmentIdx;
	protected int m_weaponAnimState = -1; /// animation state the weapon is in, -1 == uninitialized
	protected int m_magazineSimpleSelectionIndex = -1;
	protected int m_weaponHideBarrelIdx = -1; //index in simpleHiddenSelections cfg array
	protected float m_DmgPerShot = 0; //default is set to zero, since C++ solution has been implemented. See 'damageBarrel' and 'barrelArmor' in configs.
	protected float m_WeaponLength;
	protected float m_WeaponLiftCheckVerticalOffset;
	protected float m_ShoulderDistance;
	protected float m_ObstructionDistance;
	protected vector m_LastLiftPosition;
	protected int m_LastLiftHit;
	ref array<int> m_bulletSelectionIndex = new array<int>;
	ref array<float> m_DOFProperties;
	ref array<float> m_ChanceToJam = new array<float>;
	protected float m_ChanceToJamSync = 0;
	protected ref PropertyModifiers m_PropertyModifierObject;
	protected PhxInteractionLayers hit_mask = PhxInteractionLayers.CHARACTER | PhxInteractionLayers.BUILDING | PhxInteractionLayers.DOOR | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ITEM_SMALL | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.FENCE | PhxInteractionLayers.AI;
	protected ref Timer m_DelayedValidationTimer;
	private float m_coolDownTime = 0;
	
	void Weapon_Base()
	{
		//m_DmgPerShot		= ConfigGetFloat("damagePerShot");
		m_BayonetAttached 	= false;
		m_ButtstockAttached = false;
		m_WasIronSight = true;	// initially uses ironsights by default
		m_BayonetAttachmentIdx = -1;
		m_ButtstockAttachmentIdx = -1;
		m_BurstCount = 0;
		m_DOFProperties = new array<float>;
		if (GetGame().IsClient())
		{
			m_DelayedValidationTimer = new Timer();
		}
		
		if ( ConfigIsExisting("simpleHiddenSelections") )
		{
			TStringArray selectionNames = new TStringArray;
			ConfigGetTextArray("simpleHiddenSelections",selectionNames);
			m_weaponHideBarrelIdx = selectionNames.Find("hide_barrel");
			m_magazineSimpleSelectionIndex = selectionNames.Find("magazine");
			
			int bulletIndex = selectionNames.Find("bullet");
			if ( bulletIndex != -1 )
			{ 
				m_bulletSelectionIndex.Insert(bulletIndex);
			
				for (int i = 2; i < 100; i++)
				{
					bulletIndex = selectionNames.Find(string.Format("bullet%1",i));
					if (bulletIndex != -1)
					{
						m_bulletSelectionIndex.Insert(bulletIndex);
					}
					else
					{
						break;
					}
				}
			}
		}
		
		InitWeaponLength();
		InitWeaponLiftCheckVerticalOffset();
		InitShoulderDistance();
		InitObstructionDistance();
		InitDOFProperties(m_DOFProperties);
		if (GetGame().IsServer())
		{
			InitReliability(m_ChanceToJam);
		}
		InitStateMachine();
	}

	void InitStateMachine() { }
	
	override void EEInit()
	{
		super.EEInit();
		
		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( AssembleGun );
		}
	}
	
	void SetInitialState(WeaponStableState initState)
	{
		m_fsm.SetInitialState(initState);
		SetCharged(!initState.IsDischarged());
		SetWeaponOpen(initState.IsWeaponOpen());
		SetGroundAnimFrameIndex(initState.m_animState);
	}
	
	bool IsCharged()
	{
		return m_Charged;
	}
	
	void SetCharged(bool value)
	{
		m_Charged = value;
	}	
	
	bool IsWeaponOpen()
	{
		return m_WeaponOpen;
	}
	
	void SetWeaponOpen(bool value)
	{
		m_WeaponOpen = value;
	}

	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float baseWeight = GetInventoryAndCargoWeight(forceRecalc);
		float ammoWeight;
		float ammoDamage;
		string bulletTypeName, ammoTypeName;
		
		int muzzleCount = GetMuzzleCount();
		#ifdef DEVELOPER
		if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
		{
			WeightDebugData data1 = WeightDebug.GetWeightDebug(this);
			data1.SetCalcDetails("TWPN: " + m_ConfigWeight+"(item weight) + " + baseWeight +"(contents weight)" );
		}
		#endif
		for (int muzzleIndex = 0; muzzleIndex < muzzleCount; muzzleIndex++)
		{
			//chamber weight
			if (!IsChamberEmpty(muzzleIndex))
			{
				ammoTypeName = GetChamberAmmoTypeName(muzzleIndex);
				ammoWeight += g_Game.ConfigGetFloat(string.Format("CfgMagazines %1 weight", ammoTypeName));
				
				#ifdef DEVELOPER
				if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
				{
					WeightDebugData data2 = WeightDebug.GetWeightDebug(this);
					data2.AddCalcDetails( g_Game.ConfigGetFloat("CfgMagazines " + ammoTypeName + " weight").ToString() +"(chamber weight)");
				}
				#endif
			}
			
			//correctly calculates internal magazine weight based on the ammo type of each bullet
			if (HasInternalMagazine(muzzleIndex))
			{
				#ifdef DEVELOPER
				float debugInternalMagWeight;
				#endif
				int cartridgeCount = GetInternalMagazineCartridgeCount(muzzleIndex);
				for (int cartridgeIndex = 0; cartridgeIndex < cartridgeCount; cartridgeIndex++)
				{
					GetInternalMagazineCartridgeInfo(muzzleIndex, cartridgeIndex, ammoDamage, bulletTypeName);
					ammoWeight += Ammunition_Base.GetAmmoWeightByBulletType(bulletTypeName);
					#ifdef DEVELOPER
					debugInternalMagWeight += g_Game.ConfigGetFloat("CfgMagazines " + ammoTypeName + " weight");
					#endif
				}
				#ifdef DEVELOPER

				if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
				{
					WeightDebugData data3 = WeightDebug.GetWeightDebug(this);
					data3.AddCalcDetails(debugInternalMagWeight.ToString()+ "(internal mag weight)");
				}
				#endif
			}
			
		}
		return ammoWeight + baseWeight + GetConfigWeightModified();
	}
	//! override on weapons with some assembly required
	void AssembleGun();

	bool CanProcessAction(int action, int actionType)
	{
		return false; // @TODO
	}
	/**@fn		HasActionAbility
	 * @brief	query if weapon supports action and actionType
	 * @param[in]	action	\p	one of Human.actions (i.e. RELOAD, MECHANISM, ...)
	 * @param[in]	actionType	\p	one of Human.actionTypes (i.e. CHAMBERING_ONEBULLET_CLOSED, MECHANISM_CLOSED...)
	 * @return	true if weapon supports operation
	 **/
	bool HasActionAbility(int action, int actionType)
	{
		int count = GetAbilityCount();
		for (int i = 0; i < count; ++i)
		{
			AbilityRecord rec = GetAbility(i);
			if (rec.m_action == action && rec.m_actionType == actionType)
				return true;
		}
		return false;
	}
	/**@fn		GetAbilityCount
	 * @return number of stored abilities
	 **/
	int GetAbilityCount() { return m_abilities.Count(); }
	/**@fn		GetAbility
	 * @param[in]	index	\p	index into m_abilities storage
	 * @return ability record
	 **/
	AbilityRecord GetAbility(int index) { return m_abilities.Get(index); }

	/**@fn		CanProcessWeaponEvents
	 * @return	true if weapon has running fsm
	 **/
	bool CanProcessWeaponEvents() { return m_fsm && m_fsm.IsRunning(); }

	/**@fn		GetCurrentState
	 * @brief		returns currently active state
	 * @return	current state the FSM is in (or NULL)
	 **/
	WeaponStateBase GetCurrentState() { return m_fsm.GetCurrentState(); }

	/**@fn		IsWaitingForActionFinish
	 * @brief	returns true if state machine started playing action/actionType and waits for finish
	 **/
	bool IsWaitingForActionFinish()
	{
		return CanProcessWeaponEvents() && GetCurrentState().IsWaitingForActionFinish();
	}

	bool IsIdle()
	{
		return CanProcessWeaponEvents() && GetCurrentState().IsIdle();
	}

	/**@fn	ProcessWeaponEvent
	 * @brief	weapon's fsm handling of events
	 * @NOTE: warning: ProcessWeaponEvent can be called only within DayZPlayer::HandleWeapons (or ::CommandHandler)
	 **/
	bool ProcessWeaponEvent(WeaponEventBase e)
	{
		SyncEventToRemote(e);
		
		// @NOTE: synchronous events not handled by fsm
		if (e.GetEventID() == WeaponEventID.SET_NEXT_MUZZLE_MODE)
		{
			SetNextWeaponMode(GetCurrentMuzzle());
			return true;
		}

		if (m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
			return true;

		//if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("FSM refused to process event (no transition): src=" + GetCurrentState().ToString() + " event=" + e.ToString()); }
		return false;
	}
	/**@fn	ProcessWeaponAbortEvent
	 * @NOTE: warning: ProcessWeaponEvent can be called only within DayZPlayer::HandleWeapons (or ::CommandHandler)
	 **/
	bool ProcessWeaponAbortEvent(WeaponEventBase e)
	{
		SyncEventToRemote(e);
		
		ProcessEventResult aa;
		m_fsm.ProcessAbortEvent(e, aa);
		return aa == ProcessEventResult.FSM_OK;
	}

	bool CanChamberBullet(int muzzleIndex, Magazine mag)
	{
		return CanChamberFromMag(muzzleIndex, mag) && (!IsChamberFull(muzzleIndex) || IsChamberFiredOut(muzzleIndex) || !IsInternalMagazineFull(muzzleIndex));
	}

	void SetWeaponAnimState(int state)
	{
		m_weaponAnimState = state;
		SetGroundAnimFrameIndex(state);
	}
	void ResetWeaponAnimState()
	{
		if (LogManager.IsWeaponLogEnable()) fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(this) + " resetting anim state: " + typename.EnumToString(PistolAnimState, m_weaponAnimState) + " --> " + typename.EnumToString(PistolAnimState, -1));
		m_weaponAnimState = -1;
	}
	int GetWeaponAnimState() { return m_weaponAnimState; }

	void EEFired(int muzzleType, int mode, string ammoType)
	{		
		if ( !GetGame().IsDedicatedServer() )
		{
			ItemBase suppressor = GetAttachedSuppressor();
			
			// Muzzle flash & overheating effects
			ItemBase.PlayFireParticles(this, muzzleType, ammoType, this, suppressor, "CfgWeapons" );
			IncreaseOverheating(this, ammoType, this, suppressor, "CfgWeapons");
			
			if (suppressor)
			{
				ItemBase.PlayFireParticles(this, muzzleType, ammoType, suppressor, NULL, "CfgVehicles" );
				suppressor.IncreaseOverheating(this, ammoType, this, suppressor, "CfgVehicles");
			}
		}
		
		//obsolete, replaced by C++ solution!
/*
		if (GetGame().IsServer())
		{
			AddHealth("","Health",-m_DmgPerShot); //damages weapon
			if (suppressor)
				suppressor.AddHealth("","Health",-m_DmgPerShot); //damages suppressor; TODO add suppressor damage coeficient/parameter (?) to suppressors/weapons (?)
		}
*/		
		//JamCheck(muzzleType);
		
		#ifdef DIAG_DEVELOPER
		MiscGameplayFunctions.UnlimitedAmmoDebugCheck(this);
		#endif
	}
	
	bool JamCheck(int muzzleIndex )
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if ( player )
		{
			float rnd = player.GetRandomGeneratorSyncManager().GetRandom01(RandomGeneratorSyncUsage.RGSJam);
			//Print("Random Jam - " + rnd);
			if (rnd < GetSyncChanceToJam())
				return true;
		}
		return false;
	}
	
	void ShowBullet(int muzzleIndex)
	{
		if ( m_bulletSelectionIndex.Count() > muzzleIndex )
		{
			SetSimpleHiddenSelectionState(m_bulletSelectionIndex[muzzleIndex],1);
		}
		else
			SelectionBulletShow();
	}
	
	void HideBullet(int muzzleIndex)
	{
		if ( m_bulletSelectionIndex.Count() > muzzleIndex )
		{
			SetSimpleHiddenSelectionState(m_bulletSelectionIndex[muzzleIndex],0);
		}
		else
			SelectionBulletHide();
	}
	
	bool IsJammed() { return m_isJammed; }
	bool CanEjectBullet() {return true;}
	void SetJammed(bool value) { m_isJammed = value; }
	float GetSyncChanceToJam() { return m_ChanceToJamSync; }
	float GetChanceToJam()
	{
		int level = GetHealthLevel();
	
		if (level >= 0 && level < m_ChanceToJam.Count())
			return m_ChanceToJam[level];
		else
			return 0.0;
	}
	
	void SyncSelectionState(bool has_bullet, bool has_mag)
	{
		if (has_bullet)
		{	
			string chamberedAmmoTypeName;
			float chamberedAmmoDmg;
			
			if ( GetCartridgeInfo(0, chamberedAmmoDmg, chamberedAmmoTypeName) )
			{
				EffectBulletShow(0, chamberedAmmoDmg, chamberedAmmoTypeName);
			}
			//ShowBullet(0);
			SelectionBulletShow();
		}
		else
		{	
			//HideBullet(0);
			SelectionBulletHide();
			EffectBulletHide(0);
		}

		if (has_mag)
			ShowMagazine();
		else
			HideMagazine();
	}
	
	/*override void EEOnAfterLoad()
	{
		super.EEOnAfterLoad();
		
		string chamberedAmmoTypeName;
		float chamberedAmmoDmg;

		if ( GetCartridgeInfo(0, chamberedAmmoDmg, chamberedAmmoTypeName) )
		{
			EffectBulletShow(0, chamberedAmmoDmg, chamberedAmmoTypeName);
		}
		
	}*/
	
	void ForceSyncSelectionState()
	{
		int nMuzzles = GetMuzzleCount();
		for (int i = 0; i < nMuzzles; ++i)
		{
			if (IsChamberFull(i))
			{
				ShowBullet(i);
				float damage;
				string ammoTypeName;
				GetCartridgeInfo(i, damage, ammoTypeName);
				EffectBulletShow(i, damage, ammoTypeName);
			}
			else
			{
				HideBullet(i);
				EffectBulletHide(i);
			}
			
			Magazine mag = GetMagazine(i);
			if (mag)
				ShowMagazine();
			else
				HideMagazine();
		}
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		
		if (version >= 113)
		{
			int current_muzzle = 0;
			if (!ctx.Read(current_muzzle))
			{
				Error("Weapon.OnStoreLoad " + this + " cannot read current muzzle!");
				return false;
			}
			
			if (current_muzzle >= GetMuzzleCount() || current_muzzle < 0)
				Error("Weapon.OnStoreLoad " + this + " trying to set muzzle index " + current_muzzle + " while it only has " + GetMuzzleCount() + " muzzles!");
			else
				SetCurrentMuzzle(current_muzzle);
		}		
		
		if (version >= 105)
		{
			int mode_count = 0;
			if (!ctx.Read(mode_count))
			{
				Error("Weapon.OnStoreLoad " + this + " cannot read mode count!");
				return false;
			}
				
			for (int m = 0; m < mode_count; ++m)
			{
				int mode = 0;
				if (!ctx.Read(mode))
				{
					Error("Weapon.OnStoreLoad " + this + " cannot read mode[" + m + "]");
					return false;
				}
				
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " OnStoreLoad - loaded muzzle[" + m + "].mode = " + mode); }
				SetCurrentMode(m, mode);
			}
		}
		
		if ( version >= 106 )
		{
			if ( !ctx.Read(m_isJammed) )
			{
				Error("Weapon.OnStoreLoad cannot load jamming state");
				return false;
			}
		}

		if (m_fsm)
		{
			if (!m_fsm.OnStoreLoad(ctx, version))
				return false;
			
			WeaponStableState wss = WeaponStableState.Cast(m_fsm.GetCurrentState());
			if (wss)
			{
				SetGroundAnimFrameIndex(wss.m_animState);
			}
			
		}
		else
		{
			int dummy = 0;
			if (!ctx.Read(dummy))
				return false;
		}

		return true;
	}

	void SaveCurrentFSMState(ParamsWriteContext ctx)
	{
		if (m_fsm && m_fsm.IsRunning())
		{
			if (m_fsm.SaveCurrentFSMState(ctx))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " state saved."); }
			}
			else
				Error("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " state NOT saved.");
		}
		else
			Error("[wpnfsm] " + Object.GetDebugName(this) + " Weapon.SaveCurrentFSMState: trying to save weapon without FSM (or uninitialized weapon) this=" + this + " type=" + GetType());
	}

	bool LoadCurrentFSMState(ParamsReadContext ctx, int version)
	{
		if (m_fsm)
		{
			if (m_fsm.LoadCurrentFSMState(ctx, version))
			{
				WeaponStableState state = WeaponStableState.Cast(GetCurrentState());
				if (state)
				{
					SyncSelectionState(state.HasBullet(), state.HasMagazine());
					state.SyncAnimState();
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " stable state loaded and synced."); }
					return true;
				}
				else
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " unstable/error state loaded."); }
					return false;
				}
			}
			else
			{
				Error("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " did not load.");
				return false;
			}
		}
		else
		{
			Error("[wpnfsm] " + Object.GetDebugName(this) + " Weapon.LoadCurrentFSMState: trying to load weapon without FSM (or uninitialized weapon) this=" + this + " type=" + GetType());
			return false;
		}
	}

	override void AfterStoreLoad()
	{
		if (m_fsm)
		{
			int mi = GetCurrentMuzzle();
			Magazine mag = GetMagazine(mi);
			bool has_mag = mag != null;
			bool has_bullet = !IsChamberEmpty(mi);
			SyncSelectionState(has_bullet, has_mag);
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		// current muzzle added in version 113
		int current_muzzle = GetCurrentMuzzle();
		ctx.Write(current_muzzle);
		
		// fire mode added in version 105
		int mode_count = GetMuzzleCount();
		ctx.Write(mode_count);
		for (int m = 0; m < mode_count; ++m)
			ctx.Write(GetCurrentMode(m));
		
		ctx.Write(m_isJammed);

		if (m_fsm)
			m_fsm.OnStoreSave(ctx);
		else
		{
			int dummy = 0;
			ctx.Write(dummy);
		}
	}

	/**@fn		GetCurrentStateID
	 * @brief	tries to return identifier of current state
	 **/
	int GetInternalStateID()
	{
		if (m_fsm)
			return m_fsm.GetInternalStateID();
		return 0;
	}
	
	/**@fn		GetCurrentStableStateID
	 * @brief	tries to return identifier of current stable state (or nearest stable state if unstable state is currently running)
	 **/
	int GetCurrentStableStateID()
	{
		if (m_fsm)
		{
			return m_fsm.GetCurrentStableStateID();
		}
		return 0;
	}

	/**@fn		RandomizeFSMState
	 * @brief	With the parameters given, selects a random suitable state for the FSM of the weapon
	 * @WARNING:	Weapon_Base.Synchronize call might be needed, if this method is called while clients are connected
	 **/
	void RandomizeFSMState()
	{
		if (m_fsm)
		{
			int mi = GetCurrentMuzzle();
			Magazine mag = GetMagazine(mi);
			bool has_mag = mag != null;
			bool has_bullet = !IsChamberEmpty(mi);
			bool has_jam = IsJammed();
			array<MuzzleState> muzzleStates = GetMuzzleStates();
			m_fsm.RandomizeFSMStateEx(muzzleStates, has_mag, has_jam);
			ForceSyncSelectionState();
		}
	}

	//! Helper method for RandomizeFSMState
	protected array<MuzzleState> GetMuzzleStates()
	{
		array<MuzzleState> muzzleStates = new array<MuzzleState>;
		
		int nMuzzles = GetMuzzleCount();
		for (int i = 0; i < nMuzzles; ++i)
		{
			MuzzleState state = MuzzleState.U;
			if (IsChamberFiredOut(i))
				state = MuzzleState.F;
			else if (IsChamberFull(i))
				state = MuzzleState.L;
			else if (IsChamberEmpty(i))
				state = MuzzleState.E;
			else
				ErrorEx(string.Format("Unable to identify chamber state of muzzle %1", i));
			
			muzzleStates.Insert(state);
		}
		
		return muzzleStates;
	}
	
	/** \name Weapon With Ammo
	*	Helpers for spawning ammo/magazine in weapon
	*	For the flags, either a combination of WeaponWithAmmoFlags can be used
	*	Or one of the preset 'const int' with 'SAMF_' prefix (SAMF_DEFAULT, SAMF_RNG)
	*/
	//@{

	/**@fn		CreateWeaponWithAmmo
	 * @brief	Create weapon with ammo
	 * @param[in]	weaponType		\p string	The weapon to create
	 * @param[in]	magazineType	\p string	The magazine to attach or ammo to load, passing in empty string will select random
	 * @param[in]	flags			\p int		Setup flags, please read WeaponWithAmmoFlags
	 * @return	The created weapon
	 **/
	static Weapon_Base CreateWeaponWithAmmo( string weaponType, string magazineType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
    {   
        Weapon_Base wpn = Weapon_Base.Cast(GetGame().CreateObjectEx( weaponType, vector.Zero, ECE_PLACE_ON_SURFACE ));
    
        if ( !wpn )
		{
			ErrorEx(string.Format("%1 does not exist or is not a weapon.", weaponType));
            return null;
		}
    
		wpn.SpawnAmmo(magazineType, flags);		
		return wpn;
    }
	
	/**@fn		SpawnAmmo
	 * @brief	General method trying to attch magazine, fill inner magazine and fill chamber
	 * @param[in]	magazineType	\p string	The magazine to attach or ammo to load, passing in empty string will select random
	 * @param[in]	flags			\p int		Setup flags, please read WeaponWithAmmoFlags
	 * @return	whether anything was spawned or done
	 **/
	bool SpawnAmmo( string magazineType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
	{
		// Attempt internal mag
		if ( HasInternalMagazine(-1) && FillInnerMagazine(magazineType, flags) )
			return true;
		
		// Attempt mag attachment
		if ( GetMagazineTypeCount(0) > 0 && SpawnAttachedMagazine(magazineType, flags) )
			return true;
		
		// Attempt chamber
		if ( FillChamber(magazineType, flags) )
			return true;
		
		return false;
	}
	
	/**@fn		SpawnAttachedMagazine
	 * @brief	Try to spawn and attach a magazine
	 * @param[in]	magazineType	\p string	The magazine to attach, passing in empty string will select random
	 * @param[in]	flags			\p int		Setup flags, please read WeaponWithAmmoFlags
	 * @return	The created magazine or null
	 **/
	Magazine SpawnAttachedMagazine( string magazineType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
	{
		// Check if the gun has any magazines registered in config
		if ( GetMagazineTypeCount(0) == 0 )
		{
			ErrorEx(string.Format("No 'magazines' config entry for %1.", this));
			return null;
		}
		
		// Randomize when no specific one is given
		if ( magazineType == "" )
		{
			if ( flags & WeaponWithAmmoFlags.MAX_CAPACITY_MAG)
				magazineType = GetMaxMagazineTypeName(0);
			else
				magazineType = GetRandomMagazineTypeName(0);
		}
		
		EntityAI magAI = GetInventory().CreateAttachment(magazineType);
		if (!magAI)
		{
			ErrorEx(string.Format("Failed to create and attach %1 to %2", GetDebugName(magAI), this));
			return null;
		}
		
		Magazine mag;
		if (!CastTo(mag, magAI))
		{
			ErrorEx(string.Format("Expected magazine, created: %1", GetDebugName(magAI)));
			return null;
		}
		
		// Decide random quantity when enabled
		if (flags & WeaponWithAmmoFlags.QUANTITY_RNG)
			mag.ServerSetAmmoCount(Math.RandomIntInclusive(0, mag.GetAmmoMax()));
			
		if(MustBeChambered(0))
		{
			string bulletType;
			float dmg;
			if(mag.ServerAcquireCartridge(dmg,bulletType))
			{
				FillChamber(bulletType, flags);
			}
		}
		
		// Fill chamber when flagged
		bool chamberRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER) || chamberRng;		
		if (chamber || chamberRng)
		{
			FillChamber(magazineType, flags);
		}

		// FSM cares about magazine state
		RandomizeFSMState();
		Synchronize();

		return mag;
	}

	/**@fn		FillInnerMagazine
	 * @brief	Try to fill the inner magazine
	 * @param[in]	ammoType		\p string	The ammo to load, passing in empty string will select random
	 * @note 		It is best to fill in the actual 'ammo', as in the ones usually prefixed by 'Bullet_', to skip searching for it
	 * @param[in]	flags			\p int		Setup flags, please read WeaponWithAmmoFlags
	 * @return	Whether any ammo was added to the gun or not
	 **/
	bool FillInnerMagazine( string ammoType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
	{
		// Don't try to fill it when there are none
		if (!HasInternalMagazine(-1))
			return false;
	
		// Make sure the given ammoType is actually useable
		if (ammoType != "")
		{
			if (!AmmoTypesAPI.MagazineTypeToAmmoType(ammoType, ammoType))
				return false;
		}
		
		
		bool didSomething = false;
		bool needUpdateStateMachine = false;
		int muzzCount = GetMuzzleCount();
		
		bool ammoRng = ammoType == "";
		bool ammoFullRng = ammoRng && (flags & WeaponWithAmmoFlags.AMMO_MAG_RNG);
		
		// No full RNG flag, so pick one random and use only this one
		if (ammoRng && !ammoFullRng)
			ammoType = GetRandomChamberableAmmoTypeName(0);
		
		// Fill the internal magazine
		for (int i = 0; i < muzzCount; ++i)
		{
			bool loadAnyBullet = false;	
			int ammoCount = GetInternalMagazineMaxCartridgeCount(i);
			
			// Decide random quantity when enabled
			if ( flags & WeaponWithAmmoFlags.QUANTITY_RNG )
				ammoCount = Math.RandomIntInclusive(0, ammoCount);
			
			// Only do the things when there is actually ammo to fill
			if (ammoCount > 0)
			{
				// Push in the cartridges
				for (int j = 0; j < ammoCount; ++j)
				{
					// Full random, decide a new one for every cartridge
					if ( ammoFullRng )
						ammoType = GetRandomChamberableAmmoTypeName(i);
					
					PushCartridgeToInternalMagazine(i, 0, ammoType);
					loadAnyBullet = true;
					didSomething = true;
				}
					
				if (loadAnyBullet && MustBeChambered(i))
				{
					if ( ammoFullRng )
						ammoType = GetRandomChamberableAmmoTypeName(i);
						
					if (FillSpecificChamber(i, 0, ammoType))
						needUpdateStateMachine = true;
					
				}
			}
		}
		
		// Call the chamber method if asked for
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER) || (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		if (chamber && FillChamber(ammoType, flags))
		{
			didSomething = true;
		}
			
		// Only fix the FSM and Synchronize when absolutely needed
		if (!didSomething)
			return false;
		
		if( needUpdateStateMachine )
		{
			// FSM cares about chamber state
			RandomizeFSMState();		
			Synchronize();
		}
		
		return true;
	}
	
	/**@fn		FillChamber
	 * @brief	Try to fill the chamber
	 * @param[in]	ammoType		\p string	The ammo to load, passing in empty string will select random
	 * @note 		It is best to fill in the actual 'ammo', as in the ones usually prefixed by 'Bullet_', to skip searching for it
	 * @param[in]	flags			\p int		Setup flags, please read WeaponWithAmmoFlags
	 * @return	Whether any chamber was filled
	 **/
	bool FillChamber( string ammoType = "", int flags = WeaponWithAmmoFlags.CHAMBER )
	{
		bool didSomething = false;
		bool chamberFullRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG_SPORADIC);
		bool chamberRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER);
			
			
		if (chamber || chamberFullRng)
		{
			// Make sure the given ammoType is actually useable
			if (ammoType != "")
			{
				if (!AmmoTypesAPI.MagazineTypeToAmmoType(ammoType, ammoType))
					return false;
			}
			else if (!(flags & WeaponWithAmmoFlags.AMMO_CHAMBER_RNG))
			{
				ammoType = GetRandomChamberableAmmoTypeName(0);
			}

			// fill chambers 
			int muzzCount = GetMuzzleCount();
			int amountToChamber = muzzCount;
					
			if (chamberRng)
				amountToChamber = Math.RandomIntInclusive(0, muzzCount);
			
		
			for (int m = 0; m < muzzCount; ++m)
			{
				if (chamberFullRng)
					chamber = Math.RandomIntInclusive(0, 1);
					
				if (chamber)
				{
					if (FillSpecificChamber(m))
					{
						didSomething = true;
						amountToChamber--;	
						if (amountToChamber <= 0)
							break;
					}
				}
			}
		}
			
		if (!didSomething)
			return false;
			
		// FSM cares about chamber state
		RandomizeFSMState();		
		Synchronize();
		
		return true;
	}
		
	bool FillSpecificChamber(int muzzleIndex, float dmg = 0, string ammoType = "")
	{
		if(!IsChamberEmpty(muzzleIndex))
			return false;
		
		if (ammoType == "")
		{
			ammoType = GetRandomChamberableAmmoTypeName(muzzleIndex);
		}
		else
		{
			if (!AmmoTypesAPI.MagazineTypeToAmmoType(ammoType, ammoType))
				return false;
		}

		return PushCartridgeToChamber(muzzleIndex, dmg, ammoType);
	}
	
	//@}
	
	
	
	/**
	 * @brief Returns number of slots for attachments corrected for weapons
	 **/
	override int GetSlotsCountCorrect()
	{
		int ac = GetInventory().AttachmentCount();
		int	sc = GetInventory().GetAttachmentSlotsCount() + GetMuzzleCount();
		if (ac > sc) sc = ac; // fix of some weapons which has 1 attachments but 0 slots...
		return sc;
	};

	PropertyModifiers GetPropertyModifierObject() 
	{
		if (!m_PropertyModifierObject)
		{
			m_PropertyModifierObject = new PropertyModifiers(this);
		}
		return m_PropertyModifierObject;
	}
	
	void OnFire(int muzzle_index)
	{
/*
		array<Man> players();
		GetGame().GetPlayers(players);
		
		Man root = GetHierarchyRootPlayer();
		
		if (!root)
		{
			return;
		}
		
		vector safePosition = root.GetPosition() + (root.GetDirection() * "0 1 0" * 3.0);
		
		Man other = null;
		foreach (auto player : players)
		{
			if (player != GetHierarchyRootPlayer())
			{
				player.SetPosition(safePosition);
			}
		}
*/

		m_BurstCount++;
	}
	
	void OnFireModeChange(int fireMode)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			EffectSound eff;
			
			if ( fireMode == 0 )
				eff = SEffectManager.PlaySound("Fire_Mode_Switch_Marked_Click_SoundSet", GetPosition());
			else
				eff = SEffectManager.PlaySound("Fire_Mode_Switch_Simple_Click_SoundSet", GetPosition());
			
			eff.SetAutodestroy(true);
		}
		
		ResetBurstCount();
	}
	
	void DelayedValidateAndRepair()
	{
		if (m_DelayedValidationTimer)
		{
			m_DelayedValidationTimer.Run(VALIDATE_DELAY , this, "ValidateAndRepair");
		}
		else
		{
			Error("[wpn] Weapon_Base::DelayedValidateAndRepair m_DelayedValidationTimer not initialized.");
			ValidateAndRepair();
		}
	}
	
	void ValidateAndRepair()
	{
		if ( m_fsm )
			m_fsm.ValidateAndRepair();
	}
	
	override void OnInventoryEnter(Man player)
	{
		m_PropertyModifierObject = null;

		if (GetGame().IsServer())
		{
		    // The server knows about all of its attachments
		    ValidateAndRepair();
		}
		else
		{
		    // The client doesn't know it has attachments yet... give it a moment
		    DelayedValidateAndRepair();
		}
		
		super.OnInventoryEnter(player);
	}
	
	override void OnInventoryExit(Man player)
	{
		m_PropertyModifierObject = null;
		super.OnInventoryExit(player);
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		GetPropertyModifierObject().UpdateModifiers();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		GetPropertyModifierObject().UpdateModifiers();
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		if (newLoc.GetType() == InventoryLocationType.HANDS)
		{
			PlayerBase player;
			if (newLoc.GetParent() && PlayerBase.CastTo(player, newLoc.GetParent()))
			{ 
				HumanCommandMove cm = player.GetCommand_Move();
				if (cm)
				{
					cm.SetMeleeBlock(false);
				}
			}
		}
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		/*
		// TODO(kumarjac):	Solve this in code instead, OnItemLocationChanged is called too late. 
		//					Maybe extend an option for items to specify what attachments they must
		//					be synchronized with? Moving to 'DelayedValidateAndRepair' works for now.
		int muzzles = GetMuzzleCount();
		for (int muzzleIdx = 0; muzzleIdx < muzzles; muzzleIdx++)
		{
			Magazine mag = GetMagazine(muzzleIdx);
			Print(mag);
			if (!mag)
				continue;
				
			Print(mag.GetInventory());
			if (!mag.GetInventory())
				continue;
				
			InventoryLocation invLoc = new InventoryLocation();
			mag.GetInventory().GetCurrentInventoryLocation(invLoc);
				
			GetGame().AddInventoryJunctureEx(null, this, invLoc, true, 1000);
		}
		*/
			
		super.OnItemLocationChanged(old_owner,new_owner);
		
		// "resets" optics memory on optics
		PlayerBase player;
		if (PlayerBase.CastTo(player,old_owner))
		{ 
			player.SetReturnToOptics(false);
			
			//optics item state reset
			ItemOptics optics;
			if (Class.CastTo(optics,GetAttachedOptics()))
			{
				player.SwitchOptics(optics,false);
			}
		}
		
		if (old_owner != new_owner && PlayerBase.Cast(new_owner))
			SetWasIronSight(true);	// reset ironsight/optic default

		HideWeaponBarrel(false);
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if ( !super.CanReleaseAttachment( attachment ) )
			return false;
		Magazine mag = Magazine.Cast(attachment);
		if (mag)
		{
			PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
			if ( player )
			{
				if ( player.GetItemInHands() == this )
					return true;
			}
			return false;
		}

		return true;
	}
	
	override bool CanRemoveFromHands(EntityAI parent)
	{
		if (IsIdle())
		{
			return true;
		}
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " Weapon=" + this + " not in stable state=" + GetCurrentState().Type()); }
		return false; // do not allow removal of weapon while weapon is busy
	}

	bool IsRemoteWeapon()
	{
		InventoryLocation il = new InventoryLocation;
		if (GetInventory().GetCurrentInventoryLocation(il))
		{
			EntityAI parent = il.GetParent();
			DayZPlayer dayzp = DayZPlayer.Cast(parent);
			if (il.GetType() == InventoryLocationType.HANDS && dayzp)
			{
				bool remote = dayzp.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_REMOTE;
				return remote;
			}
		}
		return true;
	}

	void SyncEventToRemote(WeaponEventBase e)
	{
		DayZPlayer p = DayZPlayer.Cast(GetHierarchyParent());
		if (p && p.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData();

			ctx.Write(INPUT_UDT_WEAPON_REMOTE_EVENT);
			e.WriteToContext(ctx);

			if (LogManager.IsWeaponLogEnable())
				wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " send 2 remote: sending e=" + e + " id=" + e.GetEventID() + " p=" + e.m_player + "  m=" + e.m_magazine);

			p.StoreInputForRemotes(ctx);
		}
	}

	
	RecoilBase SpawnRecoilObject()
	{
		return new DefaultRecoil(this);
	}

	int GetWeaponSpecificCommand(int weaponAction, int subCommand)
	{
		return subCommand;
	}

	bool CanFire()
	{
		if (!IsChamberEmpty(GetCurrentMuzzle()) && !IsChamberFiredOut(GetCurrentMuzzle()) && !IsJammed() && !m_LiftWeapon && !IsDamageDestroyed())
			return true;
		return false;
	}
			
	bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (!optic)
			return true;
		
		return optic.HasWeaponIronsightsOverride();
	}
	
	//! Initializes DOF properties for weapon's ironsight/optics cameras
	bool InitDOFProperties(out array<float> temp_array)
	{
		if (GetGame().ConfigIsExisting("cfgWeapons " + GetType() + " PPDOFProperties"))
		{
			GetGame().ConfigGetFloatArray("cfgWeapons " + GetType() + " PPDOFProperties", temp_array);
			return true;
		}
		return false;
	}
	
	bool InitReliability(out array<float> reliability_array)
	{
		if (GetGame().ConfigIsExisting("cfgWeapons " + GetType() + " Reliability ChanceToJam"))
		{
			GetGame().ConfigGetFloatArray("cfgWeapons " + GetType() + " Reliability ChanceToJam", reliability_array);
			return true;
		}
		return false;
	}
	
	//!gets weapon length from config for weaponlift raycast
	bool InitWeaponLength()
	{
		if (ConfigIsExisting("WeaponLength"))
		{
			m_WeaponLength = ConfigGetFloat("WeaponLength");
			return true;
		}
		m_WeaponLength = 0.8; //default value if not set in config; should not be zero
		return false;
	}
	
	//!gets weapon vertical offset from config for weaponlift raycast
	bool InitWeaponLiftCheckVerticalOffset()
	{
		if (ConfigIsExisting("WeaponLiftCheckVerticalOffset"))
		{
			m_WeaponLiftCheckVerticalOffset = ConfigGetFloat("WeaponLiftCheckVerticalOffset");
			return true;
		}
		m_WeaponLiftCheckVerticalOffset = 0.0; //no offset by default
		return false;
	}
	
	//!gets approximate weapon distance from shoulder from config
	protected bool InitShoulderDistance()
	{
		if (ConfigIsExisting("ShoulderDistance"))
		{
			m_ShoulderDistance = ConfigGetFloat("ShoulderDistance");
			return true;
		}
		
		m_ShoulderDistance = 0;
		return false;
	}	
	
	//!gets weapon obstruction distance from shoulder at which the weapon is fully obstructed
	protected bool InitObstructionDistance()
	{
		if (ConfigIsExisting("ObstructionDistance"))
		{
			m_ObstructionDistance = ConfigGetFloat("ObstructionDistance");
			return true;
		}
			
		m_ObstructionDistance = 0;
		return false;
	}
	
	ref array<float> GetWeaponDOF()
	{
		return m_DOFProperties;
	}
	
	bool GetWasIronSight()
	{
		return m_WasIronSight;
	}
	
	// used to remember last used ironsight/optic state
	void SetWasIronSight(bool state)
	{
		m_WasIronSight = state;
	}
	
	// lifting weapon on obstcles
	bool LiftWeaponCheck(PlayerBase player)
	{
		Object object;
		float obstruction;
		return LiftWeaponCheckEx(player, obstruction, object);
	}

	/*!
		Returns whether this weapon can use obstruction instead of weapon lift.
		Implement simple conditions only (e.g. checking for attachments, weapon types, ...) to prevent possible desyncs.
		\param obstructionValue The percentage of penetration into hit object
		\param hitObject The object obstructing the weapon
	*/
	bool UseWeaponObstruction(PlayerBase player, float obstructionValue, Object hitObject)
	{
		HumanMovementState ms = new HumanMovementState();
		player.GetMovementState(ms);

		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 2) // allow always
			return true;
		if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 6) // neverEver
			return false;
		#endif

		// Obstruction in prone does not really work well, the weapon has no physical room
		// to move, so we instead always lift in such stance
		if (ms.IsInProne() || ms.IsInRaisedProne())
		{
			return false;
		}
		
		// if ( m_ObstructionDistance != 0 && m_ObstructionDistance < 0.7 && ZombieBase.Cast( hitObject ) ) return true;
		
		bool isDynamic;
		bool isStatic;
		if (hitObject)
		{
			isDynamic = dBodyIsDynamic(hitObject);
			isStatic  = !isDynamic;
		}
			
		#ifdef DIAG_DEVELOPER
		// alwaysDynamic || alwaysDynamicNeverStatic
		bool diagAlwaysDynamic = DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 3 || DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 5;
		if (diagAlwaysDynamic && isDynamic) // alwaysDynamic
			return true;
		
		// neverStatic   || alwaysDynamicNeverStatic
		bool diagNeverStatic   = DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 4 || DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 5;
		if (diagNeverStatic && isStatic) // neverStatic
			return false;
		#endif
		
		//CFGGAMEPLAY
		EWeaponObstructionMode staticMode  = CfgGameplayHandler.GetWeaponObstructionModeStatic();
		EWeaponObstructionMode dynamicMode = CfgGameplayHandler.GetWeaponObstructionModeDynamic();
		if (hitObject) // Can determine logic reliably
		{
			if ((isStatic && staticMode == EWeaponObstructionMode.DISABLED) || (isDynamic && dynamicMode == EWeaponObstructionMode.DISABLED))
			{
				return false;
			}
			else if ((isStatic && staticMode == EWeaponObstructionMode.ALWAYS) || (isDynamic && dynamicMode == EWeaponObstructionMode.ALWAYS))
			{
				return true;
			}
		}
		else if (obstructionValue > 0) // With no hit we have to guess whether object was dynamic or static
		{
			// Allow obstruction if it was already going on	(and it is allowed in either mode)
			return staticMode != EWeaponObstructionMode.DISABLED && dynamicMode != EWeaponObstructionMode.DISABLED;
		}
		//!CFGGAMEPLAY
		
		
		// Create a buffer between entering and leaving the lift to prevent
		// continuous state changes while the value is near the edge. (hysteresis)
		bool isLift = player.IsLiftWeapon();
		
		if (isLift && obstructionValue > 0.9) // Retain lift while already lifted
		{
			return false;
		}
		
		if (!isLift && obstructionValue >= 1.0) // Enter lift while not lifted and obstructed enough
		{
			return false;
		}

		#ifdef DIAG_DEVELOPER // Keep this diag below all state conditions but above weapon type checks
		if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_FORCEALLOW_OBSTRUCTION) == 1) // allow conditionally
			return true;
		#endif
		
		// Allow obstruction with weapons that have their distance defined, otherwise don't	
		return m_ObstructionDistance != 0;
	}
	
	//----------------------------------------------------------------------------------------	
	/*
		Computes/fills the provided `dst` with aim offsets relevant for the provided `characterStance`.
		Aiming angles are sampled as the normalized < -1.0, +1.0 > range.
	*/
	protected void GetApproximateAimOffsets(Blend2DVector dst, int characterStance)
	{
		if (characterStance >= 	DayZPlayerConstants.STANCEIDX_RAISED)
			characterStance -= DayZPlayerConstants.STANCEIDX_RAISED;
			
		// All following values were set by inspecting the character with
		// a weapon in-game and adjusting the offsets as such that with
		// the weapon lift diagnostic enabled, the shapes would nearly
		// perfectly overlap an equipped RIFLE.
		if (characterStance == DayZPlayerConstants.STANCEIDX_CROUCH)
		{
			dst.Insert( 0.0, -1.0, " 0.16  0.22 -0.04"); // fully down
			dst.Insert( 0.0, -0.5, " 0.14  0.13  0.00"); // halway down
			dst.Insert( 0.0,  0.0, " 0.13  0.04 -0.02"); // forward
			dst.Insert( 0.0,  0.5, " 0.13  0.01 -0.03"); // halfway up
			dst.Insert( 0.0,  1.0, " 0.14 -0.01 -0.04"); // fully up
		}
		else if (characterStance == DayZPlayerConstants.STANCEIDX_PRONE)
		{			
			dst.Insert( 0.0, -1.0, " 0.120 -0.080 -0.030"); // fully down
			dst.Insert( 0.0, -0.5, " 0.120 -0.040 -0.040"); // halfway down
			dst.Insert( 0.0,  0.0, " 0.120  0.010 -0.022");// forward
			dst.Insert( 0.0,  0.5, " 0.120 -0.080 -0.050"); // halfway up
			dst.Insert( 0.0,  1.0, " 0.120 -0.160 -0.130"); // fully up
			
			// prone is very special, so there are some points mapped
			// when aiming right and left (and up), to at least somewhat
			// correspond with the actual character animation
			dst.Insert( 0.3,  0.0, " 0.110  0.008  0.010");
			dst.Insert( 0.5,  0.0, " 0.000  0.100  0.025");
			dst.Insert( 0.8,  0.0, " 0.070  0.150 -0.014");
			dst.Insert( 1.0,  0.0, " 0.140 -0.050  0.020");
				
			dst.Insert(-0.3,  0.0, " 0.090 -0.100 -0.025");
			dst.Insert(-0.5,  0.0, " 0.072 -0.064 -0.002");
			dst.Insert(-0.9,  0.0, " 0.129 -0.080  0.015");
			dst.Insert(-1.0,  0.0, " 0.140 -0.050  0.020");	
			
			dst.Insert( 0.5,  1.0, "-0.050  0.150  0.120");
			dst.Insert( 1.0,  1.0, " 0.150 -0.035  0.030");
			dst.Insert(-0.5,  1.0, " 0.050 -0.124 -0.040");
			dst.Insert(-1.0,  1.0, " 0.150 -0.035  0.030");
		}
		else
		{
			dst.Insert( 0.0, -1.0, "0.13  0.14  0.082"); // fully down
			dst.Insert( 0.0, -0.5, "0.13  0.05  0.048"); // halfway down
			dst.Insert( 0.0,  0.0, "0.13  0.01 -0.008"); // forward
			dst.Insert( 0.0,  0.5, "0.13  0.00 -0.015"); // halfway up
			dst.Insert( 0.0,  1.0, "0.13 -0.04 -0.016"); // fully up
		}
	}
	
	//----------------------------------------------------------------------------------------
	/*
		Computes approximate offset during movement for this weapon.
	*/
	protected vector GetApproximateMovementOffset(vector localVelocity, int characterStance, float lean, float ud11, float lr11)
	{
		vector offset;
		if (lean != 0)
		{
			const float LEAN_VERT_OFFSET = -0.1;
			const float LEAN_HORIZ_OFFSET_L = 0;
			const float LEAN_HORIZ_OFFSET_R = 0.01;
			float aimStraightWeight = 1.0 - Math.AbsFloat(ud11); // 1 when aiming forward
			float leanOffset  = lean * aimStraightWeight;
			offset += Vector( leanOffset * Math.Lerp(LEAN_HORIZ_OFFSET_L, LEAN_HORIZ_OFFSET_R, lean * 0.5 + 0.5), leanOffset * LEAN_VERT_OFFSET, 0);
		}
		
			
		float maxVelocity = Math.Max( Math.AbsFloat(localVelocity[0]), Math.AbsFloat(localVelocity[2]) );
		float peakVelocity = 0.5;
		float moveAmount01 = Math.Clamp(maxVelocity / peakVelocity, 0.0, 1.0);
		if (moveAmount01 != 0.0)
		{
			vector moveOffset = "0 -0.2 -0.1";
			float ud01 = (ud11 * 0.5) + 0.5;
			float aimWeight = Math.Clamp(1.0 - (ud01 * 2), 0, 1);
			// The issue is only apparent when looking down and the 2nd power seems
			// to follow the actual visual relatively accurately
			float moveWeight = moveAmount01 * Math.Pow(aimWeight, 2.0);
			offset = offset + (moveWeight * moveOffset);
		}
		
		return offset;	
	}
	
	//----------------------------------------------------------------------------------------	
	/*!
		Update provided `start` position and lift check `direction` to include appproximated character state.
		\param start 		Ray start position, to be modified
		\param direction 	Ray direction
	*/
	protected void ApproximateWeaponLiftTransform(inout vector start, inout vector direction, HumanMovementState hms, HumanInputController hic, HumanCommandWeapons hcw, HumanCommandMove hcm, vector localVelocity = "0 0 0")
	{
		// Construct stable trasformation matrix that somewhat aligns with the weapon transform,
		// without actually using the weapon as reference - the weapon will move during the lift/obstruction
		// in more than 1 axis and is therefore not realiable source of truth.
		vector resTM[4];
		resTM[0] = Vector(direction[0], 0, direction[2]).Normalized();
		resTM[0] = vector.RotateAroundZeroDeg(resTM[0], vector.Up, 90);
		resTM[2] = direction;
		resTM[1] = resTM[2] * resTM[0];
		resTM[3] = start;

		// Approximate the roll angle of leaning
		float leanAngle = hms.m_fLeaning * 35;
		vector rotTM[3];
		float xAimHandsOffset = hcw.GetAimingHandsOffsetLR();
		float yAimHandsOffset = hcw.GetAimingHandsOffsetUD();
		Math3D.YawPitchRollMatrix( Vector(xAimHandsOffset , yAimHandsOffset, leanAngle), rotTM );
		Math3D.MatrixMultiply3(resTM, rotTM, resTM);

		// Draw relative transformation matrix diagnostic
		#ifndef SERVER
		#ifdef DIAG_DEVELOPER
		PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetTransform(resTM);
		#endif
		#endif

		// Compute normalized aiming angles
		float udAngle = hcw.GetBaseAimingAngleUD();
		float lrAngle = hcw.GetBaseAimingAngleLR();
		
		float ud01 = Math.InverseLerp(DayZPlayerCamera1stPerson.CONST_UD_MIN, DayZPlayerCamera1stPerson.CONST_UD_MAX, udAngle); // 0-1
		float ud11 = Math.Clamp((ud01 * 2) - 1, -1, 1); // -1, 1
		float lr01 = Math.InverseLerp(DayZPlayerCamera1stPerson.CONST_LR_MIN, DayZPlayerCamera1stPerson.CONST_LR_MAX, lrAngle); // 0-1
		float lr11 = Math.Clamp((lr01 * 2) - 1, -1, 1);
			
		#ifndef SERVER
		#ifdef DIAG_DEVELOPER
		PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetAimAngles(udAngle, lrAngle, ud11, lr11);
		#endif
		#endif
		
		// Fetch approximate aim offset position based on current state
		Blend2DVector aimOffsets = new Blend2DVector();
		GetApproximateAimOffsets(aimOffsets, hms.m_iStanceIdx);
		vector offset = aimOffsets.Blend(lr11, ud11);
		
		// Apply height offset if any is defined
		if (m_WeaponLiftCheckVerticalOffset != 0)
		{
			offset[1] = offset[1] + m_WeaponLiftCheckVerticalOffset;
		}

		// Approximate the shift caused by movement. There is an enormous shift when aiming
		// downwards, creating insane shift that we will compensate by shifting the offset
		// based on the movement and aiming angle:
		vector moveOffset = GetApproximateMovementOffset(localVelocity, hms.m_iStanceIdx, hms.m_fLeaning, ud11, lr11);
		offset += moveOffset;

		// While changing stances the weapon moves slightly forward and although this may
		// cause some unwanted lifts ocasionally, it should prevent some unwanted clipping
		if (hcm.IsChangingStance())
		{
			offset[2] = offset[2] + 0.05;
		}

		offset = offset.InvMultiply3(rotTM);

		// Finally use the computed offset as the start position.
		start = offset.Multiply4(resTM);
	}
	//----------------------------------------------------------------------------------------
	//! Approximate `ObstructionDistance` for weapons with no configuration. Returned length doesn't account for attachments.	
	private float ApproximateBaseObstructionLength()
	{
		float approximateLength = Math.Max(0, m_WeaponLength / 1.5) * m_WeaponLength;
		return m_ShoulderDistance + approximateLength;
	}	

	//----------------------------------------------------------------------------------------
	/*!
		Perform weapon obstruction check by the provided `player`.
		\param player The player to perform the check.
		\param outObstruction Result obstruction value [0 .. 1] or > 1 when out of range and needs lift instead
		\param outHitObject Object that was hit (if any)
		\return True whenever weapon should be lifted or obstruted. (See also Weapon_Base.UseWeaponObstruction)
	*/
	bool LiftWeaponCheckEx(PlayerBase player, out float outObstruction, out Object outHitObject)
	{
		bool wasLift = m_LiftWeapon;
		vector lastLiftPosition = m_LastLiftPosition;
		
		m_LiftWeapon = false;

		// [14.2.2025]
		// The following selection check should be *unused* for some time now.
		// Leaving commented out for future reference, should the removal have unforseen consequences.
		//	if ( HasSelection("Usti hlavne") )
		//		return false;

		if (!player)
		{
			#ifndef SERVER
			#ifdef DIAG_DEVELOPER
				PluginDiagMenuClient.GetWeaponLiftDiag().Reset();
			#endif
			#endif
			Print("Error: No weapon owner for LiftWeaponCheckEx, returning.");
			return false;
		}
		
		// Obstruction can only occur if the weapon is rasied
		HumanMovementState movementState = new HumanMovementState();
		player.GetMovementState(movementState);
		if (!movementState.IsRaised())
			return false;
		
		// Suppress weapon obstruction during melee attack preventing state inconsistency
		if (movementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE || movementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE2)
		{
			#ifndef SERVER
			#ifdef DIAG_DEVELOPER
				PluginDiagMenuClient.GetWeaponLiftDiag().Reset();
			#endif
			#endif
			return false;
		}
		
		
		// If possible use aiming angles instead as these will work consistently
		// and independently of any cameras, etc.
		vector direction;
		HumanInputController hic = player.GetInputController();
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
		HumanCommandMove hcm = player.GetCommand_Move();
		if (hcw)
		{
			vector yawPitchRoll = Vector(
				hcw.GetBaseAimingAngleLR() + player.GetOrientation()[0],
				hcw.GetBaseAimingAngleUD(),
				0.0);
			
			float xAimHandsOffset = hcw.GetAimingHandsOffsetLR();
			float yAimHandsOffset = hcw.GetAimingHandsOffsetUD();
			
			yawPitchRoll[0] = yawPitchRoll[0] + xAimHandsOffset;
			yawPitchRoll[1] = Math.Clamp( yawPitchRoll[1] + yAimHandsOffset, DayZPlayerCamera1stPerson.CONST_UD_MIN, DayZPlayerCamera1stPerson.CONST_UD_MAX );
			
			direction = yawPitchRoll.AnglesToVector();
		}
		else // Fallback to previously implemented logic
		{
			// freelook raycast
			if (hic.CameraIsFreeLook())
			{
				if (player.m_DirectionToCursor != vector.Zero)
				{
					direction = player.m_DirectionToCursor;
				}
				// if player raises weapon in freelook
				else
				{
					direction = MiscGameplayFunctions.GetHeadingVector(player);
				}
			}
			else
			{
				direction = GetGame().GetCurrentCameraDirection(); // exception for freelook. Much better this way!
			}
		}
		
		// Reference bone to perform checks from
		vector start;
		int boneIdx = player.GetBoneIndexByName("Neck");
		if (boneIdx == -1)
		{
			start = player.GetPosition()[1] + 1.5;
		}
		else
		{
			 start = player.GetBonePositionWS(boneIdx);
		}
		
		// Update the ray origin and direction with approximations of character state
		vector velocity = GetVelocity(player);
		velocity = player.VectorToLocal(velocity);
		ApproximateWeaponLiftTransform(start, direction, movementState, hic, hcw, hcm, velocity);
		
		float effectiveAttachmentLength = GetEffectiveAttachmentLength();

		// weapon length including effective attachments
		float weaponLength = m_WeaponLength + effectiveAttachmentLength;
		// distance of point from 'shoulder' (start) from which the weapon length should be computed,
		// pistols have the start point further, near the character wrist whereas longer rifles, typically
		// the ones with buttstocks have the start point at 0 distance, ie. directly at shoulder	
		float weaponStartDist = m_ShoulderDistance;
		float weaponEndDist = weaponStartDist + weaponLength;
		
		// Weapon start and end positions as an offset from the character shoulder
		vector weaponStart = start + (weaponStartDist * direction);
		vector weaponEnd = start + (weaponEndDist * direction);
		
		// distance of point from 'shoulder' (start) at which the weapon is "fully obstructed", i.e.
		// needs to be lifted to be able to be physically fit wherever it is. Shifted by effective
		// attachment length to account for e.g. suppressors that make the weapon "longer".
		// Defaults to FLT_MAX if no value is configured, effectivelly always lifting weapon instead of obstructing.
		float baseObstructionLength = m_ObstructionDistance;
		if (baseObstructionLength==0)
		{
			baseObstructionLength = ApproximateBaseObstructionLength();
		}
		
		float weaponObstructionDist = baseObstructionLength + effectiveAttachmentLength;
		float rayRadius = 0.02;
		
		#ifndef SERVER
		#ifdef DIAG_DEVELOPER // Retrieve possible override
		float overrideObstDist = PluginDiagMenuClient.GetWeaponLiftDiag().Data().m_ObstructionDistance;
		PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetWeaponRayParams(start, direction, weaponStartDist, weaponEndDist, effectiveAttachmentLength, m_ObstructionDistance, weaponObstructionDist, rayRadius);
		weaponObstructionDist = overrideObstDist;
		rayRadius = PluginDiagMenuClient.GetWeaponLiftDiag().Data().m_RayRadiusOverride;
		#endif
		#endif

		// Extend the raycast range by 30 cm to allow checking for intersections even
		// under shallow angles and other odd cases
		float rayEndDist = weaponEndDist + 0.30;
		vector rayEnd = start + rayEndDist * direction;
		
		// Prepare raycast params and perform the cast in fire geo
		RaycastRVParams rayParm = new RaycastRVParams(start, rayEnd, player, rayRadius);
		rayParm.flags = CollisionFlags.ALLOBJECTS;
		rayParm.type = ObjIntersect.Fire;
		
		RaycastRVResult hitResult;
		float hitFraction;
		float hitDist;
		
		array<ref RaycastRVResult> results = {};
		if (!DayZPhysics.RaycastRVProxy(rayParm, results) || results.Count() == 0)
		{
			hitFraction = 0;
		}
		else
		{
			// In case of multiple results, find the best (nearest) match, RaycastRVProxy doesn't guarantee any sensible order
			int numRes = results.Count();
			if (numRes == 1)
			{
				hitResult = results[0];
			}
			else
			{
				int bi = -1;
				float maxDist = float.MAX;
				for (int i = 0, nr = results.Count(); i < nr; ++i)
				{
					float sqDist = vector.DistanceSq(results[i].pos, weaponStart);
					if (sqDist < maxDist)
					{
						maxDist = sqDist;
						bi = i;
					}
				}
				hitResult = results[bi];
			}

			if (LiftWeaponRaycastResultCheck(hitResult))
			{
				float len0 = (hitResult.pos - start).Length();
				float len1 = (weaponEnd - start).Length(); // Do not include 'rayEnd' - pretend as if the computation happened in <start, weaponEnd>
				
				if (len0 <= 0 || len1 <= 0)
				{
					hitFraction = 1;
				}
				else
				{
					hitFraction = len0 / len1;
				}
				hitDist = hitFraction * weaponEndDist;
			}
			else
			{
				hitFraction = 0;
				hitDist = 0;
			}
		}
			
		#ifndef SERVER
		#ifdef DIAG_DEVELOPER	// isect diag
		PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetIntersectionParams(hitResult, hitFraction, hitDist);
		PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetLastPosition(lastLiftPosition);
		#endif	// !isect diag
		#endif
		
		// Start by assuming that we want to retain state
		bool wantsLift = wasLift;
		
		// Sq. distance of weapon movement required to trigger lift (in)
		const float inThreshold = 0.002;
		// Sq. distance of weapon movement required to trigger lift (out)
		const float outThreshold = 0.003;
		const float noIsctOutThreshold = 0.01;
		// Max num of ticks with no hit for which hysteresis will persist
		// value chosen by iteration, should be approx 0.333s
		const int maxNumMissedTicks = 10;
		
		// Min angle in degrees change from last lift to stop lifting
		// Base threshold of 0.75 degrees + 0.6 degrees per meter of weapon length
		float angleThreshold = 0.75 + Math.Clamp( m_WeaponLength * 0.6, 0, 1.5 );
		
		// Update state when a hit is registered
		if (hitFraction != 0)
		{
			vector v1 = hitResult.pos - weaponEnd;
			vector v2 = hitResult.pos - rayEnd;
			float d = vector.Dot(v1, v2);
			// But leave some threshold where previous state is kept
			// to prevent excessive switches from occuring
			if (!wasLift && d > inThreshold)
			{
				wantsLift = true;
			}
			else if (wasLift && d < -outThreshold)
			{
				wantsLift = false;
			}
			
			m_LastLiftPosition = hitResult.pos;
			m_LastLiftHit = player.GetSimulationTimeStamp();
		}
		else
		{
			// With no hit and no previous lift
			if (lastLiftPosition == vector.Zero)
			{
				wantsLift = false;
				m_LastLiftPosition = vector.Zero;
			}
			// See if previous hit wasn't very close to our current position,
			// in which case simply don't lift the weapon
			else
			{
				vector v3 = (lastLiftPosition - start).Normalized();
				vector v4 = (weaponEnd-start).Normalized();
				float d2 = vector.Dot(v3, v4);
				// no isect, angle delta check
				if (Math.Acos(d2) > (angleThreshold * Math.DEG2RAD)) // if relative angle is > x degree, stop lifting
				{
					wantsLift = false;
					m_LastLiftPosition = vector.Zero;
				}
				// no isect, distance check
				else
				{
					float d3 = vector.Dot( lastLiftPosition - weaponEnd, (start-weaponEnd).Normalized() );
					if (d3 < -noIsctOutThreshold)
					{
						wantsLift = false;
						m_LastLiftPosition = vector.Zero;
					}
					float lastObstruction = hcw.GetWeaponObstruction();	
					// fallback in case offending object disappears or moves
					int timeSinceHit = player.GetSimulationTimeStamp() - m_LastLiftHit;
					if (timeSinceHit > maxNumMissedTicks)
					{
						wantsLift = false;
						m_LastLiftPosition = vector.Zero;
					}
					else if (wantsLift && m_LastLiftPosition != vector.Zero) // pretended hit to retain obstruction in this very tight edge case
					{
						float l0 = (m_LastLiftPosition - start).Length();
						float l1 = (weaponEnd - start).Length();
						if (l0 <= 0 || l1 <= 0)
						{
							hitFraction = 1;
						}
						else
						{
							hitFraction = l0 / l1;
						}
						hitDist = hitFraction * weaponEndDist;
					}
				}
			}
		}
		
		// lift is desired
		if (wantsLift)
		{
			// Remap the hit distance into the <obstruction, weaponWithAttachmentLength> range as 0..1 (and beyond)
			float begDist = weaponObstructionDist;
			float endDist = weaponStartDist + weaponLength;
			
			float obstFraction;
			if (begDist < endDist)
				obstFraction = Math.InverseLerp( begDist, endDist, hitDist );
			
			if (hitResult)
				outHitObject = hitResult.obj;
			
			outObstruction 	= 1.0 - obstFraction;
			m_LiftWeapon 	= true;
			return true;
		}

		return false;
	}
	
	/*!
		Recomputes the provided `obstruction01` value typically returned by `LiftWeaponCheckEx`
		from the [0 ... 1] range to distance in meters the weapon penetrates the obstacle.
		\param obstruction01 Obstruction progress
		\return Penetration depth in meters
	*/
	float GetObstructionPenetrationDistance(float obstruction01)
	{
		float baseObstructionLength = m_ObstructionDistance;
		if (baseObstructionLength==0)
		{
			baseObstructionLength = ApproximateBaseObstructionLength();
		}
		
		float effectiveAttachmentLength = GetEffectiveAttachmentLength();
		float weaponEnd = m_ShoulderDistance + m_WeaponLength + effectiveAttachmentLength;
		return weaponEnd - Math.Lerp(weaponEnd, baseObstructionLength + effectiveAttachmentLength, obstruction01);		
	}
	
	//! Return whether provided material triggers weapon lift (true) or not (false).
	bool LiftWeaponRaycastResultCheck(notnull RaycastRVResult res)
	{
		return res.surface.IsSolid();
	}
	
	//! Returns effective length of attachments that influence total weapon length
	float GetEffectiveAttachmentLength()
	{
		ItemBase attachment;
		if (HasBayonetAttached())
		{
			int bayonetIndex = GetBayonetAttachmentIdx();
			attachment = ItemBase.Cast(GetInventory().FindAttachment(bayonetIndex));
		}
		else
		{
			attachment = GetAttachedSuppressor();
		}
		
		if (attachment)
		{
			return Math.Max(attachment.m_ItemModelLength + attachment.m_ItemAttachOffset, 0);
		}
		else
		{
			return 0;
		}
	}
	
	void SetSyncJammingChance( float jamming_chance )
	{
		m_ChanceToJamSync = jamming_chance;
	}
	
	/**@fn		EjectCartridge
	 * @brief	unload bullet from chamber or internal magazine
	 *
	 * @NOTE: 	EjectCartridge = GetCartridgeInfo + PopCartridge
	 *
	 * @param[in] muzzleIndex
	 * @param[out] ammoDamage \p  damage of the ammo
	 * @param[out] ammoTypeName \p	 type name of the ejected ammo
	 * @return	true if bullet removed from chamber
	 **/
	bool EjectCartridge(int muzzleIndex, out float ammoDamage, out string ammoTypeName)
	{
		if (IsChamberEjectable(muzzleIndex))
		{
			if (PopCartridgeFromChamber(muzzleIndex, ammoDamage, ammoTypeName))
				return true;
		}
		else if (GetInternalMagazineCartridgeCount(muzzleIndex) > 0)
		{
			if (PopCartridgeFromInternalMagazine(muzzleIndex, ammoDamage, ammoTypeName))
				return true;
		}
		return false;
	}
	
	bool CopyWeaponStateFrom(notnull Weapon_Base src)
	{
		float damage = 0.0;
		string type;

		for (int mi = 0; mi < src.GetMuzzleCount(); ++mi)
		{
			if (!src.IsChamberEmpty(mi))
			{
				if (src.GetCartridgeInfo(mi, damage, type))
				{
					PushCartridgeToChamber(mi, damage, type);
				}
			}
			
			for (int ci = 0; ci < src.GetInternalMagazineCartridgeCount(mi); ++ci)
			{
				if (src.GetInternalMagazineCartridgeInfo(mi, ci, damage, type))
				{
					PushCartridgeToInternalMagazine(mi, damage, type);
				}
			}
		}
		
		int dummy_version = int.MAX;
		PlayerBase parentPlayer = PlayerBase.Cast(src.GetHierarchyRootPlayer());
		if (!parentPlayer)
			dummy_version -= 1;
		ScriptReadWriteContext ctx = new ScriptReadWriteContext;
		src.OnStoreSave(ctx.GetWriteContext());
		OnStoreLoad(ctx.GetReadContext(), dummy_version);
		return true;
	}
	
	//! attachment helpers (firearm melee)
	override void SetBayonetAttached(bool pState, int slot_idx = -1)
	{
		m_BayonetAttached = pState;
		m_BayonetAttachmentIdx = slot_idx;
	}
	
	override bool HasBayonetAttached()
	{
		return m_BayonetAttached;
	}
	
	override int GetBayonetAttachmentIdx()
	{
		return m_BayonetAttachmentIdx;
	}
	
	override void SetButtstockAttached(bool pState, int slot_idx = -1)
	{
		m_ButtstockAttached = pState;
		m_ButtstockAttachmentIdx = slot_idx;
	}

	override bool HasButtstockAttached()
	{
		return m_ButtstockAttached;
	}
	
	override int GetButtstockAttachmentIdx()
	{
		return m_ButtstockAttachmentIdx;
	}
	
	void HideWeaponBarrel(bool state)
	{
		if ( !GetGame().IsDedicatedServer() )//hidden for client only
		{
			ItemOptics optics = GetAttachedOptics();
			if ( optics && !optics.AllowsDOF() && m_weaponHideBarrelIdx != -1 )
			{
				SetSimpleHiddenSelectionState(m_weaponHideBarrelIdx,!state);
			}
		}
	}
	
	void ShowMagazine()
	{
		if (m_magazineSimpleSelectionIndex > -1)
			SetSimpleHiddenSelectionState(m_magazineSimpleSelectionIndex,1);
		else
			SelectionMagazineShow();
	}
	
	void HideMagazine()
	{
		if (m_magazineSimpleSelectionIndex > -1)
			SetSimpleHiddenSelectionState(m_magazineSimpleSelectionIndex,0);
		else
			SelectionMagazineHide();
	}
	
	override EntityAI ProcessMeleeItemDamage(int mode = 0)
	{
		EntityAI attachment;
		
		switch (mode)
		{
			case 0:
				super.ProcessMeleeItemDamage();
			break;
			
			case 1:
				attachment = GetInventory().FindAttachment(m_ButtstockAttachmentIdx);
			break;
			
			case 2:
				attachment = GetInventory().FindAttachment(m_BayonetAttachmentIdx);
			break;
			
			default:
				super.ProcessMeleeItemDamage();
			break;
		}
		
		if (attachment)
		{
			attachment.ProcessMeleeItemDamage();
			return attachment;
		}
		
		return this;
	}
	
	bool IsShowingChamberedBullet()
	{
		return true;
	}
	
	int GetBurstCount()
	{
		return m_BurstCount;
	}
	
	void ResetBurstCount()
	{
		m_BurstCount = 0;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionUnjam);
		AddAction(FirearmActionAttachMagazine);
		AddAction(FirearmActionLoadBullet);
		AddAction(FirearmActionMechanicManipulate);
		AddAction(ActionTurnOnWeaponFlashlight);
		AddAction(ActionTurnOffWeaponFlashlight);

		AddAction(FirearmActionAttachMagazineQuick); // Easy reload
		AddAction(FirearmActionLoadBulletQuick); // Easy reload
	}
	
	override bool CanBeUsedForSuicide()
	{
		if (!ConfigGetBool("isSuicideWeapon"))
			return false;
		
		return super.CanBeUsedForSuicide();
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnAmmo("", SAMF_DEFAULT);
	}
	
	bool AddJunctureToAttachedMagazine(PlayerBase player, int timeoutMS)
	{
		Magazine mag = GetMagazine(GetCurrentMuzzle());
		InventoryLocation il = new InventoryLocation();
		if (mag)
		{
			return GetGame().AddInventoryJunctureEx(player, mag, il, false, timeoutMS);
		}
		
		return true;
	}
	
	void ClearJunctureToAttachedMagazine(PlayerBase player)
	{
		Magazine mag = GetMagazine(GetCurrentMuzzle());
		if (mag)
		{
			GetGame().ClearJunctureEx(player, mag);
		}
	}
	
	void SetNextWeaponMode(int muzzleIndex)
	{
		SetNextMuzzleMode(muzzleIndex);
	}

	// CoolDown
	void SetCoolDown( float coolDownTime ) 
	{
		m_coolDownTime = coolDownTime; 
	}		
	void UpdateCoolDown( float dt ) { m_coolDownTime -= dt; }	
	bool IsCoolDown() 
	{ 		
		return m_coolDownTime > 0; 
	}
	
	// If there are bullet in attached / internal magazine then bullet must be in chamber also	
	bool MustBeChambered(int muzzleIndex)
	{
		return false;
	}
	
#ifdef TEST_WEAPON_SYSNC_REPAIR
	void SetSyncStable(bool value)
	{
		m_SyncStable = value;
		if (!value)
		{
			m_SyncStableTime = GetGame().GetTickTime();
		}
	}
	
	bool IsSyncStable()
	{
		return m_SyncStable;
	}
#endif
};

