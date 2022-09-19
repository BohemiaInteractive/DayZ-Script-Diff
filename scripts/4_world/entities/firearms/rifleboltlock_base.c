enum RBLAnimState
{
	DEFAULT 			= 0, 	///< default weapon state, closed and discharged
	OPENED 				= 1,
	JAMMED 				= 2,
};

enum RBLStableStateID
{
	UNKNOWN				=  0,
	RBL_CLO_BU0_MA0		=  1,
	RBL_CLO_BU1_MA0		=  2,
	RBL_CLO_BU1_MA1		=  3,
	RBL_CLO_BU0_MA1		=  4,
	RBL_OPN_BU0_MA1		=  5,
	RBL_OPN_BU0_MA0		=  6,
	RBL_JAM_BU1_MA0		=  7,
	RBL_JAM_BU1_MA1		=  8
}

class RBL_CLO_BU0_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close nobull nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close nobull nomag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_CLO_BU0_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class RBL_CLO_BU1_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close bullet nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close bullet nomag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_CLO_BU1_MA0; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class RBL_CLO_BU1_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close bullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close bullet mag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_CLO_BU1_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class RBL_CLO_BU0_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close nobull mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close nobull mag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_CLO_BU0_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class RBL_OPN_BU0_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { lock nobull mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } open nobull mag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_OPN_BU0_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class RBL_OPN_BU0_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { lock nobull nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } open nobull nomag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_OPN_BU0_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class RBL_JAM_BU1_MA0 extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { jammed bullet nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } jammed bullet nomag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_JAM_BU1_MA0; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};
class RBL_JAM_BU1_MA1 extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { jammed bullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } jammed bullet mag"); } }
	override int GetCurrentStateID () { return RBLStableStateID.RBL_JAM_BU1_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};


/**@class		Rifle_Base
 * @brief		base for rifles
 * @NOTE		name copies config base class
 **/
class RifleBoltLock_Base extends Rifle_Base
{
	ref WeaponStateBase C00;
	ref	WeaponStateBase C10;
	ref	WeaponStateBase C11;
	ref	WeaponStateBase C01;
	ref	WeaponStateBase O00;
	ref	WeaponStateBase O01;
	ref	WeaponStateBase JF0;
	ref	WeaponStateBase JF1;

	void RifleBoltLock_Base ()
	{
	}
	
	override void InitStateMachine()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET_OPEN)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET_OPEN)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH)));

		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_UNIQUE_CLOSED)));
		
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED)));//????
		
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END)));
		
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM)));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY)));
		
		//m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));

		// setup state machine
		// basic weapon states
		// close-locked-jammed | nobullet-bullet | nomag-mag
		// regexp: [CLJ][01][01]
		C00 = new RBL_CLO_BU0_MA0(this, NULL, RBLAnimState.DEFAULT);
		C10 = new RBL_CLO_BU1_MA0(this, NULL, RBLAnimState.DEFAULT);
		C11 = new RBL_CLO_BU1_MA1(this, NULL, RBLAnimState.DEFAULT);
		C01 = new RBL_CLO_BU0_MA1(this, NULL, RBLAnimState.DEFAULT);
		O00 = new RBL_OPN_BU0_MA0(this, NULL, RBLAnimState.OPENED);
		O01 = new RBL_OPN_BU0_MA1(this, NULL, RBLAnimState.OPENED);
		JF0 = new RBL_JAM_BU1_MA0(this, NULL, RBLAnimState.JAMMED);
		JF1 = new RBL_JAM_BU1_MA1(this, NULL, RBLAnimState.JAMMED);

		// unstable (intermediate) states
		WeaponCharging Mech_C00 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		WeaponCharging Mech_C01 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		WeaponCharging Mech_O00 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
		WeaponCharging Mech_O01 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
	
		WeaponCharging Mech_C10 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		WeaponCharging Mech_C11 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		
		WeaponStateBase Trigger_C00 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY)); // cock without clip
		WeaponStateBase Trigger_C10 = new WeaponFireLast(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL)); // cock with clip
		WeaponFireAndChamberNext Trigger_C11 = new WeaponFireAndChamberNext(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		WeaponStateBase Trigger_C11L = new WeaponFireLast(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST));
		WeaponStateBase Trigger_C01 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_O00 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_O01 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_JF0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_JF1 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
	
		//Fire and jam
		WeaponStateBase Trigger_C10J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM)); // cock with clip
		WeaponStateBase Trigger_C11J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));
		
//		WeaponStateBase Trigger_C11J = new WeaponFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));

		WeaponStateBase Unjam_JF0 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		WeaponStateBase Unjam_JF1 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));

		WeaponChambering Chamber_C00 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED));
		WeaponChambering Chamber_C01 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED));
		WeaponChambering Chamber_O00 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_UNIQUE_CLOSED));
		WeaponChambering Chamber_O01 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, GetWeaponSpecificCommand(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_UNIQUE_CLOSED));

		WeaponAttachMagazine Attach_C00 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET));
		WeaponAttachMagazine Attach_C10 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET));
		WeaponAttachMagazine Attach_O00 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET_OPEN));//TODO Need add open animation
		WeaponAttachMagazine Attach_JF0 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET));
		
		WeaponReplacingMagAndChamberNext Reload_C11 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET));
		WeaponReplacingMagAndChamberNext Reload_C01 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET));
		WeaponReplacingMagAndChamberNext Reload_O01 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET_OPEN));//TODO Need add open animation
		WeaponReplacingMagAndChamberNext Reload_JF1 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET));
		
		WeaponDetachingMag Detach_C11 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		WeaponDetachingMag Detach_C01 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		WeaponDetachingMag Detach_O01 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		WeaponDetachingMag Detach_JF1 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		
		//WeaponStateBase chmMag = new WeaponChamberFromAttMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		// events
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		WeaponEventBase __TJ_ = new WeaponEventTriggerToJam;
		WeaponEventBase __U__ = new WeaponEventUnjam;
		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase __A__ = new WeaponEventAttachMagazine;
		WeaponEventBase __D__ = new WeaponEventDetachMagazine;
		WeaponEventBase __S__ = new WeaponEventSwapMagazine;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

		m_fsm = new WeaponFSM();

		// order: Discharged-Charged | nobullet-bullet | nomag-mag
		// charging
		m_fsm.AddTransition(new WeaponTransition( C00, 				__M__,	Mech_C00)); // charge from dischg nobullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_C00,		_fin_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C00,		_abt_,	C00));

		m_fsm.AddTransition(new WeaponTransition( C10,				__M__,	Mech_C10)); // charge from bullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_C10,		_fin_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C10,		_abt_,	C00, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C10, 		_abt_,	C10));
		
		m_fsm.AddTransition(new WeaponTransition( C11,				__M__,	Mech_C11)); // charge from dischg nobullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_C11,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C11,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C11,		_abt_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C11,		_abt_,	C11));
		
		
		m_fsm.AddTransition(new WeaponTransition( C01,				__M__,	Mech_C01)); // charge from dischg nobullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_C01,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C01,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C01,		_abt_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C01,		_abt_,	C11));
		
		m_fsm.AddTransition(new WeaponTransition( O00,				__M__,	Mech_O00)); // charge from dischg nobullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_O00,		_fin_,	C00));
			Mech_O00.AddTransition(new WeaponTransition( Mech_O00.m_start,			_abt_,	O00));
			Mech_O00.AddTransition(new WeaponTransition( Mech_O00.m_eject,			_abt_,	C00));
			Mech_O00.AddTransition(new WeaponTransition( Mech_O00.m_onCK,			_abt_,	C00));
			Mech_O00.AddTransition(new WeaponTransition( Mech_O00.m_chamber,		_abt_,	C00));
		
		m_fsm.AddTransition(new WeaponTransition( O01,				__M__,	Mech_O01)); // charge from dischg nobullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_O01,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_O01,		_fin_,	C11)); 
			Mech_O01.AddTransition(new WeaponTransition( Mech_O01.m_start,			_abt_,	O01));
			Mech_O01.AddTransition(new WeaponTransition( Mech_O01.m_eject,			_abt_,	C01));
			Mech_O01.AddTransition(new WeaponTransition( Mech_O01.m_onCK,			_abt_,	C01));
			Mech_O01.AddTransition(new WeaponTransition( Mech_O01.m_chamber,		_abt_,	C11));

		// fire
		m_fsm.AddTransition(new WeaponTransition( C00,				__T__,	Trigger_C00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C00,		_fin_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C00,		_dto_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C00,		_abt_,	C00));
		
		m_fsm.AddTransition(new WeaponTransition( C01,				__T__,	Trigger_C01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C01,		_fin_,	C01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C01,		_dto_,	C01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C01,		_abt_,	C01));
		
		m_fsm.AddTransition(new WeaponTransition( C10,				__T__,	Trigger_C10));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10,		_fin_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10,		_rto_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10,		_abt_,	C00));
		
		m_fsm.AddTransition(new WeaponTransition( C11,				__T__,	Trigger_C11, NULL, new WeaponGuardHasAmmo(this)));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11,		_rto_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11,		_abt_,	C11));
		
		m_fsm.AddTransition(new WeaponTransition( C11,				__T__,	Trigger_C11L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11L,	_fin_,	O01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11L,	_rto_,	O01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11L,	_abt_,	O01));
	
		m_fsm.AddTransition(new WeaponTransition( O00,				__T__,	Trigger_O00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O00,		_fin_,	O00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O00,		_dto_,	O00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O00,		_abt_,	O00));
		
		m_fsm.AddTransition(new WeaponTransition( O01,				__T__,	Trigger_O01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O01,		_fin_,	O01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O01,		_dto_,	O01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O01,		_abt_,	O01));
		
		m_fsm.AddTransition(new WeaponTransition( JF0,				__T__,	Trigger_JF0)); // opened fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF0,		_fin_,	JF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF0,		_dto_,	JF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF0,		_abt_,	JF0));
		
		m_fsm.AddTransition(new WeaponTransition( JF1,				__T__,	Trigger_JF1)); // opened fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF1,		_fin_,	JF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF1,		_dto_,	JF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF1,		_abt_,	JF1));
		

		m_fsm.AddTransition(new WeaponTransition( C10,				__TJ_,	Trigger_C10J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10J,	_fin_,	JF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10J,	_rto_,	JF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C10J,	_abt_,	JF0));		
		
		m_fsm.AddTransition(new WeaponTransition( C11,				__TJ_,	Trigger_C11J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11J,	_fin_,	JF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11J,	_rto_,	JF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C11J,	_abt_,	JF1));	


		// load cartridge
		m_fsm.AddTransition(new WeaponTransition( C00,				__L__,	Chamber_C00)); // chamber from closed=1
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C00,		_fin_,	C10));
			Chamber_C00.AddTransition(new WeaponTransition( Chamber_C00.m_start,	_abt_,	C00));
			Chamber_C00.AddTransition(new WeaponTransition( Chamber_C00.m_eject,	_abt_,	O00));
			Chamber_C00.AddTransition(new WeaponTransition( Chamber_C00.m_chamber,	_abt_,	O00));
			Chamber_C00.AddTransition(new WeaponTransition( Chamber_C00.m_w4t,		_abt_,	C10));

		m_fsm.AddTransition(new WeaponTransition( C01,				__L__,	Chamber_C01)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C01,		_fin_,	C11));
			Chamber_C01.AddTransition(new WeaponTransition( Chamber_C01.m_start,	_abt_,	C01));
			Chamber_C01.AddTransition(new WeaponTransition( Chamber_C01.m_eject,	_abt_,	O01));
			Chamber_C01.AddTransition(new WeaponTransition( Chamber_C01.m_chamber,	_abt_,	O01));
			Chamber_C01.AddTransition(new WeaponTransition( Chamber_C01.m_w4t,		_abt_,	C11));
	
		m_fsm.AddTransition(new WeaponTransition( O00,				__L__,	Chamber_O00)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O00,		_fin_,	C10));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O00,		_abt_ , O00, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O00,		_abt_ , C10));
	
		m_fsm.AddTransition(new WeaponTransition( O01,				__L__,	Chamber_O01)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O01,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O01,		_abt_,	O01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_O01,		_abt_,	C11));

		// attach magazine (no mag in weapon)
		m_fsm.AddTransition(new WeaponTransition( C00,				__A__,	Attach_C00, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b0/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_C00,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Attach_C00,		_fin_,	C11));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_C00,		_abt_,	C11, NULL, new WeaponGuardCurrentChamberFull(this)) );
		//m_fsm.AddTransition(new WeaponTransition(  Attach_C00,		_abt_,	C01, NULL, new WeaponGuardHasMag(this)) );
		//m_fsm.AddTransition(new WeaponTransition(  Attach_C00,		_abt_,	C00));
			Attach_C00.AddTransition(new WeaponTransition( Attach_C00.m_start,		_abt_,	C00));
			Attach_C00.AddTransition(new WeaponTransition( Attach_C00.m_attach,		_abt_,	C00));
			Attach_C00.AddTransition(new WeaponTransition( Attach_C00.m_onCK,		_abt_,	C01));
			Attach_C00.AddTransition(new WeaponTransition( Attach_C00.m_chamber,	_abt_,	C11));		

		m_fsm.AddTransition(new WeaponTransition( C10,				__A__,	Attach_C10, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b1/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_C10,		_fin_,	C11));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_C10,		_abt_,	C11, NULL, new WeaponGuardHasMag(this)) );
		//m_fsm.AddTransition(new WeaponTransition(  Attach_C10,		_abt_,	C10));
			Attach_C10.AddTransition(new WeaponTransition( Attach_C10.m_start,		_abt_,	C10));
			Attach_C10.AddTransition(new WeaponTransition( Attach_C10.m_attach,		_abt_,	C10));
	
		m_fsm.AddTransition(new WeaponTransition( O00,				__A__,	Attach_O00, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b0/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_O00,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O00,		_fin_,	C11));
			Attach_O00.AddTransition(new WeaponTransition( Attach_O00.m_start,		_abt_,	O00));
			Attach_O00.AddTransition(new WeaponTransition( Attach_O00.m_attach,		_abt_,	O00));
			Attach_O00.AddTransition(new WeaponTransition( Attach_O00.m_onCK,		_abt_,	C01));
			Attach_O00.AddTransition(new WeaponTransition( Attach_O00.m_chamber,	_abt_,	C11));
		
		m_fsm.AddTransition(new WeaponTransition( JF0,				__A__,	Attach_JF0, NULL, new WeaponGuardCanAttachMag(this))); // attach from JAM/b1/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_JF0,	_fin_,	JF1));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_JF0,	_abt_,	JF1, NULL, new WeaponGuardHasMag(this)));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_JF0,	_abt_,	JF0));
			Attach_JF0.AddTransition(new WeaponTransition( Attach_JF0.m_start,		_abt_,	JF0));
			Attach_JF0.AddTransition(new WeaponTransition( Attach_JF0.m_attach,		_abt_,	JF0));


		// replace magazine
		m_fsm.AddTransition(new WeaponTransition( C11,				__S__,	Reload_C11, NULL, new WeaponGuardCanSwapMag(this))); // swap in Chg/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_C11,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C11,		_abt_,	C11, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C11,		_abt_,	C10));

	
		m_fsm.AddTransition(new WeaponTransition( C01,				__S__,	Reload_C01, NULL, new WeaponGuardCanSwapMag(this))); // swap in CLO/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_C01,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C01,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C01,		_abt_,	C11, NULL, new WeaponGuardCurrentChamberFull(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_C01,		_abt_,	C01, NULL, new WeaponGuardHasMag(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_C01,		_abt_,	C00));
		
	
		m_fsm.AddTransition(new WeaponTransition( O01,				__S__,	Reload_O01, NULL, new WeaponGuardCanSwapMag(this))); // swap in CLO/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_O01,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_O01,		_fin_,	C11));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_start ,    _abt_,   O01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_eject,     _abt_,   O01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_detach,    _abt_,   O01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_hideOld,   _abt_,   O00));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_swapMags,  _abt_,   O01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_attach,    _abt_,   O01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_onCK,      _abt_,   C01));
			Reload_O01.AddTransition(new WeaponTransition( Reload_O01.m_chamber,   _abt_,   C11));

		m_fsm.AddTransition(new WeaponTransition( JF1,				__S__,	Reload_JF1, NULL, new WeaponGuardCanSwapMag(this))); // swap in JAM/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_JF1,		_fin_,	JF1));
		m_fsm.AddTransition(new WeaponTransition(  Reload_JF1,		_abt_,	JF1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_JF1,		_abt_,	JF0));
			
      // @TODO: abort in jam

		// detach magazine
		m_fsm.AddTransition(new WeaponTransition( C11,				__D__,	Detach_C11, NULL, new WeaponGuardCanDetachMag(this))); // detach from Chg/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_C11,		_fin_,	C10));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C11,		_abt_,	C11, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C11,		_abt_,	C10));
		
		m_fsm.AddTransition(new WeaponTransition( C01,				__D__,	Detach_C01, NULL, new WeaponGuardCanDetachMag(this))); // detach from Chg/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_C01,		_fin_,	C00));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C01,		_abt_,	C01, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C01, 		_abt_,	C00));
		
		m_fsm.AddTransition(new WeaponTransition( O01,				__D__,	Detach_O01, NULL, new WeaponGuardCanDetachMag(this))); // detach from Dic/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_O01,		_fin_,	O00));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O01,		_abt_,	O01, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O01,		_abt_,	O00));
		
		m_fsm.AddTransition(new WeaponTransition( JF1,				__D__,	Detach_JF1, NULL, new WeaponGuardCanDetachMag(this))); // detach from CLO/b1/m1 jammed
		m_fsm.AddTransition(new WeaponTransition(  Detach_JF1,		_fin_,	JF0));
		m_fsm.AddTransition(new WeaponTransition(  Detach_JF1,		_abt_,	JF1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_JF1,		_abt_,	JF0));



		// unjam
		m_fsm.AddTransition(new WeaponTransition( JF0,				__U__,	Unjam_JF0)); // unjam nomag
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF0,		_fin_,	JF0, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF0,		_fin_,	C00, NULL, new GuardNot(new WeaponGuardJammed(this))));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF0,		_abt_,	JF0));
		
		m_fsm.AddTransition(new WeaponTransition( JF1,				__U__,	Unjam_JF1)); // unjam with mag with ammo
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF1,		_fin_,	JF1, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF1,		_fin_,	C01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF1,		_fin_,	C11));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF1,		_abt_,	JF1));

		m_fsm.SetInitialState(C00);

		SelectionBulletHide();
		HideMagazine();

		m_fsm.Start();
	}
	
	override float GetChanceToJam()
	{
		float chanceToJam = super.GetChanceToJam();
		Magazine mag = GetMagazine(GetCurrentMuzzle());
		
		if(mag)
		{
			chanceToJam = chanceToJam + ((1.0 - chanceToJam) * mag.GetChanceToJam());
		}
		else
		{
			chanceToJam = chanceToJam + ((1.0 - chanceToJam) * 0.1);
		}
		
		return chanceToJam;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionDetachMagazine);
		//AddAction(ActionAdvancedDetachMagazine);
	}
};


