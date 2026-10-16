class MP18_Base extends OpenBolt_Base
{
	void MP18_Base()
	{
	}

	override RecoilBase SpawnRecoilObject()
	{
		return new MP18Recoil(this);
	}
	
	override float GetChanceToJam()
	{
		float chanceToJam = super.GetChanceToJam();
		Magazine mag = GetMagazine(GetCurrentMuzzle());
		
		if(mag)
		{
			chanceToJam = chanceToJam + ((1.0 - chanceToJam) * mag.GetChanceToJam());
		}
		
		return chanceToJam;
	}
	
	override bool MustBeChambered(int muzzleIndex)
	{
		return true;
	}
};