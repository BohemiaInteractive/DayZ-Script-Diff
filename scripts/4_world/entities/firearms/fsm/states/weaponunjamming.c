class WeaponChamberFromAttMag_W4T extends WeaponChamberFromAttMag
{
	override bool IsWaitingForActionFinish () { return true; }
};

class WeaponChamberFromInnerMag_W4T extends WeaponChamberFromInnerMag
{
	override bool IsWaitingForActionFinish () { return true; }
};

class WeaponUnjamming_Start extends WeaponStartAction
{
	float m_dtAccumulator;
	float m_jamTime;

	void WeaponUnjamming_Start (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_dtAccumulator = 0.0;
		m_jamTime = 0.0;
	}

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
	}

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;

		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());
		if(p)
		{
			HumanInputController hic = p.GetInputController();
			HumanCommandWeapons hcw = p.GetCommandModifier_Weapons();

			if (m_dtAccumulator >= m_jamTime)
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " Unjammed!"); }
				hcw.StartAction(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_END);
				m_weapon.ProcessWeaponEvent(new WeaponEventUnjammingTimeout(p));
				return; // unjammed successfuly
			}

			
			if (hic.IsReloadOrMechanismContinuousUse())
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " Unjamming..."); }
			}
			#ifndef PLATFORM_CONSOLE
			#ifndef SERVER_FOR_CONSOLE
			else
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " Unjamming failed, weapon still jammed"); }
				hcw.StartAction(WeaponActions.UNJAMMING, WeaponActionUnjammingTypes.UNJAMMING_INTERRUPT);
				m_weapon.ProcessWeaponEvent(new WeaponEventUnjammingFailedTimeout(p));
			}
			#endif
			#endif
		}
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dtAccumulator = 0.0;
		m_jamTime = 0.0;
		super.OnExit(e);
	}
};

class WeaponUnjamming_Cartridge extends WeaponStateBase
{
	float m_damage;
	string m_type;

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			m_type = string.Empty;
			
			float dmg;
			string type;
			
			m_weapon.SetJammed(false);
		
			int mi = m_weapon.GetCurrentMuzzle();
			
			if (m_weapon.IsChamberFiredOut(mi))
			{
				m_weapon.EjectCasing(mi);
				m_weapon.EffectBulletHide(mi);
				m_weapon.SelectionBulletHide();
			}
			
			if (!m_weapon.IsChamberEmpty(mi))
			{
				DayZPlayer p = e.m_player;
	
				m_weapon.CreateRound(mi);
				ejectBulletAndStoreInMagazine(m_weapon, mi, null, p);
				
				m_weapon.EffectBulletHide(mi);
				m_weapon.SelectionBulletHide();
			}
		}
	}
	override void OnExit (WeaponEventBase e)
	{
		m_type = string.Empty;
		super.OnExit(e);
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_damage))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponUnjamming_Cartridge.SaveCurrentFSMState: cannot write m_damage for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Write(m_type))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponUnjamming_Cartridge.SaveCurrentFSMState: cannot write m_type for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_damage))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponUnjamming_Cartridge.LoadCurrentFSMState: cannot read m_damage for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Read(m_type))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponUnjamming_Cartridge.LoadCurrentFSMState: cannot read m_type for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

};

class WeaponUnjamming_Cartridge_W4T extends WeaponUnjamming_Cartridge
{
	override bool IsWaitingForActionFinish () { return true; }
};

class WeaponUnjamming_W4T extends WeaponStateBase
{
	override bool IsWaitingForActionFinish () { return true; }
};


/**@class		WeaponUnjamming
 * @brief		unjam submachine
 **/
class WeaponUnjamming extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;
	float m_dtAccumulator;
	float m_jamTime;

	ref WeaponUnjamming_Start m_start;
	ref WeaponUnjamming_Cartridge_W4T m_eject;
	ref WeaponUnjamming_W4T m_w4t;
	ref WeaponChamberFromAttMag_W4T m_chamber;

	void WeaponUnjamming (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponUnjamming_Start(m_weapon, this, m_action, m_actionType);
		m_eject = new WeaponUnjamming_Cartridge_W4T(m_weapon, this);
		m_chamber = new WeaponChamberFromAttMag_W4T(m_weapon, this);
		m_w4t = new WeaponUnjamming_W4T(m_weapon, this);

		// events
		WeaponEventBase _uto_ = new WeaponEventUnjammingTimeout;
		WeaponEventBase _fto_ = new WeaponEventUnjammingFailedTimeout;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventBase __uj_ = new WeaponEventAnimUnjammed;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		// transitions
		m_fsm.AddTransition(new WeaponTransition(  m_start, _uto_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(  m_start, _fto_, m_w4t));
		m_fsm.AddTransition(new WeaponTransition(  m_eject, __uj_, m_chamber, NULL, new WeaponGuardHasAmmo(m_weapon)));
		m_fsm.AddTransition(new WeaponTransition(  m_eject, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(    m_w4t, _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_start,    _fin_, null));	

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			m_dtAccumulator = 0.0;
			m_jamTime = 5.0; // @TODO: rand

			m_start.m_dtAccumulator = m_dtAccumulator;
			m_start.m_jamTime = m_jamTime;
		}
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dtAccumulator = 0.0;
		m_jamTime = 0.0;
		super.OnExit(e);
	}
};

