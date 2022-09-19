enum DoubleBarrelAnimState
{
	DEFAULT 			= 0,	///< default weapon state, closed and discharged
};

enum DoubleBarrelStableStateID
{
	UNKNOWN				=	0,
	LoadedLoaded		=	1,
	LoadedEmpty			=	2,
	FireoutLoaded		=	3,
	FireoutFireout		=	4,
	FireoutEmpty		=	5,
	EmptyEmpty			=	6,
	EmptyLoaded			=	7,
	EmptyFireout		=	8,
}

enum MuzzleIndex
{
	First				=	0,
	Second				=	1,
}

class DoubleBarrelLoadedLoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { LoadedLoaded L_L"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } LoadedLoaded L_L"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.LoadedLoaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L, MuzzleState.L}; }
};
class DoubleBarrelLoadedEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { LoadedEmpty L_E"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } LoadedEmpty L_E"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.LoadedEmpty; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L, MuzzleState.E}; }
};
class DoubleBarrelEmptyLoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { EmptyLoaded E_L"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.Second);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } EmptyLoaded E_L"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.EmptyLoaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E, MuzzleState.L}; }
};
class DoubleBarrelFireoutLoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { FireoutLoaded F_L"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.Second);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } FireoutLoaded F_L"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.FireoutLoaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F, MuzzleState.L}; }
};
class DoubleBarrelFireoutFireout extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { FireoutFireout F_F"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } FireoutFireout F_F"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.FireoutFireout; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F, MuzzleState.F}; }
};

class DoubleBarrelFireoutEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { FireoutEmpty F_E"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } FireoutEmpty F_E"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.FireoutEmpty; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.F, MuzzleState.E}; }
};

class DoubleBarrelEmptyFireout extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { EmptyFireout E_F"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } EmptyFireout E_F"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.EmptyFireout; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E, MuzzleState.F}; }
};

class DoubleBarrelEmptyEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { EmptyEmpty E_E"); } super.OnEntry(e); m_weapon.SetCurrentMuzzle(MuzzleIndex.First);}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } EmptyEmpty E_E"); } }
	override int GetCurrentStateID () { return DoubleBarrelStableStateID.EmptyEmpty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E, MuzzleState.E}; }
};



class DoubleBarrel_Base : Rifle_Base
{
	ref WeaponStateBase L_L;
	ref WeaponStateBase L_E;
	ref WeaponStateBase E_L;
	ref WeaponStateBase F_L;
	ref WeaponStateBase F_F;
	ref WeaponStateBase F_E;
	ref WeaponStateBase E_F;
	ref WeaponStateBase E_E;
	
	override void InitStateMachine()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_END));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_1));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_2));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_3));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_4));
		// setup abilities
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END));
	
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY));
		

	
		// basic weapon states
		L_L = new DoubleBarrelLoadedLoaded(this, NULL, DoubleBarrelAnimState.DEFAULT);
		L_E = new DoubleBarrelLoadedEmpty(this, NULL, DoubleBarrelAnimState.DEFAULT);
		E_L = new DoubleBarrelEmptyLoaded(this, NULL, DoubleBarrelAnimState.DEFAULT);
		F_L = new DoubleBarrelFireoutLoaded(this, NULL, DoubleBarrelAnimState.DEFAULT);
		F_F = new DoubleBarrelFireoutFireout(this, NULL, DoubleBarrelAnimState.DEFAULT);
		F_E = new DoubleBarrelFireoutEmpty(this, NULL, DoubleBarrelAnimState.DEFAULT);
		E_F = new DoubleBarrelEmptyFireout(this, NULL, DoubleBarrelAnimState.DEFAULT);
		E_E = new DoubleBarrelEmptyEmpty(this, NULL, DoubleBarrelAnimState.DEFAULT);
	
		
		WeaponStateBase		Mech = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);

		ChamberMultiBullet	Chamber_E = new ChamberMultiBullet(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_2,WeaponActionChamberingTypes.CHAMBERING_END);
		ChamberMultiBullet	Chamber_L_E = new ChamberMultiBullet(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_3,WeaponActionChamberingTypes.CHAMBERING_END);
		ChamberMultiBullet	Chamber_F_L = new ChamberMultiBullet(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_4,WeaponActionChamberingTypes.CHAMBERING_END);
		ChamberMultiBullet	Chamber_E_L = new ChamberMultiBullet(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_DOUBLE_1,WeaponActionChamberingTypes.CHAMBERING_END);
		
		WeaponStateBase		Unjam_J = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);
		
		WeaponStateBase		Trigger_L_E = new WeaponFireMultiMuzzle(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase		Trigger_E_L = new WeaponFireMultiMuzzle(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase		Trigger_F_L = new WeaponFireMultiMuzzle(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase		Trigger_L_L = new WeaponFireMultiMuzzle(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		
		WeaponStateBase		Trigger_F_F = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase		Trigger_F_E = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase		Trigger_E_F = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		WeaponStateBase		Trigger_E_E = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_DRY);
		
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		//WeaponEventBase __U__ = new WeaponEventUnjam;
		WeaponEventBase	__M__ = new WeaponEventMechanism;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

		m_fsm = new WeaponFSM();
		
		m_fsm.AddTransition(new WeaponTransition( E_E,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( L_L,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( L_E,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( E_L,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( F_L,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( F_F,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( F_E,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition( E_F,			__M__,	Mech));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_fin_,	E_E));
		
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	F_F, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	F_E, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberEmpty(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	E_F, NULL, new GuardAnd( new WeaponGuardChamberEmpty(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	F_L, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	L_L, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	L_E, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberEmpty(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	E_L, NULL, new GuardAnd( new WeaponGuardChamberEmpty(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	E_E));
		
		m_fsm.AddTransition(new WeaponTransition( F_F,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition( F_E,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition( E_F,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition( E_E,			__L__,	Chamber_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	L_E, NULL, new WeaponGuardChamberEmpty(this, MuzzleIndex.Second)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	L_L));
		
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	F_F, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	F_E, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberEmpty(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	E_F, NULL, new GuardAnd( new WeaponGuardChamberEmpty(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	L_L, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	L_E, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberEmpty(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	E_E));
		
		
		m_fsm.AddTransition(new WeaponTransition( L_E,			__L__,	Chamber_L_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L_E,	_abt_,	L_L, NULL, new WeaponGuardChamberFull(this, MuzzleIndex.Second)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L_E,	_abt_,	L_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L_E,	_fin_,	L_L, NULL, new WeaponGuardChamberFull(this, MuzzleIndex.Second)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_L_E,	_fin_,	L_E));
		
		m_fsm.AddTransition(new WeaponTransition( E_L,			__L__,	Chamber_E_L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E_L,	_abt_,	L_L, NULL, new WeaponGuardChamberFull(this, MuzzleIndex.First)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E_L,	_abt_,	E_L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E_L,	_fin_,	L_L, NULL, new WeaponGuardChamberFull(this, MuzzleIndex.First)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E_L,	_fin_,	E_L));
		
		m_fsm.AddTransition(new WeaponTransition( F_L,			__L__,	Chamber_F_L));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_abt_,	F_L, NULL, new WeaponGuardChamberFiredOut(this, MuzzleIndex.First)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_abt_,	L_L, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_abt_,	E_L, NULL, new GuardAnd( new WeaponGuardChamberEmpty(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_abt_,	L_E));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_fin_,	F_L, NULL, new WeaponGuardChamberFiredOut(this, MuzzleIndex.First)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_fin_,	L_L, NULL, new GuardAnd( new WeaponGuardChamberFull(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_fin_,	E_L, NULL, new GuardAnd( new WeaponGuardChamberEmpty(this, MuzzleIndex.First), new WeaponGuardChamberFull(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_F_L,	_fin_,	L_E));
		
		m_fsm.AddTransition(new WeaponTransition( L_E,			__T__,	Trigger_L_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_E,	_fin_,	F_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_E,	_rto_,	F_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_E,	_abt_,	F_E));
		
		m_fsm.AddTransition(new WeaponTransition( E_L,			__T__,	Trigger_E_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_L,	_fin_,	E_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_L,	_rto_,	E_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_L,	_abt_,	E_F));
		
		m_fsm.AddTransition(new WeaponTransition( F_L,			__T__,	Trigger_F_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_L,	_fin_,	F_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_L,	_rto_,	F_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_L,	_abt_,	F_F));
		
		m_fsm.AddTransition(new WeaponTransition( L_L,			__T__,	Trigger_L_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_fin_,	F_F, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_rto_,	F_F, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_abt_,	F_F, NULL, new GuardAnd( new WeaponGuardChamberFiredOut(this, MuzzleIndex.First), new WeaponGuardChamberFiredOut(this, MuzzleIndex.Second))));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_fin_,	F_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_rto_,	F_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L_L,	_abt_,	F_L));
		
		m_fsm.AddTransition(new WeaponTransition( F_F,			__T__,	Trigger_F_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_F,	_fin_,	F_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_F,	_dto_,	F_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_F,	_abt_,	F_F));
		
		m_fsm.AddTransition(new WeaponTransition( F_E,			__T__,	Trigger_F_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_E,	_fin_,	F_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_E,	_dto_,	F_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_F_E,	_abt_,	F_E));
		
		m_fsm.AddTransition(new WeaponTransition( E_F,			__T__,	Trigger_E_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_F,	_fin_,	E_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_F,	_dto_,	E_F));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_F,	_abt_,	E_F));
		
		m_fsm.AddTransition(new WeaponTransition( E_E,			__T__,	Trigger_E_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_E,	_fin_,	E_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_E,	_dto_,	E_E));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_E_E,	_abt_,	E_E));
		
		
		
		m_fsm.SetInitialState(E_E);
		
		for(int i = 0; i < GetMuzzleCount(); i++)
		{
			HideBullet(i);
		}
		m_fsm.Start();
	
	}

	override bool CanChamberBullet (int muzzleIndex, Magazine mag)
	{
		for(int i = 0; i < GetMuzzleCount(); i++)
		{
			if( CanChamberFromMag(i, mag) )
			{
				if(IsChamberEmpty(i))
					return true;
				
				if(IsChamberFiredOut(i))
					return true;
			} 
		}
		return false;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionLoadMultiBullet);

		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
		AddAction(FirearmActionLoadMultiBulletQuick); // Easy reload
	}
};