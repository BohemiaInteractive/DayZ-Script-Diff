/**@class	HandStateBase
 * @brief	represent hand state base
 *
 * Class comes with entry/update/exit hooks that can be overriden in custom states
 **/
class HandStateBase
{
	Man m_Player; /// entity that this state relates to
	HandStateBase m_parentState; /// hierarchical parent state of this state (or null)
	ref HandFSM m_FSM; /// nested state machine (or null)
	void HandStateBase (Man player = NULL, HandStateBase parent = NULL) { m_Player = player; m_parentState = parent; }

	/**@fn		SetParentState
	 * @brief	allows construction of hierarchical state machine
	 **/
	void SetParentState (HandStateBase parent) { m_parentState = parent; }
	/**@fn		GetParentState
	 * @return	state that owns this sub-state (or null if plain state)
	 **/
	HandStateBase GetParentState () { return m_parentState; }

	bool HasFSM () { return m_FSM != NULL; }
	HandFSM GetFSM () { return m_FSM; }

	bool ProcessEvent (HandEventBase e)
	{
		if (HasFSM())
			return m_FSM.ProcessEvent(e);
		return false;
	}

	/**@fn		AddTransition
	 * @brief	adds transition into m_FSM transition table
	 **/
	void AddTransition (HandTransition t)
	{
		if (HasFSM())
			m_FSM.AddTransition(t);
		else
			Error("[hndfsm] adding transition to state without FSM. Configure FSM first.");
	}


	/**@fn		OnEntry
	 * @brief	called upon entry to state
	 * @NOTE	if state has (non-running) sub-machine, it's started on entry
	 * @param[in] e	the event that triggered transition to this state
	 **/
	void OnEntry (HandEventBase e)
	{
		if (HasFSM() && !m_FSM.IsRunning())
		{
			if (e)
				hndDebugPrint("[hndfsm] { " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " " + this.Type().ToString() + "  Has Sub-FSM! Starting submachine...");
			else
			hndDebugPrint("[hndfsm] { " + this.Type().ToString() + "  Has Sub-FSM! Starting submachine...");
			m_FSM.Start(e);
		}
		else
		{
			if (e)
				hndDebugPrint("[hndfsm] { " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " " + this.Type().ToString());
		else
			hndDebugPrint("[hndfsm] { " + this.Type().ToString());
	}
	}

	/**@fn		OnUpdate
	 * @brief ongoing behavior, performed while being in the state
	 *
	 * @NOTE: this is supposed to be the Do() operation in UML speak
	 **/
	void OnUpdate (float dt)
	{
		if (HasFSM() && m_FSM.IsRunning())
			m_FSM.GetCurrentState().OnUpdate(dt);
	}

	/**@fn		OnAbort
	 * @brief called when abort signal arrives
	 * @param[in] e	the event that triggered abort from this state
	 **/
	void OnAbort (HandEventBase e)
	{
		if (HasFSM() && m_FSM.IsRunning())
		{
			hndDebugPrint("[hndfsm] OnAbort " + this.Type().ToString() + "  Has Sub-FSM! Aborting submachine...");
			m_FSM.Abort(e);
		}
		//Debug.InventoryHFSMLog("ABORTED " + e.m_Player.GetSimulationTimeStamp(), ""/*typename.EnumToString(HandEventID, GetEventID()) */, "n/a", "OnAbort", m_Player.ToString() );
		hndDebugPrint("[hndfsm] } " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " ABORTED " + this.Type().ToString());
	}

	/**@fn		OnExit
	 * @brief called on exit from state
	 * @param[in] e	the event that triggered transition from this state
	 **/
	void OnExit (HandEventBase e)
	{
		hndDebugPrint("[hndfsm] } " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " " + this.Type().ToString());
	}

	/**@fn			IsWaitingForActionFinish
	 * @brief		waiting for active animation action/actionType finish
	 * @return		true if this state is waiting for finish signal
	 **/
	bool IsWaitingForActionFinish () { return HasFSM() && m_FSM.IsRunning() && m_FSM.GetCurrentState().IsWaitingForActionFinish(); }

	/**@fn			IsIdle
	 * @brief		idle state does not expect any animation events
	 * @return		true if this state is idle
	 **/
	bool IsIdle () { return false; }

	/**@fn		OnSubMachineChanged
	 * @brief	called when sub-machine has changed its state
	 * @param[in]	src		from state (previous)
	 * @param[in]	dst		to state (current)
	 **/
	void OnSubMachineChanged (HandStateBase src, HandStateBase dst) { }

	/**@fn		OnStateChanged
	 * @brief	called on current state when state machine has changed its state
	 * @param[in]	src		from state (previous)
	 * @param[in]	dst		to state (current)
	 **/
	void OnStateChanged (HandStateBase src, HandStateBase dst)
	{
		m_Player.GetHumanInventory().OnHandsStateChanged(src, dst);
	}
};



