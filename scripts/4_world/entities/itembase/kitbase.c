class KitBase extends ItemBase
{
	ref protected EffectSound 		m_DeployLoopSound;
	protected bool 					m_DeployedRegularly;

	void KitBase()
	{
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsDeploySound");
	}

	void ~KitBase()
	{
		SEffectManager.DestroyEffect( m_DeployLoopSound );
	}

	override bool IsBasebuildingKit()
	{
		return true;
	}
	
	override bool HasProxyParts()
	{
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( IsDeploySound() )
		{
			PlayDeploySound();
		}
				
		if ( CanPlayDeployLoopSound() )
		{
			PlayDeployLoopSound();
		}
					
		if ( m_DeployLoopSound && !CanPlayDeployLoopSound() )
		{
			StopDeployLoopSound();
		}
		
		if ( m_DeployedRegularly && IsSoundSynchRemote() )
		{
			PlayDeployFinishSound();
		}
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		//set visual on init
		UpdateVisuals();
		UpdatePhysics();
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( AssembleKit );
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
		SoundSynchRemote();
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
		//return "BarbedWire_Deploy_loop_SoundSet";
		return "Shelter_Site_Build_Loop_SoundSet";
	}
	
	override string GetDeployFinishSoundset()
	{
		return "";
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
	
	void PlayDeployLoopSound()
	{		
		if ( !GetGame().IsDedicatedServer() )
		{		
			if ( !m_DeployLoopSound || !m_DeployLoopSound.IsSoundPlaying() )
			{
				m_DeployLoopSound = SEffectManager.PlaySound( GetLoopDeploySoundset(), GetPosition() );
			}
		}
	}
	
	void StopDeployLoopSound()
	{
		if ( !GetGame().IsDedicatedServer() )
		{	
			m_DeployLoopSound.SetSoundFadeOut(0.5);
			m_DeployLoopSound.SoundStop();
		}
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
}