class HandTakingAnimated_Hide extends HandStartAction
{ };

class HandTakingAnimated_Show extends HandStartAction
{
	ref InventoryLocation m_Src;
	ref InventoryLocation m_Dst;

	void HandTakingAnimated_Show (Man player = NULL, HandStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_Src = null;
		m_Dst = null;
	}

	override void OnEntry (HandEventBase e)
	{
		super.OnEntry(e);

		if (m_Src)
		{
			if (m_Src.IsValid())
			{
				#ifdef DEVELOPER
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "OnEntry", e.m_Player.ToString() );
				}
				#endif

				GameInventory.LocationSyncMoveEntity(m_Src, m_Dst);
				e.m_Player.OnItemInHandsChanged();
			}
			else
			{
				Error("[hndfsm] " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " HandTakingAnimated_Show m_Src=invalid, item not in bubble?");
			}
		}
		else
			Error("[hndfsm] HandTakingAnimated_Show, error - m_Src not configured");
	}

	override void OnAbort (HandEventBase e)
	{
		m_Src = null;
		m_Dst = null;
		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		m_Src = null;
		m_Dst = null;
		super.OnExit(e);
	}

	override bool IsWaitingForActionFinish () { return true; }
};


class HandAnimatedTakingFromAtt extends HandStateBase
{
	ref HandTakingAnimated_Hide m_Hide;
	ref HandTakingAnimated_Show m_Show;
	
	ref InventoryLocation m_Dst;

	void HandAnimatedTakingFromAtt (Man player = NULL, HandStateBase parent = NULL)
	{
		// setup nested state machine
		m_Hide = new HandTakingAnimated_Hide(player, this, WeaponActions.HIDE, -1);
		m_Show = new HandTakingAnimated_Show(player, this, WeaponActions.SHOW, -1);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;
		HandEventBase _AEh_ = new HandAnimEventChanged;
		HandEventBase __Xd_ = new HandEventDestroyed;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Hide, _AEh_,   m_Show ));
		m_FSM.AddTransition(new HandTransition(   m_Hide, __Xd_,     NULL ));
		m_FSM.AddTransition(new HandTransition(   m_Show, _fin_,     NULL ));
		m_FSM.AddTransition(new HandTransition(   m_Show, __Xd_,     NULL ));
		
		m_FSM.SetInitialState(m_Hide);
	}

	override void OnEntry (HandEventBase e)
	{
		m_Dst = e.GetDst();
		m_Show.m_Src = e.GetSrc();
		m_Show.m_Dst = e.GetDst();

		m_Hide.m_ActionType = e.GetAnimationID();
		m_Show.m_ActionType = e.GetAnimationID();
			
		e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst.GetItem(), m_Dst, GameInventory.c_InventoryReservationTimeoutShortMS);

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "OnAbort", e.m_Player.ToString() );
		}
		#endif
		e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst.GetItem(), m_Dst);
		if ( GetGame().IsServer() )
			GetGame().ClearJuncture(e.m_Player, m_Dst.GetItem());
		m_Dst = null;

		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst.GetItem(), m_Dst);
		m_Dst = null;

		super.OnExit(e);
	}
};

