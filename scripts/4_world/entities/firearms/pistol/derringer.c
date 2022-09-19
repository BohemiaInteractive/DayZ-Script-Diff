class Derringer_Base : DoubleBarrel_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new DerringerRecoil(this);
	}
};

class Derringer : Derringer_Base {};