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
	
	protected const float DEFAULT_DAMAGE_ON_SHOT = 0.05;
	protected ref array<ref AbilityRecord> m_abilities = new array<ref AbilityRecord>;		/// weapon abilities
	protected ref WeaponFSM m_fsm;	/// weapon state machine
	protected bool m_isJammed = false;
	protected bool m_LiftWeapon = false;
	protected bool m_BayonetAttached;
	protected bool m_ButtstockAttached;
	protected int m_BurstCount;
	protected int m_BayonetAttachmentIdx;
	protected int m_ButtstockAttachmentIdx;
	protected int m_weaponAnimState = -1; /// animation state the weapon is in, -1 == uninitialized
	protected int m_magazineSimpleSelectionIndex = -1;
	protected int m_weaponHideBarrelIdx = -1; //index in simpleHiddenSelections cfg array
	protected float m_DmgPerShot = 0; //default is set to zero, since C++ solution has been implemented. See 'damageBarrel' and 'barrelArmor' in configs.
	protected float m_WeaponLength;
	ref array<int> m_bulletSelectionIndex = new array<int>;
	ref array<float> m_DOFProperties;
	ref array<float> m_ChanceToJam = new array<float>;
	protected float m_ChanceToJamSync = 0;
	protected ref PropertyModifiers m_PropertyModifierObject;
	protected PhxInteractionLayers hit_mask = PhxInteractionLayers.CHARACTER | PhxInteractionLayers.BUILDING | PhxInteractionLayers.DOOR | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ITEM_SMALL | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.FENCE | PhxInteractionLayers.AI;

	void Weapon_Base()
	{
		//m_DmgPerShot		= ConfigGetFloat("damagePerShot");
		m_BayonetAttached 	= false;
		m_ButtstockAttached = false;
		m_BayonetAttachmentIdx = -1;
		m_ButtstockAttachmentIdx = -1;
		m_BurstCount = 0;
		m_DOFProperties = new array<float>;
		
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
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( AssembleGun );
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
			SetNextMuzzleMode(GetCurrentMuzzle());
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
	}
	void ResetWeaponAnimState()
	{
		fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(this) + " resetting anim state: " + typename.EnumToString(PistolAnimState, m_weaponAnimState) + " --> " + typename.EnumToString(PistolAnimState, -1));
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
		
		#ifdef DEVELOPER
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
			SelectionBulletShow();
		else
			SelectionBulletHide();

		if (has_mag)
			ShowMagazine();
		else
			HideMagazine();
	}
	
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
	
	void ValidateAndRepair()
	{
		if ( m_fsm )
			m_fsm.ValidateAndRepair();
	}
	
	override void OnInventoryEnter(Man player)
	{
		m_PropertyModifierObject = null;

		ValidateAndRepair();
		
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
		
		/*if (ItemOptics.Cast(item))
		{
			PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
			if( player )
			{
				if( player.GetItemInHands() == this )
				{
					player.SetOpticsPreload(true,item);
				}
			}
		}*/
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		GetPropertyModifierObject().UpdateModifiers();
		
		/*if (ItemOptics.Cast(item))
		{
			PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
			if( player )
			{
				if( player.GetItemInHands() == this )
				{
					player.SetOpticsPreload(false,item);
				}
			}
		}*/
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
		super.OnItemLocationChanged(old_owner,new_owner);
		
		// HACK "resets" optics memory on optics
		PlayerBase player;
		if ( PlayerBase.CastTo(player,old_owner) )
		{ 
			player.SetReturnToOptics(false);
		}
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
	
	/*override bool CanReceiveAttachment(EntityAI attachment,int slotId)
	{
		if( !super.CanReleaseAttachment( attachment ) )
			return false;
		Magazine mag = Magazine.Cast(attachment);
		if(mag)
		{
			PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
			if( player )
			{
				if( player.GetItemInHands() == this )
				return true;
			}
			return false;
		}

		return true;
	}*/
	
	
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
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData;

			ctx.Write(INPUT_UDT_WEAPON_REMOTE_EVENT);
			e.WriteToContext(ctx);

			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(this) + " send 2 remote: sending e=" + e + " id=" + e.GetEventID() + " p=" + e.m_player + "  m=" + e.m_magazine); }
			p.StoreInputForRemotes(ctx);
		}
	}

	
	RecoilBase SpawnRecoilObject()
	{
		return new DefaultRecoil(this);
	}

	int GetWeaponSpecificCommand(int weaponAction, int subCommand) { return subCommand; }

	bool CanFire()
	{
		if (!IsChamberEmpty(GetCurrentMuzzle()) && !IsChamberFiredOut(GetCurrentMuzzle()) && !IsJammed() && !m_LiftWeapon)
			return true;
		return false;
	}
			
	bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if ( !optic )
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
	
	ref array<float> GetWeaponDOF()
	{
		return m_DOFProperties;
	}
	
	// lifting weapon on obstcles
	bool LiftWeaponCheck(PlayerBase player)
	{
		int idx;
		float distance;
		float hit_fraction; //junk
		vector start, end;
		vector direction;
		vector usti_hlavne_position;
		vector trigger_axis_position;
		vector hit_pos, hit_normal; //junk
		Object obj;
		ItemBase attachment;
		
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
		if ( player.GetInputController() && !player.GetInputController().IsWeaponRaised() )
			return false;
		
		usti_hlavne_position = GetSelectionPositionLS( "Usti hlavne" ); 	// Usti hlavne
		trigger_axis_position = GetSelectionPositionLS("trigger_axis");
		
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
		
		idx = player.GetBoneIndexByName("Neck"); //RightHandIndex1
		if ( idx == -1 )
			{ start = player.GetPosition()[1] + 1.5; }
		else
			{ start = player.GetBonePositionWS(idx); }
		
		//! snippet below measures distance from "RightHandIndex1" bone for lifting calibration
		/*usti_hlavne_position = ModelToWorld(usti_hlavne_position);
		distance = vector.Distance(start,usti_hlavne_position);*/
		distance = m_WeaponLength;// - 0.05; //adjusted raycast length

		// if weapon has battel attachment, does longer cast
		if (ItemBase.CastTo(attachment,FindAttachmentBySlotName("weaponBayonet")) || ItemBase.CastTo(attachment,FindAttachmentBySlotName("weaponBayonetAK")) || ItemBase.CastTo(attachment,FindAttachmentBySlotName("weaponBayonetMosin")) || ItemBase.CastTo(attachment,FindAttachmentBySlotName("weaponBayonetSKS")) || ItemBase.CastTo(attachment,GetAttachedSuppressor()))
		{
			distance += attachment.m_ItemModelLength;
		}
		end = start + (direction * distance);
		DayZPhysics.RayCastBullet(start, end, hit_mask, player, obj, hit_pos, hit_normal, hit_fraction);
		
		// something is hit
		if (hit_pos != vector.Zero)
		{
			//Print(distance);
			m_LiftWeapon = true;
			return true;
		}
		return false;
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
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnAmmo("", SAMF_DEFAULT);
	}
};

