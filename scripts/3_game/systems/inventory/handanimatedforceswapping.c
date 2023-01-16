// When editing this, take a look at HandAnimatedMoveToDst_W4T_Basic as well
// They can not be inherited from each other because of different inheritance
class HandAnimatedMoveToDst_W4T_Basic extends HandStateBase
{
	ref InventoryLocation m_Dst;

	override void OnEntry(HandEventBase e)
	{
		Man player = e.m_Player;
		if (m_Dst && m_Dst.IsValid())
		{
			EntityAI item = m_Dst.GetItem();
			InventoryLocation src = new InventoryLocation;
			if (item.GetInventory().GetCurrentInventoryLocation(src))
			{
				if (GameInventory.LocationCanMoveEntity(src, m_Dst))
				{
					GameInventory.LocationSyncMoveEntity(src, m_Dst);
					player.OnItemInHandsChanged();
				}
				else
				{
					#ifdef DEVELOPER
					if ( LogManager.IsInventoryHFSMLogEnable() )
					{	
						Debug.InventoryHFSMLog("[hndfsm] HandAnimatedMoveToDst_W4T_Basic - not allowed");
					}
					#endif
				}
			}
			else
				Error("[hndfsm] " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " HandAnimatedMoveToDst_W4T_Basic - item " + item + " has no Inventory or Location, inv=" + item.GetInventory());
		}
		else
			Error("[hndfsm] HandAnimatedMoveToDst_W4T_Basic - event has no valid m_Dst");

		super.OnEntry(e);
	}

	override void OnAbort(HandEventBase e)
	{
		m_Dst = null;
		super.OnAbort(e);
	}

	override void OnExit(HandEventBase e)
	{
		m_Dst = null;
		super.OnExit(e);
	}

	override bool IsWaitingForActionFinish() { return true; }
};

class HandForceSwappingAnimated_Show extends HandStartAction
{
	ref InventoryLocation m_Src1 = null;
	ref InventoryLocation m_Src2 = null;
	ref InventoryLocation m_Dst1 = null;
	ref InventoryLocation m_Dst2 = null;

	void HandForceSwapingAnimated_Show(Man player = null, HandStateBase parent = null, WeaponActions action = WeaponActions.NONE, int actionType = -1) { }

	override void OnEntry(HandEventBase e)
	{
		if (m_Src1 && m_Src2 && m_Dst1 && m_Dst2)
		{
			//InventoryLocation dummy;
			//dummy.Copy(m_Dst2);			
			//if (GameInventory.CanForceSwapEntitiesEx(m_Src1.GetItem(), m_Dst1, m_Src2.GetItem(), dummy) && dummy == m_Dst2)
			//{
				GameInventory.LocationSwap(m_Src1, m_Src2, m_Dst1, m_Dst2);
				e.m_Player.OnItemInHandsChanged();
			//}
			//else
			//{
			//	hndDebugPrint("[hndfsm] HandForceSwapingAnimated_Show - not allowed");
			//}
		}
		else
			Error("[hndfsm] HandForceSwappingAnimated_Show is not properly configured!");

		super.OnEntry(e);
	}

	override void OnAbort(HandEventBase e)
	{
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;
		super.OnAbort(e);
	}

	override void OnExit(HandEventBase e)
	{
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;
		super.OnExit(e);
	}

	override bool IsWaitingForActionFinish() { return true; }
};


class HandAnimatedForceSwapping extends HandStateBase
{
	ref InventoryLocation m_Src1 = null;
	ref InventoryLocation m_Src2 = null;
	ref InventoryLocation m_Dst1 = null;
	ref InventoryLocation m_Dst2 = null;

	ref HandStartHidingAnimated m_Start;
	ref HandAnimatedMoveToDst_W4T m_Show;
	ref HandAnimatedMoveToDst_W4T_Basic m_Hide;

	void HandAnimatedForceSwapping(Man player = null, HandStateBase parent = null)
	{
		// setup nested state machine
		m_Start = new HandStartHidingAnimated(player, this, WeaponActions.HIDE, -1);
		m_Show = new HandAnimatedMoveToDst_W4T(player, this, WeaponActions.SHOW, -1);
		m_Hide = new HandAnimatedMoveToDst_W4T_Basic(player, this);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;
		HandEventBase _AEh_ = new HandAnimEventChanged;
		HandEventBase __Xd_ = new HandEventDestroyed;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Start,		_AEh_,	m_Hide ));
		m_FSM.AddTransition(new HandTransition(   m_Hide,		_AEh_,	m_Show )); // no animation in Hide step
		m_FSM.AddTransition(new HandTransition(   m_Show,		_fin_,	null ));
		m_FSM.AddTransition(new HandTransition(   m_Hide,		_fin_,	null ));
		m_FSM.AddTransition(new HandTransition(   m_Show,		__Xd_,	null ));
		m_FSM.AddTransition(new HandTransition(   m_Hide,		__Xd_,	null ));

		m_FSM.SetInitialState(m_Start);
	}

	override void OnEntry(HandEventBase e)
	{		
		HandEventForceSwap efs = HandEventForceSwap.Cast(e);
		if (efs)
		{
			if ( efs.GetSrc().GetType() == InventoryLocationType.HANDS )
			{
				m_Start.m_ActionType = efs.m_AnimationID;
			
				m_Src1 = efs.GetSrc();
				m_Src2 = efs.m_Src2;
				m_Dst1 = efs.GetDst();
				m_Dst2 = efs.m_Dst2;
			
				m_Show.m_ActionType = efs.m_Animation2ID;
			}
			else
			{
				m_Start.m_ActionType = efs.m_Animation2ID;
			
				m_Src1 = efs.m_Src2;
				m_Src2 = efs.GetSrc();
				m_Dst1 = efs.m_Dst2;
				m_Dst2 = efs.GetDst();
			
				m_Show.m_ActionType = efs.m_AnimationID;
			}
			
			m_Hide.m_Dst = m_Dst1;
			m_Show.m_Dst = m_Dst2;

			if (!GetGame().IsDedicatedServer())
			{
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst1.GetItem(), m_Dst1, GameInventory.c_InventoryReservationTimeoutShortMS);
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst2.GetItem(), m_Dst2, GameInventory.c_InventoryReservationTimeoutShortMS);
			}
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort(HandEventBase e)
	{
		if ( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Dst1.GetItem());
			GetGame().ClearJuncture(e.m_Player, m_Dst2.GetItem());
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;
		
		super.OnAbort(e);
	}

	override void OnExit(HandEventBase e)
	{
		if ( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Dst1.GetItem());
			GetGame().ClearJuncture(e.m_Player, m_Dst2.GetItem());
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;

		super.OnExit(e);
	}
};

class HandAnimatedForceSwapping_Inst extends HandStateBase
{
	ref InventoryLocation m_Src1 = null;
	ref InventoryLocation m_Src2 = null;
	ref InventoryLocation m_Dst1 = null;
	ref InventoryLocation m_Dst2 = null;

	ref HandStartHidingAnimated m_Start;
	ref HandForceSwappingAnimated_Show m_Swap;

	void HandAnimatedForceSwapping_Inst(Man player = null, HandStateBase parent = null)
	{
		// setup nested state machine
		m_Start = new HandStartHidingAnimated(player, this, WeaponActions.HIDE, -1);
		m_Swap = new HandForceSwappingAnimated_Show(player, this, WeaponActions.SHOW, -1);

		// events:
		HandEventBase _fin_ = new HandEventHumanCommandActionFinished;
		HandEventBase _AEh_ = new HandAnimEventChanged;
		HandEventBase __Xd_ = new HandEventDestroyed;

		m_FSM = new HandFSM(this); // @NOTE: set owner of the submachine fsm

		m_FSM.AddTransition(new HandTransition(   m_Start,	_AEh_,	m_Swap ));
		m_FSM.AddTransition(new HandTransition(   m_Swap,	_fin_,	null ));
		m_FSM.AddTransition(new HandTransition(   m_Swap,	__Xd_,	null ));

		m_FSM.SetInitialState(m_Start);
	}

	override void OnEntry(HandEventBase e)
	{		
		HandEventForceSwap efs = HandEventForceSwap.Cast(e);
		if (efs)
		{
			if ( efs.GetSrc().GetType() == InventoryLocationType.HANDS )
			{
				m_Start.m_ActionType = efs.m_AnimationID;
			
				m_Src1 = efs.GetSrc();
				m_Src2 = efs.m_Src2;
				m_Dst1 = efs.GetDst();
				m_Dst2 = efs.m_Dst2;

				m_Swap.m_ActionType = efs.m_Animation2ID;
			}
			else
			{
				m_Start.m_ActionType = efs.m_Animation2ID;
			
				m_Src1 = efs.m_Src2;
				m_Src2 = efs.GetSrc();
				m_Dst1 = efs.m_Dst2;
				m_Dst2 = efs.GetDst();

				m_Swap.m_ActionType = efs.m_AnimationID;				
			}
			
			m_Swap.m_Src1 = m_Src1;
			m_Swap.m_Dst1 = m_Dst1;
			m_Swap.m_Src2 = m_Src2;
			m_Swap.m_Dst2 = m_Dst2;
			
			if (!GetGame().IsDedicatedServer())
			{
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst1.GetItem(), m_Dst1, GameInventory.c_InventoryReservationTimeoutShortMS);
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst2.GetItem(), m_Dst2, GameInventory.c_InventoryReservationTimeoutShortMS);
			}
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort(HandEventBase e)
	{
		if ( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Dst1.GetItem());
			GetGame().ClearJuncture(e.m_Player, m_Dst2.GetItem());
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;
		
		super.OnAbort(e);
	}

	override void OnExit(HandEventBase e)
	{
		if ( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Dst1.GetItem());
			GetGame().ClearJuncture(e.m_Player, m_Dst2.GetItem());
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;

		super.OnExit(e);
	}
};

