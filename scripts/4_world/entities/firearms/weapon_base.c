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
		SetWeaponOpen(!initState.IsWeaponOpen());
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
		int muzzCount = GetMuzzleCount();
		
		bool ammoRng = ammoType == "";
		bool ammoFullRng = ammoRng && (flags & WeaponWithAmmoFlags.AMMO_MAG_RNG);
		
		// No full RNG flag, so pick one random and use only this one
		if (ammoRng && !ammoFullRng)
			ammoType = GetRandomChamberableAmmoTypeName(0);
		
		// Fill the internal magazine
		for (int i = 0; i < muzzCount; ++i)
		{
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
					didSomething = true;
				}
			}
		}
		
		// Call the chamber method if asked for
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER) || (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		if (chamber && FillChamber(ammoType, flags))
		{
			didSomething = true;
		}
		
		// Does not need any FSM fixing, FSM does not care about inner magazines
		
		return didSomething;
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
		// Quickly check if there are any chambers we can fill
		int muzzCount = GetMuzzleCount();
		bool anyEmpty = false;
		
		for (int m = 0; m < muzzCount; ++m)
		{
			if (IsChamberEmpty(m))
			{
				anyEmpty = true;
				break;
			}
		}

		if (!anyEmpty)
			return false;
		
		// Make sure the given ammoType is actually useable
		if (ammoType != "")
			if (!AmmoTypesAPI.MagazineTypeToAmmoType(ammoType, ammoType))
				return false;
		
		// Just so we don't '&' wastefully in a loop
		bool didSomething = false;		
		bool chamberFullRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG_SPORADIC);
		bool chamberRng = (flags & WeaponWithAmmoFlags.CHAMBER_RNG);
		bool chamber = (flags & WeaponWithAmmoFlags.CHAMBER);
		
		if (chamber || chamberRng || chamberFullRng)
		{
			int amountToChamber = muzzCount;
			
			// No need to do this for full rng, as that will roll for every muzzle
			if (chamberRng)
				amountToChamber = Math.RandomIntInclusive(0, muzzCount);
			
			bool chamberAmmoRng = (ammoType == "");
			bool chamberAmmoFullRng = chamberAmmoRng && (flags & WeaponWithAmmoFlags.AMMO_CHAMBER_RNG);
			
			// No full RNG flag, so pick one random and use only this one
			if (chamberAmmoRng && !chamberAmmoFullRng)
				ammoType = GetRandomChamberableAmmoTypeName(0);
			
			for (int i = 0; i < muzzCount; ++i)
			{
				// Skip when there's already something in the chamber
				if (!IsChamberEmpty(i))
					continue;
				
				// Roll the rng when enabled
				if (chamberFullRng)
					chamber = Math.RandomIntInclusive(0, 1);
				
				// We chambering
				if (chamber)
				{
					// Full random, decide a new one for every muzzle
					if ( chamberAmmoFullRng )
						ammoType = GetRandomChamberableAmmoTypeName(i);
					
					// Push it
					PushCartridgeToChamber(i, 0, ammoType);
					didSomething = true;
					
					// Stop chambering when we hit the desired amount
					--amountToChamber;				
					if (amountToChamber <= 0)
						break;
				}
			}
		}
		
		// Only fix the FSM and Synchronize when absolutely needed
		if (!didSomething)
			return false;
		
		// FSM cares about chamber state
		RandomizeFSMState();		
		Synchronize();
		
		return true;
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
		int idx;
		float distance;
		float hit_fraction;
		vector start, end;
		vector direction;
		vector hit_pos, hit_normal;
		Object obj;		
		
		bool wasLift = m_LiftWeapon;
		vector lastLiftPosition = m_LastLiftPosition;
		
		m_LiftWeapon = false;
		// not a gun, no weap.raise for now
		if ( HasSelection("Usti hlavne") )
			return false;
		
		if (!player)
		{
			Print("Error: No weapon owner, returning");
			return false;
		}
		
		// weapon not raised
		HumanMovementState movementState = new HumanMovementState();
		player.GetMovementState(movementState);
		if (!movementState.IsRaised())
			return false;
		
		// suppress raising of weapon during melee attack preventing state inconsistency
		if (movementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE || movementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE2)
		{
			return false;
		}
		
		
		// If possible use aiming angles instead as these will work consistently
		// and independently of any cameras, etc. 
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
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
			if (player.GetInputController().CameraIsFreeLook())
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
		
		idx = player.GetBoneIndexByName("Neck"); //RightHandIndex1
		if ( idx == -1 )
			{ start = player.GetPosition()[1] + 1.5; }
		else
			{ start = player.GetBonePositionWS(idx); }
		
		
		// Updated weapon lift detection code prototype
		{
			// 0: construct stable trasformation matrix that
			// approximately aligns with the weapon transform
			// without actually using the weapon as reference
			// (as the weapon can be moved unpredictably by anims)
			vector resTM[4];
			resTM[0] = Vector(direction[0], 0, direction[2]).Normalized();
			resTM[0] = vector.RotateAroundZeroDeg(resTM[0], vector.Up, 90);
			resTM[2] = direction;
			resTM[1] = resTM[2] * resTM[0]; 
			resTM[3] = start;
			
			// Approximate the roll of leaning
			HumanMovementState hms = new HumanMovementState();
			player.GetMovementState(hms);
			float leanAngle = hms.m_fLeaning * 35;
			vector rotTM[3];
			Math3D.YawPitchRollMatrix( Vector(xAimHandsOffset , yAimHandsOffset, leanAngle), rotTM );
			Math3D.MatrixMultiply3(resTM, rotTM, resTM);
			
			// Draw relative TM diagnostic
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_LIFT_DEBUG))
			{
				Shape.CreateArrow(resTM[3], resTM[3] + resTM[0], 0.05, COLOR_RED, ShapeFlags.ONCE);
				Shape.CreateArrow(resTM[3], resTM[3] + resTM[1], 0.05, COLOR_GREEN, ShapeFlags.ONCE);
				Shape.CreateArrow(resTM[3], resTM[3] + resTM[2], 0.05, COLOR_BLUE, ShapeFlags.ONCE);
			}
			#endif
			
			// 1: pick from predefined offset relative to
			// the previously constructed transform 
			float udAngle = Math.Asin(direction[1]) * Math.RAD2DEG;
			
			// offsets are [right, up, forward]
			// values are based on what felt right after iterating
			vector offsets[] = 
			{ 
				"0.11 0.17 0.0",  // offset while aiming down
				"0.12 0.05 0.0", // offset while aiming forward
				"0.112 0.03 0.0" // offset while aiming up
			};
			const int lastIndex = 2; // length of offsets - 1
			
			// <0,1> range of aiming
			float a = Math.Clamp(Math.InverseLerp(DayZPlayerCamera1stPerson.CONST_UD_MIN, DayZPlayerCamera1stPerson.CONST_UD_MAX, udAngle), 0, 0.9999);
			int lo = a * lastIndex;
			int hi = Math.Clamp(lo+1, 0, lastIndex);
			
			// remap to current lo-hi range
			float t = Math.Clamp(a * lastIndex - lo, 0, 1);
			vector offset = vector.Lerp(offsets[lo], offsets[hi], t);
			
			// offsets are [right, up forward]
			// additional offsets added to previous offsets per stance
			vector stanceOffsets[] = 
			{
				"0 -0.015 0",	// erect
				"0 0.03 0",	// crouch
				"0 -0.04 0",// prone
			};
			
			// 2. pick from predefined offsets based on stance,
			// allows to even further approximate the alignment
			int stanceOffsetIndex = hms.m_iStanceIdx;
			if (stanceOffsetIndex >= DayZPlayerConstants.STANCEIDX_PRONE)
				stanceOffsetIndex -= DayZPlayerConstants.STANCEIDX_RAISED;
			
			stanceOffsetIndex -= DayZPlayerConstants.STANCEIDX_ERECT;
			offset += stanceOffsets[stanceOffsetIndex];
			
			// if any additional height offset is defined, apply it
			if (m_WeaponLiftCheckVerticalOffset != 0)
			{
				offset[1] = offset[1] + m_WeaponLiftCheckVerticalOffset;
			}
			
			// 
			offset = offset.InvMultiply3(rotTM);
			
			// 3. use the offset as the start position.
			// it will not be perfect, but it should reflect
			// the actual weapon transform more accurately
			start = offset.Multiply4(resTM);
		}
		
		
		distance = m_WeaponLength + GetEffectiveAttachmentLength();
		
		vector weaponStart = start + (m_ShoulderDistance * direction);
		vector weaponEnd = weaponStart + (distance * direction);
		
		// Draw diagnostics: Script -> Weapon -> Weapon Lift
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_LIFT_DEBUG))
		{
			vector diagNoAttachEnd = weaponStart + (m_WeaponLength * direction);
			int diagPtsShpFlgs = ShapeFlags.ONCE | ShapeFlags.NOOUTLINE;
			float diagPtsRadius = 0.025;
			Shape.CreateSphere(COLOR_GREEN, diagPtsShpFlgs, weaponStart, diagPtsRadius);
			Shape.CreateSphere(COLOR_YELLOW, diagPtsShpFlgs, diagNoAttachEnd, diagPtsRadius);
			Shape.CreateSphere(COLOR_BLUE, diagPtsShpFlgs, weaponEnd, diagPtsRadius);
		}
		#endif
		
		// For the physical cast, extend the distance by x%
		// to allow for smoother transition handling in some cases
		end = weaponEnd + ((0.1 * distance) * direction);
		
		// Prepare raycast params and perform the cast in fire geo
		RaycastRVParams rayParm = new RaycastRVParams(start, end, player, 0.02);
		rayParm.flags = CollisionFlags.ALLOBJECTS;
		rayParm.type = ObjIntersect.Fire;
		
		#ifdef DIAG_DEVELOPER
		DbgUI.BeginCleanupScope();
		#endif
		array<ref RaycastRVResult> results = {};
		if (!DayZPhysics.RaycastRVProxy(rayParm, results) || results.Count() == 0)
		{
			hit_pos = vector.Zero;
			hit_fraction = 0;
		}
		else
		{
			RaycastRVResult res = results[0];
			
			#ifdef DIAG_DEVELOPER	// isect diag
			if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_LIFT_DEBUG) == 2)
			{
				DbgUI.Begin("Weapon Lift Diag");
				{
					if (res.surface)
					{
						DbgUI.Text("Intersection data:");
						DbgUI.Text("  Name: " + res.surface.GetName());
						DbgUI.Text("  EntryName: " + res.surface.GetEntryName());
						DbgUI.Text("  SurfaceType: " + res.surface.GetSurfaceType());
						
						DbgUI.Text("  IsPassThrough: " + res.surface.IsPassthrough());
						DbgUI.Text("  IsSolid: " + res.surface.IsSolid());
					}
					else
					{
						DbgUI.Text("Intersection with no surface");
					}
				}
				DbgUI.End();
			}
			#endif	// !isect diag
			
			if (LiftWeaponRaycastResultCheck(res))
			{
				hit_pos = res.pos;
				float len0 = (hit_pos - start).Length();
				float len1 = (end - start).Length();
				if (len0 <= 0 || len1 <= 0)
				{
					hit_fraction = 1;
				}
				else
				{
					hit_fraction = len0 / len1;
				}
			}
			else
			{
				hit_pos = vector.Zero;
				hit_fraction = 0;
			}
		}
		#ifdef DIAG_DEVELOPER
		DbgUI.EndCleanupScope();
		#endif
		
		// Draw diagnostics: Script -> Weapon -> Weapon Lift
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_LIFT_DEBUG))
		{
			const vector epsilon = "0 0.0002 0"; // to overcome excessive z-fighting for diag
			if (lastLiftPosition!=vector.Zero)
			{
				Shape.CreateArrow(start-epsilon, lastLiftPosition-epsilon, 0.05, COLOR_YELLOW, ShapeFlags.ONCE);
			}
			
			Shape.CreateArrow(start, weaponEnd, 0.05, COLOR_WHITE, ShapeFlags.ONCE | ShapeFlags.NOZBUFFER );
			
			if (hit_fraction != 0)
			{
				Shape.CreateArrow(start+epsilon, hit_pos+epsilon, 0.05, COLOR_RED, ShapeFlags.ONCE);
			}
		}
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
		if (hit_fraction != 0)
		{
			vector v1 = hit_pos - weaponEnd;
			vector v2 = hit_pos - end;
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
			
			m_LastLiftPosition = hit_pos;
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
				vector v4 = (end-start).Normalized();
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
					float d3 = vector.Dot( lastLiftPosition - weaponEnd, (start-end).Normalized() );
					if (d3 < -noIsctOutThreshold)
					{
						wantsLift = false;
						m_LastLiftPosition = vector.Zero;
					}
					
					// fallback in case offending object disappears or moves
					int timeSinceHit = player.GetSimulationTimeStamp() - m_LastLiftHit;
					if (timeSinceHit > maxNumMissedTicks)
					{
						wantsLift = false;
						m_LastLiftPosition = vector.Zero;
					}
				}
			}
		}
		
		// lift is desired
		if (wantsLift)
		{
			//Print(distance);
			m_LiftWeapon = true;
			return true;
		}
		return false;
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
			GetGame().ClearJuncture(player, mag);
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
};

