class HandStartHidingAnimated extends HandStartAction
{
	void HandStartHidingAnimated (Man player = NULL, HandStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{ }

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
	
	override bool IsWaitingForActionFinish () { return m_ActionType == -1; }
};


class HandAnimatedMoveToDst_W4T extends HandStartAction
{
	ref InventoryLocation m_Dst;

	override void OnEntry (HandEventBase e)
	{
		Man player = e.m_Player;
		EntityAI item = m_Dst.GetItem();

		if (m_Dst && m_Dst.IsValid())
		{
			InventoryLocation src = new InventoryLocation;
			if (item.GetInventory().GetCurrentInventoryLocation(src))
			{
				GameInventory.LocationSyncMoveEntity(src, m_Dst);

				player.OnItemInHandsChanged();
			}
			else
				Error("[hndfsm] " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " HandAnimatedMoveToDst_W4T- item " + item + " has no Inventory or Location, inv=" + item.GetInventory());
		}
		else
			Error("[hndfsm] HandAnimatedMoveToDst_W4T - event has no valid m_Dst");

		super.OnEntry(e);
	}

	override void OnAbort (HandEventBase e)
	{
		m_Dst = NULL;
		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		m_Dst = NULL;
		super.OnExit(e);
	}

	override bool IsWaitingForActionFinish () { return true; }
};

class HandAnimatedMovingToAtt extends HandStateBase
{
	EntityAI m_Entity; /// weapon to be taken

	ref HandStartHidingAnimated m_Hide;
	ref HandAnimatedMoveToDst_W4T m_Show;
	
	ref InventoryLocation m_ilEntity;

	void HandAnimatedMovingToAtt (Man player = NULL, HandStateBase parent = NULL)
	{
		// setup nested state machine
		m_Hide = new HandStartHidingAnimated(player, this, WeaponActions.HIDE, -1);
		m_Show = new HandAnimatedMoveToDst_W4T(player, this, WeaponActions.SHOW, -1);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;
		HandEventBase _AEh_ = new HandAnimEventChanged;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Hide, _AEh_,   m_Show ));
		m_FSM.AddTransition(new HandTransition(   m_Show, _fin_,     NULL ));
		
		m_FSM.SetInitialState(m_Hide);
	}

	override void OnEntry (HandEventBase e)
	{
		m_Entity = e.GetSrcEntity();
		HandEventMoveTo ev_move = HandEventMoveTo.Cast(e);
		if (ev_move)
		{
			m_Show.m_Dst = ev_move.GetDst();
			m_Hide.m_ActionType = ev_move.GetAnimationID();
			m_Show.m_ActionType = ev_move.GetAnimationID();
			
			m_ilEntity = m_Show.m_Dst;
		}
		
		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort (HandEventBase e)
	{
		if( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Entity, m_ilEntity);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Entity);
		}
		
		m_Entity = null;
		m_ilEntity = null;

		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		if( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Entity, m_ilEntity);
		}
		
		m_Entity = null;
		m_ilEntity = null;
		
		super.OnExit(e);
	}
};


