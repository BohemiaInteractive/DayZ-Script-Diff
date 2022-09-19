class WeaponFireAndChamberNext extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;

	float m_dtAccumulator;
	ref WeaponFire m_fire;

	void WeaponFireAndChamberNext (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_fire = new WeaponFireAndChamber(m_weapon, this, m_action, m_actionType);

		// events
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventAnimBulletEject  __be_ = new WeaponEventAnimBulletEject;
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

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;
		DayZPlayerImplement p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());
		if( p )
		{
			HumanInputController hic = p.GetInputController();

			int muzzleIndex = m_weapon.GetCurrentMuzzle();
			float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
			
			if ( m_dtAccumulator >= reloadTime && ( hic.IsAttackButton() || (m_weapon.GetBurstCount() < m_weapon.GetCurrentModeBurstSize(muzzleIndex))))
			{
				if (m_weapon.CanProcessWeaponEvents())
				{
					m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
				}
			}
		}
	}

	override void OnExit (WeaponEventBase e)
	{
		//m_weapon.ResetBurstCount();
		m_dtAccumulator = 0;
		super.OnExit(e);
	}
	
	override void OnAbort (WeaponEventBase e)
	{
		m_weapon.ResetBurstCount();
		super.OnAbort(e);
	}
};


