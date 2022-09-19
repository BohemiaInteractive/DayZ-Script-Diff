#ifdef BOT
class BotEventEntitySpawned : BotEventBase { };

class BotSpawnDead extends BotStateBase
{
	string m_Type;

	void BotSpawnDead (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "")
	{
		if (new_type == string.Empty)
			m_Type = "ZmbM_HermitSkinny_Beige";
		else
			m_Type = new_type;
	}

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);

		if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			EntityAI Z = EntityAI.Cast(GetGame().CreateObject(m_Type, m_Owner.GetPosition()));
			Z.SetHealth( "", "", 0 );
			Error("TODO");
			//m_Bot.PostEvent(new BotEventEntitySpawned(m_Owner, Z));
		}
	}
};

class Bot_TestSpawnDeadSelectBury extends BotStateBase
{
	string m_Type;
	ref BotSpawnDead m_SpawnDead;
	ref BotSpawnEntityInHands m_SpawnShovel;
	ref BotSelectZombieTarget m_Selecting;
	ref BotPerformAction m_Burying;

	void Bot_TestSpawnDeadSelectBury (Bot bot = NULL, BotStateBase parent = NULL, string new_type = "ZmbM_HermitSkinny_Beige")
	{
		m_Type = new_type;

		// setup nested state machine
		m_FSM = new BotFSM(this); // @NOTE: set owner of the submachine fsm

		m_SpawnDead = new BotSpawnDead(m_Bot, this, m_Type);
		m_SpawnShovel = new BotSpawnEntityInHands(m_Bot, this, "Shovel");
		m_Selecting = new BotSelectZombieTarget(m_Bot, this);
		m_Burying = new BotPerformAction(m_Bot, this, AT_BURY_BODY);
		
		// events
		BotEventBase __SpwnZ__ = new BotEventEntitySpawned;
		BotEventBase __EntInH__ = new BotEventEntityInHands;
		BotEventBase __Select__ = new BotEventSelectedTarget;

		// transitions
		m_FSM.AddTransition(new BotTransition(   m_SpawnDead,       NULL, m_SpawnShovel));
		m_FSM.AddTransition(new BotTransition(   m_SpawnShovel,  __EntInH__, m_Selecting));
		m_FSM.AddTransition(new BotTransition(   m_Selecting,   __Select__, m_Burying));
		m_FSM.AddTransition(new BotTransition(   m_Burying,   NULL, NULL));

		m_FSM.SetInitialState(m_SpawnDead);
	}

	override void OnEntry (BotEventBase e) { super.OnEntry(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};


class BotEventSelectedTarget : BotEventBase { };

class BotSelectZombieTarget extends BotStateBase
{
	ZombieMaleBase m_Target;
	float m_dtAccumulator = 0.0;

	void SelectTarget ()
	{
		//m_Target = NearestTargetSelector<ZombieMaleBase>.Find(GetPlayerOwner(), NULL);
		botDebugPrint("[bot] + " + m_Owner + " hunt SelectTarget target=" + m_Target);
	}

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		m_dtAccumulator = 0.0;
		SelectTarget();
	}

	override void OnExit (BotEventBase e)
	{
		m_dtAccumulator = 0.0;
		m_Target = null;

		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		m_dtAccumulator += dt;
		
		if (m_Target == null)
		{
			if (m_dtAccumulator > 2.0)
			{
				botDebugPrint("[bot] + " + m_Owner + " searching...");
				SelectTarget();
				if (m_Target)
				{
					Error("TODO");
					//m_Bot.PostEvent(new BotEventSelectedTarget(m_Owner, m_Target));
				}

				m_dtAccumulator = 0.0;
			}
		}
		else
		{
					Error("TODO");
			//m_Bot.PostEvent(new BotEventSelectedTarget(m_Owner, m_Target));
		}
	}
};

class BotPerformAction extends BotStateBase
{
	EntityAI m_Entity;
	int m_ActionID;

	void BotPerformAction (Bot bot = NULL, BotStateBase parent = NULL, int action = AT_EAT)
	{
		m_ActionID = action;
	}

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);

		if (e.m_Entity)
		{
			botDebugPrint("[bot] + " + m_Owner + " performing action on item=" + e.m_Entity);

			if (m_Owner.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
#ifdef BOT
				ActionManagerClient cli_mgr = ActionManagerClient.Cast(m_Owner.GetActionManager());
				if (cli_mgr)
					cli_mgr.PerformAction(m_ActionID, new ActionTarget(e.m_Entity, null, -1, vector.Zero, -1), e.m_Entity);
#endif
			}
		}
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }
	override void OnExit (BotEventBase e) { super.OnExit(e); }
	override void OnUpdate (float dt) { super.OnUpdate(dt); }
};



/*class BotHunt extends BotStateBase
{
	float m_dtAccumulator = 0.0;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		m_dtAccumulator = 0.0;
	}

	override void OnExit (BotEventBase e)
	{
		m_dtAccumulator = 0.0;
		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		m_dtAccumulator += dt;
		
		float rescanTime = 3.0;
		if (m_dtAccumulator >= rescanTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_Bot.ProcessEvent(new WeaponEventReloadTimeout(p));
	}
};
*/


#endif
