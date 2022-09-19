
// load bullet from att mag (no anim)
class WeaponChamberFromAttMag extends WeaponStateBase
{
	void WeaponChamberFromAttMag (Weapon_Base w = NULL, WeaponStateBase parent = NULL, int action = -1, int actionType = -1)
	{ }

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
		{
			int mi = m_weapon.GetCurrentMuzzle();
			pushToChamberFromAttachedMagazine(m_weapon, mi);
		}
	}
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

// load bullet from att mag (no anim)
class WeaponChamberFromInnerMag extends WeaponStateBase
{
	void WeaponChamberFromInnerMag (Weapon_Base w = NULL, WeaponStateBase parent = NULL, int action = -1, int actionType = -1)
	{ }

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			int mi = m_weapon.GetCurrentMuzzle();
			pushToChamberFromInnerMagazine(m_weapon, mi);
		}
	}
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

// load bullet from att mag (no anim)
class WeaponChamberFromAttMagOnExit extends WeaponStateBase
{
	void WeaponChamberFromAttMagOnExit (Weapon_Base w = NULL, WeaponStateBase parent = NULL, int action = -1, int actionType = -1)
	{ }

	override void OnExit (WeaponEventBase e)
	{
		int mi = m_weapon.GetCurrentMuzzle();
		pushToChamberFromAttachedMagazine(m_weapon, mi);

		super.OnExit(e);
	}
};



