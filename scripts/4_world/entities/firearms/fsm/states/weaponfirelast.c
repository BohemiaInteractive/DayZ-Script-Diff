
class WeaponFireLast extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;
	
	float m_dtAccumulator;
	ref WeaponFireWithEject m_fire;

	void WeaponFireLast (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_fire = new WeaponFireWithEject(m_weapon, this, m_action, m_actionType);

		// events
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventReloadTimeout __to_ = new WeaponEventReloadTimeout;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm

		// transitions
		m_fsm.AddTransition(new WeaponTransition(m_fire, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(m_fire, __to_, NULL));

		m_fsm.SetInitialState(m_fire);
	}

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
			m_dtAccumulator = 0;
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dtAccumulator = 0;
		super.OnExit(e);
	}
};


