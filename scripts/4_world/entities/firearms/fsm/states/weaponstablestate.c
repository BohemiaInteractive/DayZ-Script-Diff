/**@class		WeaponStableState
 * @brief	  represents weapon's stable state (i.e. the basic states that the weapon will spend the most time in)
 *
 * Stable states have associated animation states that they supposed to be in.
 * If they are not, the SetWeaponAnimState is called on the weapon in order to
 * set to required (configured) state
 *
 * @NOTE: OnExit from stable state, the weapon's animation state (@see
 * Weapon_Base::m_weaponAnimState) is set to -1 (unknown) In case of action
 * abort the final stable state is forced to set proper animation state
 * according to configuration (@see m_animState)
 **/

enum MuzzleState
{
	//! UNKNOWN
	U	= -1,
	//! EMPTY
	E	=  0,
	//! FIRED
	F	=  1,
	//! LOADED
	L	=  2
}

class WeaponStableState extends WeaponStateBase
{
	int m_animState;
	
	ref array<MuzzleState> m_muzzleHasBullet = new array<MuzzleState>();

	void WeaponStableState(Weapon_Base w = NULL, WeaponStateBase parent = NULL, int anim_state = -1)
	{
		m_animState = anim_state;
		InitMuzzleArray();
		ValidateMuzzleArray();
	}

	void SyncAnimState()
	{
		int curr = m_weapon.GetWeaponAnimState();
		if (curr != m_animState)
		{			
			//fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(m_weapon) + " synchronizing anim state: " + curr + " --> " + m_animState);
			DayZPlayer p;
			if (Class.CastTo(p, m_weapon.GetHierarchyParent()))
			{
				HumanCommandWeapons hcw = p.GetCommandModifier_Weapons();
				if (hcw)
				{
					hcw.SetInitState(m_animState);
					m_weapon.SetWeaponAnimState(m_animState);
					fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(m_weapon) + " state=" + m_weapon.GetCurrentState().Type() + " synchronized anim state: " + typename.EnumToString(PistolAnimState, curr) + " --> " + typename.EnumToString(PistolAnimState, m_animState));
				}
				else
				{
					Human wpnOwner = Human.Cast(m_weapon.GetHierarchyRootPlayer());
					HumanCommandWeapons.StaticSetInitState(wpnOwner, m_animState);
					m_weapon.SetWeaponAnimState(m_animState);
					fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(m_weapon) + " state=" + m_weapon.GetCurrentState().Type() + " synchronized remote anim state: " + typename.EnumToString(PistolAnimState, curr) + " --> " + typename.EnumToString(PistolAnimState, m_animState));
				}
			}		
		}
	}

	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);
		m_weapon.SetJammed(false);
		if (e)
			SyncAnimState();
	}
	override void OnUpdate(float dt)
	{
		super.OnUpdate(dt);
		SyncAnimState();
	}
	override void OnExit(WeaponEventBase e)
	{
		m_weapon.ResetWeaponAnimState();
		super.OnExit(e);
	}

	override bool IsIdle() { return true; }

	int GetCurrentStateID() { return 0; }
	
	/** \name Weapon state properties
	*	Properties defining the state in several properties for other systems
	*	Override these to set them up
	*
	*	Several systems depends on this:
	*	WeaponFSM.RandomizeFSMState, which picks out a suitable FSM state when attaching magazine through CE
	*	WeaponFSM.ValidateAndRepair, which will attempt to identify a desync and repair the FSM state if so
	*
	* @WARNING	So it is important to set these up correctly to prevent any desync of weapon FSM and actual wepon state
	*/
	//@{
	
	/**@brief Whether WeaponFSM.ValidateAndRepair should be applied on this state
	 * @NOTE:   This property was implemented at the same time as ValARep
	 *			To have it disabled by default
	 *			In case there are modders who created their own state
	 *			But potentially did not set up their properties correctly
	 *			As having ValARep run on an improper setup would result in horrible VME spam and desync
	 * @WARNING:	When enabling repair, it is imperative that all properties are set up correctly.
	 **/
	bool IsRepairEnabled() { return false; }
	
	/**@brief Whether there is a bullet in the chamber
	 * @NOTE:   This should only be false when it is empty
	 *			So this is true when there is a bullet in the chamber
	 *			Regardless of the bullet being loaded, firedout or jammed
	 **/
    bool HasBullet() { return false; }
	
	//! Whether there is a magazine attached
	bool HasMagazine() { return false; }
	
	//! Whether the gun is jammed
	bool IsJammed() { return false; }
	
	//! Override with the filling of m_muzzleHasBullet
	void InitMuzzleArray() { m_muzzleHasBullet = { MuzzleState.U }; }
	
	//! Special one for when the weapon only has one singular state (like Magnum)
	bool IsSingleState() { return false; }
	
	//@}
	
	/** \name Weapon state properties helpers
	*	Several helpers for better access of certain properties
	*/
	//@{
	
	//! Get chamber state of the muzzle at index
	MuzzleState GetMuzzleState(int idx) { return m_muzzleHasBullet[idx]; }	
	int GetMuzzleStateCount() { return m_muzzleHasBullet.Count(); }	
	bool IsChamberValid(int idx) { return m_muzzleHasBullet[idx] != MuzzleState.U; }
	bool IsChamberFiredOut(int idx) { return m_muzzleHasBullet[idx] == MuzzleState.F; }
	bool IsChamberFull(int idx) { return m_muzzleHasBullet[idx] != MuzzleState.E; }
	
	//@}
	
	//! Safety check and error message in case not set up correctly
	void ValidateMuzzleArray()
	{
		// There is only one
		if (IsSingleState())
			return;
		
		bool failed = false;
		
		if (!failed)
		{
			foreach (MuzzleState state : m_muzzleHasBullet)
			{
				if ( state == MuzzleState.U )
				{
					failed = true;
					break;
				}
			}
		}
		
		if (failed)
		{
			ErrorExSeverity severity = ErrorExSeverity.ERROR;
			
			#ifdef DEVELOPER
			if (DayZGame.m_IsPreviewSpawn)
			{
				// we do not want VMEs when spawning the item in order to visualize it for preview in script console
				severity = ErrorExSeverity.INFO;
			}
			#endif
			
			ErrorEx("Muzzle array validation has failed. Please set up the correct muzzle states by overriding InitMuzzleArray.", severity);
		}
	}
};


