enum CFIMSAAnimState
{
	DEFAULT 			= 0, 	///< default weapon state
	OPENNED				= 1,
	JAMMED 				= 2,
};

enum CFIMSAStableStateID
{
	UNKNOWN		=	0,
	Empty		=	1,
	Open		= 	2,
	Loaded		=	3,
	Jammed		=	4,
}


class CFIMSAEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Empty E"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Empty E"); } }
	override int GetCurrentStateID () { return CFIMSAStableStateID.Empty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class CFIMSAOpenned extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Open O"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Open O"); } }
	override int GetCurrentStateID () { return CFIMSAStableStateID.Empty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class CFIMSALoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Loaded L"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Loaded L"); } }
	override int GetCurrentStateID () { return CFIMSAStableStateID.Loaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class CFIMSAJammed extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Jammed J"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Jammed J"); } }
	override int GetCurrentStateID () { return CFIMSAStableStateID.Jammed; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};


/**@class		BoltRifle_Base
 * @brief		base for bolt action rifles
 * @NOTE		name copies config base class
 **/
class ChamberFirst_InnerMagazine_SemiAutomatic_Base extends Rifle_Base
{
	ref WeaponStableState E;
	ref WeaponStableState O;
	ref	WeaponStableState L;
	ref	WeaponStableState J;
	
	
	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_EXTRA));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_WITHCHAMBER));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));
		
		
		E = new CFIMSAEmpty(this, NULL, CFIMSAAnimState.DEFAULT);
		O = new CFIMSAOpenned(this, NULL, CFIMSAAnimState.OPENNED);
		L = new CFIMSALoaded(this, NULL, CFIMSAAnimState.DEFAULT);
		J = new CFIMSAJammed(this, NULL, CFIMSAAnimState.JAMMED);
		
		WeaponStateBase	Mech_E = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponStateBase	Mech_O = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED);
		WeaponStateBase	Mech_L = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		
		LoopedChamberingCombineChamberInternalMagazine Chamber_E = new LoopedChamberingCombineChamberInternalMagazine(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_EXTRA, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingCombineChamberInternalMagazine Chamber_O = new LoopedChamberingCombineChamberInternalMagazine(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_OPENED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast Chamber_L = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_WITHCHAMBER, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		
		WeaponStateBase	Unjam_J = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);
		
		WeaponStateBase	Trigger_E = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase	Trigger_O = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase	Trigger_L = new WeaponFireAndChamberFromInnerMagazine(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase	Trigger_J = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase Trigger_LL = new WeaponFireLast(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST));
		
		WeaponStateBase	Trigger_LJ = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM);

		
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		WeaponEventBase __TJ_ = new WeaponEventTriggerToJam;
		WeaponEventBase __U__ = new WeaponEventUnjam;
		WeaponEventBase	__M__ = new WeaponEventMechanism;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

		m_fsm = new WeaponFSM();
		
		// Mechanism
//----------------------------------------
		m_fsm.AddTransition(new WeaponTransition( E,			__M__,	Mech_E));
		m_fsm.AddTransition(new WeaponTransition(  Mech_E,		_fin_,	E));
		m_fsm.AddTransition(new WeaponTransition(  Mech_E,		_abt_,	E));
		
		m_fsm.AddTransition(new WeaponTransition( O,			__M__,	Mech_O));
		m_fsm.AddTransition(new WeaponTransition(  Mech_O,		_fin_,	O, NULL, new WeaponGuardWeaponOpen(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_O,		_fin_,	E));
		m_fsm.AddTransition(new WeaponTransition(  Mech_O,		_abt_,	O, NULL, new WeaponGuardWeaponOpen(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_O,		_abt_,	E));
				
		m_fsm.AddTransition(new WeaponTransition( L,			__M__,	Mech_L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	L));		
//----------------------------------------
		
		m_fsm.AddTransition(new WeaponTransition( E,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	O, NULL, new WeaponGuardWeaponOpen(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	L));
		
		m_fsm.AddTransition(new WeaponTransition( O,			__L__,	Chamber_O));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O,	_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O,	_abt_,	O, NULL, new WeaponGuardWeaponOpen(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O,	_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O,	_abt_,	L));
		
		
		m_fsm.AddTransition(new WeaponTransition( L,			__L__,	Chamber_L, NULL, new GuardNot(new WeaponGuardInnerMagazineFullShareChamber(this))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L,	_abt_,	L));	
//------------------------------------------
		
		m_fsm.AddTransition(new WeaponTransition( J, 			__U__,	Unjam_J));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_fin_,	J, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_abt_,	J, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J,		_abt_,	L));
		
		
//-----------------------------------------
		
		// fire
		m_fsm.AddTransition(new WeaponTransition( E,			__T__,	Trigger_E)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E,	_fin_,	E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E,	_dto_,	E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E,	_abt_,	E));
		
		m_fsm.AddTransition(new WeaponTransition( O,			__T__,	Trigger_O)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O,	_fin_,	O));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O,	_dto_,	O));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O,	_abt_,	O));
		
		m_fsm.AddTransition(new WeaponTransition( L,				__T__,	Trigger_L, NULL, new WeaponGuardHasAmmoInnerMagazine(this)));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,		_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,		_rto_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,		_abt_,	L));
		
		m_fsm.AddTransition(new WeaponTransition( L,				__T__,	Trigger_LL));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LL,	_fin_,	O));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LL,	_rto_,	O));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LL,	_abt_,	O));
		
		m_fsm.AddTransition(new WeaponTransition( J,			__T__,	Trigger_J)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_fin_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_dto_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_abt_,	J));
		
		
		m_fsm.AddTransition(new WeaponTransition( L,			__TJ_,	Trigger_LJ)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_fin_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_rto_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_abt_,	J));
		
//---------------------------------------------	
		
		SetInitialState(E);

		SelectionBulletHide();
		HideMagazine();

		m_fsm.Start();
	}	
	
	override void SetActions()
	{
		AddAction(FirearmActionLoadMultiBulletQuick);
		super.SetActions();
		AddAction(FirearmActionLoadMultiBullet);
	}
};


