class AmmoTypesAPI
{
	private void AmmoTypesAPI() {}
	private void ~AmmoTypesAPI() {}
	
	//! Check if ammoType exists (registered in cfgAmmoTypes)
	static proto bool Exists(string ammoType);
	//! Get the default ammoType of a magazine
	static proto bool GetAmmoType(string magazineType, out string ammoType);
	
	//! Helper method
	static bool MagazineTypeToAmmoType(string magazineType, out string ammoType)
	{
		if ( !AmmoTypesAPI.Exists(magazineType) )
		{
			if ( !AmmoTypesAPI.GetAmmoType(magazineType, ammoType) )
			{
				ErrorEx(string.Format("%1 is not a magazineType or ammoType or has no default 'ammo' set up in config.", magazineType));
				return false;
			}
			return true;
		}
		else
		{
			// This is already an ammoType
			ammoType = magazineType;
			return true;
		}
	}
}