#ifdef ROBOCLIENT
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
	protected ref BotFSM m_ActiveFSM = null;
	protected DayZPlayerInstanceType m_InstanceType = DayZPlayerInstanceType.INSTANCETYPE_CLIENT;
	private ref BotStateBase m_Attack;
	private ref BotStateBase m_TurnValidForward;
	private ref BotStateBase m_MoveForward;
	protected ref BotStateBase m_IdleState = null;

	private ref map<ref BotFSM, BotStateMachineData> m_StateMachines;
	private ref map<string, BotStateBase> m_States;
	private ref map<string, BotEventBase> m_Events;
	private ref map<string, BotActionBase> m_Actions;
	private ref map<string, BotGuardBase> m_Guards;

	private const vector m_InfectedHalfRange = "2 0 2";
	EntityAI m_Target = null;	

	bool m_Active = true;

	// events
	ref BotEventBase ___Bgn__ = new BotEventStart;
	ref BotEventBase __Stop__ = new BotEventStop;
	ref BotEventBase ___OK___ = new BotEventEndOK;
	ref BotEventBase __Fail__ = new BotEventEndFail;
	ref BotEventBase __Tout__ = new BotEventEndTimeout;
	ref BotEventBase __IChg__ = new BotEventOnItemInHandsChanged;
	ref BotEventBase __Obst__ = new BotEventObstacle;
	ref BotEventBase __VaFw__ = new BotEventValidForward;
	ref BotEventBase __Atkd__ = new BotEventAttacked;


	void Bot (PlayerBase ow)
	{
		m_Owner = ow;

		// basic states
		m_IdleState = new BotStateIdle(this, NULL);
		m_TurnValidForward = new BotTurnToValidForward(this, NULL);
		m_Attack = new BotAttack(this, NULL);
		m_MoveForward = new BotMoveForward(this, NULL);

		m_States = new map<string, BotStateBase>;
		m_States.Insert(m_IdleState.ClassName(), m_IdleState);
		m_States.Insert(m_MoveForward.ClassName(), m_MoveForward);
		m_States.Insert(m_TurnValidForward.ClassName(), m_TurnValidForward);
		m_States.Insert(m_Attack.ClassName(), m_Attack);
		m_States.Insert("None", NULL);

		m_Events = new map<string, BotEventBase>;
		m_Events.Insert(__Obst__.ClassName(), __Obst__);
		m_Events.Insert(__VaFw__.ClassName(), __VaFw__);
		m_Events.Insert(__Atkd__.ClassName(), __Atkd__);
		m_Events.Insert("None", NULL);

		m_Actions = new map<string, BotActionBase>;
		m_Actions.Insert("None", NULL);

		m_Guards = new map<string, BotGuardBase>;
		m_Guards.Insert("None", NULL);

		m_StateMachines = new map<ref BotFSM, BotStateMachineData>;
		RoboclientUtils.ReadStateMachines();
		GetStateMachines();

		InitFSM();
		RegisterStateMachineDiag();
	}

	void ~Bot()
	{
		delete m_States;
		delete m_Events;
		delete m_Actions;
		delete m_Guards;
		delete m_StateMachines;	
	}

	void RegisterStateMachineDiag()
	{
		string stateMachineNames;
		int activeStateMachineIndex = 0;
		for(int i = 0; i < m_StateMachines.Count(); i++)
		{
			stateMachineNames += m_StateMachines.GetElement(i).GetName();
			if (i != m_StateMachines.Count() - 1)
				stateMachineNames += ",";
			if (m_ActiveFSM == m_StateMachines.GetKey(i))
				activeStateMachineIndex = i;
		}

#ifdef DIAG_DEVELOPER
		DiagMenu.RegisterItem(DiagMenuIDs.ROBOCLIENTS_STATE_MACHINES, "", "State Machine", DiagMenuIDs.ROBOCLIENTS_MENU, stateMachineNames);
		DiagMenu.SetValue(DiagMenuIDs.ROBOCLIENTS_STATE_MACHINES, activeStateMachineIndex);
		DiagMenu.BindCallback(DiagMenuIDs.ROBOCLIENTS_STATE_MACHINES, PluginDiagMenuClient.CBRoboclientChangeStateMachine);
#endif
	}
	
	void SetInstanceType (DayZPlayerInstanceType t) { m_InstanceType = t; }

	void GetStateMachines()
	{
		array<BotStateMachineData> stateMachinesData = new array<BotStateMachineData>;
		RoboclientUtils.GetStateMachines(stateMachinesData);
		for (int i = 0; i < stateMachinesData.Count(); i++)
		{
			string name = stateMachinesData[i].GetName();
			TStringArray from = new TStringArray;
			stateMachinesData[i].GetFromStates(from);
			TStringArray to = new TStringArray;
			stateMachinesData[i].GetToStates(to);
			TStringArray evnt = new TStringArray;
			stateMachinesData[i].GetEvents(evnt);
			TStringArray action = new TStringArray; 
			stateMachinesData[i].GetActions(action);
			TStringArray guard = new TStringArray;
			TStringArray guardDecorator = new TStringArray;
			stateMachinesData[i].GetGuards(guard, guardDecorator);
			//checking the number of from states, transition events, to states and actions are equal since there is one for each transition
			//there are 2 guards for each transition, so they must be twice as many as from states (or events, to states, action)
			if (from.Count() != to.Count() || from.Count() != evnt.Count() || from.Count() != action.Count() || from.Count() != guardDecorator.Count() || 2 * from.Count() != guard.Count())
				continue;

			BotFSM fsm = new BotFSM();
			for(int j = 0; j < from.Count(); j++)
			{
				BotGuardBase resultGuard = NULL;
				if (m_Guards[guard[2 * j]])
				{
					switch (guardDecorator[j])
					{
						case "Not":
							resultGuard = BotGuardNot(m_Guards[guard[2 * j]]);
							break;
						case "And":
							if (m_Guards[guard[2 * j + 1]])
								resultGuard = BotGuardAnd(m_Guards[guard[2 * j]], m_Guards[guard[2 * j + 1]]);
							break;
						case "Or":
							if (m_Guards[guard[2 * j + 1]])
								resultGuard = BotGuardOr(m_Guards[guard[2 * j]], m_Guards[guard[2 * j + 1]]);
							break;
						default:
							if (guardDecorator[j] != "None")
							break;
					}
					
					if (!resultGuard)
						resultGuard = m_Guards[guard[2 * j]];
				}
				fsm.AddTransition(new BotTransition(m_States[from[j]], m_Events[evnt[j]], m_States[to[j]], m_Actions[action[j]], resultGuard));
			}
			fsm.SetInitialState(m_States[stateMachinesData[i].GetInitialState()]);
			m_StateMachines.Insert(fsm, stateMachinesData[i]);
			if (stateMachinesData[i].GetActive())
				m_ActiveFSM = fsm;
		}
	}

	void Start ()
	{
		if (!m_ActiveFSM || m_ActiveFSM.IsRunning())
			return;

		m_Target = null;
		m_ActiveFSM.Start();
	}

	void Stop ()
	{
		if (!m_ActiveFSM)
			return;

		m_ActiveFSM.Terminate();
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveAllOverrides();
#endif	
	}

	void Restart()
	{
		Stop();
		Start();
	}

	void StartAction(int action)
	{
		switch (action)
		{
		case EActions.PLAYER_BOT_STOP_CURRENT:
			ProcessEvent(new BotEventStop(m_Owner, null));
			return;
		}

		ProcessEvent(new BotEventStartDebug(m_Owner, null, action));
	}

	void OnUpdate (float dt)
	{
		if (!m_ActiveFSM)
			return;

		if (m_ActiveFSM.IsRunning())
			m_ActiveFSM.GetCurrentState().OnUpdate(dt);
	}

	bool GetShouldRespawn()
	{
		BotStateMachineData data = NULL;
		if (m_StateMachines.Find(m_ActiveFSM, data))
			return data.GetRoboRespawn();
		return RoboclientUtils.GetDefaultRoboRespawn();
	}

	int GetRespawnTime()
	{
		BotStateMachineData data = NULL;
		if (m_StateMachines.Find(m_ActiveFSM, data))
			return data.GetRoboRespawnTime();
		return RoboclientUtils.GetDefaultRoboRespawnTime();
	}

	void GetRespawnData(out bool shouldRespawn, out int respawnTime)
	{
		BotStateMachineData data = NULL;
		if (m_StateMachines.Find(m_ActiveFSM, data))
		{
			shouldRespawn = data.GetRoboRespawn();
			respawnTime = data.GetRoboRespawnTime();
			return;
		}
		shouldRespawn = RoboclientUtils.GetDefaultRoboRespawn();
		respawnTime = RoboclientUtils.GetDefaultRoboRespawnTime();
	}

	void ChangeStateMachine (int index)
	{
		if (index >= m_StateMachines.Count())
			return;

		Stop();	
		m_ActiveFSM = m_StateMachines.GetKey(index);
		if (DiagMenu.GetBool(DiagMenuIDs.ROBOCLIENTS_ACTIVE))
			Start();	
	}

	void ChangeStateMachine (string stateMachineName)
	{
		BotFSM newStateMachine;
		for (int i = 0; i < m_StateMachines.Count(); i++)
		{
			if (m_StateMachines.GetElement(i).GetName() == stateMachineName)
			{		
				ChangeStateMachine(i);
				return;
			}	
		}
	}

	private bool IsEntityEnemy(EntityAI entity)
	{
		if (DayZInfected.Cast(entity))
			return true;

		PlayerBase p = PlayerBase.Cast(entity);
		if (p)
		{
			return !p.IsUnconscious();
		}

		if (DayZAnimal.Cast(entity))
			return true;

		return false;
	}

	void CheckForEnemies(bool Force = false)
	{
		if (!m_ActiveFSM)
			return;

		vector min = m_Owner.GetPosition() - m_InfectedHalfRange;
		vector max = m_Owner.GetPosition() + m_InfectedHalfRange + "0 2 0";
		array<EntityAI> entities = {};
		DayZPlayerUtils.SceneGetEntitiesInBox(min, max, entities);

		if (m_Target && m_Target.IsAlive() && entities.Find(m_Target) != -1)
		{
			if (Force || m_ActiveFSM.GetCurrentState() != m_Attack)
				ProcessEvent(__Atkd__);

			return;
		}

		for (int i = 0; i < entities.Count(); i++)
		{
			if (entities[i] != m_Owner && entities[i].IsAlive() && IsEntityEnemy(entities[i]))
			{
				if (Force || m_ActiveFSM.GetCurrentState() != m_Attack)
				{
					m_Target = entities[i];
					ProcessEvent(__Atkd__);
				}
				return;
			}
		}

		m_Target = null;
		ProcessEvent(__VaFw__);
	}

	void AddTransition(BotStateBase state, int id)
	{
		BotEventStartDebug evt = new BotEventStartDebug(null, null, id);

		m_ActiveFSM.AddTransition(new BotTransition(m_IdleState, evt, state, null, new BotGuardDebugEventMatches(evt)));

		m_ActiveFSM.AddTransition(new BotTransition(state, ___OK___, m_IdleState));
		m_ActiveFSM.AddTransition(new BotTransition(state, __Fail__, m_IdleState));
		m_ActiveFSM.AddTransition(new BotTransition(state, __Tout__, m_IdleState));

		m_ActiveFSM.AddTransition(new BotTransition(state, __Stop__, m_IdleState));
	}

	void InitFSM ()
	{	
		if (GetGame().m_IsRoboclient)
		{
			//Script version of the default state machine that's defined in the config file:
			//m_ActiveFSM = new BotFSM();
			//m_ActiveFSM.AddTransition(new BotTransition(m_MoveForward, __Obst__, m_TurnValidForward));
			//m_ActiveFSM.AddTransition(new BotTransition(m_MoveForward, __Atkd__, m_Attack));
			//m_ActiveFSM.AddTransition(new BotTransition(m_TurnValidForward, __VaFw__, m_MoveForward));
			//m_ActiveFSM.AddTransition(new BotTransition(m_TurnValidForward, __Atkd__, m_Attack));
			//m_ActiveFSM.AddTransition(new BotTransition(m_Attack, __VaFw__, m_MoveForward));
			//m_ActiveFSM.SetInitialState(m_MoveForward);
			//BotStateMachineData data = new BotStateMachineData();
			//data.SetName("Default");
			//data.SetRoboRespawn(true);
			//data.SetRoboRespawnTime(5);
			//m_StateMachines.Insert(m_ActiveFSM, data);
		}
		else
		{
			m_ActiveFSM = new BotFSM();
			AddTransition(new BotStanceRandomizer(this, NULL), EActions.PLAYER_BOT_RANDOMIZE_STANCE);
			AddTransition(new BotMovementRandomizer(this, NULL), EActions.PLAYER_BOT_RANDOMIZE_MOVEMENT);
			AddTransition(new BotTestSpamUserActions(this, NULL), EActions.PLAYER_BOT_SPAM_USER_ACTIONS);
			AddTransition(new BotTestAttachAndDropCycle(this, NULL), EActions.PLAYER_BOT_TEST_ATTACH_AND_DROP_CYCLE);
			AddTransition(new BotTestItemMoveBackAndForth(this, NULL), EActions.PLAYER_BOT_TEST_ITEM_MOVE_BACK_AND_FORTH);
			AddTransition(new Bot_TestSpawnOpen(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN);
			AddTransition(new Bot_TestSpawnOpenDestroy(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN_DESTROY);
			AddTransition(new Bot_TestSpawnOpenEat(this, NULL), EActions.PLAYER_BOT_TEST_SPAWN_OPEN_EAT);
			AddTransition(new BotTestSwapG2H(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_G2H);
			//AddTransition(new BotTestSwapC2H(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_C2H);
			AddTransition(new BotTestSwapInternal(this, NULL), EActions.PLAYER_BOT_TEST_SWAP_INTERNAL);

			///@} transition table

			m_ActiveFSM.SetInitialState(m_IdleState);
		}
	}
	
	bool ProcessEvent (BotEventBase e)
	{
		if (m_ActiveFSM.ProcessEvent(e) == ProcessEventResult.FSM_OK)
		{
			botDebugSpam("[botfsm] Processed event e=" + e.ToString());
			return true;
		}
		else
		{
			botDebugSpam("[botfsm] FSM refused to process event (no transition): src=" + m_ActiveFSM.GetCurrentState().ToString() + " event=" + e.ToString());
			return false;
		}
	}
};

void botDebugPrint (string s)
{
#ifdef BOT_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

void botDebugSpam (string s)
{
#ifdef BOT_DEBUG_SPAM
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
#endif