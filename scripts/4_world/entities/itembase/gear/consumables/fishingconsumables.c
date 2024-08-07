class FishingHookBase: ItemBase
{
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("Worm");
	}
};

class Hook:  FishingHookBase {};
class BoneHook:  FishingHookBase {};
class WoodenHook:  FishingHookBase {};
class Jig:  FishingHookBase {};

///////////////////////////
// Obsolete item prison //
/////////////////////////
class BaitBase : ItemBase
{
	// replaces stray baits with hook + attachment combo
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc,newLoc);
		
		string hookType = ConfigGetString("hookType");
		if (GetGame().IsDedicatedServer() && hookType != string.Empty)
		{
			DeleteSafe();
			vector spawnPos;
			if (newLoc.GetType() == InventoryLocationType.GROUND)
				spawnPos = newLoc.GetPos();
			else
				spawnPos = newLoc.GetParent().GetHierarchyRoot().GetPosition();
			
			EntityAI newItem = EntityAI.Cast(GetGame().CreateObjectEx(hookType,spawnPos,ECE_PLACE_ON_SURFACE,RF_DEFAULT));
			EntityAI worm = newItem.GetInventory().CreateAttachment("Worm");
			MiscGameplayFunctions.TransferItemProperties(this,newItem);
			MiscGameplayFunctions.TransferItemProperties(this,worm);
		}
	}
};

class Bait: BaitBase {};
class BoneBait: BaitBase {};
////////////////////////