/**@class		WeaponFSM
 * @brief		weapon finite state machine
 **/
class WeaponFSM extends HFSMBase<WeaponStateBase, WeaponEventBase, WeaponActionBase, WeaponGuardBase>
{
	private static const int MAX_SYNCHRONIZE_ATTEMPTS = 12;
	private static const int MIN_SYNCHRONIZE_INTERVAL = 3000; // ms
	private static const int RESET_SYNCHRONIZE_THRESHOLD = 3600000; // ms
	private int m_SynchronizeAttempts;
	private int m_LastSynchronizeTime;
	
	protected int m_NextStateId = 0; /// counter for InternalID: each state in a fsm is assigned an unique number
	protected ref array<WeaponStateBase> m_UniqueStates = new array<WeaponStateBase>; /// unique list of states in this machine (automation of save/load)

	protected void SetInternalID(WeaponStateBase state)
	{
		if (state && state.GetInternalStateID() == -1)
		{
			state.SetInternalStateID(m_NextStateId);

			//if (LogManager.IsWeaponLogEnable()) { wpnDebugSpam("[wpnfsm] " + Object.GetDebugName(m_weapon) + " unique state=" + state + " has id=" + m_NextStateId); }
			m_UniqueStates.Insert(state);
			++m_NextStateId;
		}
	}

	/**@fn		AddTransition
	 * @brief	adds transition into transition table
	 * As a side effect registers the state(s) into m_UniqueStates
	 **/
	override void AddTransition(FSMTransition<WeaponStateBase, WeaponEventBase, WeaponActionBase, WeaponGuardBase> t)
	{
		super.AddTransition(t);

		SetInternalID(t.m_srcState);
		SetInternalID(t.m_dstState);
	}
	
	override protected ProcessEventResult ProcessLocalTransition(FSMTransition<WeaponStateBase, WeaponEventBase, WeaponActionBase, WeaponGuardBase> t, WeaponEventBase e)
	{
		ProcessEventResult res = super.ProcessLocalTransition(t, e);
		ValidateAndRepair();
		return res;
	}
	
	override WeaponStateBase ProcessAbortEvent(WeaponEventBase e, out ProcessEventResult result)
	{
		WeaponStateBase res = super.ProcessAbortEvent(e, result);
		ValidateAndRepair();
		return res;
	}
	
	override protected ProcessEventResult ProcessAbortTransition(FSMTransition<WeaponStateBase, WeaponEventBase, WeaponActionBase, WeaponGuardBase> t, WeaponEventBase e)
	{
		ProcessEventResult res = super.ProcessAbortTransition(t, e);
		ValidateAndRepair();
		return res;
	}

	/**@fn		FindStateForInternalID
	 * @brief	retrieve base state that matches given internal id
	 * @param[in]	id	the id stored in database during save
	 **/
	WeaponStateBase FindStateForInternalID(int id)
	{
		int state_count = m_UniqueStates.Count();
		for (int idx = 0; idx < state_count; ++idx)
		{
			int state_id = m_UniqueStates.Get(idx).GetInternalStateID();
			if (state_id == id)
				return m_UniqueStates.Get(idx);
		}
		return null;
	}

	/**@fn		FindStableStateForID
	 * @brief	load from database - reverse lookup for state from saved id
	 * @param[in]	id	the id stored in database during save
	 **/
	WeaponStableState FindStableStateForID(int id)
	{
		if (id == 0)
			return null;

		int count = m_Transitions.Count();
		for (int i = 0; i < count; ++i)
		{
			WeaponTransition trans = m_Transitions.Get(i);
			WeaponStableState state = WeaponStableState.Cast(trans.m_srcState);
			if (state && id == state.GetCurrentStateID())
				return state;
		}
		return null;
	}

	protected bool LoadAndSetCurrentFSMState(ParamsReadContext ctx, int version)
	{
		int curr_state_id = -1;
		if (!ctx.Read(curr_state_id))
		{
			Error("[wpnfsm] LoadCurrentFSMState - cannot read current state");
			return false;
		}

		WeaponStateBase state = FindStateForInternalID(curr_state_id);
		if (state)
		{
			Terminate();
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] synced current state=" + state + " id=" + curr_state_id); }	
			m_State = state;	
			Start(null, true);
			return true;
		}
		else
			Error("[wpnfsm] LoadCurrentFSMState - cannot find state for id=" + curr_state_id);
		return false;

	}

	/**@fn		LoadCurrentFSMState
	 * @brief	load current state of fsm
	 **/
	bool LoadCurrentFSMState(ParamsReadContext ctx, int version)
	{
		if (LoadAndSetCurrentFSMState(ctx, version))
		{
			bool res = m_State.LoadCurrentFSMState(ctx, version);
			if (LogManager.IsWeaponLogEnable()) { wpnDebugSpam("[wpnfsm] LoadCurrentFSMState - loaded current state=" + GetCurrentState()); }
			return res;
		}
		return false;
	}
	
	bool LoadCurrentUnstableFSMState(ParamsWriteContext ctx, int version)
	{
		if (LoadAndSetCurrentFSMState(ctx, version))
		{
			// read all substates
			int state_count = m_UniqueStates.Count();
			for (int idx = 0; idx < state_count; ++idx)
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugSpam("[wpnfsm] LoadCurrentUnstableFSMState " + idx + "/" + state_count + " id=" + m_UniqueStates.Get(idx).GetInternalStateID() + " state=" + m_UniqueStates.Get(idx)); }
				if (!m_UniqueStates.Get(idx).LoadCurrentFSMState(ctx, version))
					Error("[wpnfsm] LoadCurrentUnstableFSMState - cannot load unique state " + idx + "/" + state_count + " with id=" + m_UniqueStates.Get(idx).GetInternalStateID() + " state=" + m_UniqueStates.Get(idx));
			}
			return true;
		}
		return false;
	}

	/**@fn		SaveCurrentFSMState
	 * @brief	save current state of fsm
	 **/
	bool SaveCurrentFSMState(ParamsWriteContext ctx)
	{
		WeaponStateBase state = GetCurrentState();
		int curr_state_id = state.GetInternalStateID();
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] SaveCurrentFSMState - saving current state=" + GetCurrentState() + " id=" + curr_state_id); }

		if (!ctx.Write(curr_state_id))
		{
			Error("[wpnfsm] SaveCurrentFSMState - cannot save curr_state_id=" + curr_state_id);
			return false;
		}

		// write only current state
		if (!state.SaveCurrentFSMState(ctx))
		{
			Error("[wpnfsm] SaveCurrentFSMState - cannot save currrent state=" +state);
			return false;
		}
		return true;
	}
	
	bool SaveCurrentUnstableFSMState(ParamsWriteContext ctx)
	{
		WeaponStateBase state = GetCurrentState();
		int curr_state_id = state.GetInternalStateID();
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] SaveCurrentUnstableFSMState - saving current state=" + GetCurrentState() + " id=" + curr_state_id); }
		
		if (!ctx.Write(curr_state_id))
		{
			Error("[wpnfsm] SaveCurrentFSMState - cannot save curr_state_id=" + curr_state_id);
			return false;
		}

		// write all substates
		int state_count = m_UniqueStates.Count();
		for (int idx = 0; idx < state_count; ++idx)
		{
			int state_id = m_UniqueStates.Get(idx).GetInternalStateID();
			if (state_id != -1)
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugSpam("[wpnfsm] SaveCurrentUnstableFSMState " + idx + "/" + state_count + " id=" + state_id + " name=" + m_UniqueStates.Get(idx)); }
				if (!m_UniqueStates.Get(idx).SaveCurrentFSMState(ctx))
					Error("SaveCurrentUnstableFSMState - cannot save unique state=" + m_UniqueStates.Get(idx) + " idx=" + idx + "/" + state_count + " with id=" + state_id);
			}
			else
				Error("[wpnfsm] SaveCurrentUnstableFSMState state=" + m_UniqueStates.Get(idx) + " with unassigned ID!");
		}
		return true;
	}
	
    /**@fn		ValidateAndRepair
	 * @brief	validate the state of the gun and repair if mismatch
	 **/
	void ValidateAndRepair()
	{
		Internal_ValidateAndRepair();
	}
	
	/**@fn		Internal_ValidateAndRepair
	 * @brief	validate the state of the gun and repair if mismatch
	 * @return	bool whether it performed repairing or not
	 **/
	protected bool Internal_ValidateAndRepair()
	{
		bool repaired = false;
		
		// Only repair stable states
		WeaponStableState state = WeaponStableState.Cast(m_State);				
		if (state && state.IsRepairEnabled())
		{		
			Weapon_Base weapon = state.m_weapon;
			if (weapon)
			{				
				repaired |= ValidateAndRepairHelper(weapon,
						"MagazineRepair",
						state.HasMagazine(), ( weapon.GetMagazine(0) != null ),
						new WeaponEventAttachMagazine, new WeaponEventDetachMagazine,
						state);
				
				repaired |= ValidateAndRepairHelper(weapon,
						"JammedRepair",
						state.IsJammed(), weapon.IsJammed(),
						new WeaponEventTriggerToJam, new WeaponEventUnjam,
						state);
				
				if (weapon.IsJammed())
					return repaired;
				
				int nMuzzles = weapon.GetMuzzleCount();
				switch (nMuzzles)
				{
					case 1:
					{
						repaired |= ValidateAndRepairHelper(weapon,
							"ChamberFiredRepair",
							state.IsChamberFiredOut(0), weapon.IsChamberFiredOut(0),
							new WeaponEventTrigger, new WeaponEventMechanism,
							state);
					
						repaired |= ValidateAndRepairHelper(weapon,
							"ChamberRepair",
							state.IsChamberFull(0), weapon.IsChamberFull(0),
							new WeaponEventLoad1Bullet, new WeaponEventMechanism,
							state);
						
						break;
					}
					default:
					{		
						for (int i = 0; i < nMuzzles; ++i)
						{
							repaired |= ValidateAndRepairHelper(weapon,
								"ChamberFiredRepair",
								state.IsChamberFiredOut(i), weapon.IsChamberFiredOut(i),
								null, null, // A bit brute forced, not really any clean way to transition
								state);
					
							repaired |= ValidateAndRepairHelper(weapon,
								"ChamberRepair",
								state.IsChamberFull(i), weapon.IsChamberFull(i),
								null, null, // A bit brute forced, not really any clean way to transition
								state);
						}
					
						break;
					}
				}
			}
		}

		return repaired;
	}
	
	protected bool ValidateAndRepairHelper(Weapon_Base weapon, string name, bool stateCondition, bool gunCondition, WeaponEventBase e1, WeaponEventBase e2, out WeaponStableState state)
	{
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + weapon.GetDebugName(weapon) + " ValidateAndRepair - " + name + " - " + m_State + " - state: " + stateCondition + " & weapon: " + gunCondition); }
		
		if (stateCondition != gunCondition)
		{
			WeaponStableState repairedState;
			
			// Seeing this message is not TOO bad, it just means this system is working
			// It is simply being listed in the logs to identify how much the FSM state and weapon state still desyncs
			// Which can be because of a myriad of causes, such as incorrectly set up transitions
			// Or simply certain timings of certain actions or interrupts lined up perfectly, which can have repro rates such as 1/300
			Error(string.Format("[wpnfsm] ValidateAndRepair Attempting to repair: %1 - %2 - %3 - state: %4 != weapon: %5",
				weapon.GetDebugName(weapon), name, m_State, stateCondition, gunCondition));
			
			if (e1 && e2)
				repairedState = ValidateAndRepairStateFinder(gunCondition, e1, e2, state);
			
			if (repairedState)
			{
				Terminate();
				m_State = repairedState;
				Start(null, true);
				state = repairedState;
				weapon.SyncSelectionState(state.HasBullet(), state.HasMagazine());
				repairedState.SyncAnimState();

				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + weapon.GetDebugName(weapon) + " ValidateAndRepair - " + name + " - Result - " + m_State); }
				return true;
			}
			else
			{
				// Last ditch effort
				if (m_SynchronizeAttempts < MAX_SYNCHRONIZE_ATTEMPTS)
				{
					int currentTime = g_Game.GetTime();
					int timeDiff = currentTime - m_LastSynchronizeTime;
					
					// Careful with calling synchronize
					if (timeDiff > MIN_SYNCHRONIZE_INTERVAL)
					{
						// If a lot of time passed since last attempt
						// There is a possibility the weapon was fixed for a period
						if (timeDiff > RESET_SYNCHRONIZE_THRESHOLD)
							m_SynchronizeAttempts = 0;
						
						// Only call this on server or in SP
						// Synchronize will ask the server for its FSM state anyways
						if (g_Game.IsServer())
							weapon.RandomizeFSMState();
						
						weapon.Synchronize();
							
						++m_SynchronizeAttempts;
						m_LastSynchronizeTime = currentTime;
					}
				}
				else
				{
					OnFailThresholdBreached(weapon, name, stateCondition, gunCondition);
				}
			}
		}
		
		return false;
	}
	
	protected void OnFailThresholdBreached(Weapon weapon, string name, bool stateCondition, bool gunCondition)
	{
		// Now seeing THIS one, after the one above, THIS one CAN be bad
		// As the state was identified as being desynced with the actual weapon state
		// But the system was unable to fix it, so the weapon is now working improperly or not at all
		// There is even the possibility that this weapon is now permanently broken
		Error(string.Format("[wpnfsm] %1 ValidateAndRepair THRESHOLD BREACH - %2 - %3 - state: %4 != weapon: %5",
					weapon.GetDebugName(weapon), name, m_State, stateCondition, gunCondition));
		
		// At this point might even consider just deleting the weapon :c
	}
	
	protected WeaponStableState ValidateAndRepairStateFinder(bool condition, WeaponEventBase e1, WeaponEventBase e2, WeaponStableState state)
	{
		WeaponStateBase interState;
		if (condition)
			interState = FindTransitionState(state, e1);
		else
			interState = FindTransitionState(state, e2);
			
		WeaponEventBase e = new WeaponEventHumanCommandActionFinished;
		return WeaponStableState.Cast(FindGuardedTransitionState(interState, e));
	}
	
	/**@fn		OnStoreLoad
	 * @brief	load state of fsm
	 **/
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		int id = 0;
		ctx.Read(id);
		WeaponStableState state = FindStableStateForID(id);
		if (state)
		{
			Terminate();
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] OnStoreLoad - loading current state=" + state + " id=" + id); }	
			m_State = state;
			Start(null, true);
		}
		else
		{
			Print("[wpnfsm] Warning! OnStoreLoad - cannot load curent weapon state, id=" + id);
		}

		return true;
	}

	/**@fn		GetCurrentStateID
	 * @brief	tries to return identifier of current stable state
	 *
	 * if the weapon is in stable state, i.e. not reloading, firing, detaching, ... than
	 * the identifier of the stable state is used directly.
	 * otherwise if the weapon is in unstable state, than the function uses abort information
	 * to determine the closest stable state that is coherent with weapon's state.
	 *
	 * @return	integer id that will be stored to database
	 **/
	int GetCurrentStableStateID()
	{
		// 1) if current state is stable state then return ID directly
		WeaponStableState state = WeaponStableState.Cast(GetCurrentState());
		if (state)
			return state.GetCurrentStateID();

		// 2) otherwise find closest stable state (by looking on abort event)
		WeaponStateBase abort_dst = FindAbortDestinationState(new WeaponEventHumanCommandActionAborted(null));
		WeaponStableState closest_stable_state = WeaponStableState.Cast(abort_dst);
		
		if (closest_stable_state)
		{
			Print("[wpnfsm] Save occured in fsm transition! current state=" + GetCurrentState() + " closes stable state=" + closest_stable_state + " id=" + closest_stable_state.GetCurrentStateID());
			return closest_stable_state.GetCurrentStateID();
		}		

		Print("[wpnfsm] Warning! Save occured in fsm transition! GetCurrentStateID - cannot find closest weapon stable state.");
		return 0;
	}

	/**@fn		GetCurrentStateID
	 * @brief	return internal identifier of current state
	 **/
	int GetInternalStateID()
	{
		WeaponStateBase curr = GetCurrentState();
		int id = 0;
		if (curr)
			id = curr.GetInternalStateID();
		return id;
	}

	/**@fn		OnStoreSave
	 * @brief	save state of fsm
	 **/
	void OnStoreSave(ParamsWriteContext ctx)
	{
		int id = GetCurrentStableStateID();
		if (LogManager.IsWeaponLogEnable()) { wpnDebugSpamALot("[wpnfsm] OnStoreSave - saving current state=" + GetCurrentState() + " id=" + id); }
		ctx.Write(id);
	}

	/**@fn		RandomizeFSMState
	 * @brief	Deprecated, use RandomizeFSMStateEx for better results
	 **/
	void RandomizeFSMState(bool hasBullet, bool hasMagazine, bool isJammed)
	{
		array<MuzzleState> muzzleStates;
		if (hasBullet)
			muzzleStates = { MuzzleState.L };
		else
			muzzleStates = { MuzzleState.E };
		
		RandomizeFSMStateEx(muzzleStates, hasMagazine, isJammed);
	}
	
	/**@fn		RandomizeFSMStateEx
	 * @brief	With the parameters given, selects a random suitable state for the FSM of the weapon
	 * @NOTE:	It is better to use Weapon_Base.RandomizeFSMState instead of calling this one
	 * @WARNING:	Weapon_Base.Synchronize call might be needed, if this method is called while clients are connected
	 **/
	void RandomizeFSMStateEx(array<MuzzleState> muzzleStates, bool hasMagazine, bool isJammed)
	{
		array<WeaponStableState> candidates = new array<WeaponStableState>;
		int tc = m_Transitions.Count();
		foreach (WeaponTransition trans : m_Transitions)
		{
			WeaponStableState state = WeaponStableState.Cast(trans.m_srcState);
			if (state && state.HasMagazine() == hasMagazine && state.IsJammed() == isJammed)
			{
				if (state.IsSingleState())
				{
					// There is only one, insert it and stop
					candidates.Insert(state);
					break;
				}
				
				int nMuzzles = muzzleStates.Count();
				int nMuzzlesState = state.GetMuzzleStateCount();
				if (nMuzzles != nMuzzlesState)
				{
					ErrorEx(string.Format("Number of muzzles on the weapon (%1) does not correspond with what state has configured (%2).", nMuzzles, nMuzzlesState));
					continue;
				}
				
				bool equal = true;
				for (int i = 0; i < nMuzzles; ++i)
				{
					if (muzzleStates[i] != state.GetMuzzleState(i))
					{
						equal = false;
						break;
					}
				}

				if (equal)
					candidates.Insert(state);
			}
		}

		int cc = candidates.Count();
		if (cc)
		{
			int randomIndex = Math.RandomInt(0, cc);
			WeaponStableState selected = candidates.Get(randomIndex);
			Terminate();
			m_State = selected;
			if (!Internal_ValidateAndRepair())
				Start(null, true);
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] RandomizeFSMState - randomized current state=" + m_State + " id=" + selected.GetCurrentStateID()); }
			selected.SyncAnimState();
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] RandomizeFSMState - warning - cannot randomize, no states available"); }
		}
	}
};

