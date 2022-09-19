class RifleChambering extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;
	Magazine m_srcMagazine; /// source of the cartridge

	ref WeaponStateBase m_start;
	ref WeaponEjectCasing m_eject;
	ref WeaponChambering_Cartridge m_chamber;
	ref WeaponChambering_W4T m_w4t;

	void RifleChambering (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_actionType);
		m_eject = new WeaponEjectCasing(m_weapon, this);
		m_chamber = new WeaponChambering_Cartridge(m_weapon, this);
		m_w4t = new WeaponChambering_W4T(m_weapon, this);
		// events
		WeaponEventAnimBulletEject __be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletShow  __bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletInChamber __bc_ = new WeaponEventAnimBulletInChamber;
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start  , __be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , __bs_, m_chamber));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bc_, m_w4t));
		m_fsm.AddTransition(new WeaponTransition(m_w4t    , _fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_start  , _fin_, null));	

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		if (e != NULL)
		{
			if (e.m_magazine != NULL)
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " RifleChambering, taking mag from event e.mag=" + e.m_magazine.ToString()); }
				m_srcMagazine = e.m_magazine;
				m_chamber.m_srcMagazine = m_srcMagazine;
			}
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " RifleChambering (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString()); }
		}
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " m_srcMagazine=" + m_srcMagazine.ToString()); }
		
		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort (WeaponEventBase e)
	{
		super.OnAbort(e);
		m_srcMagazine = NULL;
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		m_srcMagazine = NULL;
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " RifleChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " RifleChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};

