
//-----------------------------------------------------------------------------
class ItemSuppressor extends InventoryItemSuper
{
};

typedef ItemSuppressor SuppressorBase;

//-----------------------------------------------------------------------------
class ItemWatch extends InventoryItemSuper
{
};

//-----------------------------------------------------------------------------
class ItemTransmitter extends InventoryItemSuper
{
	proto native void SetNextChannel();
	proto native void SetPrevChannel();
	proto native float GetTunedFrequency();
	proto native void EnableBroadcast(bool state);
	proto native void EnableReceive(bool state);
	proto native bool IsBroadcasting();
	proto native bool IsReceiving();
	proto native int GetTunedFrequencyIndex();
	proto native void SetFrequencyByIndex(int index);
};

//-----------------------------------------------------------------------------
class ItemMegaphone extends InventoryItemSuper
{
	proto native bool CanSpeak();
	proto native void SetCanSpeak(bool onOff); 
}

//-----------------------------------------------------------------------------
class ItemRadio extends InventoryItemSuper
{
	proto native void TuneNext();
	proto native void TunePrev();
};

//-----------------------------------------------------------------------------
class ItemBarrel extends InventoryItemSuper
{
};

//-----------------------------------------------------------------------------
class UnderSlugLauncher extends Weapon
{
};

//-----------------------------------------------------------------------------
class ItemGPS extends InventoryItemSuper
{
	bool IsTurnedOn()
	{
		return true;
	}
	
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);

		PlayerBase pb = PlayerBase.Cast(player);
		if ( pb && IsTurnedOn() )
		{
			pb.MapNavigationItemInPossession(this);
		}
	}

	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		PlayerBase pb = PlayerBase.Cast(player);
		if ( pb )
		{
			pb.MapNavigationItemNotInPossession(this);
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		PlayerBase player = PlayerBase.Cast(parent);
		if ( parent.GetHierarchyRoot() )
		{
			player = PlayerBase.Cast(parent.GetHierarchyRoot());			
		}

		if ( player && IsTurnedOn() )
		{
			player.MapNavigationItemInPossession(this);
		}
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);

		PlayerBase player = PlayerBase.Cast(parent);
		if ( parent.GetHierarchyRoot() )
		{
			player = PlayerBase.Cast(parent.GetHierarchyRoot());			
		}

		if ( player )
		{
			player.MapNavigationItemNotInPossession(this);
		}
	}
};

//-----------------------------------------------------------------------------
class ItemCompass extends InventoryItemSuper
{
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		if ( IsRuined() )
		{
			return;
		}

		PlayerBase pb = PlayerBase.Cast(player);
		if ( pb )
		{
			pb.MapNavigationItemInPossession(this);
		}
	}

	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);

		if ( IsRuined() )
		{
			return;
		}

		PlayerBase pb = PlayerBase.Cast(player);
		if ( pb )
		{
			pb.MapNavigationItemNotInPossession(this);
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if ( IsRuined() )
		{
			return;
		}
		
		PlayerBase player = PlayerBase.Cast(parent);
		if ( parent.GetHierarchyParent() )
		{
			player = PlayerBase.Cast(parent.GetHierarchyParent());			
		}

		if ( player )
		{
			player.MapNavigationItemInPossession(this);
		}
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);

		if ( IsRuined() )
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast(parent);
		if ( parent.GetHierarchyParent() )
		{
			player = PlayerBase.Cast(parent.GetHierarchyParent());			
		}

		if ( player )
		{
			player.MapNavigationItemNotInPossession(this);
		}
	}
};

//-----------------------------------------------------------------------------
class CarWheel extends InventoryItemSuper
{
	//! Returns current wheel radius.
	proto native float GetRadius();

	//! Returns wheel width.
	proto native float GetWidth();

/*	Invoke car phys, when wheel was attached
	not working properly, postponed for now
	override void OnWasAttached( EntityAI parent, int slot_name )
	{
		IEntity iePrnt = IEntity.Cast( parent );
		dBodyActive( iePrnt, ActiveState.ACTIVE);
		dBodyApplyImpulse( iePrnt, "0 1 0");
	}
*/


	/*
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		Print("CarWheel>>> EEHitBy");
		Print( dmgZone );
		Print( damageResult );
		Print( source );
		Print( component );
		Print( damageResult.GetDamage(dmgZone, "Health") );
	}
	*/

	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged( oldLevel, newLevel, zone );
		
		if ( newLevel ==  GameConstants.STATE_RUINED )
		{
			string newWheel = "";
			switch ( GetType() )
			{
				case "HatchbackWheel":
					newWheel = "HatchbackWheel_Ruined";
				break;
	
				case "CivSedanWheel":
					newWheel = "CivSedanWheel_Ruined";
				break;
	
				case "Hatchback_02_Wheel":
					newWheel = "Hatchback_02_Wheel_Ruined";
				break;
				
				case "Sedan_02_Wheel":
					newWheel = "Sedan_02_Wheel_Ruined";
				break;
	
				case "Truck_01_Wheel":
					newWheel = "Truck_01_Wheel_Ruined";
				break;
	
				case "Truck_01_WheelDouble":
					newWheel = "Truck_01_WheelDouble_Ruined";
				break;
				case "Offroad_02_Wheel":
					newWheel = "Offroad_02_Wheel_Ruined";
				break;
			}

			if ( newWheel != "" )
			{
				//Unlock to allow creating a new item
				if (IsLockedInSlot())
				{
					UnlockFromParent();
				}

				ReplaceWheelLambda lambda = new ReplaceWheelLambda(this, newWheel, null);
				lambda.SetTransferParams(true, true, true);
				GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
			}
		}
	}
	
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	override void SetActions()
	{
		super.SetActions();
		//AddAction(ActionDetach);
		//AddAction(ActionAttachOnSelection);
	}
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		return false;
	}
};

class CarWheel_Ruined : CarWheel
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		// So that the lambda can always put it to the Transport
		if ( parent.IsInherited(Transport) && parent.IsRuined() )
		{
			InventoryLocation loc = new InventoryLocation();
			if ( GetInventory().GetCurrentInventoryLocation(loc) );
			{
				return loc.GetType() == InventoryLocationType.UNKNOWN;
			}
			
			return false;
		}

		if ( !super.CanPutAsAttachment(parent) )
		{
			return false;
		}

		return true;
	}
}

class ReplaceWheelLambda : TurnItemIntoItemLambda
{
	vector m_oldOri;
	void ReplaceWheelLambda(EntityAI old_item, string new_item_type, PlayerBase player) {}
	
	// No super, because we want to skip the LocationCanRemove check...
	override protected bool CanExecuteLambda()
	{
		return m_OldItem != null;
	}

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
		m_oldOri = old_item.GetOrientation();
	}
	
	override protected void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess( new_item );
		if ( new_item )
			new_item.SetOrientation( m_oldOri );
	}
}

class Hatchback_02_Wheel extends CarWheel {};
class Hatchback_02_Wheel_Ruined extends CarWheel_Ruined {};

class Sedan_02_Wheel extends CarWheel {};
class Sedan_02_Wheel_Ruined extends CarWheel_Ruined {};

class HatchbackWheel extends CarWheel {};
class HatchbackWheel_Ruined extends CarWheel_Ruined {};

class CivSedanWheel extends CarWheel {};
class CivSedanWheel_Ruined extends CarWheel_Ruined {};

class Truck_01_Wheel extends CarWheel {};
class Truck_01_Wheel_Ruined extends CarWheel_Ruined {};

class Truck_01_WheelDouble extends CarWheel {};
class Truck_01_WheelDouble_Ruined extends CarWheel_Ruined {};

class Offroad_02_Wheel extends CarWheel {};
class Offroad_02_Wheel_Ruined extends CarWheel_Ruined {};

class CarDoor extends InventoryItemSuper
{

	override bool CanDetachAttachment(EntityAI parent)
	{
		//parent.FindAtt
		InventoryLocation loc = new InventoryLocation();
		
		bool isPresent = GetInventory().GetCurrentInventoryLocation( loc );
		
		if ( !isPresent || loc.GetSlot() == -1 )
			return false;
		
		string slotName = InventorySlots.GetSlotName( loc.GetSlot() );

		if ( slotName && CarScript.Cast( parent ).GetCarDoorsState( slotName ) == CarDoorState.DOORS_OPEN )
			return true;
		
		return false;
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if ( newLevel ==  GameConstants.STATE_RUINED && m_Initialized )
		{
			EffectSound sound_plug;
			switch ( zone )
			{
				case "Window":
					PlaySoundSet( sound_plug, "offroad_hit_window_small_SoundSet", 0, 0 );
				break;
			}
		}
	}
	
	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
/*
	override void OnWasAttached( EntityAI parent, int slot_name )
	{
		
		SoundParams soundParams = new SoundParams("offroad_door_close_SoundSet");
		SoundObjectBuilder soundBuilder = new SoundObjectBuilder(soundParams);
		SoundObject soundObject = soundBuilder.BuildSoundObject();
		soundObject.SetPosition(GetPosition());
		GetGame().GetSoundScene().Play3D(soundObject, soundBuilder);
	}
*/
/*
	void CloseDoors()
	{
		SetAnimationPhase();
		
	}
*/
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
	}
	
};

//-------------------------------------
class Hatchback_02_Door_1_1 extends CarDoor {};
class Hatchback_02_Door_1_2 extends CarDoor {};
class Hatchback_02_Door_2_1 extends CarDoor {};
class Hatchback_02_Door_2_2 extends CarDoor {};
class Hatchback_02_Hood extends CarDoor {};
class Hatchback_02_Trunk extends CarDoor {};

class Hatchback_02_Door_1_1_Black extends Hatchback_02_Door_1_1 {};
class Hatchback_02_Door_1_2_Black extends Hatchback_02_Door_1_2 {};
class Hatchback_02_Door_2_1_Black extends Hatchback_02_Door_2_1 {};
class Hatchback_02_Door_2_2_Black extends Hatchback_02_Door_2_2 {};
class Hatchback_02_Hood_Black extends Hatchback_02_Hood {};
class Hatchback_02_Trunk_Black extends Hatchback_02_Trunk {};

class Hatchback_02_Door_1_1_Blue extends Hatchback_02_Door_1_1 {};
class Hatchback_02_Door_1_2_Blue extends Hatchback_02_Door_1_2 {};
class Hatchback_02_Door_2_1_Blue extends Hatchback_02_Door_2_1 {};
class Hatchback_02_Door_2_2_Blue extends Hatchback_02_Door_2_2 {};
class Hatchback_02_Hood_Blue extends Hatchback_02_Hood {};
class Hatchback_02_Trunk_Blue extends Hatchback_02_Trunk {};

//-------------------------------------
class Sedan_02_Door_1_1 extends CarDoor {};
class Sedan_02_Door_1_2 extends CarDoor {};
class Sedan_02_Door_2_1 extends CarDoor {};
class Sedan_02_Door_2_2 extends CarDoor {};
class Sedan_02_Hood extends CarDoor {};
class Sedan_02_Trunk extends CarDoor {};

class Sedan_02_Door_1_1_YellowRust extends CarDoor {};
class Sedan_02_Door_1_2_YellowRust extends CarDoor {};
class Sedan_02_Door_2_1_YellowRust extends CarDoor {};
class Sedan_02_Door_2_2_YellowRust extends CarDoor {};
class Sedan_02_Hood_YellowRust extends CarDoor {};
class Sedan_02_Trunk_YellowRust extends CarDoor {};

class Sedan_02_Door_1_1_Red extends Sedan_02_Door_1_1 {};
class Sedan_02_Door_1_2_Red extends Sedan_02_Door_1_2 {};
class Sedan_02_Door_2_1_Red extends Sedan_02_Door_2_1 {};
class Sedan_02_Door_2_2_Red extends Sedan_02_Door_2_2 {};
class Sedan_02_Hood_Red extends Sedan_02_Hood {};
class Sedan_02_Trunk_Red extends Sedan_02_Trunk {};

class Sedan_02_Door_1_1_RedRust extends Sedan_02_Door_1_1 {};
class Sedan_02_Door_1_2_RedRust extends Sedan_02_Door_1_2 {};
class Sedan_02_Door_2_1_RedRust extends Sedan_02_Door_2_1 {};
class Sedan_02_Door_2_2_RedRust extends Sedan_02_Door_2_2 {};
class Sedan_02_Hood_RedRust extends Sedan_02_Hood {};
class Sedan_02_Trunk_RedRust extends Sedan_02_Trunk {};

class Sedan_02_Door_1_1_Grey extends Sedan_02_Door_1_1 {};
class Sedan_02_Door_1_2_Grey extends Sedan_02_Door_1_2 {};
class Sedan_02_Door_2_1_Grey extends Sedan_02_Door_2_1 {};
class Sedan_02_Door_2_2_Grey extends Sedan_02_Door_2_2 {};
class Sedan_02_Hood_Grey extends Sedan_02_Hood {};
class Sedan_02_Trunk_Grey extends Sedan_02_Trunk {};

class Sedan_02_Door_1_1_GreyRust extends Sedan_02_Door_1_1 {};
class Sedan_02_Door_1_2_GreyRust extends Sedan_02_Door_1_2 {};
class Sedan_02_Door_2_1_GreyRust extends Sedan_02_Door_2_1 {};
class Sedan_02_Door_2_2_GreyRust extends Sedan_02_Door_2_2 {};
class Sedan_02_Hood_GreyRust extends Sedan_02_Hood {};
class Sedan_02_Trunk_GreyRust extends Sedan_02_Trunk {};

//-------------------------------------
class HatchbackDoors_Driver extends CarDoor {};
class HatchbackDoors_CoDriver extends CarDoor {};
class HatchbackHood extends CarDoor {};
class HatchbackTrunk extends CarDoor {};

class HatchbackDoors_Driver_White extends HatchbackDoors_Driver {};
class HatchbackDoors_CoDriver_White extends HatchbackDoors_CoDriver {};
class HatchbackHood_White extends HatchbackHood {};
class HatchbackTrunk_White extends HatchbackTrunk {};

class HatchbackDoors_Driver_Blue extends HatchbackDoors_Driver {};
class HatchbackDoors_CoDriver_Blue extends HatchbackDoors_CoDriver {};
class HatchbackHood_Blue extends HatchbackHood {};
class HatchbackTrunk_Blue extends HatchbackTrunk {};

class HatchbackDoors_Driver_GreenRust extends HatchbackDoors_Driver {};
class HatchbackDoors_CoDriver_GreenRust extends HatchbackDoors_CoDriver {};
class HatchbackHood_GreenRust extends HatchbackHood {};
class HatchbackTrunk_GreenRust extends HatchbackTrunk {};

class HatchbackDoors_Driver_WhiteRust extends HatchbackDoors_Driver {};
class HatchbackDoors_CoDriver_WhiteRust extends HatchbackDoors_CoDriver {};
class HatchbackHood_WhiteRust extends HatchbackHood {};
class HatchbackTrunk_WhiteRust extends HatchbackTrunk {};

class HatchbackDoors_Driver_BlueRust extends HatchbackDoors_Driver {};
class HatchbackDoors_CoDriver_BlueRust extends HatchbackDoors_CoDriver {};
class HatchbackHood_BlueRust extends HatchbackHood {};
class HatchbackTrunk_BlueRust extends HatchbackTrunk {};

//-------------------------------------
class CivSedanDoors_Driver extends CarDoor {};
class CivSedanDoors_CoDriver extends CarDoor {};
class CivSedanDoors_BackLeft extends CarDoor {};
class CivSedanDoors_BackRight extends CarDoor {};
class CivSedanHood extends CarDoor {};
class CivSedanTrunk extends CarDoor {};

class CivSedanDoors_Driver_Black extends CivSedanDoors_Driver {};
class CivSedanDoors_CoDriver_Black extends CivSedanDoors_CoDriver {};
class CivSedanDoors_BackLeft_Black extends CivSedanDoors_BackLeft {};
class CivSedanDoors_BackRight_Black extends CivSedanDoors_BackRight {};
class CivSedanHood_Black extends CivSedanHood {};
class CivSedanTrunk_Black extends CivSedanTrunk {};

class CivSedanDoors_Driver_Wine extends CivSedanDoors_Driver {};
class CivSedanDoors_CoDriver_Wine extends CivSedanDoors_CoDriver {};
class CivSedanDoors_BackLeft_Wine extends CivSedanDoors_BackLeft {};
class CivSedanDoors_BackRight_Wine extends CivSedanDoors_BackRight {};
class CivSedanHood_Wine extends CivSedanHood {};
class CivSedanTrunk_Wine extends CivSedanTrunk {};


class CivSedanDoors_Driver_WhiteRust extends CivSedanDoors_Driver {};
class CivSedanDoors_CoDriver_WhiteRust extends CivSedanDoors_CoDriver {};
class CivSedanDoors_BackLeft_WhiteRust extends CivSedanDoors_BackLeft {};
class CivSedanDoors_BackRight_WhiteRust extends CivSedanDoors_BackRight {};
class CivSedanHood_WhiteRust extends CivSedanHood {};
class CivSedanTrunk_WhiteRust extends CivSedanTrunk {};

class CivSedanDoors_Driver_WineRust extends CivSedanDoors_Driver {};
class CivSedanDoors_CoDriver_WineRust extends CivSedanDoors_CoDriver {};
class CivSedanDoors_BackLeft_WineRust extends CivSedanDoors_BackLeft {};
class CivSedanDoors_BackRight_WineRust extends CivSedanDoors_BackRight {};
class CivSedanHood_WineRust extends CivSedanHood {};
class CivSedanTrunk_WineRust extends CivSedanTrunk {};

class CivSedanDoors_Driver_BlackRust extends CivSedanDoors_Driver {};
class CivSedanDoors_CoDriver_BlackRust extends CivSedanDoors_CoDriver {};
class CivSedanDoors_BackLeft_BlackRust extends CivSedanDoors_BackLeft {};
class CivSedanDoors_BackRight_BlackRust extends CivSedanDoors_BackRight {};
class CivSedanHood_BlackRust extends CivSedanHood {};
class CivSedanTrunk_BlackRust extends CivSedanTrunk {};


//-------------------------------------
class Truck_01_Door_1_1 extends CarDoor {};
class Truck_01_Door_2_1 extends CarDoor {};
class Truck_01_Door_Hood extends CarDoor {};

class Truck_01_Door_1_1_Blue extends Truck_01_Door_1_1 {};
class Truck_01_Door_2_1_Blue extends Truck_01_Door_2_1 {};
class Truck_01_Door_Hood_Blue extends Truck_01_Door_Hood {};

class Truck_01_Door_1_1_Orange extends Truck_01_Door_1_1 {};
class Truck_01_Door_2_1_Orange extends Truck_01_Door_2_1 {};
class Truck_01_Door_Hood_Orange extends Truck_01_Door_Hood {};

class Truck_01_Door_1_1_GreenRust extends Truck_01_Door_1_1 {};
class Truck_01_Door_2_1_GreenRust extends Truck_01_Door_2_1 {};
class Truck_01_Hood_GreenRust extends Truck_01_Door_Hood {};

class Truck_01_Door_1_1_BlueRust extends Truck_01_Door_1_1 {};
class Truck_01_Door_2_1_BlueRust extends Truck_01_Door_2_1 {};
class Truck_01_Hood_BlueRust extends Truck_01_Door_Hood {};

class Truck_01_Door_1_1_OrangeRust extends Truck_01_Door_1_1 {};
class Truck_01_Door_2_1_OrangeRust extends Truck_01_Door_2_1 {};
class Truck_01_Hood_OrangeRust extends Truck_01_Door_Hood {};


//-------------------------------------
class Offroad_02_Door_1_1 extends CarDoor {};
class Offroad_02_Door_1_2 extends CarDoor {};
class Offroad_02_Door_2_1 extends CarDoor {};
class Offroad_02_Door_2_2 extends CarDoor {};
class Offroad_02_Trunk extends CarDoor {};

class Offroad_02_Hood extends CarDoor
{
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionDetach);
		RemoveAction(ActionAttachOnSelection);
	}
};

class Offroad_02_Door_1_1_Rust extends CarDoor {};
class Offroad_02_Door_1_2_Rust extends CarDoor {};
class Offroad_02_Door_2_1_Rust extends CarDoor {};
class Offroad_02_Door_2_2_Rust extends CarDoor {};
class Offroad_02_Trunk_Rust extends CarDoor {};

//-------------------------------------
class CarRadiator extends InventoryItemSuper
{
	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}

	override void EEKilled(Object killer)
	{
		if ( GetGame().IsServer() )
		{
			Car car;
			EntityAI parent = GetHierarchyParent();
			
			Class.CastTo( car, parent );

		
			if ( car )
			{
				float amount = car.GetFluidFraction( CarFluid.COOLANT );
				float newAmount = Math.RandomFloat( amount * 0.2, amount * 0.75 );

				car.Leak( CarFluid.COOLANT, newAmount );
			}
		}
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
	}
};

class TruckRadiator extends CarRadiator {};

class TruckExhaust extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttach);
	}
};

class EngineBelt extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttach);
	}
};

class BrakeFluid extends ItemBase {};

class EngineOil extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionFillOil);
	}
};

class TireRepairKit extends ItemBase {};

class HeadlightH7 extends ItemBase
{
	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionAttach); //REMOVING DUPLICATE METHODS
		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
	}
};

class HeadlightH7_Box extends Box_Base {};

class WindscreenBox extends ItemBase {};

class GlowPlug extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
	}
};

class SparkPlug extends ItemBase
{
	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionAttach); //REMOVING DUPLICATE METHODS
		AddAction(ActionAttachOnSelection);
		//AddAction(ActionInsertSparkplug); //DEPRECATED, not fully functionnal DO NOT USE
		AddAction(ActionDetach);
	}
};


//-----------------------------------------------------------------------------
class Clothing_Base extends ItemBase
{
	float m_ItemVisibilityModifier;
	
	void Clothing_Base()
	{
		m_ItemVisibilityModifier = ConfigGetFloat("visibilityModifier");
	}
	
	void ~Clothing_Base()
	{
		HandleVoiceEffect(false, PlayerBase.Cast(GetHierarchyRootPlayer()));
	}
	
	override bool IsClothing()
	{
		return true;
	}

	float GetItemVisibility()
	{
		return m_ItemVisibilityModifier;
	}
	
	void UpdateNVGStatus(PlayerBase player, bool attaching = false, bool force_disable = false)
	{
		NVGoggles NVGAttachment;
		NVGAttachment = NVGoggles.Cast(FindAttachmentBySlotName("NVG"));
		bool has_nvg_slot;
		for (int i = 0; i < GetInventory().GetAttachmentSlotsCount(); i++)
		{
			has_nvg_slot = GetInventory().GetAttachmentSlotId(i) == InventorySlots.GetSlotIdFromString("NVG");
			if (has_nvg_slot)
				break;
		}
		
		if ( player && has_nvg_slot )
		{
			if ( NVGAttachment )
			{
				NVGAttachment.LoweredCheck();
				
				if ( attaching && NVGAttachment.m_Strap && NVGAttachment.m_IsLowered )
				{
					NVGAttachment.SetPlayer(player);
					player.SetNVGLowered(true);
					
					if ( player.IsControlledPlayer() )
					{
						if ( NVGAttachment.IsWorking() )
						{
							player.AddActiveNV(NVTypes.NV_GOGGLES);
							//player.RemoveActiveNV(NVTypes.NV_GOGGLES_OFF);
						}
						else
						{
							//player.AddActiveNV(NVTypes.NV_GOGGLES_OFF);
							player.RemoveActiveNV(NVTypes.NV_GOGGLES);
						}
					}
				}
				else
				{
					NVGAttachment.SetPlayer(null);
					player.SetNVGLowered(false);
					
					if ( player.IsControlledPlayer() )
					{
						player.RemoveActiveNV(NVTypes.NV_GOGGLES);
						//player.RemoveActiveNV(NVTypes.NV_GOGGLES_OFF);
					}
				}
			}
			else if ( player.IsNVGWorking() && force_disable )
			{
				player.SetNVGLowered(false);
				
				if ( player.IsControlledPlayer() )
				{
					player.RemoveActiveNV(NVTypes.NV_GOGGLES);
					//player.RemoveActiveNV(NVTypes.NV_GOGGLES_OFF);
				}
			}
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if ( slot_id == InventorySlots.HEADGEAR || slot_id == InventorySlots.MASK )
			HandleVoiceEffect(true, PlayerBase.Cast(GetHierarchyRootPlayer()));
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		HandleVoiceEffect(false, PlayerBase.Cast(parent));
	}
	
	//! Returns if this entity obsructs player's voice
	bool IsObstructingVoice()
	{
		return false;
	}
	
	//! This items has effect on player's voice
	// Better name for this would have been "MufflePlayer" or "ChangeVoiceEffect" (Too late to change in case a mod already uses this)
	void MutePlayer(PlayerBase player, bool state)
	{
		if ( GetGame() )
		{	
			if (( GetGame().IsServer() && GetGame().IsMultiplayer() ) || ( GetGame().GetPlayer() == player ))
			{
				GetGame().SetVoiceEffect(player, GetVoiceEffect(), state);
			}
		}
	}
	
	//!
	void HandleVoiceEffect(bool enable, PlayerBase player)
	{
		if ( IsObstructingVoice() && player )
			MutePlayer(player, enable);
	}
	
	//! The voice effect this item uses, default is 0 (none)
	// Options: (VoiceEffectMumbling = 1, VoiceEffectExtortion = 2, VoiceEffectObstruction = 4)
	int GetVoiceEffect()
	{
		return 0;
	}
	
	bool IsGasMask()
	{
		return false;
	}
};

//-----------------------------------------------------------------------------
class Box_Base extends InventoryItemSuper
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionUnpackBox);
	}
};

//-----------------------------------------------------------------------------
class ItemGrenade extends InventoryItemSuper
{
	/**@fn		SetPinned
	 * @brief	sets grenade to state GR_PINNED (if the state transition is possible)
	 * @return	true if state already set or if state transition performed, false otherwise
	 **/
	proto native bool SetPinned ();
	/**@fn		SetUnpinned
	 * @brief	sets grenade to state GR_UNPINNED (if the state transition is possible)
	 * @return	true if state already set or if state transition performed, false otherwise
	 **/
	proto native bool SetUnpinned ();
	/**@fn		SetIgnited
	 * @brief	sets grenade to state GR_TRIGGERED (if the state transition is possible)
	 * @return	true if state already set or if state transition performed, false otherwise
	 **/
	proto native bool SetIgnited ();
	/**@fn		IsPinned
	 * @return	true if grenade is pinned
	 **/
	proto native bool IsPinned ();
	/**@fn		IsActivated
	 * @return	true if grenade is GR_TRIGGERED or GR_EXPLODED
	 **/
	proto native bool IsActivated ();
};

typedef ItemGrenade GrenadeBase;

//-----------------------------------------------------------------------------
class ItemMap extends InventoryItemSuper
{
	//protected ref array<vector,int,int,string> 	m_MarkerArray;
	protected ref array<ref MapMarker> 	m_MapMarkerArray;
	
	string	m_TextureClosed;
	string 	m_TextureOpened;
	string 	m_TextureLegend;
	string 	m_DisplayName;
	string 	m_Description;
	
	void ItemMap()
	{
		InitMapState();
	}
	
	void InitMapState()
	{
		string path = "CfgWorlds " + GetGame().GetWorldName();
		
		GetGame().ConfigGetText(path + " mapDisplayNameKey",m_DisplayName);
		GetGame().ConfigGetText(path + " mapDescriptionKey",m_Description);
		GetGame().ConfigGetText(path + " mapTextureClosed",m_TextureClosed);
		GetGame().ConfigGetText(path + " mapTextureOpened",m_TextureOpened);
		GetGame().ConfigGetText(path + " mapTextureLegend",m_TextureLegend);
		
		SetObjectTexture(0,m_TextureClosed);
		SetObjectTexture(1,m_TextureOpened);
		SetObjectTexture(2,m_TextureLegend);
		
		//m_MarkerArray = new ref array<vector,int,int,string>;
		m_MapMarkerArray = new ref array<ref MapMarker>;
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
		{
			SyncMapMarkers();
		}
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner,new_owner);
		
		SetMapStateOpen(false, PlayerBase.Cast(old_owner));
	}
	
	//! displays open/closed selections; 1 == opened
	void SetMapStateOpen(bool state, PlayerBase player)
	{
		if (player)
		{
			player.SetMapOpen(state);
			if (state)
				player.OnItemInHandsChanged();
		}
		
		if (state)
		{
			ShowSelection("map_opened");
			HideSelection("map_closed");
		}
		else
		{
			ShowSelection("map_closed");
			HideSelection("map_opened");
		}
	}
	
	bool GetMapStateAnimation()
	{
		if (GetAnimationPhase("map_opened") == 0)
			return true;
		return false;
	}
	
	void SyncMapMarkers()
	{
		if (m_MapMarkerArray.Count() <= 0)
			return;
		
		PlayerIdentity pid; 
		//Param1<ref array<vector,int,int,string>> params = new Param1<ref array<vector,int,int,string>>( m_MarkerArray );
		Param1<ref array<ref MapMarker>> params = new Param1<ref array<ref MapMarker>>( m_MapMarkerArray );
		
		if (GetGame().IsServer() && GetHierarchyRootPlayer()) //TODO do we need PlayerIdentity here?
		{
			pid = GetHierarchyRootPlayer().GetIdentity();
			RPCSingleParam(ERPCs.RPC_SEND_MAP_MARKERS,params,true,pid);
		}
		else
		{
			RPCSingleParam(ERPCs.RPC_SEND_MAP_MARKERS,params,true);
		}
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		//array<vector,int,int,string> tmp_array = new array<vector,int,int,string>;
		//Param1<ref array<vector,int,int,string>> param = new Param1<ref array<vector,int,int,string>>(m_MarkerArray);
		Param1<ref array<ref MapMarker>> params = new Param1<ref array<ref MapMarker>>( m_MapMarkerArray );
		
		if (rpc_type == ERPCs.RPC_SEND_MAP_MARKERS)
		{
			if (ctx.Read(params))
			{
				//Print("Map | OnRPC | m_MapMarkerArray_new count: " + m_MapMarkerArray_new.Count());
			}
		}
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		if (version >= 108 && !ctx.Read(m_MapMarkerArray))
		{
			return false;
		}
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_MapMarkerArray);		
	}
	
	override bool NameOverride(out string output)
	{
		output = m_DisplayName;
		
		return true;
	}
	
	override bool DescriptionOverride(out string output)
	{
		output = m_Description;
		
		return true;
	}
	
	/*array<vector,int,int,string> GetMarkerArray()
	{
		return m_MarkerArray;
	}*/
	
	void InsertMarker(vector pos, string text, int color, int idx)
	{
		ref MapMarker marker = MapMarker(pos,text,color,idx);
		m_MapMarkerArray.Insert(marker);
	}
	
	array<ref MapMarker> GetMarkerArray()
	{
		return m_MapMarkerArray;
	}
};

class MapMarker
{
	protected vector 	m_Position;
	protected int  		m_Color;
	protected int 		m_IconIdx;
	protected string 	m_Text;
	
	void MapMarker(vector pos, string text, int color, int idx)
	{
		m_Position = pos;
		m_Text = text;
		m_Color = color;
		m_IconIdx = idx;
	}
	
	vector GetMarkerPos()
	{
		return m_Position;
	}
	
	string GetMarkerText()
	{
		return m_Text;
	}
	
	int GetMarkerColor()
	{
		return m_Color;
	}
	
	int GetMarkerIcon()
	{
		return m_IconIdx;
	}
};