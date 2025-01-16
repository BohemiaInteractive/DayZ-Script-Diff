//TODO trees are static objects, there is no script event for playing sounds on clients when they are chopped down.
class ToolBase extends ItemBase
{	
	protected int m_MineDisarmRate = 60; //Success rate when disarming with this tool
	
	void ToolBase()
	{
		
	}
	
	//! If used to un/lock doors, which does it open. Bitwise.
	int GetKeyCompatibilityType()
	{
		//you can combine the bit values like so:
		//return (1 << EBuildingLockType.LOCKPICK) | (1 << EBuildingLockType.SHIP_CONTAINER_1);
		return EBuildingLockType.NONE; //pure '0'
	}
	
	int GetDisarmRate()
	{
		return m_MineDisarmRate;
	}
			
	override void OnRPC(PlayerIdentity sender, int rpc_type,ParamsReadContext  ctx) 
	{
		super.OnRPC(sender, rpc_type,ctx);
		
		switch(rpc_type)
		{
			case PlantType.TREE_HARD:
				SoundHardTreeFallingPlay();
			break;
			
			case PlantType.TREE_SOFT:
				SoundSoftTreeFallingPlay();
			break;
			
			case PlantType.BUSH_HARD:
				SoundHardBushFallingPlay();
			break;
			
			case PlantType.BUSH_SOFT:
				SoundSoftBushFallingPlay();
			break;
		}
	}
}