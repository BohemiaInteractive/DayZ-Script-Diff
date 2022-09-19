class BulletShow extends WeaponStateBase
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
			m_weapon.ShowBullet(0);
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

class BulletShow2 extends WeaponStateBase
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
			m_weapon.ShowBullet(1);
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

class BulletShow_W4T extends BulletShow
{
	override bool IsWaitingForActionFinish () { return true; }
};

class BulletShow2_W4T extends BulletShow2
{
	override bool IsWaitingForActionFinish () { return true; }
};

