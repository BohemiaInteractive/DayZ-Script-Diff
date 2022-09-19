/**@class		BoltRifle_Base
 * @brief		base for bolt action rifles
 * @NOTE		name copies config base class
 **/
class BoltActionRifle_Base extends Weapon_Base
{
	override float GetChanceToJam()
	{
		return 0.0;
	}
};


