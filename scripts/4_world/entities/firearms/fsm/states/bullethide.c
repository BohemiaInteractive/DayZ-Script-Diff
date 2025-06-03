class BulletHide extends WeaponStateBase
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if(e)
		{
			m_weapon.SelectionBulletHide();
			m_weapon.EffectBulletHide(m_weapon.GetCurrentMuzzle());
		}
	}

	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
	}
};

class BulletHide_W4T extends BulletHide
{
	override bool IsWaitingForActionFinish () { return true; }
};

