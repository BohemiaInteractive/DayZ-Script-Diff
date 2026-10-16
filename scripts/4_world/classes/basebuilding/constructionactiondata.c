class ConstructionActionData
{
	Object m_Target;
	
	//base building
	ref array<ConstructionPart> 	m_BuildParts;
	ref array<ConstructionPart>	 	m_BuildPartsNoTool; //used only for ShelterSite building (legacy)
	ref array<ConstructionPart>	 	m_BuildPartsHandsGeneric; //used for generic construction, EXCEPT for ShelterSites
	ref array<ConstructionPart>	 	m_BuildPartsJustInfos;
	#ifdef DIAG_DEVELOPER
	ref array<ConstructionPart>	 	m_BuildPartsInstant;
	#endif
	protected bool 					m_IsValid; //validated from main update method on each simulation step
	
	int 							m_PartIndex; //used on client only, action synchronizes it to server to avoid mismatch
	string 							m_MainPartName; //unused?
	string 							m_MainPartNameNoTool;
	ref ConstructionPart 			m_TargetPart;
	
	//combination lock
	CombinationLock 				m_CombinationLock;
	int								m_CombinationLockSide = -1;
	
	//attaching
	PlayerBase 						m_ActionInitiator;
	//detaching
	ref array<EntityAI> 			m_Attachments;
	
	protected ActionVariantManager	m_ActionVariantManager;
	protected ActionVariantManager	m_ActionNoToolVariantManager;
	protected ActionVariantManager	m_ActionHandsGenericVariantManager;
	protected ActionVariantManager	m_ActionInfoContinuousVariantManager;
	#ifdef DIAG_DEVELOPER
	protected ActionVariantManager	m_ActionVariantManagerInstant;
	#endif
	//protected ActionVariantManager	m_ActionInfoInteractVariantManager;

	int 							m_AttachmentsIndex;
	#ifdef DIAG_DEVELOPER
	private int 					m_FastConstructionMode;
	#endif
	void ConstructionActionData()
	{
		m_BuildParts = new array<ConstructionPart>();
		m_BuildPartsNoTool = new array<ConstructionPart>();
		m_BuildPartsHandsGeneric = new array<ConstructionPart>();
		m_BuildPartsJustInfos = new array<ConstructionPart>();
		#ifdef DIAG_DEVELOPER
		m_BuildPartsInstant = new array<ConstructionPart>();
		#endif
		m_PartIndex = 0;
		
		m_Attachments = new array<EntityAI>;
		m_AttachmentsIndex = 0;

		if ( g_Game.IsClient() || !g_Game.IsMultiplayer() )
		{
			m_IsValid = false;
			
			m_ActionVariantManager = ActionManagerClient.GetVariantManager(ActionBuildPart);
			m_ActionVariantManager.GetOnUpdateInvoker().Clear();
			m_ActionVariantManager.GetOnUpdateInvoker().Insert(OnUpdateActions);
			
			m_ActionHandsGenericVariantManager = ActionManagerClient.GetVariantManager(ActionBuildPartHandsGeneric);
			m_ActionHandsGenericVariantManager.GetOnUpdateInvoker().Clear();
			m_ActionHandsGenericVariantManager.GetOnUpdateInvoker().Insert(OnUpdateActionsHandsGeneric);
			
			m_ActionInfoContinuousVariantManager = ActionManagerClient.GetVariantManager(ActionConstructionPartInfoContinuous);
			m_ActionInfoContinuousVariantManager.GetOnUpdateInvoker().Clear();
			m_ActionInfoContinuousVariantManager.GetOnUpdateInvoker().Insert(OnUpdateInfoActions);
			
			#ifdef DIAG_DEVELOPER
			m_ActionVariantManagerInstant = ActionManagerClient.GetVariantManager(ActionBuildPartInstantDebug);
			m_ActionVariantManagerInstant.GetOnUpdateInvoker().Clear();
			m_ActionVariantManagerInstant.GetOnUpdateInvoker().Insert(OnUpdateActionsInstant);
			#endif
			/*m_ActionInfoInteractVariantManager = ActionManagerClient.GetVariantManager(ActionConstructionPartInfoInteract);
			m_ActionInfoInteractVariantManager.GetOnUpdateInvoker().Clear();
			m_ActionInfoInteractVariantManager.GetOnUpdateInvoker().Insert(OnUpdateInfoActions);*/
		}
	}
	
	void ~ConstructionActionData()
	{
		if (g_Game && (g_Game.IsClient() || !g_Game.IsMultiplayer()))
		{
			if (m_ActionVariantManager)
				m_ActionVariantManager.GetOnUpdateInvoker().Remove(OnUpdateActions);
			if (m_ActionHandsGenericVariantManager)
				m_ActionHandsGenericVariantManager.GetOnUpdateInvoker().Remove(OnUpdateActionsHandsGeneric);
			if (m_ActionInfoContinuousVariantManager)
				m_ActionInfoContinuousVariantManager.GetOnUpdateInvoker().Remove(OnUpdateInfoActions);
			/*if (m_ActionInfoInteractVariantManager)
				m_ActionInfoInteractVariantManager.GetOnUpdateInvoker().Remove(OnUpdateInfoActions);*/
		}
	}
	
	//************************************************/
	//  Base building
	//************************************************/
	string GetMainPartName()
	{
		return m_MainPartName;
	}
	
	string GetMainPartNameNoTool()
	{
		return m_MainPartNameNoTool;
	}
	
	void SetTarget( Object target )
	{
		m_Target = target;
	}
	
	Object GetTarget()
	{
		return m_Target;
	}
	
	void SetTargetPart( ConstructionPart target_part )
	{
		m_TargetPart = target_part;
	}

	ConstructionPart GetTargetPart()
	{
		return m_TargetPart;
	}
	
	void SetActionInitiator( PlayerBase action_initiator )
	{
		m_ActionInitiator = action_initiator;
	}
	
	PlayerBase GetActionInitiator()
	{
		return m_ActionInitiator;
	}
	
	//! to be called from outside
	void Invalidate()
	{
		m_IsValid = false;
	}
	
	protected void Validate()
	{
		m_IsValid = true;
	}
	
	//! Single update method to prepare and validate all construction data for the update, to avoid multiple iteration over the same parts array
	protected void ValidateConstructionData(Object item, Object target, int component_index)
	{
		if (!m_IsValid)
		{
			//TODO
			Validate();
		}
	}
	
	//! regular tool actions
	void OnUpdateActions( Object item, Object target, int component_index )
	{
		ValidateConstructionData(item,target,component_index);
		
		ItemBase tool = ItemBase.Cast(item);
		EntityAI ent = EntityAI.Cast(target);
		if (ent)
		{
			ConstructionBase con = ConstructionBase.Cast(ent.GetConstructionBasic());
			if (con)
			{
				array<string> compNames = new array<string>;
				target.GetActionComponentNameList(component_index, compNames, LOD.NAME_VIEW); //in case component is part of more than one named selection
				m_BuildParts.Clear();
				foreach (string compName : compNames)
				{
					array<ConstructionPart> tmp = new array<ConstructionPart>();
					con.GetConstructionPartsToBuild(compName, tmp, tool, m_MainPartName, true); //m_MainPartName unused?
					foreach (ConstructionPart part : tmp)
					{
						if (m_BuildParts.Find(part) == -1)
							m_BuildParts.Insert(part);
					}
				}
				m_ActionVariantManager.SetActionVariantCount(m_BuildParts.Count());
			}
			else
			{
				m_BuildParts.Clear();
				m_ActionVariantManager.Clear();
			}
		}
		else
		{
			m_BuildParts.Clear();
			m_ActionVariantManager.Clear();
		}		
		//not needed
		//m_Target = target;
	}
	
	//! used for ShelterSite only (legacy)
	void OnUpdateActionsNoTool( Object item, Object target, int component_index )
	{
		ValidateConstructionData(item,target,component_index);
		
		ItemBase tool = ItemBase.Cast(item);
		ShelterSite shelter = ShelterSite.Cast(target);
		ConstructionBase con;
		if (!tool && shelter && Class.CastTo(con,shelter.GetConstructionBasic()))
		{
			array<string> compNames = new array<string>;
			target.GetActionComponentNameList(component_index, compNames, LOD.NAME_VIEW); //in case component is part of more than one named selection
			m_BuildPartsNoTool.Clear();
			foreach (string compName : compNames)
			{
				array<ConstructionPart> tmp = new array<ConstructionPart>();
				con.GetConstructionPartsToBuild(compName, tmp, null, m_MainPartNameNoTool, false); //m_MainPartNameNoTool unused?
				foreach (ConstructionPart part : tmp)
				{
					if (m_BuildPartsNoTool.Find(part) == -1)
						m_BuildPartsNoTool.Insert(part);
				}
			}
			m_ActionNoToolVariantManager.SetActionVariantCount(m_BuildPartsNoTool.Count());
		}
		else
		{
			m_BuildPartsNoTool.Clear();
			m_ActionNoToolVariantManager.Clear();
		}
	}
	
	void OnUpdateActionsHandsGeneric(Object item, Object target, int componentIndex)
	{
		ValidateConstructionData(item,target,componentIndex);
		
		ItemBase tool = ItemBase.Cast(item);
		EntityAI ent = EntityAI.Cast(target);
		ConstructionBase con;
		if (!tool && ent && Class.CastTo(con,ent.GetConstructionBasic()))
		{
			array<string> compNames = new array<string>;
			target.GetActionComponentNameList(componentIndex, compNames, LOD.NAME_VIEW); //in case component is part of more than one named selection
			m_BuildPartsHandsGeneric.Clear();
			foreach (string compName : compNames)
			{
				array<ConstructionPart> tmp = new array<ConstructionPart>();
				con.GetConstructionPartsToBuild(compName, tmp, null, m_MainPartNameNoTool, false); //m_MainPartNameNoTool unused?
				foreach (ConstructionPart part : tmp)
				{
					if (m_BuildPartsHandsGeneric.Find(part) == -1)
						m_BuildPartsHandsGeneric.Insert(part);
				}
			}
			m_ActionHandsGenericVariantManager.SetActionVariantCount(m_BuildPartsHandsGeneric.Count());
		}
		else
		{
			m_BuildPartsHandsGeneric.Clear();
			m_ActionHandsGenericVariantManager.Clear();
		}
	}
	
	void OnUpdateInfoActions( Object item, Object target, int component_index )
	{
		ValidateConstructionData(item,target,component_index);
		
		ItemBase tool = ItemBase.Cast(item);
		EntityAI ent = EntityAI.Cast(target);
		if (ent)
		{
			ConstructionBase con = ConstructionBase.Cast(ent.GetConstructionBasic());
			if (con)
			{
				array<string> compNames = new array<string>;
				target.GetActionComponentNameList(component_index, compNames, LOD.NAME_VIEW); //in case component is part of more than one named selection
				m_BuildPartsJustInfos.Clear();
				foreach (string compName : compNames)
				{
					array<ConstructionPart> tmp = new array<ConstructionPart>();
					con.GetConstructionPartsUnableToBuild(compName, tmp, tool, m_MainPartName, tool != null); //m_MainPartName unused?
					foreach (ConstructionPart part : tmp)
					{
						if (m_BuildPartsJustInfos.Find(part) == -1)
							m_BuildPartsJustInfos.Insert(part);
					}
				}
				m_ActionInfoContinuousVariantManager.SetActionVariantCount(m_BuildPartsJustInfos.Count());
				//m_ActionInfoInteractVariantManager.SetActionVariantCount(m_BuildPartsJustInfos.Count());
			}
			else
			{
				m_BuildPartsJustInfos.Clear();
				m_ActionInfoContinuousVariantManager.Clear();
				//m_ActionInfoInteractVariantManager.Clear();
			}
		}
		else
		{
			m_BuildPartsJustInfos.Clear();
			m_ActionInfoContinuousVariantManager.Clear();
			//m_ActionInfoInteractVariantManager.Clear();
		}
	}
	
	#ifdef DIAG_DEVELOPER
	void OnUpdateActionsInstant(Object item, Object target, int componentIndex)
	{
		ValidateConstructionData(item,target,componentIndex);
		
		EntityAI ent;
		ConstructionBase con;
		if (Class.CastTo(ent, EntityAI.Cast(target)) && Class.CastTo(con, ent.GetConstructionBasic()))
		{
			array<string> compNames = new array<string>;
			target.GetActionComponentNameList(componentIndex, compNames, LOD.NAME_VIEW); //in case component is part of more than one named selection
			m_BuildPartsInstant.Clear();
			foreach (string compName : compNames)
			{
				array<ConstructionPart> tmp = new array<ConstructionPart>();
				con.GetConstructionPartsToBuild(compName, tmp, null, m_MainPartNameNoTool, false); //m_MainPartNameNoTool unused?
				foreach (ConstructionPart part : tmp)
				{
					if (m_BuildPartsInstant.Find(part) == -1)
						m_BuildPartsInstant.Insert(part);
				}
			}
			m_ActionVariantManagerInstant.SetActionVariantCount(m_BuildPartsInstant.Count());
		}
		else
		{
			m_BuildPartsInstant.Clear();
			m_ActionVariantManagerInstant.Clear();
		}
	}
	#endif
	
	int GetConstructionPartsCount()
	{
		return m_BuildParts.Count();
	}
	
	ConstructionPart GetBuildPartAtIndex(int idx)
	{
		if( m_BuildParts.Count() > idx )
		{
			return m_BuildParts[idx];
		}
		return null;
	}
	
	ConstructionPart GetBuildPartNoToolAtIndex(int idx)
	{
		if( m_BuildPartsNoTool.Count() > idx )
		{
			return m_BuildPartsNoTool[idx];
		}
		return null;
	}
	
	ConstructionPart GetBuildPartHandsGenericAtIndex(int idx)
	{
		if( m_BuildPartsHandsGeneric.Count() > idx )
		{
			return m_BuildPartsHandsGeneric[idx];
		}
		return null;
	}
	
	ConstructionPart GetBuildPartInfoAtIndex(int idx)
	{
		if( m_BuildPartsJustInfos.Count() > idx )
		{
			return m_BuildPartsJustInfos[idx];
		}
		return null;
	}
	
	#ifdef DIAG_DEVELOPER
	ConstructionPart GetBuildPartInstantAtIndex(int idx)
	{
		if (m_BuildPartsInstant.Count() > idx)
		{
			return m_BuildPartsInstant[idx];
		}
		return null;
	}
	#endif

	//************************************************/
	//  Combination lock
	//************************************************/	
	CombinationLock GetCombinationLock()
	{
		return m_CombinationLock;
	}
	
	void SetCombinationLock( CombinationLock combination_lock )
	{
		m_CombinationLock = CombinationLock.Cast( combination_lock );
	}

	string GetDialNumberText()
	{
		string dialText;
		
		if (m_CombinationLock && m_CombinationLockSide != -1)
		{
			string combinationText;
			if (m_CombinationLockSide == 0)
			{
				combinationText = m_CombinationLock.GetCombination().ToString();
			}
			else if (m_CombinationLockSide == 1)
			{
				combinationText = m_CombinationLock.GetCombinationInside().ToString();
			}
						
			//insert zeros to dials with 0 value
			int lengthDiff = m_CombinationLock.GetLockDigits() - combinationText.Length();
			for (int i = 0; i < lengthDiff; ++i)
			{
				combinationText = "0" + combinationText;
			}
			
			int dialIndex;
			if (m_CombinationLockSide == 0)
			{
				dialIndex = m_CombinationLock.GetDialIndex();
			}
			else if (m_CombinationLockSide == 1)
			{
				dialIndex = m_CombinationLock.GetDialIndexInside();
			}
			
			//assemble the whole combination with selected part
			for (int j = 0; j < m_CombinationLock.GetLockDigits(); ++j)
			{
				if (j == dialIndex)
				{
					dialText += string.Format("[%1]", combinationText.Get(j));
				}
				else
				{
					dialText += string.Format(" %1 ", combinationText.Get(j));
				}
			}
		}

		return dialText;
	}
	
	void SetCombinationLockSide(int lockSide)
	{
		m_CombinationLockSide = lockSide;
	}
	
	int GetCombinationLockSide()
	{
		return m_CombinationLockSide;
	}
	
	//************************************************/
	//  Attach/Detach actions
	//************************************************/	
	int GetAttachmentSlotFromSelection( PlayerBase player, EntityAI target, ItemBase item_to_attach, string selection )
	{
		TypeAttachmentGroupsDataHolder holder = target.GetTypeAttachmentGroupInfoHolder();
		array<TypeAttachmentGroupData> dataArray = holder.GetAttGroupsDataBySelectionParam(selection);
		if (!dataArray) //no valid slot paired to the selection
			return InventorySlots.INVALID;
		
		foreach (TypeAttachmentGroupData data : dataArray)
		{
			if (data)
			{
				int slotId;
				foreach (string slotName : data.m_AttSlots)
				{
					slotId = InventorySlots.GetSlotIdFromString(slotName);
					GameInventory inventory = target.GetInventory();
					EntityAI attachment = inventory.FindAttachment(slotId);
					
					if ((inventory.CanAddAttachmentEx(item_to_attach, slotId)) || (attachment && attachment.CanBeCombined(item_to_attach)))
					{
						if(target.CanDisplayAttachmentSlot(slotId) && target.CanDisplayAttachmentCategory(data.GetAttachmentGroupClassName()))
							return slotId;
					}
				}
			}
		}
		
		return InventorySlots.INVALID;
	}
	
	void GetAttachmentsFromSelection( EntityAI target, string selection, out array<EntityAI> attachments )
	{
		attachments.Clear();		//clear output
		TypeAttachmentGroupsDataHolder holder = target.GetTypeAttachmentGroupInfoHolder();
		array<TypeAttachmentGroupData> dataArray = holder.GetAttGroupsDataBySelectionParam(selection);
		foreach (TypeAttachmentGroupData data : dataArray)
		{
			if (data)
			{
				int slotId;
				EntityAI attachment;
				GameInventory inventory = target.GetInventory();
				foreach (string slotName : data.m_AttSlots)
				{
					slotId = InventorySlots.GetSlotIdFromString(slotName);
					attachment = inventory.FindAttachment(slotId);
					if (attachment && inventory.CanRemoveAttachmentEx(attachment, slotId) && !inventory.GetSlotLock(slotId))
					{
						attachments.Insert(attachment);
					}
				}
			}
		}
	}
	
	void CombineItems( ItemBase target, ItemBase item )
	{
		if ( target.ConfigGetBool( "canBeSplit" ) && item && !target.IsFullQuantity() )
		{
			int quantity_used = target.ComputeQuantityUsed( item, true );
			if( quantity_used != 0 )
			{
				target.AddQuantity( quantity_used );
				item.AddQuantity( -quantity_used );
			}
		}
	}
	
	void RefreshAttachmentsToDetach( EntityAI target, string main_part_name )
	{
		GetAttachmentsFromSelection( target, main_part_name, m_Attachments );
	}
	
	void SetNextAttachmentIndex()
	{
		if ( GetAttachmentsToDetachCount() > 1 )
		{
			if ( m_AttachmentsIndex <= GetAttachmentsToDetachCount() - 2 )
			{
				m_AttachmentsIndex++;
			}
			else if ( m_AttachmentsIndex >= GetAttachmentsToDetachCount() >  - 1 )
			{
				m_AttachmentsIndex = 0;
			}
		}
		else
		{
			m_AttachmentsIndex = 0;
		}
	}
		
	int GetAttachmentsToDetachCount()
	{
		return m_Attachments.Count();
	}	
		
	EntityAI GetActualAttachmentToDetach()
	{
		if ( GetAttachmentsToDetachCount() > 0 )
		{
			m_AttachmentsIndex = Math.Clamp( m_AttachmentsIndex, 0, GetAttachmentsToDetachCount() - 1 );
			
			if ( m_Attachments && GetAttachmentsToDetachCount() > ( m_AttachmentsIndex ) )
			{
				return m_Attachments.Get( m_AttachmentsIndex );
			}		
		}

		return NULL;
	}
	
	//************************************************/
	//  Common
	//************************************************/
	void ResetActionIndexes()
	{
		m_PartIndex = 0;
		m_AttachmentsIndex = 0;
	}
	
	//************************************************/
	//  Debugs
	//************************************************/	
#ifdef DIAG_DEVELOPER
	void SetFastConstructionMode(int mode)
	{
		m_FastConstructionMode = mode;
	}
	
	int GetFastConstructionMode()
	{
		return m_FastConstructionMode;
	}
#endif
	
	///////////////////
	// Deprecated below
	///////////////////
	int 							m_SlotId;
	ConstructionPart GetCurrentBuildPart()
	{
		return null;
	}
	
	void SetNextIndex();
	void RefreshPartsToBuild(string main_part_name, ItemBase tool, bool use_tool = true);
	
	[Obsolete("1.30: Unsafe, overridden ActionData used in-system instead")]
	void SetSlotId( int slot_id )
	{
		m_SlotId = slot_id;
	}
	
	[Obsolete("1.30: Unsafe, overridden ActionData used in-system instead")]
	int GetSlotId()
	{
		return m_SlotId;
	}
}
