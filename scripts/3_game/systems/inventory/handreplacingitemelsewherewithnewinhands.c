class HandStartReplacingItemElsewhereWithNewInHands extends HandStateBase
{
	void HandStartReplacingItemElsewhereWithNewInHands (Man player = NULL, HandStateBase parent = NULL)
	{ }

	override void OnEntry (HandEventBase e)
	{
		super.OnEntry(e);
		
		Man player = e.m_Player;
		HandEventReplaceWithNewBase edr = HandEventReplaceWithNewBase.Cast(e);
		if (edr)
		{
			hndDebugPrint("[hndfsm] HandStartReplacingItemElsewhereWithNewInHands about to execute lambda");

			HumanInventoryWithFSM inv = HumanInventoryWithFSM.Cast(player.GetInventory());
			edr.m_Lambda.Execute(inv);
			return;
		}
		else
			Error("[hndfsm] HandStartReplacingItemElsewhereWithNewInHands - not a HandEvenReplaceWithNewBase event");
	}

	override void OnAbort (HandEventBase e)
	{
		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		super.OnExit(e);
	}

	override bool IsWaitingForActionFinish () { return true; }
};


class HandReplacingItemElsewhereWithNewInHands extends HandStateBase
{
	ref HandStartReplacingItemElsewhereWithNewInHands m_Replacing;

	void HandReplacingItemElsewhereWithNewInHands (Man player = NULL, HandStateBase parent = NULL)
	{
		// setup nested state machine
		m_Replacing = new HandStartReplacingItemElsewhereWithNewInHands(player, this);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Replacing, _fin_,     NULL ));

		m_FSM.SetInitialState(m_Replacing);
	}
};


