class MouthRag extends Mask_Base
{
	bool m_IncomingLambdaChange;
	
	void MouthRag()
	{
		m_IncomingLambdaChange = false;
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		return m_IncomingLambdaChange;
	}

	override bool IsObstructingVoice()
	{
		return true;
	}
	
	override int GetVoiceEffect()
	{
		return VoiceEffectObstruction;
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc,newLoc);
		
		if (g_Game.IsDedicatedServer())
		{
			PlayerBase playerOld;
			PlayerBase playerNew;
			if (oldLoc.GetParent())
				Class.CastTo(playerOld,oldLoc.GetParent().GetHierarchyRootPlayer());
			if (newLoc.GetParent())
				Class.CastTo(playerNew,newLoc.GetParent().GetHierarchyRootPlayer());
			
			if (newLoc.GetType() == InventoryLocationType.GROUND)
			{
				ItemBase newItem;
				if (Class.CastTo(newItem,g_Game.CreateObjectEx("Rag",newLoc.GetPos(),ECE_PLACE_ON_SURFACE,RF_DEFAULT)))
				{
					MiscGameplayFunctions.TransferItemProperties(this,newItem);
					newItem.SetQuantity(1);
					DeleteSafe();
				}
			}
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if (parent.IsPlayer())
		{
			PlayerBase.Cast(parent).CheckForGag(); //gag removal action does check, since 'OnWasDetached' is not called on item destruction.
		}
	}
	
	void SetIncomingLambaBool(bool state)
	{
		m_IncomingLambdaChange = state;
	}
	
	bool GetIncomingLambdaBool()
	{
		return m_IncomingLambdaChange;
	}
};