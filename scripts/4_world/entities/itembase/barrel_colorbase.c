class Barrel_ColorBase : DeployableContainer_Base
{
	private bool m_IsLocked = false;
	private ref Timer m_BarrelOpener;
	ref RainProcurementManager m_RainProcurement;

	protected ref OpenableBehaviour m_Openable;
	
	void Barrel_ColorBase()
	{
		m_BarrelOpener = new Timer();

		m_Openable = new OpenableBehaviour(false);
		m_RainProcurement = new RainProcurementManager( this );
		
		m_HalfExtents = Vector(0.30,0.85,0.30);

		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsPlaceSound");
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		ctx.Write( m_Openable.IsOpened() );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		bool opened;
		if ( version >= 110 && !ctx.Read( opened ) )
		{
			return false;
		}
		
		if ( opened )
		{
			OpenLoad();
		}
		else
		{
			CloseLoad();
		}
		
		return true;
	}
	
	bool IsLocked()
	{
		return m_IsLocked;
	}
	
	override void Open()
	{
		m_Openable.Open();
		m_RainProcurement.InitRainProcurement();
		SoundSynchRemote();
		SetTakeable(false);

		//SetSynchDirty(); //! called also in SoundSynchRemote - TODO

		UpdateVisualState();
	}
	
	void OpenLoad()
	{
		m_Openable.Open();
		m_RainProcurement.InitRainProcurement();
		SetSynchDirty();
		SetTakeable(false);
		
		UpdateVisualState();
	}
	
	override void Close()
	{
		m_Openable.Close();
		if (m_RainProcurement.IsRunning())
			m_RainProcurement.StopRainProcurement();
		SoundSynchRemote();
		SetTakeable(true);

		//SetSynchDirty(); //! called also in SoundSynchRemote - TODO

		UpdateVisualState();
	}
	
	void CloseLoad()
	{
		m_Openable.Close();
		if (m_RainProcurement.IsRunning())
			m_RainProcurement.StopRainProcurement();
		SetSynchDirty();
		SetTakeable(true);
		
		UpdateVisualState();
	}

	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		Open();
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		Close();
	}

	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("Lid",1);
			SetAnimationPhase("Lid2",0);
		}
		else
		{
			SetAnimationPhase("Lid",0);
			SetAnimationPhase("Lid2",1);
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
		else if ( IsSoundSynchRemote() && !IsBeingPlaced() && m_Initialized )
		{
			if ( IsOpen() )
			{
				SoundBarrelOpenPlay();
			}
			else if ( !IsOpen() )
			{
				SoundBarrelClosePlay();
			}
		}
		UpdateVisualState();
	}
	
	void SoundBarrelOpenPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "barrel_open_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
	
	void SoundBarrelClosePlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "barrel_close_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}

	void Lock(float actiontime)
	{
		m_IsLocked = true;
		m_BarrelOpener.Run(actiontime, this, "Unlock", NULL,false);
	}
	
	void Unlock()
	{
		m_IsLocked = false;
		Open();
	}
	
	void DetermineAction ( PlayerBase player )
	{
		int slot_id;
		ItemBase ingredient; 
		slot_id = InventorySlots.GetSlotIdFromString("Lime");
		ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
		if ( ingredient && GetQuantity() > 10000 ) 
		{
			TanPelts(ingredient, player);
		}
		else
		{
			slot_id = InventorySlots.GetSlotIdFromString("Material_Nails");
			ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
			if ( ingredient && GetQuantity() > 10000 ) 
			{
				ColourLeatherClothes(ingredient, player, "Black");
			}
			else
			{
				slot_id = InventorySlots.GetSlotIdFromString("BirchBark");
				ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) );
				if ( ingredient && GetQuantity() > 10000 ) 
				{
					ColourLeatherClothes(ingredient, player, "Beige");
				}
				else
				{
					slot_id = InventorySlots.GetSlotIdFromString("OakBark");
					ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
					if ( ingredient && GetQuantity() > 10000 ) 
					{
						ColourLeatherClothes(ingredient, player, "Brown");	
					}
					else
					{
						
						slot_id = InventorySlots.GetSlotIdFromString("Disinfectant");
						ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
						if ( ingredient && GetQuantity() > 10000 )
						{
							BleachClothes(ingredient, player);
						}
						else
						{
							slot_id = InventorySlots.GetSlotIdFromString("Plant");
							ingredient = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
							slot_id = InventorySlots.GetSlotIdFromString("Guts");
							ItemBase reactant = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
							if ( ingredient && reactant && GetQuantity() > 5000 )
							{
								ProduceFertilizer(ingredient,reactant,player);
							}
							else
							{
								slot_id = InventorySlots.GetSlotIdFromString("BerryR");
								ItemBase ingredientR = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
								slot_id = InventorySlots.GetSlotIdFromString("Plant");
								ItemBase ingredientG = ItemBase.Cast( GetInventory().FindAttachment(slot_id) );
								slot_id = InventorySlots.GetSlotIdFromString("BerryB");
								ItemBase ingredientB = ItemBase.Cast( GetInventory().FindAttachment(slot_id) ); 
								if ( (ingredientR || ingredientG || ingredientB) && GetQuantity() > 10000) 
								{
									ColourClothes(ingredientR,ingredientG,ingredientB,player);	
								}
							}
						}
					}
				}
			}
		}
		Close();
	}
	
	void ProduceFertilizer( ItemBase guts, ItemBase plant, PlayerBase player )
	{	
		float lime_quantity = plant.GetQuantity()*GameConstants.BAREL_LIME_PER_PLANT;
		
		guts.Delete();
		plant.Delete();
			
		ItemBase lime = ItemBase.Cast( GetInventory().CreateInInventory("GardenLime") );

		lime.SetQuantity(lime_quantity);
		Lock(50);
	}
	
	void TanPelts( ItemBase lime, PlayerBase player )
	{
		EntityAI item;		
		int item_count = GetInventory().GetCargo().GetItemCount();
		int pelt_count = 0;
		int lime_amount = Math.Floor(lime.GetQuantity()/GameConstants.BAREL_LIME_PER_PELT);
		
			
		for (int i = 0; i < item_count; i++)
		{
			item = GetInventory().GetCargo().GetItem(i);
			if ( item.IsPeltBase() )
			{
				pelt_count = g_Game.ConfigGetInt("cfgVehicles " + item.GetType() + " peltGain");
				if ( pelt_count <= lime_amount )
				{
					TanLeatherLambda lambda = new TanLeatherLambda(item, "TannedLeather", player, pelt_count);
					lambda.SetTransferParams(true, true, true);
					player.ServerReplaceItemWithNew(lambda);
										
					lime_amount -= pelt_count;
					if ( lime_amount <= 0 )
					{
						lime.Delete();
						break;
					}
				}
			}
		}	
		if ( lime ) 
		{
			lime.SetQuantity(lime_amount*GameConstants.BAREL_LIME_PER_PELT);		
		}
		if ( pelt_count > 0 ) 
		{
			Lock(30);
		}
	}
	
	void ColourLeatherClothes( ItemBase color_source, PlayerBase player, string color )
	{
		EntityAI item;		
		int item_count = GetInventory().GetCargo().GetItemCount();
		string item_name = "";
		
		
		for (int i = 0; i < item_count; i++)
		{
			item = GetInventory().GetCargo().GetItem(i);
			if ( item.IsClothing() )
			{	
				if ( GetGame().ObjectIsKindOf (item, "LeatherSack_Natural") ) 
				{
					item_name = "Bag_LeatherSack_"+color;
				}
				else if ( GetGame().ObjectIsKindOf (item, "LeatherStorageVest_Natural") )
				{
					item_name = "Vest_LeatherStorage_"+color;
				}
				else if ( GetGame().ObjectIsKindOf (item, "LeatherJacket_Natural") ) 
				{
					item_name = "Top_LeatherJacket_"+color;
				}
				else if ( GetGame().ObjectIsKindOf (item, "LeatherPants_Natural") ) 
				{
					item_name = "Pants_LeatherPants_"+color;
				}
				else if ( GetGame().ObjectIsKindOf (item, "LeatherMoccasinsShoes_Natural") ) 
				{
					item_name = "Shoes_LeatherMoccasins_"+color;
				}
				else if ( GetGame().ObjectIsKindOf (item, "LeatherHat_Natural") ) 
				{
					item_name = "Hat_Leather_"+color;
				}
				if ( item_name != "" )
				{
					TurnItemIntoItemLambda lambda = new TurnItemIntoItemLambda(item, item_name, player);
					lambda.SetTransferParams(true, true, true);
					player.ServerReplaceItemWithNew(lambda);
				}				
			}	
		}	
		if ( item_name != "" ) 
		{
			Lock(20);	
		}
	}
	
	
	void BleachClothes( ItemBase bleach, PlayerBase player )
	{
		EntityAI item;	
		int bleach_amount = bleach.GetQuantity();
		int item_count = GetInventory().GetCargo().GetItemCount();
		string item_name = "";
		
		for (int i = 0; i < item_count; i++)
		{
			item = GetInventory().GetCargo().GetItem(i);
			if ( item.IsClothing() )
			{	
				if ( bleach_amount >= GameConstants.BAREL_BLEACH_PER_CLOTH )
				{
					if ( ( GetGame().ObjectIsKindOf (item, "TShirt_ColorBase") && !GetGame().ObjectIsKindOf (item, "TShirt_White") ) || ( GetGame().ObjectIsKindOf (item, "Armband_ColorBase") && !GetGame().ObjectIsKindOf (item, "Armband_White") ) ) //cannot bleach white items...?
					{	
						int index = item.GetType().IndexOf("_");
						string itemtype = item.GetType().Substring( 0, index + 1 );
						ItemBase itemIB = ItemBase.Cast(item);
						MiscGameplayFunctions.TurnItemIntoItem(itemIB,itemtype+"White",player);
						
						bleach_amount -= GameConstants.BAREL_BLEACH_PER_CLOTH;
					}
					if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherSack_ColorBase") ) 
					{
						item_name = "LeatherSack_Natural";
					}
					else if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherStorageVest_ColorBase") ) 
					{
						item_name = "LeatherStorageVest_Natural";
					}
					else if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherJacket_ColorBase") )
					{
						item_name = "LeatherJacket_Natural";
					}
					else if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherPants_ColorBase") ) 
					{
						item_name = "LeatherPants_Natural";
					}
					else if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherMoccasinsShoes_ColorBase") ) 
					{
						item_name = "LeatherMoccasinsShoes_Natural";
					}
					else if ( GetGame().IsKindOf (item.GetDisplayName(), "LeatherHat_ColorBase") ) 
					{
						item_name = "LeatherHat_Natural";
					}
					if ( item_name != "" )
					{
						TurnItemIntoItemLambda lambda = new TurnItemIntoItemLambda(item, item_name, player);
						lambda.SetTransferParams(true, true, true);
						player.ServerReplaceItemWithNew(lambda);
						bleach_amount -= GameConstants.BAREL_BLEACH_PER_CLOTH;
					}
				}
				else
				{
					bleach.Delete();
					break;
				}
			}	
		}	
		if ( bleach )
		{
			bleach.SetQuantity(bleach_amount);		
		}
		if ( item_name != "" ) 
		{
			Lock(10);
		}
	}		
	
	void ColourClothes( ItemBase rci, ItemBase gci, ItemBase bci, PlayerBase player )
	{
		EntityAI item;		
		float r = 0;
		float g = 0;
		float b = 0;
		float a = 1; //alpha
		int item_count = GetInventory().GetCargo().GetItemCount();
		bool was_colored = false;
		if ( rci ) 
		{
			r = rci.GetQuantity()/10; 
			r = Math.Clamp(r, 0, 0.78); //TODO adjust clamp as needed, full RGB would result in a white item
		}
		if ( gci )
		{ 
			g = gci.GetQuantity()/10;
			g = Math.Clamp(g, 0, 0.78);
		}
		if ( bci ) 
		{
			b = bci.GetQuantity()/10; 
			b = Math.Clamp(b, 0, 0.78);
		}
		for (int i = 0; i < item_count; i++)
		{
			item = GetInventory().GetCargo().GetItem(i);
			if ( item.IsClothing() )
			{
				if ( GetGame().ObjectIsKindOf (item, "TShirt_White") || GetGame().ObjectIsKindOf (item, "Armband_White") )
				{
					ItemBase itemIB = ItemBase.Cast(item);
					int index = itemIB.GetType().IndexOf("_");
					string itemtype = itemIB.GetType().Substring( 0, index + 1 );
					
					ColourClothesLambda clambda = new ColourClothesLambda(itemIB, itemtype + "Dyed", player, r, g, b, a);
					clambda.SetTransferParams(true, true, true);
					MiscGameplayFunctions.TurnItemIntoItemEx(player, clambda);
					was_colored = true;
				}
			}	
		}

		if ( was_colored ) 
		{
			if ( rci ) 
			{
				//rci.AddQuantity(-r*10,true,false);
				rci.Delete();
			}
			if ( gci ) 
			{
				//gci.AddQuantity(-g*10,true,false);
				gci.Delete();
			}
			if ( bci ) 
			{
				//bci.AddQuantity(-b*10,true,false);
				bci.Delete();
			}
			Lock(30);
		}
	}

	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo( parent ))
			return false;

		if ( GetNumberOfItems() == 0 && !IsOpen() )
			return true;

		return false;
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		if ( !super.CanPutIntoHands( parent ))
			return false;

		if ( GetNumberOfItems() == 0 && (GetInventory().IsAttachment() || !IsOpen()) )
			return true;

		return false;
	}
	
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( IsOpen() )
			return super.CanReceiveItemIntoCargo( item );
		
		return false;
	}
	
	override bool CanReleaseCargo( EntityAI attachment )
	{
		return IsOpen();
	}
	
	override bool CanDetachAttachment( EntityAI parent )
	{
		if ( GetNumberOfItems() == 0)
			return true;
		return false;
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
			
		SetIsPlaceSound( true );
	}
	
	override string GetPlaceSoundset()
	{
		return "placeBarrel_SoundSet";
	}
	//================================================================
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_BARREL;
	}
	
	override void SetActions()
	{
		AddAction(ActionAttachOnSelection);
		super.SetActions();
		
		AddAction(ActionOpenBarrel);
		AddAction(ActionCloseBarrel);
		
	}
};

class Barrel_Green: Barrel_ColorBase {};
class Barrel_Blue: Barrel_ColorBase {};
class Barrel_Red: Barrel_ColorBase {};
class Barrel_Yellow: Barrel_ColorBase {};

class ColourClothesLambda : TurnItemIntoItemLambda
{
	int r, g, b, a;
	void ColourClothesLambda(EntityAI old_item, string new_item_type, PlayerBase player, int rr, int gg, int bb, int aa)
	{ r = rr; g = gg; b = bb; a = aa; }

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		//Print("ColourClothesLambda::OnCreated object=" + new_item);
		ItemBase ib = ItemBase.Cast(new_item);
		if ( new_item ) 
		{	
			new_item.SetObjectTexture(new_item.GetHiddenSelectionsData().GetHiddenSelectionIndex( "personality" ), MiscGameplayFunctions.GetColorString(r, g, b, a));
			
			//nitem.SetItemVariableString("varColor", color); //SYNCFAIL
			ib.SetColor(r*255, g*255, b*255, a*255);
		}
	}
};

class TanLeatherLambda : TurnItemIntoItemLambda
{
	int m_PeltCount;

	void TanLeatherLambda (EntityAI old_item, string new_item_type, PlayerBase player, int pelt_count) { m_PeltCount = pelt_count; }

	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		if (new_item) 
		{							
			ItemBase tanned_pelt = ItemBase.Cast( new_item );
			tanned_pelt.SetQuantity(m_PeltCount);
		}
		else
		{
			Debug.LogError("TanLeatherLambda: failed to create new item","static");
		}
	}
};