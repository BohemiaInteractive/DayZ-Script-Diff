class Fireplace extends FireplaceBase
{
	bool m_ContactEventProcessing;
	
	void Fireplace()
	{
		//Particles - default for FireplaceBase
		PARTICLE_FIRE_START 	= ParticleList.CAMP_FIRE_START;
		PARTICLE_SMALL_FIRE 	= ParticleList.CAMP_SMALL_FIRE;
		PARTICLE_NORMAL_FIRE	= ParticleList.CAMP_NORMAL_FIRE;
		PARTICLE_SMALL_SMOKE 	= ParticleList.CAMP_SMALL_SMOKE;
		PARTICLE_NORMAL_SMOKE	= ParticleList.CAMP_NORMAL_SMOKE;
		PARTICLE_FIRE_END 		= ParticleList.CAMP_FIRE_END;
		PARTICLE_STEAM_END		= ParticleList.CAMP_STEAM_2END;
		
		SetEventMask( EntityEvent.CONTACT | EntityEvent.TOUCH );
	}
	
	override bool IsBaseFireplace()
	{
		return true;
	}
	
	override void EOnTouch( IEntity other, int extra )
	{
		ContactEvent( other, GetPosition() );
	}
	
	override void EOnContact( IEntity other, Contact extra )
	{
		ContactEvent( other, extra.Position );
	}
	
	void ContactEvent( IEntity other, vector position )
	{		
		if ( GetGame().IsServer() && !m_ContactEventProcessing && dBodyIsActive(this) && !IsSetForDeletion() )
		{
			m_ContactEventProcessing = true;
			MiscGameplayFunctions.ThrowAllItemsInInventory(this, 0);
			CheckForDestroy();
			m_ContactEventProcessing = false;
		}
	}
	
	//attachments
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( !super.CanReceiveAttachment(attachment, slotId) )
			return false;

		ItemBase item = ItemBase.Cast( attachment );

		//cookware
		if ( ( item.Type() == ATTACHMENT_CAULDRON ) || ( item.Type() == ATTACHMENT_COOKING_POT ) )
		{
			if ( FindAttachmentBySlotName( "CookingTripod" ) || IsOven() ) 
				return true;
			return false;
		}
		if ( item.Type() == ATTACHMENT_FRYING_PAN )
		{
			if ( IsOven() ) 
				return true;
			return false;
		}

		// food on direct cooking slots
		if ( item.IsKindOf( "Edible_Base" ) )
		{
			if ( IsOven() )
				return true;
			return false;
		}
		//tripod
		
		if ( item.IsInherited(TripodBase) )
		{
			if (!IsOven() && !GetHierarchyParent())
				return true;
			return false;
		}
		
		//stones
		if ( item.Type() == ATTACHMENT_STONES )
		{
			if ( !GetHierarchyParent() && !IsBurning() )
				return true;
			return false;
		}
		
		return true;
	}
	
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		string slot_name = InventorySlots.GetSlotName(slot_id);
		if ( super.CanDisplayAttachmentSlot(slot_id) )
		{
			if ( slot_name != "CookingEquipment" || FindAttachmentBySlotName( "CookingTripod" ) )
				return true;
		}
		
		return false;
	}
	
	override bool CanLoadAttachment( EntityAI attachment )
	{
		if ( !super.CanLoadAttachment(attachment) )
			return false;

		ItemBase item = ItemBase.Cast( attachment );

		//cookware
		if ( ( item.Type() == ATTACHMENT_CAULDRON ) || ( item.Type() == ATTACHMENT_COOKING_POT ) )
		{
			//if ( IsItemTypeAttached( ATTACHMENT_TRIPOD ) /*|| IsOven()*/ ) 
				return true;
		}
		if ( item.Type() == ATTACHMENT_FRYING_PAN )
		{
			//if ( IsOven() ) 
				return true;
		}

		// food on direct cooking slots
		//if ( IsOven() )
		//{
			if ( item.IsKindOf( "Edible_Base" ) )
				return true;
		//}
		//tripod
		if ( item.Type() == ATTACHMENT_TRIPOD )
		{
			if ( /*!IsOven() &&*/ GetHierarchyParent() == NULL )
				return true;
		}
		
		//stones
		if ( item.Type() == ATTACHMENT_STONES )
		{
			if ( GetHierarchyParent() /*|| IsBurning()*/ )
				return false;
			
			return true;
		}
		
		return true;
	}

	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}
		
		ItemBase item = ItemBase.Cast(attachment);
		//stones
		if (item.Type() == ATTACHMENT_STONES)
		{
			if (IsBurning())
			{
				return false;
			}

			int stone_quantity = item.GetQuantity();
			if (HasStoneCircle() && stone_quantity <= 8)
			{
				return false;
			}

			if (IsOven())
			{
				return false;
			}
		}
		
		return true;
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		ItemBase item_base = ItemBase.Cast(item);
		
		if (IsKindling(item_base) || IsFuel(item_base))
		{
			AddToFireConsumables(item_base);
		}
		
		//cookware
		if (item_base.Type() == ATTACHMENT_COOKING_POT)
		{
			SetCookingEquipment(item_base);
			
			//rotate handle (if not in 'Oven' stage)
			if (GetGame().IsServer() && !IsOven())
			{
				item_base.SetAnimationPhase(ANIMATION_COOKWARE_HANDLE, 0);
			}
		}
		if (item.Type() == ATTACHMENT_CAULDRON)
		{
			SetCookingEquipment(item_base);
			
			//rotate handle (if not in 'Oven' stage)
			if (GetGame().IsServer() && !IsOven())
			{
				item_base.SetAnimationPhase(ANIMATION_CAULDRON_HANDLE, 0);
			}
		}

		// direct cooking/smoking slots
		bool edible_base_attached = false;
		switch (slot_name)
		{
		case "DirectCookingA":
			m_DirectCookingSlots[0] = item_base;
			edible_base_attached = true;
		break;
		case "DirectCookingB":
			m_DirectCookingSlots[1] = item_base;
			edible_base_attached = true;
		break;
		case "DirectCookingC":
			m_DirectCookingSlots[2] = item_base;
			edible_base_attached = true;
		break;

		case "SmokingA":
			m_SmokingSlots[0] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingB":
			m_SmokingSlots[1] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingC":
			m_SmokingSlots[2] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingD":
			m_SmokingSlots[3] = item_base;
			edible_base_attached = true;
		break;
		}
		
		// reset cooking time (to prevent the cooking exploit)
		if (GetGame().IsServer() && edible_base_attached)
		{
			Edible_Base edBase = Edible_Base.Cast(item_base);
			if (edBase)
			{
				if (edBase.GetFoodStage())
				{
					edBase.SetCookingTime(0);
				}
			}
		}
		
		//TODO
		//add SetViewIndex when attaching various attachments
		
		RefreshFireplaceVisuals();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		ItemBase item_base = ItemBase.Cast(item);
		
		if (IsKindling(item_base) || IsFuel(item_base))
		{
			RemoveFromFireConsumables(GetFireConsumableByItem(item_base));
		}

		//cookware
		if (item_base.Type() == ATTACHMENT_COOKING_POT)
		{
			ClearCookingEquipment(item_base);
			
			//rotate handle
			if (GetGame().IsServer())
			{
				item_base.SetAnimationPhase(ANIMATION_COOKWARE_HANDLE, 1);
			}
			
			//remove audio visuals
			Bottle_Base cooking_pot = Bottle_Base.Cast(item);
			cooking_pot.RemoveAudioVisualsOnClient();
		}
		if (item_base.Type() == ATTACHMENT_CAULDRON)
		{
			ClearCookingEquipment(item_base);

			//rotate handle
			if (GetGame().IsServer())
			{
				item_base.SetAnimationPhase(ANIMATION_CAULDRON_HANDLE, 1);
			}
			
			//remove audio visuals
			Bottle_Base cauldron = Bottle_Base.Cast( item );
			cauldron.RemoveAudioVisualsOnClient();
		}
		if (item_base.Type() == ATTACHMENT_FRYING_PAN)
		{
			ClearCookingEquipment(item_base);

			//remove audio visuals
			FryingPan frying_pan = FryingPan.Cast(item);
			frying_pan.RemoveAudioVisualsOnClient();
		}

		// direct cooking/smoking slots
		switch (slot_name)
		{
		case "DirectCookingA":
			m_DirectCookingSlots[0] = null;
		break;
		case "DirectCookingB":
			m_DirectCookingSlots[1] = null;
		break;
		case "DirectCookingC":
			m_DirectCookingSlots[2] = null;
		break;

		case "SmokingA":
			m_SmokingSlots[0] = null;
		break;
		case "SmokingB":
			m_SmokingSlots[1] = null;
		break;
		case "SmokingC":
			m_SmokingSlots[2] = null;
		break;
		case "SmokingD":
			m_SmokingSlots[3] = null;
		break;
		}
		
		//no attachments left, no cargo items & no ashes are present
		CheckForDestroy();

		//TODO
		//add SetViewIndex when detaching various attachments

		RefreshFireplaceVisuals();
	}
	
	
	override void SetCookingEquipment(ItemBase equipment)
	{
		super.SetCookingEquipment(equipment);
		
		TripodBase stand = TripodBase.Cast(FindAttachmentBySlotName("CookingTripod"));
		if (stand)
		{
			if (equipment)
			{
				stand.LockToParent();
			}
			else
			{
				stand.UnlockFromParent();
			}
		}
	}
	
	
	override void OnBeforeTryDelete() 
	{
		super.OnBeforeTryDelete();
		MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, m_HalfExtents);
	}
	
	override bool IsPrepareToDelete()
	{
		return GetInventory().AttachmentCount() == 0 && !IsBurning() && !HasAshes();
	}
	
	//CONDITIONS
	//this into/outo parent.Cargo
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}
		
		if (HasAshes() || IsBurning() || HasStones() || HasStoneCircle() || IsOven() || !IsCargoEmpty() || HasCookingStand())
		{
			return false;
		}
		
		return true;
	}

	override bool CanRemoveFromCargo( EntityAI parent )
	{
		return true;
	}
	
	//cargo item into/outo this.Cargo
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( GetHierarchyParent() )
			return false;
		
		return super.CanReceiveItemIntoCargo( item );
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if (!super.CanLoadItemIntoCargo( item ))
			return false;
		
		if ( GetHierarchyParent() )
			return false;
		
		return true;
	}
	
	//hands
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}
		
		if (HasAshes() || IsBurning() || HasStones() || HasStoneCircle() || IsOven() || !IsCargoEmpty() || HasCookingStand())
		{
			return false;
		}
		
		return true;
	}
	
	override bool CanDisplayAttachmentCategory( string category_name )
	{
		if ( !super.CanDisplayAttachmentCategory( category_name ) )
			return false;
		
		if ( IsOven() )
		{
			if ( category_name == "CookingEquipment" )
				return false;
			if ( ( category_name == "DirectCooking" ) || ( category_name == "Smoking" ) )
				return true;
		}
		else
		{
			if ( category_name == "CookingEquipment" )
				return true;
			if ( ( category_name == "DirectCooking" ) || ( category_name == "Smoking" ) )
				return false;
		}
		return true;
	}
	
	override bool CanAssignAttachmentsToQuickbar()
	{
		return false;
	}
	
	override float HeightStartCheckOverride()
	{
		return 0.32;
	}
	
	//particles
	override bool CanShowSmoke()
	{
		return !IsOven();
	}
	
	
	void DestroyClutterCutter( Object clutter_cutter )
	{
		GetGame().ObjectDelete( clutter_cutter );
	}
	
	override void RefreshPhysics()
	{
		super.RefreshPhysics();
		
		//Oven
		if ( IsOven() )
		{
			RemoveProxyPhysics( ANIMATION_OVEN );
			AddProxyPhysics( ANIMATION_OVEN );
		}
		else
		{
			RemoveProxyPhysics( ANIMATION_OVEN );
		}
		
		//Tripod
		if ( IsItemTypeAttached( ATTACHMENT_TRIPOD ) )
		{
			RemoveProxyPhysics( ANIMATION_TRIPOD );
			AddProxyPhysics( ANIMATION_TRIPOD );
		}
		else
		{
			RemoveProxyPhysics( ANIMATION_TRIPOD );
		}	
	}
	
	override void RefreshFireplacePhysics()
	{
		RefreshPhysics();
	}

	//on store save/load
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave(ctx);
		
		if ( GetGame().SaveVersion() >= 110 )
		{
			// save stone circle state
			ctx.Write( m_HasStoneCircle );
			
			// save stone oven state
			ctx.Write( m_IsOven );
		}
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;

		if ( version >= 110 )
		{
			// read stone circle state
			if ( !ctx.Read( m_HasStoneCircle ) )
			{
				m_HasStoneCircle = false;
				return false;
			}
			// read stone oven state
			if ( !ctx.Read( m_IsOven ) )
			{
				m_IsOven = false;
				return false;
			}
		}		
		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		if ( IsBurning() )
		{
			if ( !m_ClutterCutter )
			{
				m_ClutterCutter = GetGame().CreateObjectEx( OBJECT_CLUTTER_CUTTER, GetPosition(), ECE_PLACE_ON_SURFACE );
				m_ClutterCutter.SetOrientation( GetOrientation() );
			}
		}
	}
	
	//================================================================
	// IGNITION ACTION
	//================================================================	
	
	// Item-to-item fire distribution
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		return HasAnyKindling() && !IsBurning() && !GetHierarchyParent();
	}
	
	override bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return IsBurning();
	}
	
	override bool IsIgnited()
	{
		return IsBurning();
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{	
		//remove grass
		Object cc_object = GetGame().CreateObjectEx( OBJECT_CLUTTER_CUTTER , GetPosition(), ECE_PLACE_ON_SURFACE );
		cc_object.SetOrientation ( GetOrientation() );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DestroyClutterCutter, 200, false, cc_object );
		
		//start fire
		StartFire();
		
		if ( fire_source )
		{
			Man player = fire_source.GetHierarchyRootPlayer();
			
			if ( player )
			{
				EFireIgniteType ignate_type = EFireIgniteType.Unknow;
				
				if ( fire_source.ClassName() == "Matchbox"  )
				{
					ignate_type = EFireIgniteType.Matchbox;
				}
				else if ( fire_source.ClassName() == "Roadflare"  )
				{
					ignate_type = EFireIgniteType.Roadflare;
				}
				else if ( fire_source.ClassName() == "HandDrillKit"  )
				{
					ignate_type = EFireIgniteType.HandDrill;
				}
				
				
				SyncEvents.SendPlayerIgnatedFireplace( player, ignate_type );
			}
		}
	}

	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		SetIgniteFailure( false );
		Param1<bool> failure;
		
		//check kindling
		if ( !HasAnyKindling() )
		{
			return false;
		}
		
		//check roof
		if ( !IsOven() )
		{
			if ( !IsCeilingHighEnoughForSmoke() && IsOnInteriorSurface() )
			{
				return false;
			}
		}
		
		//check surface
		if ( IsOnWaterSurface() )
		{
			return false;
		}

		//check wetness
		if ( IsWet() )
		{
			SetIgniteFailure( true );
			
			failure = new Param1<bool>( GetIgniteFailure() );
			GetGame().RPCSingleParam( this, FirePlaceFailure.WET, failure, true );
			return false;
		}

		// check if the fireplace isnt below a roof
		//  excluding this check whein oven stage
		if ( !IsOven() && !MiscGameplayFunctions.IsUnderRoof( this ) )
		{
			// if not, check if there is strong rain or wind
			if ( IsRainingAbove() )
			{
				SetIgniteFailure( true );
				
				failure = new Param1<bool>( GetIgniteFailure() );
				GetGame().RPCSingleParam( this, FirePlaceFailure.WET, failure, true );
				return false;
			}
			
			if ( IsWindy() )
			{
				SetIgniteFailure( true );
				
				failure = new Param1<bool>( GetIgniteFailure() );
				GetGame().RPCSingleParam( this, FirePlaceFailure.WIND, failure, true );
				return false;
			}
		}
		
		return true;	
	}
	
	//================================================================
	// FIREPLACE ENTITY
	//================================================================	
	static Fireplace IgniteEntityAsFireplace( notnull EntityAI entity, notnull EntityAI fire_source )
	{
		//get player
		PlayerBase player = PlayerBase.Cast( fire_source.GetHierarchyRootPlayer() );
		
		//create fireplace
		Fireplace fireplace = Fireplace.Cast( GetGame().CreateObjectEx( "Fireplace" , entity.GetPosition(), ECE_PLACE_ON_SURFACE ) );
		
		//attach
		if ( !GetGame().IsMultiplayer() )		//clear inventory reservation (single player)
		{
			InventoryLocation loc = new InventoryLocation;
			entity.GetInventory().GetCurrentInventoryLocation( loc );
			player.GetInventory().ClearInventoryReservationEx( entity, loc );
		}
		
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
		{
			player.ServerTakeEntityToTargetAttachment( fireplace, entity ); // multiplayer server side
		}
		else
		{
			player.LocalTakeEntityToTargetAttachment( fireplace, entity ); // single player or multiplayer client side
		}
		
		//start fire
		fireplace.StartFire();
		fireplace.OnIgnitedThis(fire_source);
		
		return fireplace;
	}
	
	static bool CanIgniteEntityAsFireplace(notnull EntityAI entity)
	{
		//check ceiling (enough space for smoke)
		if (MiscGameplayFunctions.IsUnderRoof(entity, FireplaceBase.MIN_CEILING_HEIGHT) && IsEntityOnInteriorSurface(entity))
		{
			return false;
		}
		
		//check surface
		if (FireplaceBase.IsEntityOnWaterSurface(entity))
		{
			return false;
		}

		if (!MiscGameplayFunctions.IsUnderRoof(entity))
			return !FireplaceBase.IsRainingAboveEntity(entity);

		return true;	
	}

	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override string GetPlaceSoundset()
	{
		return "placeFireplace_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionPlaceFireplaceIntoBarrel);
		AddAction(ActionPlaceFireplaceIndoor);
		AddAction(ActionPlaceOvenIndoor);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
		AddAction(ActionBuildOven);
		AddAction(ActionDismantleOven);
		AddAction(ActionBuildStoneCircle);
		AddAction(ActionDismantleStoneCircle);
	}
}
