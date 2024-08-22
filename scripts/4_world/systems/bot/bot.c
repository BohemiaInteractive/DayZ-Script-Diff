/**@class		BotTrigger
 **/
class BotTrigger
{
	bool CheckTrigger () { return false; }
};

class MyBotTrigger : BotTrigger
{
	PlayerBase m_Owner;
	void MyBotTrigger (PlayerBase p) { m_Owner = p; }
	override bool CheckTrigger () { return null != m_Owner.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Headgear")); }
};


/**@class		Bot
 **/
class Bot
{
	PlayerBase m_Owner = null;
	protected ref BotFSM m_FSM = null;
	protected DayZPlayerInstanceType m_InstanceType = DayZPlayerInstanceType.INSTANCETYPE_CLIENT;

	protected ref BotStateBase m_IdleState = null;

	void Bot (PlayerBase ow)
	{
		m_Owner = ow;
	}
	
	void SetInstanceType (DayZPlayerInstanceType t) { m_InstanceType = t; }

	void Start ()
	{
		InitFSM();
	}

	void Stop ()
	{
		m_FSM.Terminate();
	}

	void StartAction(int action)
	{
		switch (action)
		{
		case EActions.PLAYER_BOT_STOP_CURRENT:
			ProcessEvent(new BotEventStop(m_Owner, null));
			return;
		}

		ProcessEvent(new BotEventStartDebug(m_Owner, null, action));
	}

	void OnUpdate (float dt)
	{
		m_FSM.GetCurrentState().OnUpdate(dt);
	}

	// events
	ref BotEventBase ___Bgn__ = new BotEventStart;
	ref BotEventBase __Stop__ = new BotEventStop;
	ref BotEventBase ___OK___ = new BotEventEndOK;
	ref BotEventBase __Fail__ = new BotEventEndFail;
	ref BotEventBase __Tout__ = new BotEventEndTimeout;
	ref BotEventBase __IChg__ = new BotEventOnItemInHandsChanged;

	void AddTransition(BotStateBase state, int id)
	{
		BotEventStartDebug evt = new BotEventStartDebug(null, null, id);

		m_FSM.AddTransition(new BotTransition(m_IdleState, evt, state, null, new BotGuardDebugEventMatches(evt)));

		m_FSM.AddTransition(new BotTransition(state, ___OK___, m_IdleState));
		m_FSM.AddTransition(new BotTransition(state, __Fail__, m_IdleState));
		m_FSM.AddTransition(new BotTransition(state, __Tout__, m_IdleState));

		m_FSM.AddTransition(new BotTransition(state, __Stop__, m_IdleState));
	}

	void InitFSM ()
	{
		m_FSM = new BotFSM();

		// basic states
		m_IdleState = new BotStateIdle(this, NULL);

		///@{ transition table

		AddTransition(new BotStanceRandomizer(this, NULL), EActions.PLAYER_BOT_RANDOMIZE_STANCE);
		AddTransition(new BotTestSpamUserActions(this, NULL), EActions.PLAYER_BOT_SPAM_USER_ACTIONS);
		AddTransition(new BotTestAttachAndDropCycle(this, NULL), EActions.PLAYER_BOT_TEST_ATTACH_AND_DROP_CYCLE);
		AddTransition(new BotTestItemMoveBackAndForth(this, NULL), EActions.PLAYER_BOT_TEST_ITEM_MOVE_BACK_AND_FORTH);
		AddTransition(new Bot_TestSpawnOpen(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN);
		AddTransition(new Bot_TestSpawnOpenDestroy(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN_DESTROY);
		AddTransition(new Bot_TestSpawnOpenEat(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN_EAT);
		AddTransition(new BotTestSwapG2H(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_G2H);
		//AddTransition(new BotTestSwapC2H(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_C2H);
		AddTransition(new BotTestSwapInternal(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_INTERNAL);

		///@} transition table

		m_FSM.SetInitialState(m_IdleState);
		m_FSM.Start();
	}
	
	bool ProcessEvent (BotEventBase e)
	{
		if (m_FSM.ProcessEvent(e) == ProcessEventResult.FSM_OK)
		{
			botDebugSpam("[botfsm] Processed event e=" + e.ToString());
			return true;
		}
		else
		{
			botDebugSpam("[botfsm] FSM refused to process event (no transition): src=" + m_FSM.GetCurrentState().ToString() + " event=" + e.ToString());
			return false;
		}
	}
};

void botDebugPrint (string s)
{
#ifdef BOT_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

void botDebugSpam (string s)
{
#ifdef BOT_DEBUG_SPAM
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

