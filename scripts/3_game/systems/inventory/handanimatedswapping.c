class HandSwappingAnimated_Show extends HandForceSwappingAnimated_Show
{
};


class HandAnimatedSwapping extends HandStateBase
{
	ref InventoryLocation m_Src1 = null;
	ref InventoryLocation m_Src2 = null;
	ref InventoryLocation m_Dst1 = null;
	ref InventoryLocation m_Dst2 = null;

	ref HandStartHidingAnimated m_Hide;
	ref HandSwappingAnimated_Show m_Show;

	void HandAnimatedSwapping (Man player = NULL, HandStateBase parent = NULL)
	{
		// setup nested state machine
		m_Hide = new HandStartHidingAnimated(player, this, WeaponActions.HIDE, -1);
		m_Show = new HandSwappingAnimated_Show(player, this, WeaponActions.SHOW, -1);

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
		HandEventSwap es = HandEventSwap.Cast(e);
		if (es)
		{
			m_Src1 = es.GetSrc();
			m_Src2 = es.m_Src2;
			m_Dst1 = es.GetDst();
			m_Dst2 = es.m_Dst2;

			m_Show.m_Src1 = m_Src1;
			m_Show.m_Src2 = m_Src2;
			m_Show.m_Dst1 = m_Dst1;
			m_Show.m_Dst2 = m_Dst2;

			m_Hide.m_ActionType = es.m_AnimationID;
			m_Show.m_ActionType = es.m_Animation2ID;
			
			if (!GetGame().IsDedicatedServer())
			{
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst2.GetItem(), m_Dst2, GameInventory.c_InventoryReservationTimeoutShortMS);
				e.m_Player.GetHumanInventory().AddInventoryReservationEx(m_Dst1.GetItem(), m_Dst1, GameInventory.c_InventoryReservationTimeoutShortMS);
			}
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort (HandEventBase e)
	{
		if( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);
		}
		else
		{
			GetGame().ClearJuncture(e.m_Player, m_Dst2.GetItem());
			GetGame().ClearJuncture(e.m_Player, m_Dst1.GetItem());
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;

		super.OnAbort(e);
	}

	override void OnExit (HandEventBase e)
	{
		if( !GetGame().IsDedicatedServer())
		{
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst2.GetItem(), m_Dst2);
			e.m_Player.GetHumanInventory().ClearInventoryReservationEx(m_Dst1.GetItem(), m_Dst1);		
		}
		
		m_Src1 = null;
		m_Src2 = null;
		m_Dst1 = null;
		m_Dst2 = null;

		super.OnExit(e);
	}
};

