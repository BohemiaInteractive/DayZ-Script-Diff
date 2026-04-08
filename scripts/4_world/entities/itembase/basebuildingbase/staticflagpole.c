/* 
For use in events/by modders, not part of standard gameplay
Example object spawner json use:

{
    "Objects": [
        {
            "name": "StaticFlagPole",
            "pos": [
                3000,
                57,
                3000
            ],
            "ypr": [
                0.0,
                0.0,
                0.0
            ],
            "scale": 1,
			"customString": "Flag_Bohemia"
        }
     ]
}

*/

class StaticFlagPole extends TerritoryFlag
{
	override void OnPartDismantledServer( notnull Man player, string part_name, int action_id )
	{
	}
	
	override void OnPartDestroyedServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
	}
	
	override void OnSpawnByObjectSpawner(ITEM_SpawnerObject item)
	{
		FullyBuild();
		
		if (item.customString != string.Empty)
		{
			EntityAI flag = GetInventory().CreateInInventory(item.customString);
			if (flag)
			{
				AnimateFlagEx(0);
				AddRefresherTime01(1);
			}
		}
	}

	// version of FullyBuild which does not require a player
	override void FullyBuild()
	{
		array<ConstructionPart> parts = GetConstruction().GetConstructionParts().GetValueArray();
				
		foreach (ConstructionPart part : parts)
		{
			ConstructionPart construtionPart = GetConstruction().GetConstructionPart(part.GetPartName());
	
			if (construtionPart.IsBase())
			{
				SetBaseState(true);
				CreateConstructionKit();
			}
				
			RegisterPartForSync(construtionPart.GetId());
			RegisterActionForSync(construtionPart.GetId(), AT_BUILD_PART);
			SynchronizeBaseState();
			SetPartFromSyncData(construtionPart); // server part of sync, client will be synced from SetPartsFromSyncData	
			UpdateNavmesh();
			UpdateVisuals();
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResetActionSyncData, 100, false, this);
		}
		
		GetConstruction().UpdateVisuals();
	}
}