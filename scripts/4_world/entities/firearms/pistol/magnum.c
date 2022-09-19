const float MAGNUM_ROTATION_POSITION_M1 = -0.167;
const float MAGNUM_ROTATION_POSITION_0 = 0.0;
const float MAGNUM_ROTATION_POSITION_1 = 0.167;
const float MAGNUM_ROTATION_POSITION_2 = 0.334;
const float MAGNUM_ROTATION_POSITION_3 = 0.500;
const float MAGNUM_ROTATION_POSITION_4 = 0.668;
const float MAGNUM_ROTATION_POSITION_5 = 0.835;
const float MAGNUM_ROTATION_POSITION_6 = 1.0;


enum MagnumAnimState
{
	DEFAULT 			= 0, 	///< default weapon state
};

enum MagnumStableStateID
{
	UNKNOWN				=  0,
	DEFAULT				=  1,
}

class Magnum_Static_State extends WeaponStableState
{
	bool init = false;
	override void OnEntry(WeaponEventBase e) 
	{		
		if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] { Magnum stable state"); } 
		super.OnEntry(e);
		if (init)
		{
			Magnum_Base magnum;
			if (CastTo(magnum, m_weapon))
			{
				magnum.SyncCylinderRotation();
				//magnum.SyncSelectionStateFromState();
			}
		}
		init = true;
	}
	override void OnExit (WeaponEventBase e) { super.OnExit(e); if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnstate] }  Magnum stable state"); } }
	override int GetCurrentStateID () { return MagnumStableStateID.DEFAULT; }
	override bool HasBullet() { return m_weapon.IsChamberFull(m_weapon.GetCurrentMuzzle()); }
	override bool HasMagazine() { return false; }
	override bool IsJammed() { return m_weapon.IsJammed(); }
	override bool IsSingleState() { return true; }
};



class Magnum_Base extends Weapon_Base
{
	ref WeaponStateBase C;
	int m_LastMuzzleloaded;
	int m_ActiveMuzzle;
	const string ATT_SLOT_CYLINDER = "RevolverCylinder";
	const string ATT_SLOT_EJECTOR = "RevolverEjector";
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(FirearmActionLoadMultiBullet);

		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
		AddAction(FirearmActionLoadMultiBulletQuick); // Easy reload

	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new MagnumRecoil(this);
	}
	
	void Magnum_Base ()
	{
		m_LastMuzzleloaded = 0;
		m_ActiveMuzzle = 0;
		
		/*m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionTypes.CHAMBERING_STARTLOOPABLE_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionTypes.MECHANISM_SPECIAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionTypes.UNJAMMING_START));
		m_abilities.Insert(new AbilityRecord(WeaponActions.UNJAMMING, WeaponActionTypes.UNJAMMING_END));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionTypes.FIRE_COCKED));*/
	}
	
	override void InitStateMachine ()
	{
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_KEEP));
		m_abilities.Insert(new AbilityRecord(WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_SPECIAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED));
		m_abilities.Insert(new AbilityRecord(WeaponActions.FIRE, WeaponActionFireTypes.FIRE_UNCOCKED));


		// setup state machine
		// basic weapon state
		C = new Magnum_Static_State(this, NULL, MagnumAnimState.DEFAULT);


		WeaponMagnumChambering Chamber = new WeaponMagnumChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED_KEEP,WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		WeaponMagnumChambering Chamber_E = new WeaponMagnumChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_STARTLOOPABLE_CLOSED,WeaponActionChamberingTypes.CHAMBERING_ENDLOOPABLE);
		//WeaponMagnumChambering Chamber_CC00 = new WeaponMagnumChambering(this, NULL, WeaponActions.CHAMBERING, WeaponActionChamberingTypes.CHAMBERING_END);

		WeaponCharging Mech = new WeaponCharging(this, NULL, WeaponActions.MECHANISM, WeaponActionMechanismTypes.MECHANISM_CLOSED);
		
		WeaponStateBase	Trigger_normal = new WeaponFireMagnum(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_NORMAL);
		WeaponStateBase	Trigger_dry = new WeaponFireMagnum(this, NULL, WeaponActions.FIRE, WeaponActionFireTypes.FIRE_COCKED);
		
		// events

		WeaponEventBase __L__ = new WeaponEventLoad1Bullet;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase _abt_ = new WeaponEventHumanCommandActionAborted;
		WeaponEventBase __M__ = new WeaponEventMechanism;
		WeaponEventBase __T__ = new WeaponEventTrigger;
		

		WeaponEventBase _rto_ = new WeaponEventReloadTimeout;
		WeaponEventBase _dto_ = new WeaponEventDryFireTimeout;

	
		m_fsm = new WeaponFSM();



		m_fsm.AddTransition(new WeaponTransition( C,				__L__,	Chamber, null, new WeaponGuardAnyChamberFiredOut(this))); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber,	_abt_,	C));

		
		m_fsm.AddTransition(new WeaponTransition( C,				__L__,	Chamber_E)); // chamber from closed charged
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Chamber_E,	_abt_,	C));

		
		m_fsm.AddTransition(new WeaponTransition( C,				__M__,	Mech)); // charge from closed
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Mech,		_abt_,	C));

		
		
		m_fsm.AddTransition(new WeaponTransition( C,				__T__,	Trigger_normal, null, new GuardAnd (new WeaponGuardCurrentChamberFull(this), new GuardNot(new WeaponGuardCurrentChamberFiredOut(this))) )); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal, 	_dto_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_normal, 	_abt_,	C));
		
		m_fsm.AddTransition(new WeaponTransition( C,				__T__,	Trigger_dry)); // fire.cocked
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry,	_fin_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry, 	_dto_,	C));
		m_fsm.AddTransition(new WeaponTransition(  Trigger_dry, 	_abt_,	C));


		m_fsm.SetInitialState(C);
		SelectionBulletHide();
		SetCurrentMuzzle(0);
		//HideMagazine();
		m_fsm.Start();
	}
	
	override bool CanChamberBullet(int muzzleIndex, Magazine mag)
	{
		for (int i = 0; i < GetMuzzleCount(); i++)
		{
			if ( CanChamberFromMag(i, mag) )
			{
				if (IsChamberEmpty(i))
					return true;
				
				if (IsChamberFiredOut(i))
					return true;
			} 
		}
		return false;
	}
	
	override void AssembleGun()
	{
		super.AssembleGun();
		
		if ( !FindAttachmentBySlotName(ATT_SLOT_EJECTOR) )
		{
			GetInventory().CreateAttachment("Magnum_Ejector");
		}
		
		if ( !FindAttachmentBySlotName(ATT_SLOT_CYLINDER) )
		{
			GetInventory().CreateAttachment("Magnum_Cylinder");
		}

		ForceSyncSelectionState();
		SyncCylinderRotation();
	}
	
	static float GetCylinderRotation(int muzzleIndex)
	{
		switch (muzzleIndex)
		{
			case 0:
				return MAGNUM_ROTATION_POSITION_0;
			case 1:
				return MAGNUM_ROTATION_POSITION_5;
			case 2:
				return MAGNUM_ROTATION_POSITION_4;
			case 3:
				return MAGNUM_ROTATION_POSITION_3;
			case 4:
				return MAGNUM_ROTATION_POSITION_2;
			case 5:
				return MAGNUM_ROTATION_POSITION_1;
		}
		
		ErrorEx(string.Format("Invalid muzzle index: %1", muzzleIndex));
		
		return MAGNUM_ROTATION_POSITION_0;
	}
	
	void SetCylinderRotationAnimationPhase(float rot, bool reset = false)
	{
		Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(GetAttachmentByType(Magnum_Cylinder));
		Magnum_Ejector ejector = Magnum_Ejector.Cast(GetAttachmentByType(Magnum_Ejector));
		if (cylinder && ejector)
		{
			float anim_phase = cylinder.GetAnimationPhase("Rotate_Cylinder");
			if ( Math.AbsFloat(anim_phase - rot) > 0.1 )
			{
				if (reset)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", rot );
					ejector.ResetAnimationPhase("Rotate_Ejector", rot );				
				}				
				else if (rot == MAGNUM_ROTATION_POSITION_0)
				{
					cylinder.ResetAnimationPhase("Rotate_Cylinder", MAGNUM_ROTATION_POSITION_M1 );
					ejector.ResetAnimationPhase("Rotate_Ejector", MAGNUM_ROTATION_POSITION_M1 );
				}
				
				cylinder.SetAnimationPhase("Rotate_Cylinder", rot );
				ejector.SetAnimationPhase("Rotate_Ejector", rot );
			}
		}
	}
	
	void SyncCylinderRotation(bool reset = true)
	{
		SetCylinderRotationAnimationPhase(GetCylinderRotation(GetCurrentMuzzle()), reset);
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (GetGame().IsClient())
			return;
		
		SetAttachmentsHealth();
	}
	
	//sets health of cylinder and ejector objects
	void SetAttachmentsHealth()
	{
		EntityAI entity;
		ItemBase attachment;
		for (int i = 0; i < GetInventory().AttachmentCount(); i++)
		{
			entity = GetInventory().GetAttachmentFromIndex(i);
			if ( Class.CastTo(attachment,entity) )
			{
				attachment.SetHealth01("","Health",GetHealth01());
			}
		}
	}
	
	override void OnFire(int muzzle_index)
	{
		super.OnFire(muzzle_index);
		Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(GetAttachmentByType(Magnum_Cylinder));
		if (cylinder)
		{
			string bullet_nose = "bullet_nose";
			if (muzzle_index > 0)
				bullet_nose = string.Format("bullet_nose_" + (muzzle_index + 1));
			cylinder.HideSelection(bullet_nose);
		}
	}
	
	override bool IsShowingChamberedBullet()
	{
		return false;
	}
	
	override bool CanEjectBullet() 
	{
		for (int i = 0; i < GetMuzzleCount(); i++)
		{
			if (IsChamberFull(i))
				return true;
		}
		return false;
	}
	
	override void ShowBullet(int muzzleIndex)
	{
		super.ShowBullet(muzzleIndex);
		
		Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(GetAttachmentByType(Magnum_Cylinder));
		if (cylinder)
		{		
			string bullet = "bullet";
			if (muzzleIndex > 0)
				bullet = string.Format("bullet_" + (muzzleIndex + 1));
			
			cylinder.ShowSelection(bullet);
			
			if (!IsChamberFiredOut(muzzleIndex))
			{
				string bullet_nose = "bullet_nose";
				if (muzzleIndex > 0)
					bullet_nose = string.Format("bullet_nose_" + (muzzleIndex + 1));
				cylinder.ShowSelection(bullet_nose);
			}
		}
	}
	
	override void HideBullet(int muzzleIndex)
	{
		super.HideBullet(muzzleIndex);
		
		Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(GetAttachmentByType(Magnum_Cylinder));
		if (cylinder)
		{		
			string bullet = "bullet";
			if (muzzleIndex > 0)
				bullet = string.Format("bullet_" + (muzzleIndex + 1));
			
			cylinder.HideSelection(bullet);
			
			string bullet_nose = "bullet_nose";
			if (muzzleIndex > 0)
				bullet_nose = string.Format("bullet_nose_" + (muzzleIndex + 1));
			cylinder.HideSelection(bullet_nose);
		}
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

class Magnum extends Magnum_Base {};
class SawedoffMagnum extends Magnum_Base {};
class Magnum_Cylinder extends DummyItem {};
class Magnum_Ejector extends DummyItem {};