class MagazineShow extends WeaponStateBase
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
			m_weapon.ShowMagazine();
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

class MagazineShow_W4T extends MagazineShow
{
	override bool IsWaitingForActionFinish () { return true; }
};

