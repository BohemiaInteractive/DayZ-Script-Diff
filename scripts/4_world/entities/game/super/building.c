class BuildingLockData
{
	int m_DoorIndex = -1;
	string m_SlotName;
	EntityAI m_LockEntity;
}

class BuildingBase : Building
{
	const string ATTACHMENT_SLOT_COMBINATION_LOCK = "Att_CombinationLock";
	const string ATTACHMENT_SLOT_CODE_LOCK 	= "Att_CodeLock";
	
	static ref map<typename, ref TInputActionMap> m_BuildingTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool	m_ActionsInitialize;
	protected ref ConstructionBasic 	m_Construction;
	
	void BuildingBase()
	{
		m_ActionsInitialize = false;
	}
	
	override protected ConstructionBasic CreateConstructionComponent()
	{
		m_Construction = new Rebuilding(this);
		return m_Construction;
	}
	
	override protected void RegisterConstructionSyncVariables()
	{
		super.RegisterConstructionSyncVariables();
		
		if (Rebuilding.Cast(m_Construction))
		{
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts1");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts2");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts3");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts4");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts5");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts6");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts7");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts8");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts9");
			RegisterNetSyncVariableInt("m_Construction.m_SyncParts10");
		}
	}
	
	override ConstructionBasic GetConstructionBasic()
	{
		return m_Construction;
	}
	
	override bool CanDisplayAttachmentCategory( string category_name )
	{
		if (!super.CanDisplayAttachmentCategory(category_name))
			return false;
		
		if (Rebuilding.Cast(m_Construction) && !GetConstructionBasic().CanDisplayAttachmentCategoryConstruction(category_name))
			return false;
		
		//now for the proximity checks, if applicable:
		bool hidden = false;
		DayZPlayer player = g_Game.GetPlayer();
		if (player)
		{
			TypeAttachmentGroupData attGroupData = GetTypeAttachmentGroupInfoHolder().GetAttGroupDataByClassName(category_name);
			if (!attGroupData)
				return false;
			
			vector playerPos = player.GetPosition();
			InstancedAttachmentGroupDataBuilding instData = InstancedAttachmentGroupDataBuilding.Cast(GetInstancedAttGroupData());
			if (instData && attGroupData.m_SelectionName != "")
			{
				//radial check
				vector refPos;
				float radius;
				if (instData.GetValidAttGroupPosition(category_name,refPos,radius))
				{
					//Debug.DrawSphere(refPos,0.15,COLOR_YELLOW,ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE);
					
					float distance = vector.Distance(refPos, playerPos);
					hidden |= distance > radius;
				}
				
				//box check
				vector minPos, maxPos;
				if (instData.GetValidAttGroupBox(category_name,minPos,maxPos))
				{
					hidden |= minPos[0] > playerPos[0] || maxPos[0] < playerPos[0];
					hidden |= minPos[1] > playerPos[1] || maxPos[1] < playerPos[1];
					hidden |= minPos[2] > playerPos[2] || maxPos[2] < playerPos[2];
				}
			}
		}
		
		return !hidden;
	}
		
	void InitializeActions()
	{
		m_InputActionMap = m_BuildingTypeActionsMap.Get( this.Type() );
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_BuildingTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	void SetActions()
	{
		//AddAction();
	}
	
	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if (!action)
		{
			Debug.LogError("Action " + actionName + " dosn't exist!");
			return;
		}		
		
		typename ai = action.GetInputType();
		if (!ai)
		{
			m_ActionsInitialize = false;
			return;
		}
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
#ifdef ENABLE_LOGGING
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action" );
		}
#endif
		action_array.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if (action_array)
		{
			action_array.RemoveItem(action);
		}
	}
		
	DigitalCodeLock GetCodeLock(string slotName)
	{
		DigitalCodeLock codeLock;
		EntityAI slotEntity = FindAttachmentBySlotName(slotName);
		if (Class.CastTo(codeLock, slotEntity))
			return codeLock;
				
		return null;
	}
	
	CombinationLock GetCombinationLock(string slotName)
	{
		CombinationLock combinationLock;
		EntityAI slotEntity = FindAttachmentBySlotName(slotName);
		if (Class.CastTo(combinationLock, slotEntity))
			return combinationLock;
		
		return null;
	}
		
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		
		ItemBase attachmentIB = ItemBase.Cast(attachment);
		if (attachmentIB && attachmentIB.IsExternalLockType())
		{
			string slotName = InventorySlots.GetSlotName(slotId);
			int doorIndex = -1;
			
			AdditionalDoorInfosHolder doorInfoHolder = m_AdditionalDoorInfosMap.Get(GetType());
			if (doorInfoHolder)
			{
				map<int, ref AdditionalDoorInfo> doorsInfoMap = doorInfoHolder.GetDoorInfoMap();
				if (doorsInfoMap && doorsInfoMap.Count() > 0)
				{
					bool skipDoorInfoCheck = false;
					foreach(int doorInfoIndex, AdditionalDoorInfo doorInfo: doorsInfoMap)
					{
						if (!doorInfo.m_RelatedInventorySlotNames)
						{
							skipDoorInfoCheck = true;
							break;
						}
						
						foreach(string relatedSlotName: doorInfo.m_RelatedInventorySlotNames)
						{
							if (relatedSlotName == slotName)
							{
								string cLSlotName = slotName;
								//! Check if a combination lock attachment is already attached to this doors related attachment slots
								if (HasConflictingDoorAttachment(relatedSlotName, ATTACHMENT_SLOT_CODE_LOCK, ATTACHMENT_SLOT_COMBINATION_LOCK))
									return false;
								
								//! Check if a code lock attachment is already attached to this doors related attachment slots
								if (HasConflictingDoorAttachment(relatedSlotName, ATTACHMENT_SLOT_COMBINATION_LOCK, ATTACHMENT_SLOT_CODE_LOCK))
									return false;
								
								doorIndex = doorInfoIndex;
								break;
							}
						}
						
						if (doorIndex != -1 || skipDoorInfoCheck)
							break;
					}
												
					if (doorIndex > -1)
					{
	  					if (IsDoorOpen(doorIndex))
							return false;
					}
				}
			}
		}
		
		return true;
	}
	
	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;
		
		string slotName = InventorySlots.GetSlotName(slot_id);		
		if (HasConflictingDoorAttachment(slotName, ATTACHMENT_SLOT_CODE_LOCK, ATTACHMENT_SLOT_COMBINATION_LOCK))
			return false;
		
		if (HasConflictingDoorAttachment(slotName, ATTACHMENT_SLOT_COMBINATION_LOCK, ATTACHMENT_SLOT_CODE_LOCK))
			return false;
		
		return true;
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;
		
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(attachment);
		if (codeLock && codeLock.IsLocked())
			return false;
		
		CombinationLock combinationLock = CombinationLock.Cast(attachment);
		if (combinationLock && combinationLock.IsLocked())
			return false;
		
		return true;
	}
		
	bool IsLocked(int doorIndex)
	{
		AdditionalDoorInfo doorInfo = GetDoorInfo(doorIndex);
		if (doorInfo && doorInfo.m_RelatedInventorySlotNames)
		{
			foreach(string relatedSlotName: doorInfo.m_RelatedInventorySlotNames)
			{
				EntityAI slotEntity = FindAttachmentBySlotName(relatedSlotName);
				if (!slotEntity)
					continue;
				
				DigitalCodeLock codeLock;
				if (Class.CastTo(codeLock, slotEntity) && codeLock.IsLocked())
					return false;
				
				CombinationLock combinationLock;
				if (Class.CastTo(combinationLock, slotEntity) && combinationLock.IsLocked())
					return false;
			}
		}
				
		return false;
	}
	
	override bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (!super.CanDoorBeOpened(params))
			return false;
		
		int doorIndex = params.m_DoorIndex;
		AdditionalDoorInfo doorInfo = GetDoorInfo(doorIndex);
		if (doorInfo && doorInfo.m_RelatedInventorySlotNames)
		{
			foreach(string relatedSlotName: doorInfo.m_RelatedInventorySlotNames)
			{
				EntityAI slotEntity = FindAttachmentBySlotName(relatedSlotName);
				if (!slotEntity)
					continue;
				
				DigitalCodeLock codeLock;
				if (Class.CastTo(codeLock, slotEntity) && codeLock.IsLocked())
					return false;
				
				CombinationLock combinationLock;
				if (Class.CastTo(combinationLock, slotEntity) && combinationLock.IsLocked())
					return false;
			}
		}
		
		return true;
	}
	
	//! checks against player relative to defined door direction, some doors can only be operated from one side
	bool DoorsDirectionalCheck(PlayerBase player, int componentIdx)
	{
		string memPointPos = GetDoorInfo(GetDoorIndex(componentIdx)).m_InteractLimitingPositionPoint;
		string memPointDir = GetDoorInfo(GetDoorIndex(componentIdx)).m_InteractLimitingDirPoint;
		
		return MiscGameplayFunctions.MemPointDirectionalCheck(player, this, memPointPos, memPointDir);
	}
	
	protected bool HasConflictingDoorAttachment(string slotName, string relatedSlotName, string conflictingSlotName)
	{
		string cLSlotName = slotName;
		if (slotName.IndexOf(relatedSlotName) > -1)
		{
			cLSlotName.Replace(relatedSlotName, "");
			cLSlotName = conflictingSlotName + cLSlotName;
			if (FindAttachmentBySlotName(cLSlotName))
				return true;
		}
		
		return false;
	}
	
	override protected InstancedAttachmentGroupDataBase GetInstancedAttGroupData()
	{
		return m_InstancedAttGroupData;
	}
	
	override void OnDoorOpenStart(DoorStartParams params)
	{
		super.OnDoorOpenStart(params);
		
		if (m_Construction)
			m_Construction.OnConstructionDoorOpenStart(params);
	}
	
	override void OnDoorCloseStart(DoorStartParams params)
	{
		super.OnDoorCloseStart(params);
		
		if (m_Construction)
			m_Construction.OnConstructionDoorCloseStart(params);
	}
};

class House : BuildingBase
{
	void House()
	{
	}
};

typedef House BuildingSuper;
