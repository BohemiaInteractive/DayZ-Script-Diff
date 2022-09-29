class GiftBox_Base extends Container_Base
{
	void GiftBox_Base()
	{
		m_Openable = new OpenableBehaviour(false);
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
	}
	
	protected ref OpenableBehaviour m_Openable;
	
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if( !super.CanReceiveItemIntoCargo(item) ) {return false;}
		
		if (GameInventory.GetInventoryCheckContext() == InventoryCheckContext.DEFAULT )
		{
			if(!GetGame().IsDedicatedServer())
				return IsOpen();
		}
		
		return true;
	}
	
	
	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();
	}
	
	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();
	}
	
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionUnpackGift);
	}
	
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Chemlight_Green" );	
		}
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		//Print("EEHealthLevelChanged");
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if ( newLevel == GameConstants.STATE_RUINED && GetGame().IsServer() )
		{
			MiscGameplayFunctions.ThrowAllItemsInInventory(this, 0);
			DeleteSafe();
		}
		//m_DamageSystemStarted = true;
	}
}

class GiftBox_Small extends GiftBox_Base {}
class GiftBox_Medium extends GiftBox_Base {}
class GiftBox_Large extends GiftBox_Base {}