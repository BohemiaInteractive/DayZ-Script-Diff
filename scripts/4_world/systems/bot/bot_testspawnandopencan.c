class BotEventEntityInHands : BotEventBase { };

class BotSpawnEntityInHands extends BotStateBase
{
	string m_Type;

	void BotSpawnEntityInHands (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		if (new_type == string.Empty)
			m_Type = "TunaCan";
		else
			m_Type = new_type;
	}

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);

		if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			m_Owner.GetHumanInventory().CreateInHands(m_Type);
		}
	}

	override void OnExit (BotEventBase e) { super.OnExit(e); }

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		if (inHands)
		{
			botDebugPrint("[bot] + " + m_Owner + " hand item=" + inHands + " bot=" + m_Owner);
			m_Bot.ProcessEvent(new BotEventEntityInHands(m_Owner, inHands));
		}
	}
};

class BotEventEntityInHandsOpened : BotEventBase { };

class BotOpenEntityInHands extends BotStateBase
{
	EntityAI m_Entity;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		
		if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			BotEventEntityInHands ev_ih = BotEventEntityInHands.Cast(e);
			if (ev_ih)
			{		
				m_Entity = ev_ih.m_Entity;
				Edible_Base b = Edible_Base.Cast(m_Entity);
				if (b)
				{	
					botDebugPrint("[bot] + " + m_Owner + " will open edible item=" + b + " bot=" + m_Owner);
					b.Open();
				}
			}
		}
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }

	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		botDebugSpam("[bot] + " + m_Owner + " wait for opened item inHands=" + inHands + " bot=" + m_Owner);
		if (inHands)
		{
			string t_str = inHands.GetType();
			//string t_str = t.ToString();
			if (t_str.IndexOf("_Opened") != -1)
			{
				botDebugPrint("[bot] + " + m_Owner + " opened item=" + inHands + " bot=" + m_Owner);
				m_Bot.ProcessEvent(new BotEventEntityInHandsOpened(m_Owner, inHands));
			}
		}
	}
};


class Bot_TestSpawnOpen extends BotStateBase
{
	string m_Type;
	ref BotSpawnEntityInHands m_Spawning;
	ref BotOpenEntityInHands m_Opening;

	void Bot_TestSpawnOpen (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		m_Type = new_type;

		// setup nested state machine
		m_FSM = new BotFSM(this); // @NOTE: set owner of the submachine fsm

		m_Spawning = new BotSpawnEntityInHands(m_Bot, this);
		m_Opening = new BotOpenEntityInHands(m_Bot, this);
		
		// events
		BotEventBase __EntInH__ = new BotEventEntityInHands;

		// transitions
		m_FSM.AddTransition(new BotTransition(   m_Spawning,  __EntInH__, m_Opening));
		//m_FSM.AddTransition(new BotTransition(   m_Opening,   __EntAtt__, m_Eating));
		//m_FSM.AddTransition(new BotTransition(   m_Eating,   __EntAtt__, m_Eating));

		m_FSM.SetInitialState(m_Spawning);
	}

	override void OnEntry (BotEventBase e) { super.OnEntry(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};


class BotEventHandsEmpty : BotEventBase { };

class BotWaitForEmptyHands extends BotStateBase
{
	override void OnEntry (BotEventBase e) { super.OnEntry(e); }
	override void OnAbort (BotEventBase e) { super.OnAbort(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		botDebugSpam("[bot] + " + m_Owner + " waiting for empty hands, inHands=" + inHands + " bot=" + m_Owner);
		if (!inHands)
		{
			botDebugPrint("[bot] + " + m_Owner + " hand slot empty. bot=" + m_Owner);
			m_Bot.ProcessEvent(new BotEventHandsEmpty(m_Owner, inHands));
		}
	}
};

class BotWaitForChangeInHands extends BotStateBase
{
	/*override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
		m_Bot.ProcessEvent(new BotEventEndOK(m_Owner));
	}*/
};


class BotDropEntityInHands extends BotWaitForChangeInHands
{
	EntityAI m_Entity;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		

		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		if (inHands)
		{
			botDebugPrint("[bot] + " + m_Owner + " dropping item=" + inHands + " bot=" + m_Owner);
			if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
				m_Owner.PredictiveDropEntity(inHands);
			//m_Owner.LocalDropEntity(inHands);
		}
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};

class BotEatEntityInHands extends BotWaitForChangeInHands
{
	EntityAI m_Entity;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		
		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		if (inHands)
		{
			botDebugPrint("[bot] + " + m_Owner + " eating item=" + inHands + " bot=" + m_Owner);
			if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
#ifdef BOT
				ActionManagerBase mgr = m_Owner.GetActionManager();
				ActionManagerClient cli_mgr = ActionManagerClient.Cast(mgr);
				if (cli_mgr)
				{
					ActionTarget target;
					target = new ActionTarget(inHands, null, -1, vector.Zero, -1);
					cli_mgr.PerformAction(AT_EAT, target, inHands);
				}
#endif
			}
		}
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};



class BotDestroyEntityInHands extends BotWaitForChangeInHands
{
	EntityAI m_Entity;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		
		EntityAI inHands = m_Owner.GetHumanInventory().GetEntityInHands();
		if (inHands)
		{
			botDebugPrint("[bot] + " + m_Owner + " deleting item=" + inHands + " bot=" + m_Owner);
			if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
				inHands.Delete();
		}
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};


class Bot_TestSpawnOpenDrop extends Bot_TestSpawnOpen
{
	ref BotDropEntityInHands m_Dropping;

	void Bot_TestSpawnOpenDrop (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		m_Dropping = new BotDropEntityInHands(m_Bot, this);
		
		// events
		BotEventBase __EntOpn__ = new BotEventEntityInHandsOpened;
		BotEventBase __HndChg__ = new BotEventOnItemInHandsChanged;

		// transitions
		m_FSM.AddTransition(new BotTransition(    m_Opening,  __EntOpn__, m_Dropping));
		m_FSM.AddTransition(new BotTransition(   m_Dropping,  __HndChg__, NULL));
	}

	override void OnEntry (BotEventBase e) { super.OnEntry(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};

class Bot_TestSpawnOpenDestroy extends Bot_TestSpawnOpen
{
	ref BotDestroyEntityInHands m_Destroying;

	void Bot_TestSpawnOpenDestroy (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		m_Destroying = new BotDestroyEntityInHands(m_Bot, this);
		
		// events
		BotEventBase __EntOpn__ = new BotEventEntityInHandsOpened;
		BotEventBase __HndChg__ = new BotEventOnItemInHandsChanged;

		// transitions
		m_FSM.AddTransition(new BotTransition(    m_Opening,  __EntOpn__, m_Destroying));
		m_FSM.AddTransition(new BotTransition( m_Destroying,  __HndChg__, NULL));
	}

	override void OnEntry (BotEventBase e) { super.OnEntry(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};

class Bot_TestSpawnOpenEat extends Bot_TestSpawnOpen
{
	ref BotEatEntityInHands m_Eating;

	void Bot_TestSpawnOpenEat (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		m_Eating = new BotEatEntityInHands(m_Bot, this);
		
		// events
		BotEventBase __EntOpn__ = new BotEventEntityInHandsOpened;
		BotEventBase __HndChg__ = new BotEventOnItemInHandsChanged;

		// transitions
		m_FSM.AddTransition(new BotTransition(    m_Opening,  __EntOpn__, m_Eating));
		m_FSM.AddTransition(new BotTransition(     m_Eating,  __HndChg__, NULL));
	}

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
	}

	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);
	}
};

