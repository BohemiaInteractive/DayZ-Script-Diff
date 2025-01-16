class KitBase extends ItemBase
{
	protected bool 					m_DeployedRegularly;

	override bool IsBasebuildingKit()
	{
		return true;
	}
	
	override bool HasProxyParts()
	{
		return true;
	}
	
	override bool CanProxyObstruct()
	{
		return false;
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		//set visual on init
		UpdateVisuals();
		UpdatePhysics();
		
		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( AssembleKit );
		}
	}
	
	override bool DisassembleOnLastDetach()
	{
		return true;
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached( item, slot_name );
		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if ( player && player.IsPlayerDisconnected() )
			return;
		
		if (item && slot_name == "Rope")
		{
			if (GetGame().IsServer() && !m_DeployedRegularly)
			{
				DisassembleKit(ItemBase.Cast(item));
				Delete();
			}
		}
	}
	
	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged( old_owner, new_owner );
		
		//update visuals after location change
		UpdatePhysics();
	}
	
	override void OnEndPlacement()
	{
		m_DeployedRegularly = true;
	}
	
	override void OnPlacementCancelled( Man player )
	{
		super.OnPlacementCancelled(player);
		m_DeployedRegularly = false;
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override bool CanAssignAttachmentsToQuickbar()
	{
		return false;
	}
		
	override string GetDeploySoundset()
	{
		return "putDown_FenceKit_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "Shelter_Site_Build_Loop_SoundSet";
	}
	
	override void RefreshPhysics()
	{
		super.RefreshPhysics();
		
		UpdatePhysics();
	}
	
	//Update visuals and physics
	void UpdateVisuals()
	{
		SetAnimationPhase( "Inventory", 0 );
		SetAnimationPhase( "Placing", 1 );
	}
	
	void UpdatePhysics()
	{
		AddProxyPhysics( "Inventory" );
		RemoveProxyPhysics( "Placing" );		
	}
		
	void AssembleKit()
	{
		if (!IsHologram())
		{
			Rope rope = Rope.Cast(GetInventory().CreateAttachment("Rope"));
		}
	}
	
	void DisassembleKit(ItemBase item) {}
	
	void CreateRope(Rope rope)
	{
		if (!rope)
			return;
		
		InventoryLocation targetLoc = rope.GetTargetLocation();
		if (targetLoc && targetLoc.GetType() != InventoryLocationType.GROUND)
		{
			MiscGameplayFunctions.TransferItemProperties(this, rope);
			return;
		}
		
		EntityAI newRope = EntityAI.Cast(GetGame().CreateObjectEx(rope.GetType(), GetPosition(), ECE_PLACE_ON_SURFACE));
		
		if (newRope)
			MiscGameplayFunctions.TransferItemProperties(this, newRope);
		
		rope.Delete();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}
	
	//!DEPRECATED
	protected ref EffectSound 	m_DeployLoopSound; //DEPRECATED in favor of m_DeployLoopSoundEx
	
	void PlayDeployLoopSound(); //!DEPRECATED
	void StopDeployLoopSound(); //!DEPRECATED
}