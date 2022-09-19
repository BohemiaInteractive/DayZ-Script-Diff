const int MAX_QUICKBAR_SLOTS_COUNT = 10;
// Script File
class QuickBarItem
{
	EntityAI m_entity;
	bool m_enabled;
	
	void QuickBarItem()
	{
		m_entity = NULL;
		m_enabled = false;
	}
}

class QuickBarBase
{
	protected ref QuickBarItem m_aQuickbarEnts[MAX_QUICKBAR_SLOTS_COUNT]; 
	protected PlayerBase _player; 
	protected int m_slotsCount;
	
	void QuickBarBase(PlayerBase player)
	{
		for (int i = 0; i < MAX_QUICKBAR_SLOTS_COUNT; i++)
		{
			m_aQuickbarEnts[i] = new QuickBarItem;
			m_aQuickbarEnts[i].m_enabled = false;
			m_aQuickbarEnts[i].m_entity = NULL;
		}
		
		_player = player;
		m_slotsCount = 0;
	}
//-------------------------------------------------------------		
	void SetSize(int newSize)
	{
		int i = m_slotsCount;
		if ( newSize == m_slotsCount )
			return;
		
		if (newSize > MAX_QUICKBAR_SLOTS_COUNT)
			newSize = MAX_QUICKBAR_SLOTS_COUNT;
		
		if ( newSize > i )
		{	
			for (; i < newSize; i++)
			{
				EntityAI entity = m_aQuickbarEnts[i].m_entity;
				if ( entity != NULL && entity.GetHierarchyRootPlayer() == _player )
				{
					m_aQuickbarEnts[i].m_enabled = true;
				}
			}	
		}
		else
		{	
			for (i--; i >= newSize; i--)
				m_aQuickbarEnts[i].m_enabled = false;
		}
		
		m_slotsCount = newSize;
		
		if (_player.m_Hud)
			_player.m_Hud.RefreshQuickbar(true);
	}
//-------------------------------------------------------------		
	int GetNonEmptyCount()
	{
		int count = 0;
		for ( int i = 0; i < m_slotsCount; i++ )
		{
			if (m_aQuickbarEnts[i].m_enabled)
				count++;					
		}
		
		return count;	
	}
//-------------------------------------------------------------		
	EntityAI GetEntity(int index)
	{
		if (index < 0 || index >= m_slotsCount)
			return NULL;
		
		if (m_aQuickbarEnts[index].m_enabled)
			return m_aQuickbarEnts[index].m_entity;
		
		return NULL;
	}
	
//-------------------------------------------------------------	
	int GetSize()
	{
		return m_slotsCount;
	}
//-------------------------------------------------------------		
	int FindEntityIndex(EntityAI entity)
	{
		for (int i = 0; i < MAX_QUICKBAR_SLOTS_COUNT; i++)
		{
			if (m_aQuickbarEnts[i].m_entity == entity)
				return i;
		}
		
		return -1;	
	}
//-------------------------------------------------------------
	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		if ( userDataType == INPUT_UDT_QUICKABARSHORTCUT)
		{
			OnSetEntityRequest(ctx);
			return true;
		}
		
		return false;
	}
//-------------------------------------------------------------
	void UpdateShotcutVisibility(int index)
	{
		m_aQuickbarEnts[index].m_enabled = CanAddAsShortcut(m_aQuickbarEnts[index].m_entity);
		if (!m_aQuickbarEnts[index].m_enabled)
			CancelContinuousUse(index);
		
		if (_player.m_Hud)
			_player.m_Hud.RefreshQuickbar(true);
	}
//-------------------------------------------------------------
	void SetShotcutEnable(int index, bool value)
	{
		m_aQuickbarEnts[index].m_enabled = value && CanAddAsShortcut(m_aQuickbarEnts[index].m_entity);
		if (!m_aQuickbarEnts[index].m_enabled)
			CancelContinuousUse(index);
		
		if (_player.m_Hud)
			_player.m_Hud.RefreshQuickbar(true);
	}
//-------------------------------------------------------------
	bool CanAddAsShortcut(EntityAI entity)
	{
		InventoryLocation loc = new InventoryLocation;
		entity.GetInventory().GetCurrentInventoryLocation(loc);
		EntityAI parent = loc.GetParent();
		
		return (entity && entity.GetHierarchyRootPlayer() == _player && parent.CanAssignAttachmentsToQuickbar() && entity.CanAssignToQuickbar());
	}
//-------------------------------------------------------------		
	void SetEntityShortcut(EntityAI entity, int index, bool force = false)
	{	
		//Client	
		if (GetGame().IsClient())
		{		
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_QUICKABARSHORTCUT);
				ctx.Write(entity);
				ctx.Write(index);
				ctx.Write(force);
				ctx.Send();
			
				_SetEntityShortcut(entity, index, force);	
			}
		}
		//Server
		else if (GetGame().IsMultiplayer() && GetGame().IsServer())
		{
			DayZPlayerSyncJunctures.SendQuickbarSetShortcut(_player, entity, index, force);
		}
		else
		{
			// Single player
			_SetEntityShortcut(entity,index,force);
		}
	}
//-------------------------------------------------------------
	void OnSetEntityNoSync(EntityAI entity, int index, bool force = false )
	{
		_SetEntityShortcut(entity, index, force);
	}
//-------------------------------------------------------------
	//! Reaction on Rpc from server for set inicial state for quickbar
	void OnSetEntityRPC(ParamsReadContext ctx)
	{
		Param2<Entity,int> param;
		if (!ctx.Read(param))
			return;
		
		EntityAI entity1 = EntityAI.Cast(param.param1);
		_SetEntityShortcut(entity1, param.param2, false);
	}
//-------------------------------------------------------------
	//! Reaction on SetEntityShortcut from client
	void OnSetEntityRequest(ParamsReadContext ctx)
	{
		EntityAI eai = null;
		if (!ctx.Read(eai))
			return;
		
		int index = -1;
		if (!ctx.Read(index))
			return;
		
		bool force = false;
		if (!ctx.Read(force))
			return
		
		_SetEntityShortcut(eai, index, force);
	}
//-------------------------------------------------------------		
	protected void _SetEntityShortcut(EntityAI entity, int index, bool force = false)
	{
		//TODO Check, if is in inventory
		//if(entity.GetLoca)
		if ( entity == NULL )
		{
			_RemoveEntity(index);
			return;
		}
		
		int i = FindEntityIndex(entity);
		
		if ( i != -1 )
			_RemoveEntity(i);
				
		_RemoveEntity(index);
		_SetEntity( entity, index, force);				
	}
//-------------------------------------------------------------		
	void updateSlotsCount()
	{
		int slotsCount = _player.GetQuickBarBonus();
		int attCount  = _player.GetInventory().AttachmentCount();
		
		for ( int i = 0; i < attCount; ++i)
		{
			slotsCount += _player.GetInventory().GetAttachmentFromIndex(i).GetQuickBarBonus();
		}
		
		//max slots is 10
		SetSize(Math.Min(slotsCount, 10));
	}
//-------------------------------------------------------------		
	protected void _RemoveEntity(int index)
	{
		if ( index >= 0 && index < MAX_QUICKBAR_SLOTS_COUNT )
		{
			CancelContinuousUse(index);
			
			m_aQuickbarEnts[index].m_entity = NULL;
			m_aQuickbarEnts[index].m_enabled = false;
			
			if (_player.m_Hud)
				_player.m_Hud.RefreshQuickbar(true);
		}
	}
//-------------------------------------------------------------		
	protected void _SetEntity( EntityAI entity, int index, bool force = false)
	{		
		if ( index >= 0 && index < MAX_QUICKBAR_SLOTS_COUNT )
		{		
			if ( CanAddAsShortcut(entity) )
			{
				m_aQuickbarEnts[index].m_entity = entity;
				m_aQuickbarEnts[index].m_enabled = true;
			}
			else
			{
				CancelContinuousUse(index);
				m_aQuickbarEnts[index].m_enabled = false;
			}
			
			if (_player.m_Hud)
				_player.m_Hud.RefreshQuickbar(true);
		}		
	}
//-------------------------------------------------------------	
	void ~QuickBarBase()
	{
		
	}
//-------------------------------------------------------------	
	protected void CancelContinuousUse(int index)
	{
		if (_player.m_QuickBarHold)
		{
			HumanInputController hic = _player.GetInputController();
			if (hic && hic.IsQuickBarSlot() == index + 1)
				_player.OnQuickBarContinuousUseEnd(index + 1);
		}
	}
}