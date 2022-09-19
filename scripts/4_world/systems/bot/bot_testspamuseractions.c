
class BotTestSpamUserActions_Start : BotTimedWait
{
	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);

	}

	override void OnExit (BotEventBase e)
	{
		botDebugPrint("[bot] + " + m_Owner + "BotTestSpamUserActions_Start::OnExit");

		/*if (GetGame().IsServer() && GetGame().IsMultiplayer())
		{
			botDebugPrint("[bot] + " + m_Owner + "BotTestSpamUserActions_Start spamming!");
			for (int i = 0; i < 10000; ++i)
				Print("Lovely SPAAAAAM!");
		}*/

		super.OnExit(e);
	}

}

class BotTestSpamUserActions_GetEntityFromSlot : BotTimedWait
{
	EntityAI m_Entity = null;
	bool m_Run = false;
	int m_RunStage = 0;
	ref InventoryLocation m_Src = new InventoryLocation;

	override void OnEntry (BotEventBase e)
	{
		m_Entity = m_Owner.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Legs"));
		botDebugPrint("[bot] + " + m_Owner + "BotTestSpamUserActions_GetEntityFromSlot item=" + m_Entity);

		GameInventory.SetGroundPosByOwner(m_Owner, m_Entity, m_Src);
		/*if (!m_Entity.GetInventory().GetCurrentInventoryLocation(m_Src))
		{
			Error("NI!");
		}*/

		if (!GetGame().IsDedicatedServer())
		{
			m_Entity = m_Owner.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Legs"));
			m_Owner.PredictiveDropEntity(m_Entity);
		}

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

		if (!GetGame().IsDedicatedServer())
		{
			if (m_Run && m_Entity)
			{

				switch (m_RunStage)
				{
					case 0:
						botDebugPrint("[bot] + " + m_Owner + " STS = " + m_Owner.GetSimulationTimeStamp() + " Stage=" + m_RunStage + " item=" + m_Entity);
						ScriptInputUserData ctx = new ScriptInputUserData;
						HandEventTake e = new HandEventTake(m_Owner, m_Src);
						InventoryInputUserData.SerializeHandEvent(ctx, e);
						ctx.Send();
						break;

					case 1:
						botDebugPrint("[bot] + " + m_Owner + " STS = " + m_Owner.GetSimulationTimeStamp() + " Stage=" + m_RunStage + " item=" + m_Entity);
						ScriptInputUserData ctx1 = new ScriptInputUserData;
						HandEventTake e1 = new HandEventTake(m_Owner, m_Src);
						InventoryInputUserData.SerializeHandEvent(ctx1, e1);
						ctx1.Send();
						break;

					case 5:
						botDebugPrint("[bot] + " + m_Owner + " STS = " + m_Owner.GetSimulationTimeStamp() + " Stage=" + m_RunStage + " item=" + m_Entity);
						ScriptInputUserData ctx2 = new ScriptInputUserData;
						InventoryLocation dst = new InventoryLocation;
						dst.SetAttachment(m_Owner, m_Entity, InventorySlots.GetSlotIdFromString("Legs"));
						InventoryInputUserData.SerializeMove(ctx2, InventoryCommandType.SYNC_MOVE, m_Src, dst);
						ctx2.Send();
						break;

					case 10:
						botDebugPrint("[bot] + " + m_Owner + " STS = " + m_Owner.GetSimulationTimeStamp() + " Stage=" + m_RunStage + " item=" + m_Entity);
						ScriptInputUserData ctx3 = new ScriptInputUserData;
						InventoryLocation dst2 = new InventoryLocation;
						dst2.SetAttachment(m_Owner, m_Entity, InventorySlots.GetSlotIdFromString("Legs"));
						InventoryInputUserData.SerializeMove(ctx3, InventoryCommandType.SYNC_MOVE, m_Src, dst2);
						ctx3.Send();
						break;

					/*
					case 0:
						botDebugPrint("[bot] + " + m_Owner + "Stage0 item=" + m_Entity);
						m_Owner.PredictiveTakeEntityToHands(m_Entity);
						break;

					case 1:
						botDebugPrint("[bot] + " + m_Owner + "Stage1 item=" + m_Entity);
						m_Owner.PredictiveTakeEntityToHands(m_Entity);
						break;

					case 2:
						botDebugPrint("[bot] + " + m_Owner + "Stage2 item=" + m_Entity);
						m_Owner.PredictiveTakeEntityAsAttachmentEx(m_Entity, InventorySlots.GetSlotIdFromString("Legs"));
						break;

					case 3:
						botDebugPrint("[bot] + " + m_Owner + "Stage3 item=" + m_Entity);
						m_Owner.PredictiveTakeEntityAsAttachmentEx(m_Entity, InventorySlots.GetSlotIdFromString("Legs"));
						break;*/
				}

				++m_RunStage;
			}
		}
	}

	override void OnTimeout ()
	{
		super.OnTimeout();

		botDebugPrint("[bot] + " + m_Owner + "BotTestSpamUserActions_GetEntityFromSlot item=" + m_Entity);

		if (m_Entity && m_Run == false)
		{
			m_Run = true;
			m_RunStage = 0;
		}
	}
}


class BotTestSpamUserActions : BotStateBase
{
	EntityAI m_Entity;
	ref BotTestSpamUserActions_Start m_Start;
	ref BotTestSpamUserActions_GetEntityFromSlot m_GetRef;
	//ref BotTimedWait m_Wait;

	void BotTestSpamUserActions (Bot bot = NULL, BotStateBase parent = NULL)
	{
		// setup nested state machine
		m_FSM = new BotFSM(this); // @NOTE: set owner of the submachine fsm

		m_Start = new BotTestSpamUserActions_Start(m_Bot, this, 1.0);
		m_GetRef = new BotTestSpamUserActions_GetEntityFromSlot(m_Bot, this, 2.0);
		
		// events
		BotEventBase __EntWait__ = new BotEventWaitTimeout;

		// transitions
		m_FSM.AddTransition(new BotTransition(   m_Start, __EntWait__, m_GetRef));

		m_FSM.SetInitialState(m_Start);
	}

	override void OnEntry (BotEventBase e)
	{
		m_Entity = null;

		if (GetGame().IsServer())
		{
			m_Owner.GetInventory().CreateAttachment("PolicePantsOrel"); // no assign to m_Entity
			botDebugPrint("[bot] + " + m_Owner + " created attachment item=" + m_Entity);
		}

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
}

