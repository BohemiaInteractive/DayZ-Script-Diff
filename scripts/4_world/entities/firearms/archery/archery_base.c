/**@class		Archery_Base
 * @brief		base for Archery
 * @NOTE		name copies config base class
 **/
class Archery_Base : Weapon_Base
{
	void Archery_Base();
	
	override float GetChanceToJam()
	{
		return 0.0;
	}
}
