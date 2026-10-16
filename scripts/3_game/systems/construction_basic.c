//! just declared for module 3 use
class ConstructionBasic
{
	void ConstructionBasic(EntityAI parent);
	void Init();
	void EEInitConstruction();
	void InitBaseState();
	void OnConstructionVariablesSynchronized();
	void OnHealthLevelChanged(int oldLevel, int newLevel, string zone);
	
	bool CanDisplayAttachmentCategoryConstruction(string categoryName)
	{
		return true;
	}
	
	//! Default return value is set so actions from the "inside" are always possible
	bool IsPartFacingPlayer(DayZPlayer player, string partName)
	{
		return false;
	}
	
	//! true if any part is buildable by hands
	bool CanUseHandConstruction()
	{
		return false;
	}
	
	//! already handled for BBB inside script class, overriden for Rebuilding later
	protected bool HandleStorageEvents()
	{
		return false;
	}
	
	void HandleStoreSave(ParamsWriteContext ctx);
	void HandleStoreLoad(ParamsReadContext ctx, int version);
	void ProcessConstructionDecay(float dTime);
	static bool CanConstructionDecay()
	{
		return !CfgGameplayHandler.GetDisableBaseDecay();
	}
	
	void OnConstructionDoorOpenStart(DoorStartParams params);
	void OnConstructionDoorCloseStart(DoorStartParams params);
}