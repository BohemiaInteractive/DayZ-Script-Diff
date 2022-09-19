/**@class 	WeaponCharging
 * @brief	charging of weapon without ammo to be chambered
 */
class WeaponCharging extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;

	ref WeaponCharging_Start m_start;
	ref WeaponCharging_CK m_onCK;
	ref WeaponChamberFromAttMag_W4T m_chamber;
	ref WeaponEjectAllMuzzles m_eject;

	void WeaponCharging (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponCharging_Start(m_weapon, this, m_action, m_actionType);
		m_onCK = new WeaponCharging_CK(m_weapon, this);
		m_chamber = new WeaponChamberFromAttMag_W4T(m_weapon, this);
		m_eject = new WeaponEjectAllMuzzles(m_weapon, this);

		// events
		WeaponEventBase __be_ = new WeaponEventAnimBulletEject;
		WeaponEventBase __bh_ = new WeaponEventAnimBulletHide;
		WeaponEventBase __ck_ = new WeaponEventAnimCocked;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		// transitions
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_eject ));
		m_fsm.AddTransition(new WeaponTransition(  m_start, _fin_, NULL ));
		
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_eject, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon))); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onCK)); // some anims do not send BE event
		
		m_fsm.AddTransition(new WeaponTransition(   m_eject, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_eject, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_eject, _fin_, NULL));
		
		m_fsm.AddTransition(new WeaponTransition(   m_onCK, _fin_, NULL));
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, NULL));

		m_fsm.SetInitialState(m_start);
	}
};

class WeaponCharging_Start extends WeaponStartAction
{
	override bool IsWaitingForActionFinish () 
	{
		return true; 
	}
};

class WeaponCharging_BE extends WeaponStateBase
{
};

class WeaponCharging_CK extends WeaponStateBase
{
	override bool IsWaitingForActionFinish () { return true; }
};

class WeaponEjectBullet_Cartridge extends WeaponStateBase
{
	Magazine m_dstMagazine; /// destination of the cartridge

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
		{
			DayZPlayer p = e.m_player;
			int mi = m_weapon.GetCurrentMuzzle();

			ejectBulletAndStoreInMagazine(m_weapon, mi, m_dstMagazine, p); // MP-safe
		}
	}

	override void OnAbort (WeaponEventBase e)
	{
		m_dstMagazine = NULL;
		super.OnAbort(e);
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dstMagazine = NULL;
		super.OnExit(e);
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_dstMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponEjectBullet_Cartridge.SaveCurrentFSMState: cannot write m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_dstMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponEjectBullet_Cartridge.LoadCurrentFSMState: cannot read m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

};

class WeaponEjectBulletMultiMuzzle_Cartridge extends WeaponStateBase
{
	Magazine m_dstMagazine; /// destination of the cartridge

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
		{
			DayZPlayer p = e.m_player;
			for(int i = 0; i < m_weapon.GetMuzzleCount(); i++)
			{
				m_weapon.CreateRound(i);
				ejectBulletAndStoreInMagazine(m_weapon, i, m_dstMagazine, p); // MP-safe
				m_weapon.EffectBulletHide(i);
				m_weapon.HideBullet(i);
			}
		}		
	}

	override void OnAbort (WeaponEventBase e)
	{
		m_dstMagazine = NULL;
		super.OnAbort(e);
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dstMagazine = NULL;
		super.OnExit(e);
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_dstMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponEjectBullet_Cartridge.SaveCurrentFSMState: cannot write m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_dstMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponEjectBullet_Cartridge.LoadCurrentFSMState: cannot read m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

};

class WeaponEjectBullet_Cartridge_W4T extends WeaponEjectBulletMultiMuzzle_Cartridge
{
	override bool IsWaitingForActionFinish () { return true; }
};


/**@class 	WeaponCharging
 * @brief	charging of weapon without ammo to be chambered
 */
class WeaponChargingInnerMag extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;

	ref WeaponCharging_Start m_start;
	ref WeaponEjectBullet_Cartridge_W4T m_onBE;
	ref WeaponEjectCasing_W4T m_onBEFireOut;
	ref BulletHide_W4T m_hideB;
	ref WeaponCharging_CK m_onCK;
	ref WeaponChamberFromInnerMag_W4T m_chamber;

	void WeaponChargingInnerMag (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponCharging_Start(m_weapon, this, m_action, m_actionType);
		m_onBE = new WeaponEjectBullet_Cartridge_W4T(m_weapon, this);
		m_onBEFireOut = new WeaponEjectCasing_W4T(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_onCK = new WeaponCharging_CK(m_weapon, this);
		m_chamber = new WeaponChamberFromInnerMag_W4T(m_weapon, this);

		// events
		WeaponEventBase __be_ = new WeaponEventAnimBulletEject;
		WeaponEventBase __bh_ = new WeaponEventAnimBulletHide;
		WeaponEventBase __ck_ = new WeaponEventAnimCocked;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		// transitions
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_onBEFireOut, NULL, new WeaponGuardCurrentChamberFiredOut(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_onBE, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_onBE, __bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(  m_onBEFireOut, __bh_, m_hideB));
		
		//TODO after inner magazine rework this events must load new bullet
		
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onBEFireOut, NULL, new GuardAnd(new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon)), new WeaponGuardCurrentChamberFiredOut(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onBE, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_chamber, NULL, new WeaponGuardHasAmmoInnerMagazine(m_weapon))); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onCK)); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, __ck_, m_chamber, NULL, new WeaponGuardHasAmmoInnerMagazine(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, __ck_, m_chamber, NULL, new WeaponGuardHasAmmoInnerMagazine(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, __ck_, m_chamber, NULL, new WeaponGuardHasAmmoInnerMagazine(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, __ck_, m_onCK));
		
		m_fsm.AddTransition(new WeaponTransition(  m_start, _fin_, NULL ));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_onCK, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(	m_chamber, _fin_, NULL));

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
	}
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};


/**@class 	WeaponCharging
 * @brief	charging of weapon without ammo to be chambered
 */
class WeaponChargingMultiple extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;

	ref WeaponCharging_Start m_start;
	ref WeaponEjectBullet_Cartridge_W4T m_onBE;
	ref WeaponEjectCasing_W4T m_onBEFireOut;
	ref BulletHide_W4T m_hideB;
	ref WeaponCharging_CK m_onCK;
	ref WeaponChamberFromAttMag_W4T m_chamber;

	void WeaponChargingMultiple (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponCharging_Start(m_weapon, this, m_action, m_actionType);
		m_onBE = new WeaponEjectBullet_Cartridge_W4T(m_weapon, this);
		m_onBEFireOut = new WeaponEjectCasing_W4T(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_onCK = new WeaponCharging_CK(m_weapon, this);
		m_chamber = new WeaponChamberFromAttMag_W4T(m_weapon, this);

		// events
		WeaponEventBase __be_ = new WeaponEventAnimBulletEject;
		WeaponEventBase __bh_ = new WeaponEventAnimBulletHide;
		WeaponEventBase __ck_ = new WeaponEventAnimCocked;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		// transitions
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_onBEFireOut, NULL, new WeaponGuardCurrentChamberFiredOut(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __be_, m_onBE, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_onBE, __bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(  m_onBEFireOut, __bh_, m_hideB));
		
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onBE, NULL, new GuardNot(new WeaponGuardCurrentChamberEmpty(m_weapon))));
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon))); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(  m_start, __ck_, m_onCK)); // some anims do not send BE event
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, __ck_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, __ck_, m_onCK));
		
		m_fsm.AddTransition(new WeaponTransition(   m_start, _fin_, NULL ));
		m_fsm.AddTransition(new WeaponTransition(   m_onBEFireOut, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_onBE, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_hideB, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(   m_onCK, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, NULL));

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
	}
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

