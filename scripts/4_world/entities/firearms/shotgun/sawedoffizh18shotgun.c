/**@class		SawedoffIzh18Shotgun
 * @brief		base for SawedoffIzh18Shotgun
 * @NOTE		name copies config base class
 **/
class SawedoffIzh18Shotgun extends Izh18Shotgun_Base
{
	void SawedoffIzh18Shotgun ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Izh18ShotgunRecoil(this);
	}
};
