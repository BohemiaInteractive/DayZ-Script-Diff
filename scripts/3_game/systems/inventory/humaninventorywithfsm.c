/**
 * @class	HumanInventoryWithFSM
 * @brief	HumanInventory... with FSM (synchronous, no anims)
 **/
class HumanInventoryWithFSM : HumanInventory
{
	protected ref HandFSM m_FSM;  /// hand slot state machine
	protected ref HandStateBase m_Empty;
	protected ref HandStateBase m_Equipped;

	void HumanInventoryWithFSM ()
	{
		m_FSM = new HandFSM();
	}
	
	void CreateStableStates ()
	{
		// stable states are created only if they do not exist already (for example created by derived class like DayZPlayerInventory)
		// @NOTE: this cannot be done in constructor, as there is NO owner set yet. GetManOwner() will therefore return NULL.
		if (!m_Empty)
			m_Empty = new HandStateEmpty(GetManOwner());
		if (!m_Equipped)
			m_Equipped = new HandStateEquipped(GetManOwner());
	}

	override void Init ()
	{
		// setup state machine
		hndDebugPrint("[hndfsm] Initializing Human Inventory FSM");
		
		// basic states
		CreateStableStates();
		
		// events
		HandEventBase __T__ = new HandEventTake;
		HandEventBase __D__ = new HandEventDrop;
		HandEventBase __Tw_ = new HandEventThrow;
		HandEventBase __M__ = new HandEventMoveTo;
		HandEventBase __W__ = new HandEventSwap;
		HandEventBase __F__ = new HandEventForceSwap;
		HandEventBase __X__ = new HandEventDestroy;
		HandEventBase __Xd_ = new HandEventDestroyed;
		HandEventBase __R__ = new HandEventDestroyAndReplaceWithNew;
		HandEventBase __Re_ = new HandEventDestroyAndReplaceWithNewElsewhere;
		HandEventBase __Rh_ = new HandEventDestroyElsewhereAndReplaceWithNewInHands;
		HandEventBase __Rd_ = new HandEventReplaced;
		HandEventBase __Cd_ = new HandEventCreated;
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;
		HandEventBase _abt_ = new HandEventHumanCommandActionAborted;


		HandReplacingItemInHands replacing = new HandReplacingItemInHands(GetManOwner());
		HandReplacingItemInHands replacingElsewhere = new HandReplacingItemInHands(GetManOwner());
		HandReplacingItemElsewhereWithNewInHands replacingElsewhere3 = new HandReplacingItemElsewhereWithNewInHands(GetManOwner());

		// setup transitions
		m_FSM.AddTransition(new HandTransition( m_Empty   , __T__,  m_Equipped, new HandActionTake, new HandGuardHasItemInEvent(GetManOwner())));
		m_FSM.AddTransition(new HandTransition( m_Empty   , __Cd_,  m_Equipped, new HandActionCreated, new HandGuardHasItemInEvent(GetManOwner())));

		m_FSM.AddTransition(new HandTransition(m_Equipped, __D__,    m_Empty, new HandActionDrop, null));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __Tw_,    m_Empty, new HandActionThrow, null));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __M__,    m_Empty, new HandActionMoveTo, new HandGuardAnd(HandGuardHasItemInHands(GetManOwner()), new HandGuardHasRoomForItem(GetManOwner()))));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __X__,    m_Empty, new HandActionDestroy, new HandGuardHasItemInHands(GetManOwner())));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __Xd_,    m_Empty, new HandActionDestroyed, new HandGuardHasDestroyedItemInHands(GetManOwner())));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __Xd_, m_Equipped, new HandActionDestroyed, new HandGuardNot(new HandGuardHasDestroyedItemInHands(GetManOwner()))));

		m_FSM.AddTransition(new HandTransition(m_Equipped, __R__, replacing));
		m_FSM.AddTransition(new HandTransition(replacing, _fin_, m_Equipped));
			replacing.AddTransition(new HandTransition(replacing.m_Replacing, _abt_, m_Equipped));

		m_FSM.AddTransition(new HandTransition(m_Equipped, __Re_, replacingElsewhere));
		m_FSM.AddTransition(new HandTransition(replacingElsewhere, _fin_, m_Empty));
			replacingElsewhere.AddTransition(new HandTransition(replacingElsewhere.m_Replacing, _abt_, m_Equipped));
		
		m_FSM.AddTransition(new HandTransition(m_Empty, __Rh_, replacingElsewhere3));
		m_FSM.AddTransition(new HandTransition(replacingElsewhere3, _fin_, m_Equipped));
			replacingElsewhere.AddTransition(new HandTransition(replacingElsewhere3.m_Replacing, _abt_, m_Empty));
		
		m_FSM.AddTransition(new HandTransition(m_Equipped, __Rd_, m_Equipped, new HandActionReplaced, null));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __W__, m_Equipped, new HandActionSwap, new HandGuardAnd(new HandGuardHasItemInHands(GetManOwner()), new HandGuardCanSwap(GetManOwner()))));
		m_FSM.AddTransition(new HandTransition(m_Equipped, __F__, m_Equipped, new HandActionForceSwap, new HandGuardAnd(new HandGuardHasItemInHands(GetManOwner()), new HandGuardCanForceSwap(GetManOwner()))));

		// start FSM
		m_FSM.SetInitialState(m_Empty);
		m_FSM.Start();
	}

	bool CanProcessHandEvents () { return m_FSM && m_FSM.IsRunning(); }

	/**@fn			ProcessHandEvent
	 * @brief		hand's fsm handling of events
	 * @NOTE:		warning: ProcessHandEvent can be called only within DayZPlayer::HandleInventory (or ::CommandHandler)
	 **/
	override bool ProcessHandEvent (HandEventBase e)
	{
		//SyncRemote(e);
		if (m_FSM.ProcessEvent(e) == ProcessEventResult.FSM_OK)
		{
			#ifdef DEVELOPER
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{
				Debug.InventoryHFSMLog("STS = " + GetManOwner().GetSimulationTimeStamp() , e.ToString(), "ProcessHandEvent", e.m_Player.ToString() );
			}
			#endif
			return true;
		}
		else
		{
			//hndDebugPrint("FSM refused to process event (no transition): src=" + GetCurrentState().ToString() + " event=" + e.ToString());
			return false;
		}
	}
	
	bool ProcessHandAbortEvent (HandEventBase e)
	{
		//SyncRemote(e);
		ProcessEventResult aa;
		m_FSM.ProcessAbortEvent(e, aa);
		if (aa == ProcessEventResult.FSM_OK)
		{
			hndDebugSpam("[hndfsm]  STS = " + GetManOwner().GetSimulationTimeStamp() + " Processed event e=" + e.ToString());
			return true;
		}
		else
		{
			//hndDebugPrint("FSM refused to process event (no transition): src=" + GetCurrentState().ToString() + " event=" + e.ToString());
			return false;
		}
	}

	override bool OnStoreLoad (ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		//@TODO
		//m_FSM.OnStoreLoad(ctx, version);

		return true;
	}

	override void OnStoreSave (ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
    //@TODO
		//m_FSM.OnStoreSave(ctx, version);
	}

	/**@fn			GetCurrentStateID
	 * @return		identifier of current stable state
	 **/
	int GetCurrentStateID () { return m_FSM.GetCurrentStateID(); }
	
	bool IsIdle () { return m_FSM.GetCurrentState().IsIdle(); }

	/**@fn			NetSyncCurrentStateID
	 * @brief		Engine callback - network synchronization of FSM's state. not intended to direct use.
	 **/
	void NetSyncCurrentStateID (int id)
	{
		m_FSM.NetSyncCurrentStateID(id);
	}
};

