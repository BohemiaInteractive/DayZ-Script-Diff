enum DefaultAnimState
{
	DEFAULT 			= 0, 	///< default weapon state, closed and discharged
};

/**@class		Rifle_Base
 * @brief		base for rifles
 * @NOTE		name copies config base class
 **/
class Rifle_Base extends Weapon_Base
{
	void Rifle_Base ()
	{
	}

	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));

		// setup state machine
		// basic weapon states
		WeaponStateBase E = new WeaponStableState(this, NULL, DefaultAnimState.DEFAULT);

		WeaponStateBase Mech = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		// events
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;

		m_fsm = new WeaponFSM();
		// charging
		m_fsm.AddTransition(new WeaponTransition(E		, __M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition(Mech	, _fin_,	E));
		m_fsm.AddTransition(new WeaponTransition(Mech	, _abt_,	E));
		
		m_fsm.SetInitialState(E);

		SelectionBulletHide();

		m_fsm.Start();
	}

};


