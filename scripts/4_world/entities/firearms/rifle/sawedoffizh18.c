class SawedoffIzh18 extends Izh18_Base
{
	void SawedoffIzh18 ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Izh18SawedOffRecoil(this);
	}
};
