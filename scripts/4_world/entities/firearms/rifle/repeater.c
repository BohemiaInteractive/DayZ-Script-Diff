enum RPTAnimState
{
	DEFAULT 			= 0, 	///< default weapon state, closed and discharged
	CHARGED 			= 1,
	JAMMED 				= 2,
};

enum RPTStableStateID
{
	UNKNOWN				=  0,
	EmptyDischarged		=  1,
	LoadedCharged		=  2,
	LoadedDischarged	=  3,
	LoadedJammed		=  4,
}


class RPTEmptyDischarged extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { EmptyDischarged C0"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } EmptyDischarged C0"); } }
	override int GetCurrentStateID () { return RPTStableStateID.EmptyDischarged; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class RPTLoadedCharged extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { LoadedCharged C1"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } LoadedCharged C1"); } }
	override int GetCurrentStateID () { return RPTStableStateID.LoadedCharged; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};
class RPTLoadedDischarged extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { LoadedDischarged D1"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } LoadedDischarged D1"); } }
	override int GetCurrentStateID () { return RPTStableStateID.LoadedDischarged; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};
class RPTLoadedJammed extends WeaponStateJammed
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { LoadedJammed JF"); } super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } LoadedJammed JF"); } }
	override int GetCurrentStateID () { return RPTStableStateID.LoadedJammed; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return true; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F}; }
};

/**@class	Mp133Shotgun_Base
 * @brief	basic mp5 submachine gun
 **/
class Repeater_Base extends Rifle_Base
{
	ref WeaponStateBase D0;
	ref WeaponStateBase C0;
	ref	WeaponStateBase C1;
	ref	WeaponStateBase DF;
	ref	WeaponStateBase JF;
	
	
	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		
		D0 = new RPTEmptyDischarged(this, NULL, RPTAnimState.DEFAULT);
		C0 = new RPTEmptyDischarged(this, NULL, RPTAnimState.CHARGED);
		DF = new RPTLoadedDischarged(this, NULL, RPTAnimState.DEFAULT);
		C1 = new RPTLoadedCharged(this, NULL, RPTAnimState.CHARGED);
		JF = new RPTLoadedJammed(this, NULL, RPTAnimState.JAMMED);
		
		//WeaponStateBase	Mech_D0 = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		WeaponStateBase	Mech_C1 = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED);
		WeaponStateBase	Mech_DF = new WeaponChargingInnerMag(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		LoopedChamberingEjectLast Chamber_D0 = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast Chamber_C0 = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast Chamber_C1 = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_COCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		LoopedChamberingEjectLast Chamber_DF = new LoopedChamberingEjectLast(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		
		WeaponStateBase	Unjam_JF = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);
		
		WeaponStateBase	Trigger_D0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase	Trigger_C0 = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase	Trigger_C1 = new WeaponFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase	Trigger_DF = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase	Trigger_JF = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		
		WeaponStateBase	Trigger_C1J = new WeaponFireToJam(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_JAM);

		
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
		m_fsm.AddTransition(new WeaponTransition( D0,			__M__,	Mech_DF));
		m_fsm.AddTransition(new WeaponTransition( C0,			__M__,	Mech_C1));
				
		m_fsm.AddTransition(new WeaponTransition( C1,			__M__,	Mech_C1));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_fin_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_abt_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_C1,		_abt_,	C1));
		
		m_fsm.AddTransition(new WeaponTransition( DF,			__M__,	Mech_DF));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_fin_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_fin_,	DF, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_abt_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_abt_,	DF, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_DF,		_abt_,	C1));
		
		
//----------------------------------------
		m_fsm.AddTransition(new WeaponTransition( C0,			__L__,	Chamber_D0));
		
		m_fsm.AddTransition(new WeaponTransition( D0,			__L__,	Chamber_D0));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_D0,	_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_D0,	_abt_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_D0,	_abt_,	C1));
		
		m_fsm.AddTransition(new WeaponTransition( C1,			__L__,	Chamber_C1, NULL, new GuardNot(new WeaponGuardInnerMagazineFull(this))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C1,	_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C1,	_abt_,	C1));
		
		m_fsm.AddTransition(new WeaponTransition( DF,			__L__,	Chamber_DF, NULL, new GuardNot(new WeaponGuardInnerMagazineFull(this))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_DF,	_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_DF,	_abt_,	DF, NULL, new WeaponGuardCurrentChamberFiredOut(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_DF,	_abt_,	C1));
		
//------------------------------------------
		
		m_fsm.AddTransition(new WeaponTransition( JF, 			__U__,	Unjam_JF));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_fin_,	JF, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_fin_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_fin_,	C1));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_abt_,	JF, NULL, new WeaponGuardJammed(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_abt_,	C0, NULL, new WeaponGuardCurrentChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Unjam_JF,	_abt_,	C1));
		
		
//-----------------------------------------
		
		// fire
		m_fsm.AddTransition(new WeaponTransition( D0,			__T__,	Trigger_D0)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_D0,	_fin_,	D0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_D0,	_dto_,	D0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_D0,	_abt_,	D0));
		
		m_fsm.AddTransition(new WeaponTransition( C0,			__T__,	Trigger_C0)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,	_fin_,	D0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,	_dto_,	D0));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C0,	_abt_,	D0));
		
		m_fsm.AddTransition(new WeaponTransition( C1,			__T__,	Trigger_C1)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,	_fin_,	DF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,	_rto_,	DF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1,	_abt_,	DF));
		
		m_fsm.AddTransition(new WeaponTransition( DF,			__T__,	Trigger_DF)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_DF,	_fin_,	DF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_DF,	_dto_,	DF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_DF,	_abt_,	DF));
		
		m_fsm.AddTransition(new WeaponTransition( JF,			__T__,	Trigger_JF)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF,	_fin_,	JF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF,	_dto_,	JF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_JF,	_abt_,	JF));
		
		
		m_fsm.AddTransition(new WeaponTransition( JF,			__TJ_,	Trigger_C1J)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1J,	_fin_,	JF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1J,	_rto_,	JF));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C1J,	_abt_,	JF));
		
//---------------------------------------------	
		
		m_fsm.SetInitialState(D0);

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
};

class Repeater : Repeater_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new RepeaterRecoil(this);
	}
	
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_357", entity.GetPosition());
		}
	}
};