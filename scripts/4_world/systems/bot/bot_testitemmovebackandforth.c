class BotTestItemMoveBackAndForth extends BotStateBase
{
	EntityAI m_Entity;
	ref BotTestItemMoveBackAndForth_MoveFromSlotToSlot m_Move;

	void BotTestItemMoveBackAndForth (Bot bot = NULL, BotStateBase parent = NULL)
	{
		// setup nested state machine
		m_FSM = new BotFSM(this); // @NOTE: set owner of the submachine fsm

		m_Move = new BotTestItemMoveBackAndForth_MoveFromSlotToSlot(m_Bot, this);
		
		// events
		BotEventBase __EntAtt__ = new BotEventEntityAttached;

		// transitions
		m_FSM.AddTransition(new BotTransition(   m_Move, __EntAtt__, m_Move));

		m_FSM.SetInitialState(m_Move);
	}

	override void OnEntry (BotEventBase e)
	{
		//m_Entity = m_Owner.GetInventory().CreateAttachment("TaloonBag_Orange");
		//m_Entity = m_Owner.GetInventory().FindAttachment();
		EntityAI hgear = m_Owner.GetInventory().FindAttachment( InventorySlots.GetSlotIdFromString("Headgear") );
		EntityAI mask = m_Owner.GetInventory().FindAttachment( InventorySlots.GetSlotIdFromString("Mask") );

		if (hgear)
			m_Entity = hgear;
		if (mask)
			m_Entity = mask;


		InventoryLocation loc = new InventoryLocation;
		if (m_Entity.GetInventory().GetCurrentInventoryLocation(loc))
		{
			m_Move.m_WaitingForSlot = loc.GetSlot();
		}
		else
			Error("EE");
		m_Move.m_Entity = m_Entity;

		super.OnEntry(e);
	}

	override void OnExit (BotEventBase e)
	{
		m_Entity = null;

		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);
	}
};

class BotTestItemMoveBackAndForth_MoveFromSlotToSlot extends BotStateBase
{
	EntityAI m_Entity;
	int m_WaitingForSlot = InventorySlots.INVALID;
	int m_hgSlot = InventorySlots.GetSlotIdFromString("Headgear");
	int m_mskSlot = InventorySlots.GetSlotIdFromString("Mask");

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }

	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
	}

	int GetNextSlot (int curr)
	{
		if (curr == m_hgSlot)
			return m_mskSlot;
		if (curr == m_mskSlot)
			return m_hgSlot;
		Error("EE2");
		return InventorySlots.INVALID;
	}

	override void OnUpdate (float dt)
	{
		if (m_Entity)
		{
			botDebugPrint("[bot] + " + m_Owner + " move item=" + m_Entity);
			
			InventoryLocation loc = new InventoryLocation;
			if (m_Entity.GetInventory().GetCurrentInventoryLocation(loc))
			{
				if (loc.GetType() == InventoryLocationType.ATTACHMENT)
				{
					if (loc.GetSlot() == m_WaitingForSlot)
					{
						int nextSlot = GetNextSlot(m_WaitingForSlot);
						botDebugPrint("[bot] + " + m_Owner + " will switch slot=" + nextSlot + " for item=" + m_Entity);

						m_WaitingForSlot = nextSlot;

						m_Owner.PredictiveTakeEntityAsAttachmentEx(m_Entity, nextSlot);
						//m_Bot.ProcessEvent(new BotEventEntityDet(m_Owner, m_Entity));
					}
				}
			}
		}
	}
};

