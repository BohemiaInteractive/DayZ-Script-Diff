enum OBAnimState
{
	CLOSED 			= 0, 	///< default weapon state, closed and discharged
	OPENED 			= 1,
	JAMMED 			= 2,
};

enum OBStableStateID
{
	UNKNOWN				=  0,
	OB_CLO_MA0			=  1,
	OB_OPN_MA0			=  2,
	OB_CLO_MA1			=  3,
	OB_OPN_MA1			=  4,
	OB_OPN_MA1_EMPTY	=  5,
	OB_JAM_MA0			=  6,
	OB_JAM_MA1			=  7
}

class OB_CLO_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close nomag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_CLO_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return false; }
	override bool IsRepairEnabled () { return true; }
	override bool IsDischarged () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};

class OB_OPN_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { open nomag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } open nomag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_OPN_MA0; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override bool IsDischarged () { return false; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};

class OB_CLO_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { close mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } close mag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_CLO_MA1; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return false; }
	override bool IsDischarged () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};

class OB_OPN_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { open mag"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } open mag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_OPN_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsDischarged () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};

class OB_OPN_MA1_EMPTY extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { open mag empty"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } open mag empty"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_OPN_MA1_EMPTY; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return false; }
	override bool IsBoltOpen () { return true; }
	override bool IsDischarged () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};

class OB_JAM_MA0 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { jammed nomag"); } m_weapon.SetWeaponOpen(true); super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } jammed nomag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_JAM_MA0; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsDischarged () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};

class OB_JAM_MA1 extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { jammed mag"); } m_weapon.SetWeaponOpen(true); super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } jammed mag"); } }
	override int GetCurrentStateID () { return OBStableStateID.OB_JAM_MA1; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return true; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsDischarged () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};

/**@class		OpenBolt_Base
 * @brief		base for automatic weapons fireing from open bolt position
 * @NOTE		name copies config base class
 **/
class OpenBolt_Base extends Rifle_Base
{
	ref WeaponStableState C0;
	ref	WeaponStableState O0;
	ref	WeaponStableState C1;
	ref	WeaponStableState O1;
	ref	WeaponStableState O1E;
	ref	WeaponStableState J1;
	ref	WeaponStableState J0;

	void OpenBolt_Base ()
	{
	}
	
	override void InitStateMachine()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET_OPEN));
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET_OPEN));
		
		m_abilities.Insert(new AbilityRecord(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOAD_MAGAZINE_DETACH));

		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));

		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));

		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));
		
		//m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));

		// setup state machine
		// basic weapon states
		// close-open-jammed | nomag-mag | - no bullet in chamber
		// regexp: [COJ][01][-E]
		C0 = new OB_CLO_MA0(this, NULL, OBAnimState.CLOSED);
		O0 = new OB_OPN_MA0(this, NULL, OBAnimState.OPENED);
		C1 = new OB_CLO_MA1(this, NULL, OBAnimState.CLOSED);
		O1 = new OB_OPN_MA1(this, NULL, OBAnimState.OPENED);
		O1E = new OB_OPN_MA1_EMPTY(this, NULL, OBAnimState.OPENED);
		J0 = new OB_JAM_MA0(this, NULL, OBAnimState.JAMMED);
		J1 = new OB_JAM_MA1(this, NULL, OBAnimState.JAMMED);	
	
		WeaponCharging Mech_C1 = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, GetWeaponSpecificCommand(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
		
		//-----------------------------------------------
		
		WeaponStateBase Trigger_C0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_C1 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_O1E = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		WeaponStateBase Trigger_O0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		
		WeaponFireAndChamberNext Trigger_O1 = new WeaponFireAndChamberNext(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		WeaponStateBase Trigger_O1L = new WeaponFireLast(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		WeaponStateBase Trigger_O1J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM));//Fire and jam
		
		WeaponStateBase Trigger_J0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		WeaponStateBase Trigger_J1 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, GetWeaponSpecificCommand(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));

		//-----------------------------------------------
		
		WeaponStateBase Unjam_J0 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		WeaponStateBase Unjam_J1 = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, GetWeaponSpecificCommand(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));

		//-----------------------------------------------
		
		WeaponAttachMagazineOpenBoltCharged Attach_C0 = new WeaponAttachMagazineOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET_OPEN));
		WeaponAttachMagazineOpenBoltCharged Attach_O0 = new WeaponAttachMagazineOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET));//TODO Need add open animation
		WeaponAttachMagazineOpenBoltCharged Attach_J0 = new WeaponAttachMagazineOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET));
		
		WeaponReplacingMagAndChamberNextOpenBoltCharged Reload_C1 = new WeaponReplacingMagAndChamberNextOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET_OPEN));
		WeaponReplacingMagAndChamberNextOpenBoltCharged Reload_O1 = new WeaponReplacingMagAndChamberNextOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET));//TODO Need add open animation
		WeaponReplacingMagAndChamberNextOpenBoltCharged Reload_J1 = new WeaponReplacingMagAndChamberNextOpenBoltCharged(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET));
		
		WeaponDetachingMagOpenBolt Detach_C1 = new WeaponDetachingMagOpenBolt(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		WeaponDetachingMagOpenBolt Detach_O1 = new WeaponDetachingMagOpenBolt(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		WeaponDetachingMagOpenBolt Detach_J1 = new WeaponDetachingMagOpenBolt(this, NULL, WeaponActions.RELOAD, GetWeaponSpecificCommand(WeaponActions.RELOAD, WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_DETACH));
		
		//WeaponStateBase chmMag = new WeaponChamberFromAttMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		// events
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		WeaponEventBase __TJ_ = new WeaponEventTriggerToJam;
		WeaponEventBase __U__ = new WeaponEventUnjam;
		WeaponEventBase __A__ = new WeaponEventAttachMagazine;
		WeaponEventBase __D__ = new WeaponEventDetachMagazine;
		WeaponEventBase __S__ = new WeaponEventSwapMagazine;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

		m_fsm = new WeaponFSM();

		m_fsm.AddTransition(new WeaponTransition( C1,				__M__,	Mech_C1)); // charge from bullet nomag
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_fin_,	C1, NULL, new WeaponGuardWeaponDischarged(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_fin_,	O1E, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1, 	_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_abt_,	C1, NULL, new WeaponGuardWeaponDischarged(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_abt_,	O1E, NULL, new WeaponGuardCurrentChamberEmpty(this) ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1, 	_abt_,	O1));
		
		m_fsm.AddTransition(new WeaponTransition( C0,				__M__,	C0)); 
		m_fsm.AddTransition(new WeaponTransition( O1E,				__M__,	O1E)); 
		m_fsm.AddTransition(new WeaponTransition( O0,				__M__,	O0)); 
		m_fsm.AddTransition(new WeaponTransition( O1,				__M__,	O1)); 
		m_fsm.AddTransition(new WeaponTransition( J0,				__M__,	J0)); 
		m_fsm.AddTransition(new WeaponTransition( J1,				__M__,	J1)); 

		// fire
		m_fsm.AddTransition(new WeaponTransition( C0,				__T__,	Trigger_C0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,		_fin_,	C0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,		_dto_,	C0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,		_abt_,	C0));
		
		m_fsm.AddTransition(new WeaponTransition( C1,				__T__,	Trigger_C1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,		_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,		_dto_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,		_abt_,	C1));
		
		m_fsm.AddTransition(new WeaponTransition( O1, 			__T__,	Trigger_O1, NULL, new WeaponGuardHasAmmo(this)));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1,	_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1,	_rto_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1,	_abt_,	O1));

		m_fsm.AddTransition(new WeaponTransition( O1,				__T__,	Trigger_O1L, NULL, new GuardNot(new WeaponGuardHasAmmo(this))));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1L,	_fin_,	O1E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1L,	_rto_,	O1E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1L,	_abt_,	O1E));
		
		m_fsm.AddTransition(new WeaponTransition( O1,				__TJ_,	Trigger_O1J ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1J,	_fin_,	J1 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1J,	_rto_,	J1 ));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1J,	_abt_,	J1 ));
		
		m_fsm.AddTransition(new WeaponTransition( O1E,				__T__,	Trigger_O1E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1E,		_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1E,		_rto_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O1E,		_abt_,	C1));
		
		m_fsm.AddTransition(new WeaponTransition( O0,				__T__,	Trigger_O0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O0,		_fin_,	C0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O0,		_rto_,	C0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_O0,		_abt_,	C0));
		
		m_fsm.AddTransition(new WeaponTransition( J0,				__T__,	Trigger_J0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J0,		_fin_,	J0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J0,		_dto_,	J0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J0,		_abt_,	J0));
		
		m_fsm.AddTransition(new WeaponTransition( J1,				__T__,	Trigger_J1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J1,		_fin_,	J1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J1,		_dto_,	J1));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J1,		_abt_,	J1));

		// attach magazine (no mag in weapon)
		m_fsm.AddTransition(new WeaponTransition( C0,				__A__,	Attach_C0, NULL, new WeaponGuardCanAttachMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_fin_,	C0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_fin_,	C1, NULL, new WeaponGuardWeaponDischarged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_fin_,	O1E, NULL, new WeaponGuardChamberEmpty(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_abt_,	C0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_abt_,	C1, NULL, new WeaponGuardWeaponDischarged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_abt_,	O1E, NULL, new WeaponGuardChamberEmpty(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Attach_C0,		_abt_,	O1));		
	
		m_fsm.AddTransition(new WeaponTransition( O0,				__A__,	Attach_O0, NULL, new WeaponGuardCanAttachMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_fin_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_fin_,	O1E, NULL, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_abt_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_abt_,	O1E, NULL, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_O0,		_abt_,	O1));
		
		m_fsm.AddTransition(new WeaponTransition( J0,				__A__,	Attach_J0, NULL, new WeaponGuardCanAttachMag(this))); 
		m_fsm.AddTransition(new WeaponTransition(  Attach_J0,		_fin_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_J0,		_fin_,	J0));
		m_fsm.AddTransition(new WeaponTransition(  Attach_J0,		_abt_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Attach_J0,		_abt_,	J0));


		// replace magazine
		m_fsm.AddTransition(new WeaponTransition( C1,				__S__,	Reload_C1, NULL, new WeaponGuardCanSwapMag(this))); 
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_fin_,	C0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_fin_,	C1, NULL, new WeaponGuardWeaponDischarged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_fin_,	O1E, NULL, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_abt_,	C0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_abt_,	C1, NULL, new WeaponGuardWeaponDischarged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_abt_,	O1E, NULL, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_C1,		_abt_,	O1));

		m_fsm.AddTransition(new WeaponTransition( O1,				__S__,	Reload_O1, NULL, new WeaponGuardCanSwapMag(this)));
		m_fsm.AddTransition(new WeaponTransition( O1E,				__S__,	Reload_O1, NULL, new WeaponGuardCanSwapMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_fin_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_fin_,	O1E, NULL, new WeaponGuardChamberEmpty(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_abt_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_abt_,	O1E, NULL, new WeaponGuardChamberEmpty(this)) );
		m_fsm.AddTransition(new WeaponTransition(  Reload_O1,		_abt_,	O1));

		m_fsm.AddTransition(new WeaponTransition( J1,				__S__,	Reload_J1, NULL, new WeaponGuardCanSwapMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_J1,		_fin_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_J1,		_fin_,	J0));
		m_fsm.AddTransition(new WeaponTransition(  Reload_J1,		_abt_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Reload_J1,		_abt_,	J0));
			
      // @TODO: abort in jam

		// detach magazine
		m_fsm.AddTransition(new WeaponTransition( C1,				__D__,	Detach_C1, NULL, new WeaponGuardCanDetachMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C1,		_fin_,	C1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C1,		_fin_,	C0));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C1,		_abt_,	C1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_C1,		_abt_,	C0));
		
		m_fsm.AddTransition(new WeaponTransition( O1E,				__D__,	Detach_O1, NULL, new WeaponGuardCanDetachMag(this)));
		m_fsm.AddTransition(new WeaponTransition( O1,				__D__,	Detach_O1, NULL, new WeaponGuardCanDetachMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_fin_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_fin_,	O1E, NULL,  new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_abt_,	O0, NULL, new GuardNot(new WeaponGuardHasMag(this))));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_abt_,	O1E, NULL,  new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_O1,		_abt_,	O1));
		
		m_fsm.AddTransition(new WeaponTransition( J1,				__D__,	Detach_J1, NULL, new WeaponGuardCanDetachMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_J1,		_fin_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_J1,		_fin_,	J0));
		m_fsm.AddTransition(new WeaponTransition(  Detach_J1,		_abt_,	J1, NULL, new WeaponGuardHasMag(this)));
		m_fsm.AddTransition(new WeaponTransition(  Detach_J1,		_abt_,	J0));


		// unjam
		m_fsm.AddTransition(new WeaponTransition( J0,				__U__,	Unjam_J0)); // unjam nomag
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J0,		_fin_,	J0, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J0,		_fin_,	O0));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J0,		_abt_,	J0, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J0,		_abt_,	O0));
		
		m_fsm.AddTransition(new WeaponTransition( J1,				__U__,	Unjam_J1)); // unjam with mag with ammo
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_fin_,	J1, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_fin_,	O1E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_fin_,	O1));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_abt_,	J1, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_abt_,	O1E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_J1,		_abt_,	O1));

		SetInitialState(C0);

		SelectionBulletHide();
		HideMagazine();

		m_fsm.Start();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(FirearmActionLoadBullet);
		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
	}

}