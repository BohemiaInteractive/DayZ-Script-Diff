
// rechamber (== eject cartridge + load another + store the old one)
class WeaponRechamber extends WeaponStateBase
{
	int m_actionEject;
	int m_actionTypeEject;
	int m_actionLoad;
	int m_actionTypeLoad;
	Magazine m_dstMagazine; /// destination of the ejected cartridge
	Magazine m_srcMagazine; /// source of the loaded cartridge

	ref WeaponEjectBullet m_eje;
	ref WeaponChambering m_loa;

	void WeaponRechamber (Weapon_Base w = NULL, WeaponStateBase parent = NULL, int actionEject = -1, int actionTypeEject = -1, int actionLoad = -1, int actionTypeLoad = -1)
	{
		m_actionEject = actionEject;
		m_actionTypeEject = actionTypeEject;
		m_actionLoad = actionLoad;
		m_actionTypeLoad = actionTypeLoad;

		// setup nested state machine
		m_eje = new WeaponEjectBullet(m_weapon, this, m_actionEject, m_actionTypeEject);
		m_loa = new WeaponChambering(m_weapon, this, m_actionLoad, m_actionTypeLoad);
		// events
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_eje, _fin_, m_loa));
		m_fsm.AddTransition(new WeaponTransition(m_loa, _fin_, NULL));

		m_fsm.SetInitialState(m_eje);
	}

	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponRechamber, mag=" + e.m_magazine.ToString()); }
			m_srcMagazine = e.m_magazine;
			m_loa.m_srcMagazine = m_srcMagazine;

			// prepare magazine for ejected ammo
			int mi = m_weapon.GetCurrentMuzzle();
			string magazineTypeName = m_weapon.GetChamberedCartridgeMagazineTypeName(mi);
			float damage = 0.0;
			string type;
			if (m_weapon.GetCartridgeInfo(mi, damage, type))
			{
				m_dstMagazine = DayZPlayerUtils.SelectStoreCartridge(e.m_player, m_weapon, mi, m_srcMagazine, damage, magazineTypeName);
				if (!m_dstMagazine)
				{
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + "   WeaponRechamber - error, cannot prepare mag for catridge, magType=" + magazineTypeName);
				}
			}

			e.m_magazine = m_dstMagazine; // @NOTE: override event mag - @TODO
		}
		super.OnEntry(e); // @NOTE: super after submachine init (prevent override from submachine start)
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		m_dstMagazine = NULL;
		m_eje.m_dstMagazine = NULL;
		m_loa.m_srcMagazine = NULL;
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_dstMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot save m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
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
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_dstMagazine for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};

