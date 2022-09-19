enum PumpShotgunAnimState
{
	DEFAULT 			= 0, 	///< default weapon state, closed and discharged
};

enum PumpShotgunStableStateID
{
	UNKNOWN		=  0,
	Empty		=  1,
	Fireout		=  2,
	Loaded		=  3,
	Jammed		=  4,
}

class PumpShotgunEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Empty E"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Empty E"); } }
	override int GetCurrentStateID () { return PumpShotgunStableStateID.Empty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class PumpShotgunFireout extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Fireout F"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Fireout F"); } }
	override int GetCurrentStateID () { return PumpShotgunStableStateID.Fireout; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};
class PumpShotgunLoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Loaded C"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Loaded C"); } }
	override int GetCurrentStateID () { return PumpShotgunStableStateID.Loaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class PumpShotgunJammed extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Jammed L_J"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Jammed L_J"); } }
	override int GetCurrentStateID () { return PumpShotgunStableStateID.Jammed; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsBoltOpen () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};

/**@class	Mp133Shotgun_Base
 * @brief	basic mp5 submachine gun
 **/
class Mp133Shotgun_Base : Rifle_Base
{
	ref WeaponStateBase E;
	ref WeaponStateBase F;
	ref WeaponStateBase L;
	ref WeaponStateBase J;
	
	void Mp133Shotgun ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Mp133Recoil(this);
	}
	
	override void InitStateMachine ()
	{
		// setup abilities
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));
	
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE));
	
		// basic weapon states
		E = new PumpShotgunEmpty(this, NULL, PumpShotgunAnimState.DEFAULT);
		F = new PumpShotgunFireout(this, NULL, PumpShotgunAnimState.DEFAULT);
		L = new PumpShotgunLoaded(this, NULL, PumpShotgunAnimState.DEFAULT);
		J = new PumpShotgunJammed(this, NULL, PumpShotgunAnimState.DEFAULT);
//--------------------------------------------------------------------------------------
		WeaponStateBase		Mech_E = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponStateBase		Mech_F = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponStateBase		Mech_L = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		LoopedChamberingEjectLast 	Chamber_E = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast 	Chamber_F = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast 	Chamber_L = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_COCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		
		WeaponStateBase		Unjam_J = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);
		
		WeaponStateBase		Trigger_E = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase		Trigger_F = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase		Trigger_L = new WeaponFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase		Trigger_J = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		
		WeaponStateBase		Trigger_LJ = new WeaponFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM);
		
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
		m_fsm.AddTransition(new WeaponTransition( E,			__M__,	Mech_L));
//----------------------------------------	
		
		m_fsm.AddTransition(new WeaponTransition( L,			__M__,	Mech_L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	L));
		
		m_fsm.AddTransition(new WeaponTransition( F,			__M__,	Mech_F));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_fin_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_fin_,	F, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_abt_,	F, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_F,		_abt_,	L));
		
//----------------------------------------		
		
		m_fsm.AddTransition(new WeaponTransition( E,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	E, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	L));
		
		m_fsm.AddTransition(new WeaponTransition( L,			__L__,	Chamber_L, NULL, new GuardNot(new WeaponGuardInnerMagazineFull(this))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L,	_abt_,	L));
		
		m_fsm.AddTransition(new WeaponTransition( F,			__L__,	Chamber_F, NULL, new GuardNot(new WeaponGuardInnerMagazineFull(this))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F,	_fin_,	L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F,	_abt_,	F, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F,	_abt_,	L));

		
//-------------------------------------------
		
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
		
		m_fsm.AddTransition(new WeaponTransition( L,			__T__,	Trigger_L)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_fin_,	F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_rto_,	F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_abt_,	F));
		
		m_fsm.AddTransition(new WeaponTransition( L,			__TJ_,	Trigger_LJ)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_fin_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_rto_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_LJ,	_abt_,	J));
		
		m_fsm.AddTransition(new WeaponTransition( F,			__T__,	Trigger_F)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F,	_fin_,	F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F,	_dto_,	F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F,	_abt_,	F));
		
		m_fsm.AddTransition(new WeaponTransition( J,			__T__,	Trigger_J)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_fin_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_dto_,	J));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_J,	_abt_,	J));
		
//-----------------------------------------	
		
		m_fsm.SetInitialState(E);

		SelectionBulletHide();
		HideMagazine();

		m_fsm.Start();
	}
	
	override bool CanChamberBullet (int muzzleIndex, Magazine mag)
	{
		return CanChamberFromMag(muzzleIndex, mag) && !IsInternalMagazineFull(muzzleIndex);
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionLoadMultiBullet);

		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
		AddAction(FirearmActionLoadMultiBulletQuick); // Easy reload
	}
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "FNP45_MRDSOptic" );
		inventory.CreateInInventory( "Battery9V" );
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_12gaPellets", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_12gaSlug", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_12gaRubberSlug", entity.GetPosition());
		}
	}
};

