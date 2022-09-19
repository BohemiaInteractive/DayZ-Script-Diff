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
	protected ref Timer m_Timer = new Timer;
	protected const float c_TriggerTimeoutMS = 1000.0;
	//protected const float c_UpdateMS = 2000.0;
	protected const float c_UpdateMS = 16.0;
	protected ref BotFSM m_FSM = null;
	protected bool m_UseTrigger = false;
	protected bool m_Triggered = false;
	protected DayZPlayerInstanceType m_InstanceType = DayZPlayerInstanceType.INSTANCETYPE_CLIENT;
	protected ref BotTrigger m_BotTrigger = null;
	protected ref BotStateBase m_BotTest = null;

	void Bot (PlayerBase ow)
	{
		m_Owner = ow;
	}
	
	void SetInstanceType (DayZPlayerInstanceType t) { m_InstanceType = t; }

	void Start (bool use_trigger, BotTrigger trigger = null)
	{
		m_UseTrigger = use_trigger;

		InitFSM();

		if (m_UseTrigger)
		{
			if (m_UseTrigger && trigger == null)
				Error("use trigger, but trigger null");

			m_Triggered = false;
			m_BotTrigger = trigger;
			botDebugPrint("[bot] + " + m_Owner + " Bot waiting for trigger...");
			m_Timer.Run(c_TriggerTimeoutMS / 1000.0, this, "OnTrigger", null, true);
		}
		else
		{
			botDebugPrint("[bot] + " + m_Owner + " Bot Started.");
			m_Timer.Run(c_UpdateMS / 1000.0, this, "OnTimer", null, true);
		}
	}

	void DelayedStart (float ms)
	{
		m_Timer.Run(ms / 1000.0, this, "OnDelayedStart", null, false);
	}

	protected void OnDelayedStart ()
	{
		Start(false, null);

		ProcessEvent(new BotEventStart(m_Owner));
		m_UseTrigger = false;
	}

	void Stop ()
	{
		m_Triggered = false;
		m_Timer.Stop();
		m_FSM.Terminate();
	}

	protected void OnTrigger ()
	{
		bool triggered = m_BotTrigger.CheckTrigger();

		if (!m_Triggered)
		{
			if (triggered)
			{
				m_Timer.Stop(); // stop trigger timer
				m_Triggered = true;
				ProcessEvent(new BotEventStart(m_Owner));
				m_Timer.Run(c_UpdateMS / 1000.0, this, "OnUpdate", null, true);

				botDebugPrint("[bot] + " + m_Owner + " Started test!");
			}
		}
		else
		{
			if (!triggered)
			{
				m_Timer.Stop(); // stop update timer
				m_Triggered = false;
				ProcessEvent(new BotEventStop(m_Owner));
				m_Timer.Run(c_TriggerTimeoutMS / 1000.0, this, "OnTrigger", null, true);

				botDebugPrint("[bot] + " + m_Owner + " Stopped test!");
			}
		}
	}

	void OnUpdate (float dt)
	{
		m_FSM.GetCurrentState().OnUpdate(dt);

		OnTrigger(); // to detect trigger stop
	}

	void OnTimer ()
	{
		//m_FSM.GetCurrentState().OnUpdate(c_UpdateMS / 1000.0);

		//OnTrigger(); // to detect trigger stop
	}

	void InitFSM ()
	{
		m_FSM = new BotFSM();

		// basic states
		BotStateBase BotIdle = new BotStateIdle(this, NULL);
		// unstable (intermediate) states
		m_BotTest = new BotTestSpamUserActions(this, NULL);
		//m_BotTest = new BotTestAttachAndDropCycle(this, NULL);
		//m_BotTest = new BotTestItemMoveBackAndForth(this, NULL);
		//m_BotTest = new Bot_TestSpawnOpen(this, NULL);
		//m_BotTest = new Bot_TestSpawnOpenDestroy(this, NULL);
		//m_BotTest = new Bot_TestSpawnOpenEat(this, NULL);
		//m_BotTest = new BotTestSwapG2H(this, NULL);
		//m_BotTest = new BotTestSwapC2H(this, NULL);
		//m_BotTest = new BotTestSwapInternal(this, NULL);

		// events
		BotEventBase ___Bgn__ = new BotEventStart;
		BotEventBase __Stop__ = new BotEventStop;
		BotEventBase ___OK___ = new BotEventEndOK;
		BotEventBase __Fail__ = new BotEventEndFail;
		BotEventBase __Tout__ = new BotEventEndTimeout;
		BotEventBase __IChg__ = new BotEventOnItemInHandsChanged;

		///@{ transition table
		m_FSM.AddTransition(new BotTransition(  BotIdle     , ___Bgn__,   m_BotTest));
		m_FSM.AddTransition(new BotTransition(  BotIdle     , __Stop__,        NULL));

		// causes restart of FSM
		//m_FSM.AddTransition(new BotTransition(m_BotTest     , __IChg__,   m_BotTest));

		//m_FSM.AddTransition(new BotTransition(m_BotTest     , ___OK___,     BotIdle));
		m_FSM.AddTransition(new BotTransition(m_BotTest     , __Fail__,     BotIdle));
		m_FSM.AddTransition(new BotTransition(m_BotTest     , __Tout__,     BotIdle));
		///@} transition table

		m_FSM.SetInitialState(BotIdle);
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
	Print("" + s); // comment/uncomment to hide/see debug logs
}

void botDebugSpam (string s)
{
	//Print("" + s); // comment/uncomment to hide/see debug logs
}

