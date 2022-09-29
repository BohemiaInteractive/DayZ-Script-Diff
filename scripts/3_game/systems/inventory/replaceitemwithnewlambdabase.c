/**@class		ReplaceItemWithNewLambdaBase
 * @brief		base class for transformation operations (creating one item from another)
 **/
class ReplaceItemWithNewLambdaBase
{
	EntityAI m_OldItem;
	string m_NewItemType;
	protected ref InventoryLocation m_OldLocation;
	protected ref InventoryLocation m_NewLocation;
	protected bool m_RemoveFromLocationPassed = false;
	private bool m_RemoveNetworkObjectInfoPassed = false;

	void ReplaceItemWithNewLambdaBase(EntityAI old_item, string new_item_type)
	{
		m_OldItem = old_item;
		m_NewItemType = new_item_type;
	}

	void OverrideNewLocation(InventoryLocation newLocation)
	{
		m_NewLocation = newLocation;
	}
	
	void VerifyItemTypeBySlotType() {}

	protected bool WantCreateNewEntity()
	{
		return m_NewLocation && m_NewItemType != string.Empty;
	}

	protected bool CanExecuteLambda()
	{
		if (m_OldItem)
			if (GameInventory.LocationCanRemoveEntity(m_OldLocation))
				//if (WantCreateNewEntity() GameInventory.LocationTestAddEntityType(m_NewItemType, true, true, false, true))
					return true;
		return false;
	}

	/**@fn		PrepareLocations
	 * @brief	Step A. - prepare inventory locations
	 **/
	protected bool PrepareLocations()
	{
		hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step A) Prepare inventory locations, old_item=" + m_OldItem);
		m_OldLocation = new InventoryLocation;
		if (m_OldItem.GetInventory().GetCurrentInventoryLocation(m_OldLocation)) // A.1) store old location
		{
			if (m_NewLocation == null)
			{
				m_NewLocation = new InventoryLocation;
				m_NewLocation.CopyLocationFrom(m_OldLocation, true);	// A.2) prepare new location from old
				
				//setting proper location type for ground pos
				if (!m_NewLocation.GetParent())
				{
					vector m4[4];
					Math3D.MatrixIdentity4(m4);
					m4[3] = m_NewLocation.GetPos();
					string path = "" + CFG_VEHICLESPATH + " " + m_NewItemType + " inherit_rotation";
					bool keep_rotation = GetGame().ConfigIsExisting(path) && GetGame().ConfigGetInt(path) > 0;
					
					if (m_OldLocation.GetType() == InventoryLocationType.GROUND && keep_rotation)
					{
						float dir[4];
						m_OldLocation.GetDir(dir);
						m_NewLocation.SetGroundEx(null,m_NewLocation.GetPos(),dir);
					}
					else
					{
						m_NewLocation.SetGround(null,m4);
					}
				}
			}
			return true;
		}
		else
			Error("[inv] ReplaceItemWithNewLambdaBase Step A.1) failed to get old_item inventory location");
		return false;
	}

	/**@fn		RemoveOldItemFromLocation
	 * @brief	Step B. - free location for new item
	 * @NOTE	this operation does not delete the object, only removes it from inventory location
	 **/
	protected void RemoveOldItemFromLocation()
	{
		if (!GameInventory.LocationRemoveEntity(m_OldLocation)) // B) remove entity from old inventory location (making it free for new item)
		{
			Error("[inv] ReplaceItemWithNewLambdaBase Step B) failed to remove old_item rom current inventory location");
			m_RemoveFromLocationPassed = false;
		}
		Print("[inv] ReplaceItemWithNewLambdaBase Step B) remove OK, loc=" + InventoryLocation.DumpToStringNullSafe(m_OldLocation));
		m_RemoveFromLocationPassed = true;
	}
	protected void UndoRemoveOldItemFromLocation()
	{
		if (!GameInventory.LocationAddEntity(m_OldLocation)) // B) undo
			Error("[inv] ReplaceItemWithNewLambdaBase Step B) failed to undo remove");
		Print("[inv] ReplaceItemWithNewLambdaBase Step B) undo remove OK, loc=" + InventoryLocation.DumpToStringNullSafe(m_OldLocation));
	}

	/**@fn		RemoveNetworkObjectInfo
	 * @brief	Step C. - remove network part of the object
	 * @NOTE	this operation does not delete the object, only removes its network part (and deletes it on client)
	 **/
	protected void RemoveNetworkObjectInfo()
	{
		GetGame().RemoteObjectTreeDelete(m_OldItem); // C) this forces server to send DeleteObject Message to client. This is needed for preserving the appearance of network operations on client (so that DeleteObject(old) arrives before CreateVehicle(new)). @NOTE: this does not delete the object on server, only it's network representation.
		// @NOTE: the item is not deleted right now on server, but rather after copying the properties in Step E)
		m_RemoveNetworkObjectInfoPassed = true;
	}
	protected void UndoRemoveNetworkObjectInfo()
	{
		GetGame().RemoteObjectTreeCreate(m_OldItem);
	}

	/**@fn		CreateNewEntity
	 * @brief	Step D. - create new entity (LOCAL) with specified type
	 *
	 * @NOTE: if (!m_NewLocation || m_NewItemType.Empty) ==> this function does not create a new entity
	 **/
	protected EntityAI CreateNewEntity()
	{
		if (WantCreateNewEntity())
		{
			VerifyItemTypeBySlotType();
			EntityAI new_item;
			
			switch (m_NewLocation.GetType())
			{
				case InventoryLocationType.GROUND:
					new_item = EntityAI.Cast(GetGame().CreateObjectEx(m_NewItemType,m_NewLocation.GetPos(),ECE_PLACE_ON_SURFACE|ECE_LOCAL));
					string path = "" + CFG_VEHICLESPATH + " " + m_NewItemType + " inherit_rotation";
					bool keep_rotation = GetGame().ConfigIsExisting(path) && GetGame().ConfigGetInt(path) > 0;
					if (keep_rotation)
					{
						new_item.SetOrientation(m_OldItem.GetOrientation()); //this one actually works...debug InventoryLocation
					}
				break;
				case InventoryLocationType.ATTACHMENT:
					// forces rawlocation in C++ to make location Valid
					m_NewLocation.SetAttachment(m_NewLocation.GetParent(), null, m_NewLocation.GetSlot());
					new_item = GameInventory.LocationCreateEntity(m_NewLocation, m_NewItemType, ECE_OBJECT_SWAP, RF_NONE);
				break;
				default:
					new_item = GameInventory.LocationCreateLocalEntity(m_NewLocation, m_NewItemType, ECE_OBJECT_SWAP, RF_NONE);
				break;
			}
			
			hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step D) Created new new_item=" + new_item);
			if (new_item)
			{
				return new_item;
			}
			else
			{							
				return null;

				/*InventoryLocation backupLocation = new InventoryLocation;
				vector mtx[4];
				Math3D.MatrixIdentity4(mtx);
				mtx[3] = m_OldItem.GetPosition();
				backupLocation.SetGround(null, mtx);
				hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step D) plan B - creating=" + m_NewItemType + " at bkp loc=" + backupLocation.DumpToString() + ", but failed");
				new_item = GameInventory.LocationCreateLocalEntity(backupLocation, m_NewItemType,ECE_OBJECT_SWAP,RF_NONE); // create LOCAL new one on ground
				if (!new_item)
				{
					Error("[inv] ReplaceItemWithNewLambdaBase Step D) plan B - wanted to create=" + m_NewItemType + " at bkp loc=" + backupLocation.DumpToString() + ", but failed");
					return null;
				}
				return new_item;*/
			}
		}

		// user did not asked for new entity
		return null;
	}

	/**@fn		CopyOldPropertiesToNew
	 * @brief	Step E. copy properties from old object to the created one
	 *
	 * @NOTE: This is supposed to be overriden in derived classes
	 **/
	void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step E) Copying props " + old_item + " --> " + new_item);
	}

	/**@fn		DeleteOldEntity
	 * @brief	Step F. - deletes physically old item
	 **/
	protected void DeleteOldEntity()
	{
		hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step F) delete old item=" + m_OldItem);
		GetGame().ObjectDelete(m_OldItem);
	}
	
	/**@fn		CreateNetworkObjectInfo
	 * @brief	Step G. - create NetworkObjectInfo for new item
	 *
	 * @NOTE: new_item can be null if the lambda did not create any item (intentionaly)
	 **/
	protected void CreateNetworkObjectInfo(EntityAI new_item)
	{
		hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step G) CreateNetworkObjectInfo =" + new_item);
		if (new_item)
			GetGame().RemoteObjectTreeCreate(new_item); // G) this forces server to send CreateVehicle Message to client. This is needed for preserving the appearance of network operations on client (so that DeleteObject(old) arrives before CreateVehicle(new)). @NOTE: this does not delete the object on server, only it's network representation.
	}

	/**@fn		OnSuccess
	 * @brief	Step H. - notification on finish
	 *
	 * @NOTE: new_item can be null if the lambda did not create any item (intentionaly)
	 **/
	protected void OnSuccess(EntityAI new_item)
	{
		hndDebugPrint("[inv] ReplaceItemWithNewLambdaBase Step H) OnSuccess=" + new_item);
	}

	/**@fn		OnAbort
	 * @brief	Step Out - notification on abort
	 *
	 * @NOTE: new_item can be null if the lambda did not create any item (intentionaly)
	 **/
	protected void OnAbort()
	{
		Print("Error [inv] ReplaceItemWithNewLambdaBase OnAbort");
	}

	void Execute(HumanInventoryWithFSM fsm_to_notify = null)
	{
		int t = GetGame().GetTime();
		hndDebugPrint("[syncinv] t=" + t + " lambda.Execute start ");

		// A) init
		bool prepared = PrepareLocations();

		if (prepared && CanExecuteLambda())
		{
			// B) rm old (and delete on client)
			RemoveOldItemFromLocation();
			if (!m_RemoveFromLocationPassed)
			{
				Error("[inv] ReplaceItemWithNewLambdaBase Step B) ABORT - failed while rm old item from loc=" + InventoryLocation.DumpToStringNullSafe(m_OldLocation));

				if (fsm_to_notify)
					fsm_to_notify.ProcessHandAbortEvent(new HandEventHumanCommandActionAborted(fsm_to_notify.GetManOwner()));
				OnAbort();
				return;
			}

			// C) rm NetworkObjectInfo from old item (+ send delete object tree to clients)
			RemoveNetworkObjectInfo();

			// D) mk new in place of old
			EntityAI new_item = CreateNewEntity();
			if (WantCreateNewEntity() && new_item == null)
			{
				//Error("[inv] ReplaceItemWithNewLambdaBase Step D) ABORT - wanted to create=" + m_NewItemType + " at loc=" + m_NewLocation.DumpToString() + ", but failed");
				Print("Error [inv] ReplaceItemWithNewLambdaBase Step D) ABORT - wanted to create=" + m_NewItemType + " at loc=" + InventoryLocation.DumpToStringNullSafe(m_NewLocation) + ", but failed");
				if (m_RemoveFromLocationPassed)
					UndoRemoveOldItemFromLocation();
				if (m_RemoveNetworkObjectInfoPassed)
					UndoRemoveNetworkObjectInfo();

				OnAbort();
				if (fsm_to_notify)
					fsm_to_notify.ProcessHandAbortEvent(new HandEventHumanCommandActionAborted(fsm_to_notify.GetManOwner()));
				return;
			}

			// E) cpy props
			CopyOldPropertiesToNew(m_OldItem, new_item);

			// F) del old
			DeleteOldEntity();

			// G) mk new NetworkObjectInfo
			CreateNetworkObjectInfo(new_item);

			// H) notification
			OnSuccess(new_item);
		}
		else
		{
			Print("[syncinv] warning, lambda cannot be executed, skipping!");
		}
		int te = GetGame().GetTime();
		int dt = te - t;
		hndDebugPrint("[syncinv] te=" + te + " lambda.Execute end, exec time=" + dt);
	}

	string DumpToString()
	{
		string s = "{ old=" + m_OldItem + " newType=" + m_NewItemType + "}";
		return s;
	}
};


class ItemInventory : GameInventory
{
};
