// fire
class WeaponDryFire extends WeaponStartAction
{
	float m_dtAccumulator;

	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			m_dtAccumulator = 0;
		}
		m_weapon.ResetBurstCount();
	}

	override bool IsWaitingForActionFinish () { return true; }

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;
		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());
		if (p)
		{
			HumanInputController hic = p.GetInputController();

			int muzzleIndex = m_weapon.GetCurrentMuzzle();
			float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
			if ( hic.IsAttackButton() && m_dtAccumulator >= reloadTime)
				if (m_weapon.CanProcessWeaponEvents())
					m_weapon.ProcessWeaponEvent(new WeaponEventDryFireTimeout(p));
		}
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dtAccumulator = 0;
		super.OnExit(e);
	}
	
};

// fire
class WeaponFire extends WeaponStartAction
{
	float m_dtAccumulator;

	override bool IsWaitingForActionFinish () { return true; }

	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			m_dtAccumulator = 0;
	
			if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponFire bang!"); }
			//m_weapon.Fire();
			int mi = m_weapon.GetCurrentMuzzle();
			if (TryFireWeapon(m_weapon, mi))
			{
				DayZPlayerImplement p;
				if (Class.CastTo(p, e.m_player))
					p.GetAimingModel().SetRecoil(m_weapon);
				m_weapon.OnFire(mi);
			}
		}
		super.OnEntry(e);
	}

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;

		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());

		int muzzleIndex = m_weapon.GetCurrentMuzzle();
		float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
		if (m_dtAccumulator >= reloadTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
	}

	override void OnExit (WeaponEventBase e)
	{
		if (e)
			m_dtAccumulator = 0;
		super.OnExit(e);
	}
	
	override void OnAbort (WeaponEventBase e)
	{
		m_weapon.ResetBurstCount();
		super.OnAbort(e);
	}
};

class WeaponFireWithEject extends WeaponFire
{
		override void OnEntry (WeaponEventBase e)
	{
		m_dtAccumulator = 0;

		if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponFire bang!"); }
		//m_weapon.Fire();
		int mi = m_weapon.GetCurrentMuzzle();
		if (TryFireWeapon(m_weapon, mi))
		{
			DayZPlayerImplement p;
			if (Class.CastTo(p, e.m_player))
				p.GetAimingModel().SetRecoil(m_weapon);
			m_weapon.EjectCasing(mi);
			m_weapon.OnFire(mi);
		}
		super.OnEntry(e);
	}
}

// fire
class WeaponFireMultiMuzzle extends WeaponStartAction
{
	float m_dtAccumulator;

	override bool IsWaitingForActionFinish () { return true; }

	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			m_dtAccumulator = 0;
	
			if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponFire bang bang!"); }

			int mi = m_weapon.GetCurrentMuzzle();
			int b = m_weapon.GetCurrentModeBurstSize(mi);
			if(b > 1 )
			{
				
				for (int i = 0; i < b; i++)
				{
					if (TryFireWeapon(m_weapon, i))
					{
						DayZPlayerImplement p1;
						if (Class.CastTo(p1, e.m_player))
						p1.GetAimingModel().SetRecoil(m_weapon);
						m_weapon.OnFire(i);
					}
				}
			}
			else
			{
				if (TryFireWeapon(m_weapon, mi))
				{
					DayZPlayerImplement p;
					if (Class.CastTo(p, e.m_player))
						p.GetAimingModel().SetRecoil(m_weapon);
					m_weapon.OnFire(mi);
				}
			}
			if(mi >= m_weapon.GetMuzzleCount() - 1 )
				m_weapon.SetCurrentMuzzle(0);
			else
				m_weapon.SetCurrentMuzzle(mi + 1);
		}
		super.OnEntry(e);
	}

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;

		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());

		int muzzleIndex = m_weapon.GetCurrentMuzzle();
		float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
		if (m_dtAccumulator >= reloadTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
	}

	override void OnExit (WeaponEventBase e)
	{
		if (e)
			m_dtAccumulator = 0;
		super.OnExit(e);
	}
	
	override void OnAbort (WeaponEventBase e)
	{
		m_weapon.ResetBurstCount();
		super.OnAbort(e);
	}
};


class WeaponFireMagnum extends WeaponFireMultiMuzzle
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(m_weapon.GetAttachmentByType(Magnum_Cylinder));
			Magnum_Ejector ejector = Magnum_Ejector.Cast(m_weapon.GetAttachmentByType(Magnum_Ejector));
			if(cylinder && ejector)
			{
				float a;
				int mi = m_weapon.GetCurrentMuzzle();
				switch(mi)
				{
					case 0:
						a = MAGNUM_ROTATION_POSITION_0;
						break;
					case 1:
						a = MAGNUM_ROTATION_POSITION_5;
						cylinder.ResetAnimationPhase("Rotate_Cylinder", MAGNUM_ROTATION_POSITION_6 );
						ejector.ResetAnimationPhase("Rotate_Ejector", MAGNUM_ROTATION_POSITION_6 );
						break;
					case 2:
						a = MAGNUM_ROTATION_POSITION_4;
						break;
					case 3:
						a = MAGNUM_ROTATION_POSITION_3;
						break;
					case 4:
						a = MAGNUM_ROTATION_POSITION_2;
						break;
					case 5:
						a = MAGNUM_ROTATION_POSITION_1;
						break;
				}
				cylinder.SetAnimationPhase("Rotate_Cylinder", a );
				ejector.SetAnimationPhase("Rotate_Ejector", a );
			}
		}
	}

	override void OnUpdate (float dt)
	{
		m_dtAccumulator += dt;

		DayZPlayer p;
		Class.CastTo(p, m_weapon.GetHierarchyParent());

		int muzzleIndex = m_weapon.GetCurrentMuzzle();
		float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
		if (m_dtAccumulator >= reloadTime)
			if (m_weapon.CanProcessWeaponEvents())
				m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
	}

	override void OnExit (WeaponEventBase e)
	{
		if (e)
			m_dtAccumulator = 0;
		super.OnExit(e);
	}
};



// fire to jam
class WeaponFireToJam extends WeaponStartAction
{
	float m_dtAccumulator;

	override bool IsWaitingForActionFinish () { return true; }

	override void OnEntry (WeaponEventBase e)
	{
		if (e)
		{
			m_dtAccumulator = 0;

			if (LogManager.IsWeaponLogEnable()) { wpnPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponFire bang! and jam?"); }
			//m_weapon.Fire();
			int mi = m_weapon.GetCurrentMuzzle();
			if (TryFireWeapon(m_weapon, mi))
			{
				m_weapon.SetJammed(true);
				DayZPlayerImplement p;
				if (Class.CastTo(p, e.m_player))
					p.GetAimingModel().SetRecoil(m_weapon);
				m_weapon.OnFire(mi);
			}
		}
		m_weapon.ResetBurstCount();
		super.OnEntry(e);
	}

	override void OnUpdate (float dt)
	{
			m_dtAccumulator += dt;

			DayZPlayer p;
			Class.CastTo(p, m_weapon.GetHierarchyParent());

			int muzzleIndex = m_weapon.GetCurrentMuzzle();
			float reloadTime = m_weapon.GetReloadTime(muzzleIndex);
			if (m_dtAccumulator >= reloadTime)
				if (m_weapon.CanProcessWeaponEvents())
					m_weapon.ProcessWeaponEvent(new WeaponEventReloadTimeout(p));
	}

	override void OnExit (WeaponEventBase e)
	{
		m_dtAccumulator = 0;
		super.OnExit(e);
	}
};


class WeaponFireAndChamber extends WeaponFire
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			if (!m_weapon.IsJammed())
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ejected fired out casing"); }
				int mi = m_weapon.GetCurrentMuzzle();
				m_weapon.EjectCasing(mi);
				m_weapon.EffectBulletHide(mi);
				m_weapon.SelectionBulletHide();
	
				pushToChamberFromAttachedMagazine(m_weapon, mi);
			}
		}
	}
};

class WeaponFireAndChamberFromInnerMagazine extends WeaponFire
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		
		if (!m_weapon.IsJammed())
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ejected fired out casing"); }
			int mi = m_weapon.GetCurrentMuzzle();
			m_weapon.EjectCasing(mi);
			m_weapon.EffectBulletHide(mi);
			m_weapon.SelectionBulletHide();
			
			pushToChamberFromInnerMagazine(m_weapon, mi);
		}
	}
};

