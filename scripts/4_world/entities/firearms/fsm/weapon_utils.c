bool pushToChamberFromAttachedMagazine(Weapon_Base weapon, int muzzleIndex)
{
	Magazine mag = weapon.GetMagazine(muzzleIndex);
	if (mag)
	{
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, using attached magazine mag=" + mag.ToString()); }
		float damage;
		string type;
		if (mag && mag.LocalAcquireCartridge(damage, type))
		{
			weapon.SelectionBulletShow();
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, ok - cartridge acquired: dmg=" + damage + " type=" + type); }
		}
		else
			Error("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, error - cannot take cartridge from magazine");

		if (weapon.PushCartridgeToChamber(muzzleIndex, damage, type))
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, ok - loaded chamber"); }
			return true;
		}
		else
			Error("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, error - cannot load chamber!");
	}
	else
	{
		Error("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromAttachedMagazine, error - no magazine attached");
	}
	return false;
}

bool pushToChamberFromInnerMagazine(Weapon_Base weapon, int muzzleIndex)
{

	if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromInnerMagazine, using inner magazine."); }
	float damage;
	string type;
	if (weapon.PopCartridgeFromInternalMagazine(muzzleIndex,damage, type))
	{
		weapon.SelectionBulletShow();
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromInnerMagazine, ok - cartridge acquired: dmg=" + damage + " type=" + type); }
	}
	else
		Error("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromInnerMagazine, error - cannot take cartridge from magazine");

	if (weapon.PushCartridgeToChamber(muzzleIndex, damage, type))
	{
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromInnerMagazine, ok - loaded chamber"); }
		return true;
	}
	else
		Error("[wpnfsm] " + Object.GetDebugName(weapon) + " chamberFromInnerMagazine, error - cannot load chamber!");

	return false;
}

void ejectBulletAndStoreInMagazine(Weapon_Base weapon, int muzzleIndex, Magazine mag, DayZPlayer p)
{
	float damage = 0;
	string type = string.Empty;
	string magazineTypeName = weapon.GetChamberedCartridgeMagazineTypeName(muzzleIndex);
	if (weapon.EjectCartridge(muzzleIndex, damage, type))
	{
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, ejected chambered cartridge"); }
	}
	else
		Error("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, error - cannot eject chambered cartridge!");

	bool is_single_or_server = !GetGame().IsMultiplayer() || GetGame().IsServer();
	if (is_single_or_server)
	{
		if (mag == NULL)
		{
			// no magazine configured in parent state, looking in inventory
			if (DayZPlayerUtils.HandleStoreCartridge(p, weapon, muzzleIndex, damage, type, magazineTypeName))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, ok - cartridge stored in magazine"); }
			}
			else
				Error("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, error - cannot store cartridge!");
		}
		else
		{
			if (mag.ServerStoreCartridge(damage, type))
			{
				mag.SetSynchDirty();
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, ok - cartridge stored in user defined magazine"); }
			}
			else
				Error("[wpnfsm] " + Object.GetDebugName(weapon) + " ejectBulletAndStoreInMagazine, error - cannot store cartridge in magazine");
		}
	}
}

bool magazinesHaveEqualSizes(notnull Magazine mag, notnull Magazine mag2)
{
	int w, h;
	GetGame().GetInventoryItemSize(mag, w, h);
	int w2, h2;
	GetGame().GetInventoryItemSize(mag2, w2, h2);
	if (w == w2 && h == h2)
	{
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] guard - same inventory sizes"); }
		return true;
	}

	if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] guard - different inventory sizes"); }
	return false;
}

