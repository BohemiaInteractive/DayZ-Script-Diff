#ifdef FEATURE_NETWORK_RECONCILIATION
class Person extends Pawn
{
	//! ??? 
};
#endif

#ifdef FEATURE_NETWORK_RECONCILIATION
class Man extends Person
#else
class Man extends EntityAI
#endif
{
	//! Returns player's input interface
	proto native UAInterface GetInputInterface();
	//! Returns player's identity
	proto native PlayerIdentity GetIdentity();
	//! Returns vehicle which this Man object is driving. If this Man object isn't as driver of any vehicle it will return NULL.
	proto native EntityAI GetDrivingVehicle();

	proto native owned string GetCurrentWeaponMode();

	//! Set speech restriction
	proto native void SetSpeechRestricted(bool state);
	//! Check if player has resctricted speech
	proto native bool IsSpeechRestricted();

	//! Texture that is used on the Man's face and hands
	proto native void SetFaceTexture(string texture_name);
	//! Material that is used on the Man's face and hands
	proto native void SetFaceMaterial(string material_name);

	proto native bool IsSoundInsideBuilding();
	proto native bool IsCameraInsideVehicle();

	proto native owned string GetMasterAttenuation();
	proto native void SetMasterAttenuation(string masterAttenuation);

	void Man()
	{
		SetFlags(EntityFlags.TOUCHTRIGGERS, false);
	}

	override bool IsMan()
	{
		return true;
	}

	override bool IsHealthVisible()
	{
		return false;
	}

	override bool HasFixedActionTargetCursorPosition()
	{
		return true;
	}

	bool IsUnconscious();
	//! uncon command might be still in progress but state already changed (for simple checks only - eg.: UI)
	bool IsUnconsciousStateOnly();

	int GetPlayerState()
	{
		if (IsAlive())
			return EPlayerStates.ALIVE;
		
		return EPlayerStates.DEAD;
	}
	
	void AddItemToDelete(EntityAI item);

	///@{ inventory
	//! Returns player's inventory
	proto native HumanInventory GetHumanInventory();
	//Called when an item is removed from the cargo of this item
	protected ref ScriptInvoker		m_OnItemAddedToHands;
	//Called when an item is moved around in the cargo of this item
	protected ref ScriptInvoker		m_OnItemRemovedFromHands;
	
	ScriptInvoker GetOnItemAddedToHands()
	{
		if (!m_OnItemAddedToHands)
			m_OnItemAddedToHands = new ScriptInvoker();
		
		return m_OnItemAddedToHands;
	}
	
	
	ScriptInvoker GetOnItemRemovedFromHands()
	{
		if( !m_OnItemRemovedFromHands )
			m_OnItemRemovedFromHands = new ScriptInvoker;
		
		return m_OnItemRemovedFromHands;
	}
	
	void EEItemIntoHands(EntityAI item)
	{
		SetWeightDirty();
		if( m_OnItemAddedToHands )
			m_OnItemAddedToHands.Invoke( item, this );
	}
	
	void EEItemOutOfHands(EntityAI item)
	{
		SetWeightDirty();
		if( m_OnItemRemovedFromHands )
			m_OnItemRemovedFromHands.Invoke( item, this );
	}

	///@{ drop juncture
	bool JunctureDropEntity (notnull EntityAI item)
	{
		return DropEntityImpl(InventoryMode.JUNCTURE, this, item);
	}

	override bool PredictiveDropEntity (notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveDropEntity input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			return JunctureDropEntity(item);
		else
			return DropEntityImpl(InventoryMode.PREDICTIVE, this, item);
	}

	override bool LocalDropEntity (notnull EntityAI item)
	{
		return DropEntityImpl(InventoryMode.LOCAL, this, item);
	}

	override bool ServerDropEntity (notnull EntityAI item)
	{
		return DropEntityImpl(InventoryMode.SERVER, this, item);
	}

	protected bool DropEntityImpl (InventoryMode mode, notnull EntityAI owner, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::DropEntity(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetHumanInventory().DropEntity(mode, owner, item);
		UpdateInventoryMenu();
		return code;
	}
	///@} drop juncture

	override bool CanDropEntity (notnull EntityAI item) { return true; }

	void OnItemInHandsChanged () { }

	bool NeedInventoryJunctureFromServer (notnull EntityAI item, EntityAI currParent, EntityAI newParent) { return false; }

	///@{ hand juncture
	void JunctureTakeEntityToHands (notnull EntityAI item)
	{
		TakeEntityToHandsImpl(InventoryMode.JUNCTURE, item);
	}

	void PredictiveTakeEntityToHands (EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityToHands input data not sent yet, cannot allow another input action");
			return;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			JunctureTakeEntityToHands(item);
		else
			TakeEntityToHandsImpl(InventoryMode.PREDICTIVE, item);
	}

	void LocalTakeEntityToHands (EntityAI item)
	{
		TakeEntityToHandsImpl(InventoryMode.LOCAL, item);
	}
	
	void ServerTakeEntityToHands (EntityAI item)
	{
		TakeEntityToHandsImpl(InventoryMode.SERVER, item);
	}

	void TakeEntityToHandsImpl (InventoryMode mode, EntityAI item)
	{
		if (!GetGame().IsDedicatedServer() )
		{
			InventoryLocation il = new InventoryLocation;
			il.SetHands(this, item);
			//GetInventory().AddInventoryReservationEx(item, il ,GameInventory.c_InventoryReservationTimeoutShortMS);
		}
			
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2Hands(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		EntityAI itemInHands = GetHumanInventory().GetEntityInHands();
		
		InventoryLocation src_item = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(src_item))
		{
			if (itemInHands == null)
			{	
				InventoryLocation hand_dst = new InventoryLocation;
				hand_dst.SetHands(this, item);
				GetHumanInventory().TakeToDst(mode, src_item, hand_dst);
			}
			else if (GetHumanInventory().CanSwapEntitiesEx(itemInHands, item))
				GetInventory().SwapEntities(mode, itemInHands, item);
			UpdateInventoryMenu();
		}
	}
	///@} hand juncture

	void LocalDestroyEntityInHands ()
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Destroy IH=" + GetHumanInventory().GetEntityInHands());
		GetHumanInventory().LocalDestroyEntity(GetHumanInventory().GetEntityInHands());
		UpdateInventoryMenu();
	}
	
	//! ToDo: Old system method. Might should be adjusted to new system at some point
	void PredictiveMoveItemFromHandsToInventory ()
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Stash IH=" + GetHumanInventory().GetEntityInHands());
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] PredictiveMoveItemFromHandsToInventory input data not sent yet, cannot allow another input action");
			return;
		}
		
		InventoryMode invMode = InventoryMode.PREDICTIVE;
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		
		if (NeedInventoryJunctureFromServer(entityInHands, this, this))
			invMode = InventoryMode.JUNCTURE;

		//! returns item to previous location, if available
		if (entityInHands.m_OldLocation && entityInHands.m_OldLocation.IsValid())
		{
			InventoryLocation invLoc = new InventoryLocation;
			entityInHands.GetInventory().GetCurrentInventoryLocation(invLoc);
			
			//! Check if old strored location is somewhere on this player
			if (entityInHands.m_OldLocation.GetParent() && entityInHands.m_OldLocation.GetParent().GetHierarchyRootPlayer())
			{
				GetHumanInventory().ClearInventoryReservation(entityInHands, entityInHands.m_OldLocation);
				if (GetHumanInventory().LocationCanMoveEntity(invLoc, entityInHands.m_OldLocation))
				{
					EntityAI oldLocEntity = GetHumanInventory().LocationGetEntity(entityInHands.m_OldLocation);
					if (!oldLocEntity && GetHumanInventory().TakeToDst(invMode, invLoc, entityInHands.m_OldLocation))
					{
						UpdateInventoryMenu();
						return;
					}
					else //! This should not happen after clearing inventory reservation but just in case handle also getting a new location if the old location is obscured by an item.
					{
						InventoryLocation newLocation = new InventoryLocation;
						if (GetHumanInventory().FindFreeLocationFor(entityInHands, FindInventoryLocationType.CARGO, newLocation))
						{
							if (GetHumanInventory().TakeToDst(invMode, invLoc, newLocation))
							{
								UpdateInventoryMenu();
								return;
							}
						}
					}
				}
			}
		}

		GetHumanInventory().TakeEntityToInventory(invMode, FindInventoryLocationType.ATTACHMENT | FindInventoryLocationType.CARGO, GetHumanInventory().GetEntityInHands());		
		UpdateInventoryMenu();
	}

	///@{ !hand -> !hand replace
	protected bool ReplaceItemWithNewImpl(InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Replace !HND lambda=" + lambda.DumpToString());
		bool code = GetHumanInventory().ReplaceItemWithNew(mode, lambda);
		UpdateInventoryMenu();
		return code;
	}

	bool LocalReplaceItemWithNew(ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemWithNewImpl(InventoryMode.LOCAL, lambda);
	}

	bool ServerReplaceItemWithNew(ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemWithNewImpl(InventoryMode.SERVER, lambda);
	}
	///@} !hand -> !hand replace
	
	///@{ !hand replace -> hand
	protected bool ReplaceItemElsewhereWithNewInHandsImpl (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Replace !HND->HND lambda=" + lambda.DumpToString());
		bool code = GetHumanInventory().ReplaceItemElsewhereWithNewInHands(mode, lambda);
		UpdateInventoryMenu();
		return code;
	}

	bool LocalReplaceItemElsewhereWithNewInHands (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemElsewhereWithNewInHandsImpl(InventoryMode.LOCAL, lambda);
	}

	bool ServerReplaceItemElsewhereWithNewInHands (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemElsewhereWithNewInHandsImpl(InventoryMode.SERVER, lambda);
	}
	///@} !hand replace -> hand

	///@{ hand replace
	protected bool ReplaceItemInHandsWithNewImpl (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Replace HND->HND lambda=" + lambda.DumpToString());
		bool code = GetHumanInventory().ReplaceItemInHandsWithNew(mode, lambda);
		UpdateInventoryMenu();
		return code;
	}

	bool LocalReplaceItemInHandsWithNew (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewImpl(InventoryMode.LOCAL, lambda);
	}

	bool ServerReplaceItemInHandsWithNew (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewImpl(InventoryMode.SERVER, lambda);
	}
	///@} hand replace
	
	///@{ hand replace2
	protected bool ReplaceItemInHandsWithNewElsewhereImpl (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Replace HND->elsewhere lambda=" + lambda.DumpToString());
		bool code = GetHumanInventory().ReplaceItemInHandsWithNewElsewhere(mode, lambda);
		UpdateInventoryMenu();
		return code;
	}
	
	bool LocalReplaceItemInHandsWithNewElsewhere (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewElsewhereImpl(InventoryMode.LOCAL, lambda);
	}
	
	bool ServerReplaceItemInHandsWithNewElsewhere (ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewElsewhereImpl(InventoryMode.SERVER, lambda);
	}
	///@} hand replace2

	///@{ to inv juncture
	bool JunctureTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		return TakeEntityToInventoryImpl(InventoryMode.JUNCTURE, flags, item);
	}

	override bool PredictiveTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityToInventory input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			return JunctureTakeEntityToInventory(flags, item);
		else
			return TakeEntityToInventoryImpl(InventoryMode.PREDICTIVE, flags, item);
	}

	override bool LocalTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		return TakeEntityToInventoryImpl(InventoryMode.LOCAL, flags, item);
	}

	override bool ServerTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		return TakeEntityToInventoryImpl(InventoryMode.SERVER, flags, item);
	}

	protected bool TakeEntityToInventoryImpl (InventoryMode mode, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2Inv(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetHumanInventory().TakeEntityToInventory(mode, flags, item);
		UpdateInventoryMenu();
		return code;
	}
	///@} to inv juncture

	///@{ to cgo juncture
	bool JunctureTakeEntityToCargo (notnull EntityAI item)
	{
		return TakeEntityToCargoImpl(InventoryMode.JUNCTURE, item);
	}

	override bool PredictiveTakeEntityToCargo (notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityToCargo input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			return JunctureTakeEntityToCargo(item);
		else
			return TakeEntityToCargoImpl(InventoryMode.PREDICTIVE, item);
	}

	override bool LocalTakeEntityToCargo (notnull EntityAI item)
	{
		return TakeEntityToCargoImpl(InventoryMode.LOCAL, item);
	}

	override bool ServerTakeEntityToCargo (notnull EntityAI item)
	{
		return TakeEntityToCargoImpl(InventoryMode.SERVER, item);
	}

	protected bool TakeEntityToCargoImpl (InventoryMode mode, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2Cgo(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetHumanInventory().TakeEntityToCargo(mode, item);
		UpdateInventoryMenu();
		return code;
	}
	///@} to cgo juncture

	///@{ as att juncture
	bool JunctureTakeEntityAsAttachment (notnull EntityAI item)
	{
		return TakeEntityAsAttachmentImpl(InventoryMode.JUNCTURE, item);
	}

	override bool PredictiveTakeEntityAsAttachment (notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityAsAttachment input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			return JunctureTakeEntityAsAttachment(item);
		else
			return TakeEntityAsAttachmentImpl(InventoryMode.PREDICTIVE, item);
	}

	override bool LocalTakeEntityAsAttachment (notnull EntityAI item)
	{
		return TakeEntityAsAttachmentImpl(InventoryMode.LOCAL, item);
	}

	override bool ServerTakeEntityAsAttachment (notnull EntityAI item)
	{
		return TakeEntityAsAttachmentImpl(InventoryMode.SERVER, item);
	}

	protected bool TakeEntityAsAttachmentImpl (InventoryMode mode, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2Att(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetHumanInventory().TakeEntityAsAttachment(mode, item);
		UpdateInventoryMenu();
		return code;
	}
	///@} as att juncture

	///@{ as att ex juncture
	bool JunctureTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		return TakeEntityAsAttachmentExImpl(InventoryMode.JUNCTURE, item, slot);
	}

	override bool PredictiveTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityAsAttachmentEx input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), this))
			return JunctureTakeEntityAsAttachmentEx(item, slot);
		else
			return TakeEntityAsAttachmentExImpl(InventoryMode.PREDICTIVE, item, slot);
	}

	override bool LocalTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		return TakeEntityAsAttachmentExImpl(InventoryMode.LOCAL, item, slot);
	}

	override bool ServerTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		return TakeEntityAsAttachmentExImpl(InventoryMode.SERVER, item, slot);
	}

	protected bool TakeEntityAsAttachmentExImpl (InventoryMode mode, notnull EntityAI item, int slot)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2AttEx(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetHumanInventory().TakeEntityAsAttachmentEx(mode, item, slot);
		UpdateInventoryMenu();
		return code;
	}
	///@} as att ex juncture

	///@{ swap juncture
	bool JunctureSwapEntities (notnull EntityAI item1, notnull EntityAI item2)
	{
		return SwapEntitiesImpl(InventoryMode.JUNCTURE, item1, item2);
	}

	bool PredictiveSwapEntities (notnull EntityAI item1, notnull EntityAI item2)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveSwapEntities input data not sent yet, cannot allow another input action");
			return false;
		}

		bool need_j1 = NeedInventoryJunctureFromServer(item1, item1.GetHierarchyParent(), item2.GetHierarchyParent());
		bool need_j2 = NeedInventoryJunctureFromServer(item2, item2.GetHierarchyParent(), item1.GetHierarchyParent());
		if (need_j1 || need_j2)
			return SwapEntitiesImpl(InventoryMode.JUNCTURE, item1, item2);
		else
			return SwapEntitiesImpl(InventoryMode.PREDICTIVE, item1, item2);
	}

	bool LocalSwapEntities (notnull EntityAI item1, notnull EntityAI item2)
	{
		return SwapEntitiesImpl(InventoryMode.LOCAL, item1, item2);
	}

	bool ServerSwapEntities (notnull EntityAI item1, notnull EntityAI item2)
	{
		return SwapEntitiesImpl(InventoryMode.SERVER, item1, item2);
	}

	protected bool SwapEntitiesImpl (InventoryMode mode, notnull EntityAI item1, notnull EntityAI item2)
	{
		bool code;
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::SwapImpl(" + typename.EnumToString(InventoryMode, mode) + ") item1=" + Object.GetDebugName(item1) + " item2=" + item2.GetDebugName(this));
		if (!GameInventory.CanSwapEntitiesEx(item1, item2))
			Error("[inv] (Man@" + this + ") SwapEntitiesImpl - cannot swap items!");

		code = GetHumanInventory().SwapEntities(mode, item1, item2);

		UpdateInventoryMenu();
		if (!code)
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + "  SwapEntitiesImpl - cannot swap or forceswap");
		return code;
	}
	///@} swap juncture

	///@{ ForceSwap juncture
	bool JunctureForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		return ForceSwapEntitiesImpl(InventoryMode.JUNCTURE, item1, item2, item2_dst);
	}

	bool PredictiveForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveForceSwapEntities input data not sent yet, cannot allow another input action");
			return false;
		}

		bool need_j1 = NeedInventoryJunctureFromServer(item1, item1.GetHierarchyParent(), item2.GetHierarchyParent());
		bool need_j2 = NeedInventoryJunctureFromServer(item2, item2.GetHierarchyParent(), item1.GetHierarchyParent());
		if (need_j1 || need_j2)
			return ForceSwapEntitiesImpl(InventoryMode.JUNCTURE, item1, item2, item2_dst);
		else
			return ForceSwapEntitiesImpl(InventoryMode.PREDICTIVE, item1, item2, item2_dst);
	}

	bool LocalForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		return ForceSwapEntitiesImpl(InventoryMode.LOCAL, item1, item2, item2_dst);
	}

	bool ServerForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		return ForceSwapEntitiesImpl(InventoryMode.SERVER, item1, item2, item2_dst);
	}

	protected bool ForceSwapEntitiesImpl (InventoryMode mode, notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		bool code = false;
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + "  ForceSwapImpl(" + typename.EnumToString(InventoryMode, mode) + ") item1=" + Object.GetDebugName(item1) + " item2=" + item2.GetDebugName(this));
    	code = GetHumanInventory().ForceSwapEntities(mode, item1, item2, item2_dst);

		UpdateInventoryMenu();
		if (!code)
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + "  ForceSwapEntitiesImpl - cannot Forceswap");
		return code;
	}
	///@} ForceSwap juncture

	///@{ to target inv juncture
	bool JunctureTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		return TakeEntityToTargetInventoryImpl(InventoryMode.JUNCTURE, target, flags, item);
	}

	override bool PredictiveTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " ::PredictiveTakeEntityToTargetInventory input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), target))
			return JunctureTakeEntityToTargetInventory(target, flags, item);
		else
			return TakeEntityToTargetInventoryImpl(InventoryMode.PREDICTIVE, target, flags, item);
	}
	
	bool PredictiveTakeOrSwapAttachment( notnull EntityAI item )
	{
		if( GetInventory().CanAddAttachment( item ) )
		{
			return PredictiveTakeEntityAsAttachment( item );
		}
		else
		{
			for( int i = 0; i < item.GetInventory().GetSlotIdCount(); i++ )
			{
				int slot_id = item.GetInventory().GetSlotId(i);
				EntityAI slot_item = GetInventory().FindAttachment( slot_id );
				if( slot_item && GetInventory().CanSwapEntitiesEx( item, slot_item ) )
				{
					return PredictiveSwapEntities(item, slot_item);
				}
			}
		}
		return false;
	}

	override bool LocalTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		return TakeEntityToTargetInventoryImpl(InventoryMode.LOCAL, target, flags, item);
	}

	protected bool TakeEntityToTargetInventoryImpl (InventoryMode mode, notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2TargetInv(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetInventory().TakeEntityToTargetInventory(mode, target, flags, item);
		UpdateInventoryMenu();
		return code;
	}
	///@} to target inv juncture

	///@{ to target cgo ex juncture
	bool JunctureTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		return TakeEntityToTargetCargoExImpl(InventoryMode.JUNCTURE, cargo, item, row, col);
	}

	override bool PredictiveTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::PredictiveTakeEntityToTargetCargoEx input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), cargo.GetCargoOwner()))
			return JunctureTakeEntityToTargetCargoEx(cargo, item, row, col);
		else
			return TakeEntityToTargetCargoExImpl(InventoryMode.PREDICTIVE, cargo, item, row, col);
	}

	override bool LocalTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		return TakeEntityToTargetCargoExImpl(InventoryMode.LOCAL, cargo, item, row, col);
	}
	
	override bool ServerTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		return TakeEntityToTargetCargoExImpl(InventoryMode.SERVER, cargo, item, row, col);
	}

	protected bool TakeEntityToTargetCargoExImpl (InventoryMode mode, notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2TargetCgoEx(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetInventory().TakeEntityToTargetCargoEx(mode, cargo, item, row, col);
		UpdateInventoryMenu();
		return code;
	}
	///@} to target cgo ex juncture

	///@{ to target cgo juncture
	bool JunctureTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		return TakeEntityToTargetCargoImpl(InventoryMode.JUNCTURE, target, item);
	}

	override bool PredictiveTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::PredictiveTakeEntityToTargetCargo input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), target))
			return JunctureTakeEntityToTargetCargo(target, item);
		else
			return TakeEntityToTargetCargoImpl(InventoryMode.PREDICTIVE, target, item);
	}

	override bool LocalTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		return TakeEntityToTargetCargoImpl(InventoryMode.LOCAL, target, item);
	}
	
	override bool ServerTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		if (IsAlive())
			return TakeEntityToTargetCargoImpl(InventoryMode.SERVER, target, item);
		else
			return TakeEntityToTargetCargoImpl(InventoryMode.SERVER, target, item);
	}

	protected bool TakeEntityToTargetCargoImpl (InventoryMode mode, notnull EntityAI target, notnull EntityAI item)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " Take2TargetCgo(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));

		bool code = GetInventory().TakeEntityToTargetInventory(mode, target, FindInventoryLocationType.CARGO, item);
  		UpdateInventoryMenu();
		return code;
	}
	///@} to target cgo juncture

	///@{ to target att ex juncture
	bool JunctureTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		return TakeEntityToTargetAttachmentExImpl(InventoryMode.JUNCTURE, target, item, slot);
	}

	override bool PredictiveTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::PredictiveTakeEntityToTargetAttachmentEx input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), target))
			return JunctureTakeEntityToTargetAttachmentEx(target, item, slot);
		else
			return TakeEntityToTargetAttachmentExImpl(InventoryMode.PREDICTIVE, target, item, slot);
	}

	override bool LocalTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		return TakeEntityToTargetAttachmentExImpl(InventoryMode.LOCAL, target, item, slot);
	}

	override bool ServerTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		return TakeEntityToTargetAttachmentExImpl(InventoryMode.SERVER, target, item, slot);
	}

	protected bool TakeEntityToTargetAttachmentExImpl (InventoryMode mode, notnull EntityAI target, notnull EntityAI item, int slot)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2TargetAtt(" + typename.EnumToString(InventoryMode, mode) + ") item=" + Object.GetDebugName(item));
		bool code = GetInventory().TakeEntityAsTargetAttachmentEx(mode, target, item, slot);
		UpdateInventoryMenu();
		return code;
	}
	///@} to target att ex juncture

	///@{ to target att juncture
	bool JunctureTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		return TakeEntityToTargetAttachmentImpl(InventoryMode.JUNCTURE, target, item);
	}

	override bool PredictiveTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeEntityToTargetAttachment input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(item, item.GetHierarchyParent(), target))
			return JunctureTakeEntityToTargetAttachment(target, item);
		else
			return TakeEntityToTargetAttachmentImpl(InventoryMode.PREDICTIVE, target, item);
	}

	override bool LocalTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		return TakeEntityToTargetAttachmentImpl(InventoryMode.LOCAL, target, item);
	}

	override bool ServerTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		return TakeEntityToTargetAttachmentImpl(InventoryMode.SERVER, target, item);
	}

	protected bool TakeEntityToTargetAttachmentImpl (InventoryMode mode, notnull EntityAI target, notnull EntityAI item)
	{
		InventoryLocation il = new InventoryLocation;
		if( target.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ATTACHMENT, il) )
		{
			return TakeEntityToTargetAttachmentExImpl(mode, target, item, il.GetSlot());
		}
		return false;
	}
	///@} to target att juncture

	///@{ to dst juncture
	bool JunctureTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		return TakeToDstImpl(InventoryMode.JUNCTURE, src, dst);
	}

	override bool PredictiveTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Print("[inv] " + GetSimulationTimeStamp() + " Man@" + this + " ::PredictiveTakeToDst input data not sent yet, cannot allow another input action");
			return false;
		}

		if (NeedInventoryJunctureFromServer(src.GetItem(), src.GetParent(), dst.GetParent()))
			return JunctureTakeToDst(src, dst);
		else
			return TakeToDstImpl(InventoryMode.PREDICTIVE, src, dst);
	}

	override bool LocalTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		return TakeToDstImpl(InventoryMode.LOCAL, src, dst);
	}

	override bool ServerTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		return TakeToDstImpl(InventoryMode.SERVER, src, dst);
	}

	protected bool TakeToDstImpl (InventoryMode mode, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS = " + GetSimulationTimeStamp() + " ::Take2Dst(" + typename.EnumToString(InventoryMode, mode) + ") src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
		bool code = GetHumanInventory().TakeToDst(mode, src, dst);
		UpdateInventoryMenu();
		return code;
	}
	///@} to dst juncture
	///@} inventory
	
	void JunctureDeleteItem(EntityAI item)
	{
		
	} 

	override bool CanBeSkinned()
	{
		return !GetIsFrozen();
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	void UpdateInventoryMenu();
	
	///@{ Stats
	//! Registers new stat type for this player.
	/*!
		\param[in] name The name of the stat type.
	*/
	proto native bool StatRegister( string name );
	//! Gets counter value of the specified stat type.
	/*!
		\param[in] name The name of the stat type.
	*/
	proto native float StatGet( string name );
	//! Gets counter value as string of the specified stat type.
	/*!
		\param[in]	name  The name of the stat type.
		\param[out] value A formatted string containing stat value.
	*/
	proto void StatGetCounter( string name, out string value );
	//! Gets counter value as formatted time string of the specified stat type.
	/*!
		\param[in]	name  The name of the stat type.
		\param[out] value A formatted string containing stat value.
	*/
	proto void StatGetAsTime( string name, out string value );
	//! Updates stat counter with given value.
	/*!
		\param[in] name  The name of the stat type.
		\param[in] value The specified value.
	*/
	proto native void StatUpdate( string name, float value );
	//! Updates stat counter by time.
	/*!
		Use this to measure play time.
		\param[in] name The name of the stat type.
	*/
	proto native void StatUpdateByTime( string name );
	//! Updates stat counter by player's actual position.
	/*!
		Use this to measure player's travelling distance.
		\param[in] name The name of the stat type.
	*/
	proto native void StatUpdateByPosition( string name );
	//! Updates stat counter by given position.
	/*!
		\param[in] name The name of the stat type.
		\param[in] pos	The specified position.
	*/
	proto native void StatUpdateByGivenPos( string name, vector pos );
	/*! Invokes read stats from server to client.
		\note Call this on client when you need update stats.
	*/
	proto native void StatInvokeUpdate();
	/*! Invokes save stats from server to client.
		\note Call this on server when you need update stats.
	*/
	proto native void StatSyncToClient();
	///@} Stats
	
	bool IsInventorySoftLocked()
	{
		return false;
	}

	void SetInventorySoftLock(bool status);
	
	/// returns true if man is in transport, false otherwise
	bool IsInTransport()
	{
		return Transport.Cast(GetParent()) != null;
	}
	
	void StopDeathDarkeningEffect();
	bool PhysicalPredictiveDropItem(EntityAI entity, bool heavy_item_only = true);
	void SetProcessUIWarning(bool state);
	void OnGameplayDataHandlerSync(); //depricated, sync now happens before the player is created, calling of this event still happens for legacy reasons
	bool CanPlaceItem(EntityAI item);
	
	/*!
		Called when 2D optics are about to be drawn.
		\return Collection of optics to be drawn. 
		        Draws the provided optics when of `ItemOptics` type.
		        Draws optic of current muzzle when of `Weapon_Base` type.
	*/
	protected array<InventoryItem> OnDrawOptics2D()
	{
		return null;
	}
};
