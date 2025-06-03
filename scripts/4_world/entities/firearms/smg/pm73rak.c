/**@class		Rifle_Base
 * @brief		base for rifles
 * @NOTE		name copies config base class
 **/
class PM73Rak_Base extends OpenBolt_Base
{
	void PM73Rak_Base()
	{
	}

	override RecoilBase SpawnRecoilObject()
	{
		return new PM73RakRecoil(this);
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