class HandStartReplacingItemInHands extends HandStateBase
{
	void HandStartReplacingItemInHands (Man player = NULL, HandStateBase parent = NULL)
	{ }

	override void OnEntry (HandEventBase e)
	{
		super.OnEntry(e);
		
		Man player = e.m_Player;
		EntityAI itemInHands = player.GetHumanInventory().GetEntityInHands();

		InventoryLocation src = new InventoryLocation;
		if (itemInHands.GetInventory().GetCurrentInventoryLocation(src))
		{
			HandEventDestroyAndReplaceWithNew edr = HandEventDestroyAndReplaceWithNew.Cast(e);
			if (edr)
			{
				hndDebugPrint("[hndfsm] HandStartReplacingItemInHands about to execute lambda");

				HumanInventoryWithFSM inv = HumanInventoryWithFSM.Cast(player.GetInventory());
				edr.m_Lambda.Execute(inv);
				//player.GetItemAccessor().OnItemInHandsChanged();
				return;
			}
			else
				Error("[hndfsm] HandStartReplacingItemInHands - not a HandEventDestroyAndReplaceWithNew event");
		}
		else
			Error("[hndfsm] HandStartReplacingItemInHands - itemInHands has no InventoryLocation");
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


class HandReplacingItemInHands extends HandStateBase
{
	ref HandStartReplacingItemInHands m_Replacing;

	void HandReplacingItemInHands (Man player = NULL, HandStateBase parent = NULL)
	{
		// setup nested state machine
		m_Replacing = new HandStartReplacingItemInHands(player, this);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Replacing, _fin_,     NULL ));

		m_FSM.SetInitialState(m_Replacing);
	}

	override void OnEntry (HandEventBase e)
	{
		super.OnEntry(e);
	}

	override void OnAbort (HandEventBase e)
	{
		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		super.OnExit(e);
	}
};


