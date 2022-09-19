
enum PistolAnimState
{
	DEFAULT 			= 0, 	///< default weapon state, closed and discharged
	OPENED_DISCHARGED 	= 1, 	///< opened and discharged
	CLOSED_CHARGED		= 2,	///< closed and charged
	JAMMED				= 3		///< jammed weapon
};

// naming convention of the states respect following order:
// closed/opened | charged/discharged | bullet/nobullet | magazine/nomag
enum PistolStableStateID
{
	UNKNOWN				=  0,
	CLO_DIS_BU0_MA0		=  1,
	CLO_CHG_BU0_MA0		=  2,
	CLO_CHG_BU1_MA0		=  3,
	CLO_JAM_BU1_MA0		=  4,
	OPE_DIS_BU0_MA0		=  5,
	CLO_CHG_BU0_MA1		=  6,
	CLO_DIS_BU0_MA1		=  7,
	CLO_JAM_BU1_MA1		=  8,
	CLO_CHG_BU1_MA1		=  9,
	OPE_DIS_BU0_MA1		= 10,
}

class Pistol_CLO_DIS_BU0_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CD00 closed dischgd nobull nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CD00 closed dischgd nobull nomag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_DIS_BU0_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class Pistol_CLO_CHG_BU0_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CC00 closed charged nobull nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CC00 closed charged nobull nomag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_CHG_BU0_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class Pistol_CLO_CHG_BU1_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CC10 closed charged bullet nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CC10 closed charged bullet nomag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_CHG_BU1_MA0; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class Pistol_CLO_JAM_BU1_MA0 extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CJF0 closed jammed bullet nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CJF0 closed jammed bullet nomag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_JAM_BU1_MA0; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};
class Pistol_OPE_DIS_BU0_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { OD00 opened dischgd nobullet nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } 0D00 opened dischgd nobullet nomag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.OPE_DIS_BU0_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class Pistol_CLO_CHG_BU0_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CC01 closed charged nobullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CC01 closed charged nobullet mag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_CHG_BU0_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class Pistol_CLO_DIS_BU0_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CD01 closed dischgd nobullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CD01 closed dischgd nobullet mag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_DIS_BU0_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class Pistol_CLO_JAM_BU1_MA1 extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CJF1 closed jammed bullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CJF1 closed jammed bullet mag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_JAM_BU1_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class Pistol_CLO_CHG_BU1_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { CC11 closed charged bullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } CC11 closed charged bullet mag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.CLO_CHG_BU1_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class Pistol_OPE_DIS_BU0_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { OD01 opened dischgd nobullet mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] } 0D01 opened dischgd nobullet mag"); } }
	override int GetCurrentStateID () { return PistolStableStateID.OPE_DIS_BU0_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};


/**@class		Pistol_Base
 * @brief		base for semi auto weapons
 * @NOTE		name copies config base class
 **/
class Pistol_Base extends Weapon_Base
{
	ref WeaponStateBase CD00;
	ref WeaponStateBase CC00;
	ref WeaponStateBase CC10;
	ref WeaponStateBase CJF0;
	ref WeaponStateBase OD00;
	ref WeaponStateBase CD01;
	ref WeaponStateBase CC01;
	ref WeaponStateBase CC11;
	ref WeaponStateBase CJF1;
	ref WeaponStateBase OD01;
	
	override bool CanChamberBullet (int muzzleIndex, Magazine mag)
	{
		return super.CanChamberBullet(muzzleIndex,mag) && IsChamberEmpty(muzzleIndex);
	}
	
	void Pistol_Base ()
	{
	}
	
	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_BULLET_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_BULLET_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH));

		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_OPENED));

		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED_UNCOCKED));

		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));

		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));

		// setup state machine
		// basic weapon states
		// open-closed | discharged-charged | nobullet-bullet | nomag-mag
		// regexp: [OC][CDJ][01][01]
		CD00 = new Pistol_CLO_DIS_BU0_MA0(this, NULL, PistolAnimState.DEFAULT);
		CC00 = new Pistol_CLO_CHG_BU0_MA0(this, NULL, PistolAnimState.CLOSED_CHARGED);
		CC10 = new Pistol_CLO_CHG_BU1_MA0(this, NULL, PistolAnimState.CLOSED_CHARGED);
		CJF0 = new Pistol_CLO_JAM_BU1_MA0(this, NULL, PistolAnimState.JAMMED);
		OD00 = new Pistol_OPE_DIS_BU0_MA0(this, NULL, PistolAnimState.OPENED_DISCHARGED);
		CD01 = new Pistol_CLO_DIS_BU0_MA1(this, NULL, PistolAnimState.DEFAULT);
		CC01 = new Pistol_CLO_CHG_BU0_MA1(this, NULL, PistolAnimState.CLOSED_CHARGED);
		CC11 = new Pistol_CLO_CHG_BU1_MA1(this, NULL, PistolAnimState.CLOSED_CHARGED);
		CJF1 = new Pistol_CLO_JAM_BU1_MA1(this, NULL, PistolAnimState.JAMMED);
		OD01 = new Pistol_OPE_DIS_BU0_MA1(this, NULL, PistolAnimState.OPENED_DISCHARGED);

		// unstable (intermediate) states
		WeaponCharging Mech_CD00 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED_UNCOCKED);
		WeaponCharging Mech_CC00 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponCharging Mech_CD01 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED_UNCOCKED);
		WeaponCharging Mech_CC01 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponCharging Mech_OD00 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED);
		WeaponCharging Mech_OD01 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED);
		
		WeaponCharging Mech_CC10 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED); // eject with no mag
		WeaponCharging Mech_CC11 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED); // eject with mag
		
		
		WeaponDryFire Trigger_CC00 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED); // cock without clip
		WeaponDryFire Trigger_CD00 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED);
		WeaponDryFire Trigger_OD00 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED);
		WeaponDryFire Trigger_CD01 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED);
		WeaponDryFire Trigger_OD01 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED);
		WeaponDryFire Trigger_CC01 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED); // cock with clip
		WeaponDryFire Trigger_CJF0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED);
		WeaponDryFire Trigger_CJF1 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED);
		
		WeaponFireAndChamberNext Trigger_CC11 = new WeaponFireAndChamberNext(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase Trigger_CC10 = new WeaponFireLast(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL); // fire last no mag
		WeaponStateBase Trigger_CC11L = new WeaponFireLast(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_LAST); // fire last with mag
		
		WeaponStateBase Trigger_CC10J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM); 
		WeaponStateBase Trigger_CC11J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM);

		WeaponStateBase Unjam_CJF0 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);
		WeaponStateBase Unjam_CJF1 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);

		WeaponChambering Chamber_CD00 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED_UNCOCKED);
		WeaponChambering Chamber_CC00 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED);
		WeaponChambering Chamber_OD00 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_OPENED);
		WeaponChambering Chamber_CD01 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED_UNCOCKED);
		WeaponChambering Chamber_CC01 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED);
		WeaponChambering Chamber_OD01 = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_OPENED);

		WeaponAttachMagazine Attach_CC10 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_BULLET_CLOSED);
		WeaponAttachMagazine Attach_CJF0 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_BULLET_CLOSED);
		WeaponAttachMagazine Attach_CD00 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_UNCOCKED);
		WeaponAttachMagazine Attach_CC00 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_COCKED);
		WeaponAttachMagazine Attach_OD00 = new WeaponAttachMagazine(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_NOMAGAZINE_NOBULLET_OPENED);

		WeaponReplacingMagAndChamberNext Reload_CD01 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_UNCOCKED);
		WeaponReplacingMagAndChamberNext Reload_CC01 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_COCKED);
		WeaponReplacingMagAndChamberNext Reload_CC11 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_BULLET_CLOSED);
		WeaponReplacingMagAndChamberNext Reload_CJF1 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_BULLET_CLOSED);
		WeaponReplacingMagAndChamberNext Reload_OD01 = new WeaponReplacingMagAndChamberNext(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADPISTOL_MAGAZINE_NOBULLET_OPENED);

		WeaponDetachingMag Detach_CC11 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH);
		WeaponDetachingMag Detach_CC01 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH);
		WeaponDetachingMag Detach_CD01 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH);
		WeaponDetachingMag Detach_OD01 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH);
		WeaponDetachingMag Detach_CJF1 = new WeaponDetachingMag(this, NULL, WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH);

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
		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
	
		m_fsm = new WeaponFSM();

		// charging
		m_fsm.AddTransition(new WeaponTransition( CD00,				__M__,	Mech_CD00)); // charge from closed
		m_fsm.AddTransition(new WeaponTransition(  Mech_CD00,		_fin_,	CC00));
			Mech_CD00.AddTransition(new WeaponTransition( Mech_CD00.m_start,			_abt_,	CD00));
			Mech_CD00.AddTransition(new WeaponTransition( Mech_CD00.m_eject,			_abt_,	CC00));
			Mech_CD00.AddTransition(new WeaponTransition( Mech_CD00.m_onCK,				_abt_,	CC00));
			Mech_CD00.AddTransition(new WeaponTransition( Mech_CD00.m_chamber,			_abt_,	CC00));
		
		m_fsm.AddTransition(new WeaponTransition( CC00,				__M__,	Mech_CC00)); // charge from charged
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC00,		_fin_,	CC00));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC00,		_abt_,	CC00));

		m_fsm.AddTransition(new WeaponTransition( OD00,				__M__,	Mech_OD00)); // charge from opened
		m_fsm.AddTransition(new WeaponTransition(  Mech_OD00,		_fin_,	CC00));
			Mech_OD00.AddTransition(new WeaponTransition( Mech_OD00.m_start,			_abt_,	OD00));
			Mech_OD00.AddTransition(new WeaponTransition( Mech_OD00.m_eject,			_abt_,	CC00));
			Mech_OD00.AddTransition(new WeaponTransition( Mech_OD00.m_onCK,				_abt_,	CC00));
			Mech_OD00.AddTransition(new WeaponTransition( Mech_OD00.m_chamber,			_abt_,	CC00));
		
		m_fsm.AddTransition(new WeaponTransition( OD01,				__M__,	Mech_OD01)); // charge from opened with mag
		m_fsm.AddTransition(new WeaponTransition(  Mech_OD01,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_OD01,		_fin_,	CC11));
			Mech_OD01.AddTransition(new WeaponTransition( Mech_OD01.m_start,			_abt_,	OD01));
			Mech_OD01.AddTransition(new WeaponTransition( Mech_OD01.m_eject,			_abt_,	CC01));
			Mech_OD01.AddTransition(new WeaponTransition( Mech_OD01.m_onCK,				_abt_,	CC01));
			Mech_OD01.AddTransition(new WeaponTransition( Mech_OD01.m_chamber,			_abt_,	CC11));
		
		m_fsm.AddTransition(new WeaponTransition( CD01,				__M__,	Mech_CD01)); // charge from closed=1 with mag
		m_fsm.AddTransition(new WeaponTransition(  Mech_CD01,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CD01,		_fin_,	CC11));
			Mech_CD01.AddTransition(new WeaponTransition( Mech_CD01.m_start,			_abt_,	CD01));
			Mech_CD01.AddTransition(new WeaponTransition( Mech_CD01.m_eject,			_abt_,	CC01));
			Mech_CD01.AddTransition(new WeaponTransition( Mech_CD01.m_onCK,				_abt_,	CC01));
			Mech_CD01.AddTransition(new WeaponTransition( Mech_CD01.m_chamber,			_abt_,	CC11));
		
		m_fsm.AddTransition(new WeaponTransition( CC01, 			__M__,	Mech_CC01)); // charge from closed/charged with mag
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC01,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC01,		_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC01,		_abt_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC01,		_abt_,	CC11));
		
		// eject good cartridge
		m_fsm.AddTransition(new WeaponTransition( CC10,				__M__,	Mech_CC10)); // eject chamber nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC10,		_fin_,	CC00));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC10,		_abt_,	CC00, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC10,		_abt_,	CC10));	
		
		m_fsm.AddTransition(new WeaponTransition( CC11,				__M__,	Mech_CC11 )); // eject with mag with ammo
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC11,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC11,		_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC11,		_abt_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_CC11,		_abt_,	CC11));

		// Trigger_CC11e
		m_fsm.AddTransition(new WeaponTransition( CC00,				__T__,	Trigger_CC00)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC00,	_fin_,	CD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC00, 	_dto_,	CD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC00, 	_abt_,	CD00));
		
		m_fsm.AddTransition(new WeaponTransition( CD00,				__T__,	Trigger_CD00)); // fire.uncocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD00,	_fin_,  CD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD00,	_dto_,  CD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD00,	_abt_,  CD00));
		
		m_fsm.AddTransition(new WeaponTransition( OD00,				__T__,	Trigger_OD00)); // opened fire.uncocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD00,	_fin_,  OD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD00,	_dto_,  OD00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD00,	_abt_,  OD00));
		
		m_fsm.AddTransition(new WeaponTransition( CD01,				__T__,	Trigger_CD01)); // fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD01,	_fin_,	CD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD01,	_dto_,  CD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CD01,	_abt_,  CD01));
		
		m_fsm.AddTransition(new WeaponTransition( OD01,				__T__,	Trigger_OD01)); // opened fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD01,	_fin_,	OD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD01,	_dto_,	OD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_OD01,	_abt_,	OD01));
		
		m_fsm.AddTransition(new WeaponTransition( CJF0,				__T__,	Trigger_CJF0)); // opened fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF0,	_fin_,	CJF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF0,	_dto_,	CJF0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF0,	_abt_,	CJF0));
		
		m_fsm.AddTransition(new WeaponTransition( CJF1,				__T__,	Trigger_CJF1)); // opened fire.uncocked w mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF1,	_fin_,	CJF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF1,	_dto_,	CJF1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CJF1,	_abt_,	CJF1));
		
		
		m_fsm.AddTransition(new WeaponTransition( CC10,				__T__,	Trigger_CC10)); // or fire.last (if not jammed)
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10,	_fin_,	CC00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10,	_rto_,	CC00));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10,	_abt_,	CC00));
		
		m_fsm.AddTransition(new WeaponTransition( CC10,				__TJ_,	Trigger_CC10J)); // or fire.last (if not jammed)
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10J,	_fin_,	CJF0 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10J,	_rto_,	CJF0 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC10J,	_abt_,	CJF0 ));
		
		
		m_fsm.AddTransition(new WeaponTransition( CC11, 			__T__,	Trigger_CC11, NULL, new WeaponGuardHasAmmo(this))); // fire.normal + chamber next
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11,	_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11,	_rto_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11,	_abt_,	CC11));

		m_fsm.AddTransition(new WeaponTransition( CC11,				__T__,	Trigger_CC11L, NULL, new GuardNot(new WeaponGuardHasAmmo(this)))); // fire.last with mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11L,	_fin_,	OD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11L,	_rto_,	OD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11L,	_abt_,	OD01));
		
		m_fsm.AddTransition(new WeaponTransition( CC11,				__TJ_,	Trigger_CC11J )); // fire.last with mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11J,	_fin_,	CJF1 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11J,	_rto_,	CJF1 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC11J,	_abt_,	CJF1 ));
		
		m_fsm.AddTransition(new WeaponTransition( CC01,				__T__,	Trigger_CC01)); // fire.cocked with mag
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC01,	_fin_,	CD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC01,	_dto_,	CD01));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_CC01,	_abt_,	CD01));

		// load cartridge
		m_fsm.AddTransition(new WeaponTransition( CD00,				__L__,	Chamber_CD00)); // chamber from closed=1
		m_fsm.AddTransition(new WeaponTransition(  Chamber_CD00,	_fin_,	CC10));
			Chamber_CD00.AddTransition(new WeaponTransition( Chamber_CD00.m_start,		_abt_,	CD00));
			Chamber_CD00.AddTransition(new WeaponTransition( Chamber_CD00.m_eject,		_abt_,	OD00));
			Chamber_CD00.AddTransition(new WeaponTransition( Chamber_CD00.m_chamber,	_abt_,	OD00));
			Chamber_CD00.AddTransition(new WeaponTransition( Chamber_CD00.m_w4t,		_abt_,	CC10));

		m_fsm.AddTransition(new WeaponTransition( OD00,				__L__,	Chamber_OD00)); // chamber from opened
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD00,	_fin_,	CC10));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD00,	_abt_,	OD00, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD00,	_abt_,	CC10));

		m_fsm.AddTransition(new WeaponTransition( CC00,				__L__,	Chamber_CC00)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_CC00,	_fin_,	CC10));
			Chamber_CC00.AddTransition(new WeaponTransition( Chamber_CC00.m_start,		_abt_,	CC00));
			Chamber_CC00.AddTransition(new WeaponTransition( Chamber_CC00.m_eject,		_abt_,	OD00));
			Chamber_CC00.AddTransition(new WeaponTransition( Chamber_CC00.m_chamber,	_abt_,	OD00));
			Chamber_CC00.AddTransition(new WeaponTransition( Chamber_CC00.m_w4t,		_abt_,	CC10));
		
		// load cartridge with mag
		m_fsm.AddTransition(new WeaponTransition( CD01,				__L__,	Chamber_CD01, NULL, new WeaponGuardHasMag(this))); // chamber from closed=1 with mag
		m_fsm.AddTransition(new WeaponTransition(  Chamber_CD01,	_fin_,	CC11));
			Chamber_CD01.AddTransition(new WeaponTransition( Chamber_CD01.m_start,		_abt_,	CD01));
			Chamber_CD01.AddTransition(new WeaponTransition( Chamber_CD01.m_eject,		_abt_,	OD01));
			Chamber_CD01.AddTransition(new WeaponTransition( Chamber_CD01.m_chamber,	_abt_,	OD01));
			Chamber_CD01.AddTransition(new WeaponTransition( Chamber_CD01.m_w4t,		_abt_,	CC11));
		
		m_fsm.AddTransition(new WeaponTransition( CC01,				__L__,	Chamber_CC01, NULL, new WeaponGuardHasMag(this))); // chamber from closed=1 with mag
		m_fsm.AddTransition(new WeaponTransition(  Chamber_CC01, 	_fin_,	CC11));
			Chamber_CC01.AddTransition(new WeaponTransition( Chamber_CC01.m_start,		_abt_,	CC01));
			Chamber_CC01.AddTransition(new WeaponTransition( Chamber_CC01.m_eject,		_abt_,	OD01));
			Chamber_CC01.AddTransition(new WeaponTransition( Chamber_CC01.m_chamber,	_abt_,	OD01));
			Chamber_CC01.AddTransition(new WeaponTransition( Chamber_CC01.m_w4t,		_abt_,	CC11));
		
		m_fsm.AddTransition(new WeaponTransition( OD01,				__L__,	Chamber_OD01, NULL, new WeaponGuardHasMag(this))); // chamber from opened with mag
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD01,	_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD01,	_abt_,	OD01, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_OD01,	_abt_,	CC11));;

		m_fsm.AddTransition(new WeaponTransition( CJF0,				__A__,	Attach_CJF0, NULL, new WeaponGuardCanAttachMag(this))); // attach from JAM/b1/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_CJF0,	_fin_,	CJF1));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_CJF0,	_abt_,  CJF1, NULL, new WeaponGuardHasMag(this)));
		//m_fsm.AddTransition(new WeaponTransition(  Attach_CJF0,	_abt_,  CJF0));
			Attach_CJF0.AddTransition(new WeaponTransition( Attach_CJF0.m_start,		_abt_,	CJF0));
			Attach_CJF0.AddTransition(new WeaponTransition( Attach_CJF0.m_attach,		_abt_,	CJF0));

// zdokumentovano az sem
		// attach mag with no ammo
		m_fsm.AddTransition(new WeaponTransition( CD00,				__A__,	Attach_CD00, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b0/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_CD00,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_CD00,		_fin_,	CC11));
			Attach_CD00.AddTransition(new WeaponTransition( Attach_CD00.m_start,		_abt_,	CD00));
			Attach_CD00.AddTransition(new WeaponTransition( Attach_CD00.m_attach,		_abt_,	CD00));
			Attach_CD00.AddTransition(new WeaponTransition( Attach_CD00.m_onCK,			_abt_,	CC01));
			Attach_CD00.AddTransition(new WeaponTransition( Attach_CD00.m_chamber,		_abt_,	CC11));
		
		m_fsm.AddTransition(new WeaponTransition( CC10,				__A__,	Attach_CC10, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b1/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_CC10,		_fin_,	CC11));
			Attach_CC10.AddTransition(new WeaponTransition( Attach_CC10.m_start,		_abt_,	CC10));
			Attach_CC10.AddTransition(new WeaponTransition( Attach_CC10.m_attach,		_abt_,	CC10));
		
		m_fsm.AddTransition(new WeaponTransition( OD00,				__A__,	Attach_OD00, NULL, new WeaponGuardCanAttachMag(this))); // attach from OPE/b0/m0
		m_fsm.AddTransition(new WeaponTransition(  Attach_OD00,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_OD00,		_fin_,	CC11));
			Attach_OD00.AddTransition(new WeaponTransition( Attach_OD00.m_start,		_abt_,	OD00));
			Attach_OD00.AddTransition(new WeaponTransition( Attach_OD00.m_attach,		_abt_,	OD00));
			Attach_OD00.AddTransition(new WeaponTransition( Attach_OD00.m_onCK,			_abt_,	CC01));
			Attach_OD00.AddTransition(new WeaponTransition( Attach_OD00.m_chamber,		_abt_,	CC11));

		m_fsm.AddTransition(new WeaponTransition( CC00, 			__A__,	Attach_CC00, NULL, new WeaponGuardCanAttachMag(this))); // attach from CLO/b0/m0 charged
		m_fsm.AddTransition(new WeaponTransition(  Attach_CC00,	_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_CC00,	_fin_,	CC11));
			Attach_CC00.AddTransition(new WeaponTransition( Attach_CC00.m_start,		_abt_,	CC00));
			Attach_CC00.AddTransition(new WeaponTransition( Attach_CC00.m_attach,		_abt_,	CC00));
			Attach_CC00.AddTransition(new WeaponTransition( Attach_CC00.m_onCK,			_abt_,	CC01));
			Attach_CC00.AddTransition(new WeaponTransition( Attach_CC00.m_chamber,		_abt_,	CC11));

		// replace magazine
		m_fsm.AddTransition(new WeaponTransition( CD01,				__S__,	Reload_CD01, NULL, new WeaponGuardCanSwapMag(this))); // swap in CLO/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_CD01,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CD01,		_fin_,	CC11));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_start,		_abt_,	CD01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_eject,		_abt_,	CD01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_detach,		_abt_,	CD01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_hideOld,		_abt_,	CD00));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_swapMags,		_abt_,	CD01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_attach,		_abt_,	CD01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_onCK,			_abt_,	CC01));
			Reload_CD01.AddTransition(new WeaponTransition( Reload_CD01.m_chamber,		_abt_,	CC11));


		m_fsm.AddTransition(new WeaponTransition( CC11, 			__S__, Reload_CC11, NULL, new WeaponGuardCanSwapMag(this))); // swap in CLO/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC11,	_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC11,	_abt_,	CC11, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC11,	_abt_,	CC10));

		m_fsm.AddTransition(new WeaponTransition( CC01, 			__S__,	Reload_CC01, NULL, new WeaponGuardCanSwapMag(this))); // CLO/b0/m1 swap with empty mag (no chamber)
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC01,	_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC01,	_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC01,	_abt_,	CC11, NULL, new WeaponGuardCurrentChamberFull(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC01,	_abt_,	CC01, NULL, new WeaponGuardHasMag(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_CC01,	_abt_,	CC00));

		m_fsm.AddTransition(new WeaponTransition( OD01, 			__S__,	Reload_OD01, NULL, new WeaponGuardCanSwapMag(this))); // OPE/b0/m1 swap with empty mag (no chamber)
		m_fsm.AddTransition(new WeaponTransition(  Reload_OD01,	_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_OD01,	_fin_,	CC11));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_start,			_abt_,	OD01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_eject,			_abt_,	OD01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_detach,		_abt_,	OD01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_hideOld,		_abt_,	OD00));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_swapMags,		_abt_,	OD01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_attach,		_abt_,	OD01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_onCK,			_abt_,	CC01));
			Reload_OD01.AddTransition(new WeaponTransition(Reload_OD01.m_chamber,		_abt_,	CC11));

		m_fsm.AddTransition(new WeaponTransition( CJF1,			__S__,	Reload_CJF1, NULL, new WeaponGuardCanSwapMag(this))); // swap in JAM/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Reload_CJF1,	_fin_,	CJF1));
		m_fsm.AddTransition(new WeaponTransition(  Reload_CJF1,	_abt_,	CC11, NULL, new WeaponGuardCurrentChamberFull(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_CJF1,	_abt_,	CC01, NULL, new WeaponGuardHasMag(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_CJF1,	_abt_,	CC00));


		// detach magazine
		m_fsm.AddTransition(new WeaponTransition( OD01,			__D__,	Detach_OD01, NULL, new WeaponGuardCanDetachMag(this))); // detach from OPE/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_OD01,	_fin_,	OD00));
		m_fsm.AddTransition(new WeaponTransition(  Detach_OD01,	_abt_,	OD01, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_OD01,	_abt_,	OD00));
		
		m_fsm.AddTransition(new WeaponTransition( CC11,			__D__,	Detach_CC11, NULL, new WeaponGuardCanDetachMag(this))); // detach from CLO/b1/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC11,	_fin_,	CC10));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC11,	_abt_,	CC11, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC11,	_abt_,	CC10));
		
		m_fsm.AddTransition(new WeaponTransition( CC01,			__D__,	Detach_CC01, NULL, new WeaponGuardCanDetachMag(this))); // detach from CLO/b0/m1
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC01,	_fin_,	CC00));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC01,	_abt_,	CC01, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CC01,	_abt_,	CC00));
		
		m_fsm.AddTransition(new WeaponTransition( CD01,			__D__,	Detach_CD01, NULL, new WeaponGuardCanDetachMag(this))); // detach from CLO/b0/m1 dischg
		m_fsm.AddTransition(new WeaponTransition(  Detach_CD01,	_fin_,	CD00));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CD01,	_abt_,	CD01, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CD01,	_abt_,	CD00));
		
		m_fsm.AddTransition(new WeaponTransition( CJF1,			__D__,	Detach_CJF1, NULL, new WeaponGuardCanDetachMag(this))); // detach from CLO/b1/m1 jammed
		m_fsm.AddTransition(new WeaponTransition(  Detach_CJF1,	_fin_,	CJF0));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CJF1,	_abt_,	CJF1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_CJF1,	_abt_,	CJF0));


		// unjam
		m_fsm.AddTransition(new WeaponTransition( CJF0,			__U__,	Unjam_CJF0)); // unjam nomag
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF0,		_fin_,	CJF0, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF0,		_fin_,	CC00, NULL, new GuardNot(new WeaponGuardJammed(this))));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF0,		_abt_,	CJF0));
		
		m_fsm.AddTransition(new WeaponTransition( CJF1,			__U__,	Unjam_CJF1)); // unjam with mag with ammo
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_fin_,	CJF1, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_fin_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_fin_,	CC11));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_abt_,	CJF1, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_abt_,	CC01, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_CJF1,		_abt_,	CC11));
		

		m_fsm.SetInitialState(CD00);
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
	
	/*override bool GetInventoryHandAnimation(notnull InventoryLocation loc, out int value)
	{
		int id;
		string name = "";
		if ( loc.GetParent() && loc.GetParent().GetInventory().GetCurrentAttachmentSlotInfo(id,name) && name == "Hips" )
		{
			//value = WeaponHideShowTypes.HIDESHOW_SLOT_2HDRIGHTBACK;
			value = WeaponHideShowTypes.HIDESHOW_SLOT_PISTOLBELT;
			return true;
		}
		
		return false;
	}*/
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionDetachMagazine);
		//AddAction(ActionAdvancedDetachMagazine);
	}
};

