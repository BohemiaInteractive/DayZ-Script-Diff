
class BotEventHuntedTargetInSight : BotEventBase
{
	//EntityAI m_Entity;
	//void BotEventEntityAttached (PlayerBase p = NULL, EntityAI att = NULL) { m_Entity = att; }
};

class BotEventHuntedTargetLost : BotEventBase
{
	//EntityAI m_Entity;
	//void BotEventEntityDetached (PlayerBase p = NULL, EntityAI att = NULL) { m_Entity = att; }
};


class BotHunt extends BotStateBase
{
	Man m_Target;
	float m_dtAccumulator = 0.0;

	ref BotHunt_Tracking m_Tracking;
	ref BotHunt_Hunting m_Hunting;

	void BotHunt (Bot bot = NULL, BotStateBase parent = NULL)
	{
		// setup nested state machine
		m_FSM = new BotFSM(this); // @NOTE: set owner of the submachine fsm

		m_Tracking = new BotHunt_Tracking(m_Bot, this);
		m_Hunting = new BotHunt_Hunting(m_Bot, this);
		
		// events
		BotEventBase __InSight__ = new BotEventHuntedTargetInSight;
		BotEventBase __Lost__ = new BotEventHuntedTargetLost;

		// transitions
		m_FSM.AddTransition(new BotTransition(   m_Tracking, __InSight__, m_Hunting));
		m_FSM.AddTransition(new BotTransition(   m_Hunting,  __Lost__   , m_Tracking));

		m_FSM.SetInitialState(m_Tracking);
	}

	void SelectTarget ()
	{
		m_Target = BotSelectNearestTarget(GetPlayerOwner());
		m_Tracking.m_Target = m_Target;
		m_Hunting.m_Target = m_Target;
		botDebugPrint("[bot] + " + m_Owner + " hunt SelectTarget target=" + m_Target);
	}

	override void OnEntry (BotEventBase e)
	{
		m_dtAccumulator = 0.0;
		SelectTarget();

		super.OnEntry(e);
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
		
		/*float rescanTime = 3.0;
		if (m_dtAccumulator >= rescanTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_Bot.ProcessEvent(new WeaponEventReloadTimeout(p));*/

		if (m_Target == null)
		{
			int acc = m_dtAccumulator;
			if (acc % 5 == 0)
			{
				Print("Searching...");
				SelectTarget();
			}
		}
	}
};

class BotHunt_Tracking extends BotStateBase
{
	EntityAI m_Target;
	bool m_TargetInSight = false;
	bool m_TargetLost = false;
	bool m_Tracking = true;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);

		m_TargetLost = false;
		m_TargetInSight = false;
		m_Tracking = false;
	}

	override void OnAbort (BotEventBase e)
	{
		m_TargetLost = false;
		m_TargetInSight = false;
		m_Tracking = false;

		super.OnAbort(e);
	}

	override void OnExit (BotEventBase e)
	{
		m_TargetLost = false;
		m_TargetInSight = false;
		m_Tracking = false;

		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		if (m_Target)
		{
			m_Tracking = true;
			vector targetPos = m_Target.GetPosition();
			botDebugPrint("[bot] + " + m_Owner + " hunt Tracking target=" + m_Target + " pos=" + targetPos);

			// tmp dist check
			float d = vector.Distance(m_Target.GetPosition(), GetPlayerOwner().GetPosition());
			if (d < 2.0)
			{
				m_TargetInSight = true;
			}
			else
			{
				m_TargetInSight = false;
			}

			if (!m_TargetInSight)
			{
				GetPlayerOwner().GetInputController().OverrideMovementSpeed(true, 1);
				GetPlayerOwner().GetInputController().OverrideMovementAngle(true, 1);
			}
			else
			{
				GetPlayerOwner().GetInputController().OverrideMovementSpeed(false, 0);
				GetPlayerOwner().GetInputController().OverrideMovementAngle(false, 0);
			}

			/*if ((.GetInputController().LimitsIsSprintDisabled()))
				.GetInputController().OverrideMovementSpeed( true, 2 );
			else
				.GetInputController().OverrideMovementSpeed( true, 3 );*/

		}
		else
		{
			if (m_Tracking)
			{
				m_TargetLost = true;
				m_TargetInSight = false;
				m_Tracking = false;

				GetPlayerOwner().GetInputController().OverrideMovementSpeed(false, 0);
				GetPlayerOwner().GetInputController().OverrideMovementAngle(false, 0);
			}
		}
	}
};

class BotHunt_Hunting extends BotStateBase
{
	EntityAI m_Target;

	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
	}

	override void OnAbort (BotEventBase e) { super.OnAbort(e); }

	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
	}
};

Man BotSelectNearestTarget (EntityAI bot)
{
	/*ref array<Man> players = new array<Man>;
	g_Game.GetWorld().GetPlayerList( players );
		
	bool minimal_distance_ok = true;

	float min_dist = 1234567.0;
	int min_index = -1;
	for ( int i = 0; i < players.Count(); i++ )
	{
		float d = vector.Distance(players.Get(i).GetPosition(), bot.GetPosition());
		if ( d < min_dist )
		{
			min_dist = d;
			min_index = i;
		}
	}

	if (min_index != -1)
		return players.Get(min_index);

	return null;*/
		vector pos = bot.GetPosition();
		//vector dir = player.GetDirection();

	array<Object> objects = new array<Object>;
	array<CargoBase> proxyCargos = new array<CargoBase>;
	GetGame().GetObjectsAtPosition(pos, 100.0, objects, proxyCargos);

	float min_dist = 1234567.0;
	int min_index = -1;
	int c = objects.Count();
	for (int i = 0; i < c; i++)
	{
		Object o = objects[i];
		if (o == bot)
			continue;

		float d = vector.Distance(o.GetPosition(), bot.GetPosition());
		if ( d < min_dist )
		{
			min_dist = d;
			min_index = i;
		}
	}

	if (min_index != -1)
	{
		botDebugPrint("[bot] + " + bot + " BotSelectNearestTarget idx=" + min_index + " dist=" + min_dist + " obj=" + o);
		return Man.Cast( objects.Get(min_index) );
	}
	return null;
}


