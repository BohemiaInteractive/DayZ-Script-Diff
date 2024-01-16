enum XBAnimState
{
	uncocked 			= 0, 	///< default weapon state, closed and discharged
	cocked		 		= 1,
};

enum XBStableStateID
{
	UNKNOWN				=  0,
	UncockedEmpty		=  1,
	CockedEmpty			=  2,
	Loaded				=  3,
}

class XBUncockedEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Uncocked Empty UE"); } m_weapon.SetCharged(false); super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Uncocked Empty UE"); } }
	override int GetCurrentStateID () { return XBStableStateID.UncockedEmpty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class XBCockedEmpty extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Cocked Empty CE"); } m_weapon.SetCharged(true); super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Cocked Empty CE"); } }
	override int GetCurrentStateID () { return XBStableStateID.CockedEmpty; }
	override bool HasBullet () { return false; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.E}; }
};
class XBLoaded extends WeaponStableState
{
	override void OnEntry (WeaponEventBase e) { if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " { Loaded L"); } m_weapon.SetCharged(true); super.OnEntry(e); }
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " } Loaded L"); } }
	override int GetCurrentStateID () { return XBStableStateID.Loaded; }
	override bool HasBullet () { return true; }
	override bool HasMagazine () { return false; }
	override bool IsJammed () { return false; }
	override bool IsRepairEnabled () { return true; }
	override void InitMuzzleArray () { m_muzzleHasBullet = {MuzzleState.L}; }
};

/**@class		Crossbow_Base
 * @brief		base for Crossbow
 * @NOTE		name copies config base class
 **/
class Crossbow_Base : Archery_Base
{
	void Crossbow_Base();
	
	override RecoilBase SpawnRecoilObject()
	{
		return new CrossbowRecoil(this);
	}
	
	override void InitStateMachine()
	{
		// setup abilities
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_SPECIAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_CROSSBOW_OPENED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_CROSSBOW_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));

		// setup state machine
		// basic weapon states
		WeaponStableState U = new XBUncockedEmpty(this, NULL, XBAnimState.uncocked);
		WeaponStableState C = new XBCockedEmpty(this, NULL, XBAnimState.cocked);
		WeaponStableState L = new XBLoaded(this, NULL, XBAnimState.cocked);
		// unstable (intermediate) states
		WeaponChargingStretch Mech_U = new WeaponChargingStretch(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_OPENED);
		WeaponEjectBullet Mech_L = new WeaponEjectBullet(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_SPECIAL);
		
		WeaponChambering Chamber_U = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_CROSSBOW_OPENED);
		WeaponChambering Chamber_C = new WeaponChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_CROSSBOW_CLOSED);
		//CrossbowReChambering Chamber_L = new CrossbowReChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_CLOSED);
		
		WeaponStateBase Trigger_U = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED);
		WeaponStateBase Trigger_C = new WeaponDryFire(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED);
		WeaponStateBase Trigger_L = new WeaponFireLast(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		
		WeaponStateBase Unjam_J = new WeaponUnjamming(this, NULL, WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_START);

		// events
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		WeaponEventBase __TJ_ = new WeaponEventTriggerToJam;
		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase __U__ = new WeaponEventUnjam;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

		m_fsm = new WeaponFSM();
		
		//Charging
		m_fsm.AddTransition(new WeaponTransition( C,			__M__,	C));
		
		m_fsm.AddTransition(new WeaponTransition( U,			__M__,	Mech_U));
		m_fsm.AddTransition(new WeaponTransition(  Mech_U,		_fin_,	U, null, new WeaponGuardWeaponDischarged(this)));		
		m_fsm.AddTransition(new WeaponTransition(  Mech_U,		_fin_,	C ));
		m_fsm.AddTransition(new WeaponTransition(  Mech_U,		_abt_,	U, null, new WeaponGuardWeaponDischarged(this)));		
		m_fsm.AddTransition(new WeaponTransition(  Mech_U,		_abt_,	C ));
		
		m_fsm.AddTransition(new WeaponTransition( L,			__M__,	Mech_L));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	C, null, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_fin_,	L));	
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	C, null, new WeaponGuardChamberEmpty(this)));
		m_fsm.AddTransition(new WeaponTransition(  Mech_L,		_abt_,	L));
			
		
		//Chamber bolt
		m_fsm.AddTransition(new WeaponTransition( U,			__L__,	Chamber_U));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_fin_,	L, null, new WeaponGuardChamberFull(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_fin_,	C, null, new WeaponGuardWeaponCharged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_fin_,	U));	
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_abt_,	L, null, new WeaponGuardChamberFull(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_abt_,	C, null, new WeaponGuardWeaponCharged(this)));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_U,		_abt_,	U));
		
		
		m_fsm.AddTransition(new WeaponTransition( C,			__L__,	Chamber_C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C,		_fin_,	L, null, new WeaponGuardChamberFull(this)));		
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C,		_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C,		_abt_,	L, null, new WeaponGuardChamberFull(this)));		
		m_fsm.AddTransition(new WeaponTransition(  Chamber_C,		_abt_,	C));
		
		m_fsm.AddTransition(new WeaponTransition( C,			__T__,	Trigger_C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C,	_fin_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C,	_abt_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C,	_dto_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_C,	_abt_,	U));
		
		m_fsm.AddTransition(new WeaponTransition( U,			__T__,	Trigger_U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_U,	_fin_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_U,	_abt_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_U,	_dto_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_U,	_abt_,	U));
		
		m_fsm.AddTransition(new WeaponTransition( L,			__T__,	Trigger_L));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_fin_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_abt_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_dto_,	U));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_L,	_abt_,	U));

		SelectionBulletHide();
		EffectBulletHide(0);
		
		SetInitialState(U);
		m_fsm.Start();
	}
	
	override float GetChanceToJam()
	{
		return 0.0;
	}
	
	override void OnDebugSpawnEx(DebugSpawnParams params)
	{
		//super.OnDebugSpawn();

		

		GetInventory().CreateInInventory( "ACOGOptic_6x" );
		
		EntityAI entity;

		GameInventory inv = null;
		if (params.m_Player)
		{
			inv = params.m_Player.GetInventory();
		}

		SpawnInInventoryOrGroundPos("Ammo_HuntingBolt", inv, GetPosition());
		SpawnInInventoryOrGroundPos("Ammo_ImprovisedBolt_1", inv, GetPosition());
		SpawnInInventoryOrGroundPos("Ammo_ImprovisedBolt_2", inv, GetPosition());
			
		SpawnInInventoryOrGroundPos("ACOGOptic", inv, GetPosition());
		entity = SpawnInInventoryOrGroundPos("M68Optic", inv, GetPosition());
		entity.GetInventory().CreateInInventory( "Battery9V" );
		entity = SpawnInInventoryOrGroundPos("M4_T3NRDSOptic", inv, GetPosition());
		entity.GetInventory().CreateInInventory( "Battery9V" );
		entity = SpawnInInventoryOrGroundPos("ReflexOptic", inv, GetPosition());
		entity.GetInventory().CreateInInventory( "Battery9V" );
		entity = SpawnInInventoryOrGroundPos("StarlightOptic", inv, GetPosition());
		entity.GetInventory().CreateInInventory( "Battery9V" );
	}
	
	override bool CanBeUsedForSuicide()
	{
		float ammoDamage;
		string ammoTypeName;
		GetCartridgeInfo(GetCurrentMuzzle(), ammoDamage, ammoTypeName);
		if (ammoTypeName == "Bullet_CupidsBolt")
			return false;
		
		return super.CanBeUsedForSuicide();
	}
}

class Crossbow : Crossbow_Base {}
class Crossbow_Autumn : Crossbow_Base {}
class Crossbow_Summer : Crossbow_Base {}
class Crossbow_Black : Crossbow_Base {}
class Crossbow_Wood : Crossbow_Base {}
