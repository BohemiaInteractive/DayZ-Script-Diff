enum FireplaceFireState
{
	NO_FIRE				= 1,
	START_FIRE			= 2,
	SMALL_FIRE			= 3,	
	NORMAL_FIRE			= 4,
	END_FIRE			= 5,
	EXTINGUISHING_FIRE	= 6,
	EXTINGUISHED_FIRE	= 7,
	REIGNITED_FIRE		= 8,
	
	COUNT				= 9			//for net sync purposes
}

enum FirePlaceFailure
{
	WIND			= 0,
	WET				= 1,	
}

class FireplaceBase extends ItemBase
{
	//State params
	protected bool m_IsBurning 						= false;   
	protected bool m_HasAshes						= false;
	protected bool m_IsOven							= false;
	protected bool m_HasStoneCircle 				= false;
	protected bool m_RoofAbove 						= false;
	protected bool m_NoIgnite 						= false;
	protected int m_OvenAttachmentsLockState 		= -1;
	protected FireplaceFireState m_FireState	 	= FireplaceFireState.NO_FIRE;
	protected FireplaceFireState m_LastFireState 	= FireplaceFireState.NO_FIRE;		//for synchronization purposes
	protected vector m_HalfExtents;
	
	//Fireplace params
	protected float m_TemperatureLossMP				= 1.0;		//! determines how fast will the fireplace loose its temperature when cooling (lower is better)
	protected float m_FuelBurnRateMP				= 1.0;		//! determines how fast will the fuel item burn before spending (lower is better)
	
	//cooking
	protected ref Cooking m_CookingProcess;
	
	//
	const float PARAM_SMALL_FIRE_TEMPERATURE 		= 150;		//! maximum fireplace temperature of a small fire (degree Celsius)
	const float PARAM_NORMAL_FIRE_TEMPERATURE 		= 1000;		//! maximum fireplace temperature of a normal fire (degree Celsius)
	const float PARAM_MIN_FIRE_TEMPERATURE 			= 30;		//! minimum fireplace temperature under which the fireplace is inactive (degree Celsius)
	const float	PARAM_TEMPERATURE_INCREASE 			= 9;		//! how much will temperature increase when fireplace is burning (degree Celsius)
	const float	PARAM_TEMPERATURE_DECREASE 			= 20;		//! how much will temperature decrease when fireplace is cooling (degree Celsius)
	const float	PARAM_MAX_WET_TO_IGNITE 			= 0.2;		//! maximum wetness value when the fireplace can be ignited
	const float PARAM_MIN_TEMP_TO_REIGNITE 			= 30;		//! minimum fireplace temperature under which the fireplace can be reignited using air only (degree Celsius)
	const float	PARAM_IGNITE_RAIN_THRESHOLD 		= 0.1;		//! maximum rain value when the fireplace can be ignited
	const float	PARAM_BURN_WET_THRESHOLD 			= 0.40;		//! maximum wetness value when the fireplace is able to burn
	const float	PARAM_WET_INCREASE_COEF 			= 0.02;		//! value for calculating of  wetness that fireplace gain when raining
	const float	PARAM_WET_HEATING_DECREASE_COEF 	= 0.01;		//! value for calculating wetness loss during heating process
	const float	PARAM_WET_COOLING_DECREASE_COEF 	= 0.002;	//! value for calculating wetness loss during cooling process
	const float	PARAM_FIRE_CONSUM_RATE_AMOUNT		= 0.5;		//! base value of fire consumption rate (how many base energy will be spent on each update)
	const float	PARAM_BURN_DAMAGE_COEF				= 5.0;		//! value for calculating damage on items located in fireplace cargo
	const float	PARAM_ITEM_HEAT_TEMP_INCREASE_COEF	= 10;		//! value for calculating temperature increase on each heat update interval (degree Celsius)
	const float	PARAM_ITEM_HEAT_MIN_TEMP			= 40;		//! minimum temperature for items that can be heated in fireplace cargo or as attachments (degree Celsius)
	const float PARAM_MAX_ITEM_HEAT_TEMP_INCREASE	= 200;		//! maximum value of temperature of items in fireplace when heating (degree Celsius)
	const float PARAM_FULL_HEAT_RADIUS 				= 2.0;		//! radius in which objects are fully heated by fire
	const float PARAM_HEAT_RADIUS 					= 4.0;		//! radius in which objects are heated by fire
	const float PARAM_HEAT_THROUGH_AIR_COEF			= 0.1;		//! DEPRECATED value for calculation of heat transfered from fireplace through air to player (environment)
	const float PARAM_MAX_TRANSFERED_TEMPERATURE	= 25;		//! maximum value for temperature that will be transfered to player (environment)
	//! 
	const int 	MIN_STONES_TO_BUILD_CIRCLE			= 8;		//! minimum amount of stones for circle
	const int 	MIN_STONES_TO_BUILD_OVEN			= 16;		//! minimum amount of stones for oven
	const int 	MAX_TEMPERATURE_TO_DISMANTLE_OVEN	= 40;		//! maximum temperature for dismantling oven
	const float	MIN_CEILING_HEIGHT 					= 5;		//! min height of ceiling for fire to be ignited
	//
	const float TEMPERATURE_LOSS_MP_DEFAULT			= 1.0;
	const float TEMPERATURE_LOSS_MP_STONES			= 0.90;		//10% boost
	const float TEMPERATURE_LOSS_MP_OVEN			= 0.75;		//25% boost

	const float FUEL_BURN_RATE_DEFAULT				= 1.0;
	const float FUEL_BURN_RATE_STONES				= 0.92;
	const float FUEL_BURN_RATE_OVEN					= 0.85;
	
	//! cooking
	const float PARAM_COOKING_TEMP_THRESHOLD		= 100;		//! temperature threshold for starting coooking process (degree Celsius)
	const float PARAM_COOKING_EQUIP_MAX_TEMP		= 250;		//! maximum temperature of attached cooking equipment (degree Celsius)
	const float PARAM_COOKING_EQUIP_TEMP_INCREASE	= 10;		//! how much will temperature increase when attached on burning fireplace (degree Celsius)
	//! 
	const int 	TIMER_HEATING_UPDATE_INTERVAL 		= 3;		//! update interval duration of heating process (seconds)
	const int 	TIMER_COOLING_UPDATE_INTERVAL 		= 2;		//! update interval duration of cooling process (seconds)
	//! direct cooking slots
	const int   DIRECT_COOKING_SLOT_COUNT			= 3;
	const float	DIRECT_COOKING_SPEED				= 1.5;		// per second
	const int   SMOKING_SLOT_COUNT					= 4;
	const float SMOKING_SPEED 						= 1;		// per second

	// stage lifetimes
	const int   LIFETIME_FIREPLACE_STONE_CIRCLE		= 172800;
	const int   LIFETIME_FIREPLACE_STONE_OVEN		= 604800;
	
	const float IGNITE_WIND_THRESHOLD				= 0.8; 		//fireplace can not be ignited above this multiple of max wind

	//Timers
	protected ref Timer m_HeatingTimer;
	protected ref Timer m_CoolingTimer;
	
	// Light entity
	protected FireplaceLight m_Light;
	protected float m_LightDistance 				= 2000;
	
	//Attachments
	protected ItemBase m_CookingEquipment;
	protected ItemBase m_DirectCookingSlots[DIRECT_COOKING_SLOT_COUNT];
	protected ItemBase m_SmokingSlots[SMOKING_SLOT_COUNT];
	protected ref FireConsumable m_ItemToConsume;
	
	//Particles - default for FireplaceBase
	protected int PARTICLE_FIRE_START 			= ParticleList.CAMP_FIRE_START;
	protected int PARTICLE_OVEN_FIRE_START 		= ParticleList.CAMP_STOVE_FIRE_START;
	protected int PARTICLE_SMALL_FIRE 			= ParticleList.CAMP_SMALL_FIRE;
	protected int PARTICLE_NORMAL_FIRE			= ParticleList.CAMP_NORMAL_FIRE;
	protected int PARTICLE_OVEN_FIRE 			= ParticleList.CAMP_STOVE_FIRE;
	protected int PARTICLE_SMALL_SMOKE 			= ParticleList.CAMP_SMALL_SMOKE;
	protected int PARTICLE_NORMAL_SMOKE			= ParticleList.CAMP_NORMAL_SMOKE;
	protected int PARTICLE_FIRE_END 			= ParticleList.CAMP_FIRE_END;
	protected int PARTICLE_OVEN_FIRE_END 		= ParticleList.CAMP_STOVE_FIRE_END;
	protected int PARTICLE_STEAM_END			= ParticleList.CAMP_STEAM_2END;
	protected int PARTICLE_STEAM_EXTINGUISHING	= ParticleList.CAMP_STEAM_EXTINGUISH_START;
	protected int PARTICLE_NO_IGNITE_WIND		= ParticleList.CAMP_NO_IGNITE_WIND;
	//
	protected Particle m_ParticleFireStart;
	protected Particle m_ParticleSmallFire;
	protected Particle m_ParticleNormalFire;
	protected Particle m_ParticleSmallSmoke;
	protected Particle m_ParticleNormalSmoke;
	protected Particle m_ParticleFireEnd;
	protected Particle m_ParticleSteamEnd;
	protected Particle m_ParticleSteamExtinguishing;
	
	//Sounds
	const string SOUND_FIRE_HEAVY 				= "HeavyFire_SoundSet";
	const string SOUND_FIRE_LIGHT				= "LightFire_SoundSet";
	const string SOUND_FIRE_NO_FIRE				= "NoFuelFire_SoundSet";
	const string SOUND_FIRE_EXTINGUISHING		= "ExtinguishByWater_SoundSet";
	const string SOUND_FIRE_EXTINGUISHED		= "ExtinguishByWaterEnd_SoundSet";
	
	//
	protected EffectSound m_SoundFireLoop;
	protected EffectSound m_SoundFire;
	//
	
	//Fuel
	//Kindling / Fuel item types
	protected static ref map<typename, ref FireConsumableType> 	m_FireConsumableTypes;
	protected ref map<ItemBase, ref FireConsumable> 			m_FireConsumables;
	
	//Player action / status messages
	const string MESSAGE_IGNITE_IGNIT_ITEM_DAMAGED 		= "You cannot start fire. Fireplace is ruined.";
	const string MESSAGE_IGNITE_NO_KINDLING 			= "There needs to be some kindling to start a fire.";
	const string MESSAGE_IGNITE_TOO_WET 				= "The fireplace is too wet to be ignited.";
	const string MESSAGE_IGNITE_RAIN	 				= "The fire went out because of the rain.";
	const string MESSAGE_IGNITE_WIND	 				= "The fire went out because of the wind.";
	const string MESSAGE_IGNITE_IN_WATER 				= "I cannot ignite the fireplace in the water.";
	const string MESSAGE_IGNITE_UNDER_LOW_ROOF 			= "I cannot ignite the fireplace here, the is not enough room.";
	const string MESSAGE_REIGNITE_NO_KINDLING 			= "There needs to be some kindling to start a fire.";
	const string MESSAGE_REIGNITE_RAIN	 				= "Cannot reignite fireplace in rain.";
	const string MESSAGE_REIGNITE_TOO_WET 				= "The fireplace is too wet to be reignited.";
	const string MESSAGE_BURY_ASHES_FAILED_NOT_EMPTY	= "Cannot bury ashes. There are sill items in it.";
	const string MESSAGE_BURY_ASHES_FAILED_SURFACE		= "Cannot bury ashes on hard surface.";
	const string MESSAGE_BURY_ASHES_FAILED_BURNING		= "Cannot bury ashes, the fireplace is still burning.";
	const string MESSAGE_BURY_ASHES_FAILED_TOO_HOT		= "Cannot bury ashes, the fireplace is too hot.";
	const string MESSAGE_CANNOT_BUILD_OVEN				= "Cannot build oven.";
	const string MESSAGE_CANNOT_DISMANTLE_OVEN			= "Cannot dismantle oven.";
	
	//Visual animations
	const string ANIMATION_ASHES 			= "Ashes";
	const string ANIMATION_KINDLING			= "Kindling";
	const string ANIMATION_STICKS			= "Sticks";
	const string ANIMATION_WOOD 			= "Wood";
	const string ANIMATION_BURNT_WOOD 		= "BurntWood";
	const string ANIMATION_STONE			= "Stone";
	const string ANIMATION_STONES 			= "StonesCircle";
	const string ANIMATION_TRIPOD 			= "Tripod";
	const string ANIMATION_OVEN 			= "Oven";
	const string ANIMATION_INVENTORY	 	= "Inventory";
	const string ANIMATION_COOKWARE_HANDLE 	= "handleRotate";
	const string ANIMATION_CAULDRON_HANDLE 	= "handle1Rotate";

	
	//Visual textures and materials
	const string MATERIAL_FIREPLACE_GLOW 		= "dz\\gear\\cooking\\data\\stoneground.rvmat";
	const string MATERIAL_FIREPLACE_NOGLOW 		= "dz\\gear\\cooking\\data\\stonegroundnoemit.rvmat";
	
	//Attachment types
	//Fuel
	typename ATTACHMENT_STICKS			= WoodenStick;
	typename ATTACHMENT_FIREWOOD		= Firewood;
	//Kindling
	typename ATTACHMENT_RAGS 			= Rag;
	typename ATTACHMENT_BANDAGE 		= BandageDressing;
	typename ATTACHMENT_BOOK 			= ItemBook;
	typename ATTACHMENT_BARK_OAK 		= Bark_Oak;
	typename ATTACHMENT_BARK_BIRCH 		= Bark_Birch;
	typename ATTACHMENT_PAPER 			= Paper;
	typename ATTACHMENT_GIFTWRAP		= GiftWrapPaper;
	typename ATTACHMENT_PUNCHEDCARD		= PunchedCard;
	//Other
	typename ATTACHMENT_TRIPOD 			= Tripod;
	typename ATTACHMENT_COOKINGSTAND	= CookingStand;
	typename ATTACHMENT_STONES 			= Stone;
	typename ATTACHMENT_COOKING_POT 	= Pot;
	typename ATTACHMENT_FRYING_PAN 		= FryingPan;
	typename ATTACHMENT_CAULDRON	 	= Cauldron;
	//
	const string OBJECT_CLUTTER_CUTTER 	= "ClutterCutterFireplace";
	protected Object m_ClutterCutter;
	
	//area damage
	protected ref AreaDamageManager 	m_AreaDamage;
	
	//quantity
	protected float m_TotalEnergy;
	
	// Noise
	ref NoiseParams m_NoisePar;
	private bool m_CanNoise = true; // Flip every fireplace update to limit amount of noise generation
	
	// UniversalTemperatureSource
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaFireplace m_UTSLFireplace;
	
	//================================================================
	// INIT / STORE LOAD-SAVE
	//================================================================
	//Constructor
	protected void FireplaceBase()
	{
		//STATIC: define kindling types
		if (!m_FireConsumableTypes)
		{
			m_FireConsumableTypes = new ref map<typename, ref FireConsumableType>();
			m_FireConsumableTypes.Insert(ATTACHMENT_RAGS, 			new FireConsumableType(ATTACHMENT_RAGS, 		14, 	true,	"Rags"));
			m_FireConsumableTypes.Insert(ATTACHMENT_BANDAGE, 		new FireConsumableType(ATTACHMENT_BANDAGE, 		14, 	true,	"MedicalBandage"));
			m_FireConsumableTypes.Insert(ATTACHMENT_BOOK, 			new FireConsumableType(ATTACHMENT_BOOK, 		36, 	true,	"Book"));
			m_FireConsumableTypes.Insert(ATTACHMENT_BARK_OAK, 		new FireConsumableType(ATTACHMENT_BARK_OAK, 	20, 	true,	"OakBark"));
			m_FireConsumableTypes.Insert(ATTACHMENT_BARK_BIRCH, 	new FireConsumableType(ATTACHMENT_BARK_BIRCH, 	14, 	true,	"BirchBark"));
			m_FireConsumableTypes.Insert(ATTACHMENT_PAPER, 			new FireConsumableType(ATTACHMENT_PAPER, 		10, 	true,	"Paper"));
			m_FireConsumableTypes.Insert(ATTACHMENT_GIFTWRAP, 		new FireConsumableType(ATTACHMENT_GIFTWRAP, 	10, 	true,	"GiftWrapPaper"));
			m_FireConsumableTypes.Insert(ATTACHMENT_PUNCHEDCARD,	new FireConsumableType(ATTACHMENT_PUNCHEDCARD, 	10, 	true,	"PunchedCard"));
			
			//define fuel types
			m_FireConsumableTypes.Insert( ATTACHMENT_STICKS, 		new FireConsumableType( ATTACHMENT_STICKS, 		40, 	false,	"WoodenStick"));
			m_FireConsumableTypes.Insert( ATTACHMENT_FIREWOOD, 		new FireConsumableType( ATTACHMENT_FIREWOOD, 	100, 	false,	"Firewood"));
		}

		//calculate total energy
		CalcAndSetTotalEnergy();
		
		//define fuel / kindling items (fire consumables)
		m_FireConsumables = new ref map<ItemBase, ref FireConsumable>;
		
		//default burning parameters
		SetTemperatureLossMP( TEMPERATURE_LOSS_MP_DEFAULT );
		SetFuelBurnRateMP( FUEL_BURN_RATE_DEFAULT );
		
		//synchronized variables
		RegisterNetSyncVariableBool( "m_IsBurning" );
		RegisterNetSyncVariableBool( "m_HasAshes" );
		RegisterNetSyncVariableBool( "m_IsOven" );
		RegisterNetSyncVariableBool( "m_HasStoneCircle" );
		RegisterNetSyncVariableBool( "m_RoofAbove" );
		RegisterNetSyncVariableInt( "m_FireState", FireplaceFireState.NO_FIRE, FireplaceFireState.COUNT );
		RegisterNetSyncVariableBool( "m_IsSoundSynchRemote" );
		RegisterNetSyncVariableBool( "m_IsPlaceSound" );
		RegisterNetSyncVariableBool( "m_NoIgnite" );
		
		m_HalfExtents = vector.Zero;
	}
	
	protected void ~FireplaceBase()
	{
		if (GetLightEntity())
		{
			GetLightEntity().FadeOut();
		}
	}

	override void EEInit()
	{
		super.EEInit();

		//refresh visual on init
		RefreshFireplaceVisuals();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_UpdateInterval	= 3;
			m_UTSSettings.m_Updateable		= true;
			m_UTSSettings.m_AffectStat		= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= PARAM_NORMAL_FIRE_TEMPERATURE;
			m_UTSSettings.m_TemperatureCap	= PARAM_MAX_TRANSFERED_TEMPERATURE;
			m_UTSSettings.m_RangeFull		= PARAM_FULL_HEAT_RADIUS;
			m_UTSSettings.m_RangeMax		= PARAM_HEAT_RADIUS;
			
			m_UTSLFireplace= new UniversalTemperatureSourceLambdaFireplace();
			m_UTSLFireplace.SetSmallFireplaceTemperatureMax(PARAM_SMALL_FIRE_TEMPERATURE);
			m_UTSLFireplace.SetNormalFireplaceTemperatureMax(PARAM_NORMAL_FIRE_TEMPERATURE);

			m_UTSource = new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLFireplace);
		}		
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		//cookware
		switch (item.Type())
		{
		case ATTACHMENT_CAULDRON:
		case ATTACHMENT_COOKING_POT:
		case ATTACHMENT_FRYING_PAN:
			SetCookingEquipment(ItemBase.Cast(item));
		break;
		}
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		//refresh physics after location change (with delay)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(RefreshFireplacePhysics);
	}
	
	override void OnAttachmentRuined(EntityAI attachment)
	{
		switch (attachment.Type())
		{
		case ATTACHMENT_TRIPOD:
		case ATTACHMENT_COOKINGSTAND:
			int slot = InventorySlots.GetSlotIdFromString("CookingEquipment");
			EntityAI ent = GetInventory().FindAttachment(slot);
			if (ent)
			{
				switch (ent.Type())
				{
				case ATTACHMENT_COOKING_POT:
				case ATTACHMENT_CAULDRON:
				case ATTACHMENT_FRYING_PAN:
					vector direction = ent.GetDirection();
					float dot = vector.Dot(direction, vector.Forward);
					
					float angle = Math.Acos(dot);	
					if (direction[0] < 0)
					{
						angle = -angle;
					}
			
					float cos = Math.Cos(angle);
					float sin = Math.Sin(angle);
					GetInventory().DropEntityInBounds(InventoryMode.SERVER, this, ent, "2 0 2", angle, cos, sin);
				break;
				}
			}

			attachment.Delete();
		break;
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		SetFireState(FireplaceFireState.NO_FIRE);

		// cleanup particles (for cases of leaving burning fplace and going back outside of network bubble)
		#ifndef SERVER
		StopAllParticlesAndSounds();
		#endif
	}	
		
	//on store save/load
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave(ctx);

		//Save ashes state
		ctx.Write( m_HasAshes );
		
		//Save temperature loss MP
		ctx.Write( m_TemperatureLossMP );

		//Save fuel burn rate MP
		ctx.Write( m_FuelBurnRateMP );
		
		//Save burning state
		ctx.Write( m_IsBurning );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;

		//--- Fireplace data ---
		//Load ashes state
		if ( !ctx.Read( m_HasAshes ) )
		{
			m_HasAshes = false;			//set default
			return false;
		}
		
		//Load temperature loss MP
		if ( !ctx.Read( m_TemperatureLossMP ) )
		{
			m_TemperatureLossMP = 1.0;	//set default
			return false;
		}
		
		//Load fuel burn rate MP
		if ( !ctx.Read( m_FuelBurnRateMP ) )
		{
			m_FuelBurnRateMP = 1.0;		//set default
			return false;
		}
		
		//Load burning state, if true start fire
		if ( !ctx.Read( m_IsBurning ) )
		{
			m_IsBurning = false;		//set default
			return false;
		}
		//---
		
		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
				
		Synchronize();
		
		//start fire
		if (IsBurning() )
		{
			if (GetGame() && GetGame().IsServer()) 
			{
				StartFire(true);			//will be auto-synchronized when starting fire
			}
		}		
	}		
	
	//================================================================
	// SYNCHRONIZATION
	//================================================================	
	void Synchronize()
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			SetSynchDirty();
			
			if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			{
				//Refresh visuals (on server)
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( RefreshFireplaceVisuals );
			}
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		RefreshFireplaceVisuals();
		RefreshFireParticlesAndSounds(false);
		
		if (IsPlaceSound())
		{
			PlayPlaceSound();
		}
		
		if (IsBaseFireplace() && !IsOven())
		{
			if (m_IsBurning && !m_AreaDamage)
			{
				CreateAreaDamage();
			}
			else if (!m_IsBurning && m_AreaDamage)
			{
				DestroyAreaDamage();
			}
		}
	}
	
	//================================================================
	// BASE
	//================================================================
	
	//States
	//check fireplace types
	override bool IsFireplace()
	{
		return true;
	}
	
	bool IsBaseFireplace()
	{
		return false;
	}
	
	bool IsBarrelWithHoles()
	{
		return false;
	}
	
	bool IsFireplaceIndoor()
	{
		return false;
	}

	bool IsIndoorOven()
	{
		return false;
	}
	
	override bool CanHaveWetness()
	{
		return true;
	}
	
	override bool CanHaveTemperature()
	{
		return true;
	}
	
	//cooking equipment
	ItemBase GetCookingEquipment()
	{
		return m_CookingEquipment;
	}
	
	void SetCookingEquipment(ItemBase equipment)
	{
		m_CookingEquipment = equipment;
	}
	
	void ClearCookingEquipment(ItemBase pItem)
	{
		if (m_CookingProcess)
		{
			m_CookingProcess.TerminateCookingSounds(pItem);
		}

		SetCookingEquipment(null);
	}
	
	//! DEPRECATED
	void ClearCookingEquipment()
	{
		SetCookingEquipment(null);
	}

	bool DirectCookingSlotsInUse()
	{
		for (int i = 0; i < DIRECT_COOKING_SLOT_COUNT; i++)
		{
			if ( m_DirectCookingSlots[i] )
			{
				return true;
			}
		}
		return false;
	}

	bool SmokingSlotsInUse()
	{
		for (int i = 0; i < SMOKING_SLOT_COUNT; i++)
		{
			if ( m_SmokingSlots[i] )
			{
				return true;
			}
		}
		return false;
	}

	//Destroy
	void DestroyFireplace()
	{
		//delete object
		DeleteSafe();
	}
	
	override bool IsPrepareToDelete()
	{
		return IsEmpty() && !IsBurning() && !HasAshes();
	}
	
	override void EECargoOut(EntityAI item)
	{
		super.EECargoOut(item);
		CheckForDestroy();	
	}
	
	//================================================================
	// VISUAL
	//================================================================
	//Fireplace visual
	void RefreshFireplaceVisuals()
	{
		if (IsHologram())
		{
			return;
		}
		
		// Light
		#ifndef SERVER
		if (IsBurning() ) 
		{
			//turn light on
			if (!GetLightEntity())
			{
				SetLightEntity(FireplaceLight.Cast(ScriptedLightBase.CreateLight(FireplaceLight, GetPosition(), 20)));
				GetLightEntity().AttachOnMemoryPoint(this, "light");
			}

			if (GetLightEntity() && GetLightEntity().GetBrightness() > 0)
			{
				// change brightness based on the distance of player to the fireplace
				Object player 	= GetGame().GetPlayer();
				float lightDist = m_LightDistance;

				if (IsOven())
				{
					lightDist = 50;
				}

				if (player)
				{
					if (vector.DistanceSq(player.GetPosition(), this.GetPosition()) > lightDist * lightDist)
					{
						GetLightEntity().FadeBrightnessTo(0, 5);
					}
					else
					{
						GetLightEntity().FadeBrightnessTo(FireplaceLight.m_FireplaceBrightness, 5);
					}
				}
				
				// The following solves an issue with the light point clipping through narrow geometry
				if (IsItemTypeAttached(ATTACHMENT_STONES) || IsBarrelWithHoles() || IsFireplaceIndoor())
				{
					GetLightEntity().SetInteriorMode();
				}
				else
				{
					GetLightEntity().SetExteriorMode();
				}
			}
		}
		else
		{
			//turn light off
			if (GetLightEntity())
			{
				GetLightEntity().FadeOut();
			}
		}
		#endif
		
		//set default burn parameters based on fireplace type
		if (IsBarrelWithHoles() || IsFireplaceIndoor() || IsIndoorOven() || IsOven())
		{
			SetFuelBurnRateMP(FUEL_BURN_RATE_OVEN);
			SetTemperatureLossMP(TEMPERATURE_LOSS_MP_OVEN);
		}
		else if (HasStoneCircle())
		{
			SetFuelBurnRateMP(FUEL_BURN_RATE_STONES);
			SetTemperatureLossMP(TEMPERATURE_LOSS_MP_STONES);
		}
		else
		{
			SetFuelBurnRateMP(FUEL_BURN_RATE_DEFAULT);
			SetTemperatureLossMP(TEMPERATURE_LOSS_MP_DEFAULT);
		}
		
		// Oven attachments
		if (IsOven())
		{
			//lock attachment slots
			if (!m_OvenAttachmentsLockState)
			{
				LockOvenAttachments(true);
			}
		}
		else
		{
			//unlock attachment slots
			if (m_OvenAttachmentsLockState)
			{
				LockOvenAttachments(false);
			}
		}
		
		//VISUAL STATES
		//Fuel state
		if ( GetGame().IsServer() ) // SetAnimationPhase should ideally be called on server only..
		{
			// Sticks state
			if ( IsItemTypeAttached( ATTACHMENT_STICKS ) )
				SetAnimationPhase( ANIMATION_STICKS, 0 );
			else
				SetAnimationPhase( ANIMATION_STICKS, 1 );
			
			// Firewood state
			if ( IsItemTypeAttached( ATTACHMENT_FIREWOOD ) )
			{
				if ( IsBurning() && HasAshes() )
				{
					SetAnimationPhase( ANIMATION_BURNT_WOOD, 0 ); // Because this might not be set under certain circumstances
					SetAnimationPhase( ANIMATION_WOOD, 1 );
				}
				
				if ( IsWet() || !IsBurning() )
				{
					SetAnimationPhase( ANIMATION_BURNT_WOOD, 1 );
					SetAnimationPhase( ANIMATION_WOOD, 0 );
				}
			}
			else
			{
				SetAnimationPhase( ANIMATION_WOOD, 1 );
				SetAnimationPhase( ANIMATION_BURNT_WOOD, 1 );		
			}	
				
			// Kindling state
			if ( GetKindlingCount() != 0 )
				SetAnimationPhase( ANIMATION_KINDLING, 0 );
			else
				SetAnimationPhase( ANIMATION_KINDLING, 1 );
	
			// Ashes state
			if ( HasAshes() )
				SetAnimationPhase( ANIMATION_ASHES, 0 );
			else
				SetAnimationPhase( ANIMATION_ASHES, 1 );

			// Stone variations
			if (HasStones())
			{
				if (HasStoneCircle())
				{
					SetAnimationPhase(ANIMATION_STONES, 0);
					SetAnimationPhase(ANIMATION_STONE, 1);
				}
				else if (IsOven())
				{
					SetAnimationPhase(ANIMATION_OVEN, 0);
					SetAnimationPhase(ANIMATION_STONE, 1);
				}
				else
				{
					//! single stone; everything else hidden
					SetAnimationPhase(ANIMATION_STONES, 1);
					SetAnimationPhase(ANIMATION_OVEN, 1);
					SetAnimationPhase(ANIMATION_STONE, 0);
				}
			}
			else
			{
				SetAnimationPhase(ANIMATION_STONE, 1); //! hide stones stack completely
			}
		}
		
		//refresh physics (with delay)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(RefreshFireplacePhysics);
	}

	protected void SetBurntFirewood()
	{
		if ( GetGame().IsServer() && IsInAnimPhase( ANIMATION_WOOD ) )
		{
			SetAnimationPhase( ANIMATION_WOOD, 1 );
			SetAnimationPhase( ANIMATION_BURNT_WOOD, 0 );				
		}	
	}
	
	//Refresh fireplace object physics
	void RefreshFireplacePhysics();
	
	protected void RefreshFireParticlesAndSounds( bool force_refresh )
	{
		FireplaceFireState fire_state = GetFireState();
		
		if ( m_LastFireState != fire_state || force_refresh )
		{
			switch ( fire_state )
			{
				case FireplaceFireState.START_FIRE:
				{
					//particles
					ParticleFireStartStart();
		
					//sounds
					SoundFireStop();
					SoundFireLightStart();	
					break;
				}
				case FireplaceFireState.SMALL_FIRE:
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStart();
					
					if ( CanShowSmoke() ) 
						ParticleSmallSmokeStart();
					else
						ParticleSmallSmokeStop();
					
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();
					
					ParticleSteamExtinguishingStop();
					
					//sounds
					SoundFireStop();
					SoundFireLightStart();
					break;
				}
				case FireplaceFireState.NORMAL_FIRE:
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
					
					ParticleNormalFireStart();
					
					if ( CanShowSmoke() ) 
						ParticleNormalSmokeStart();
					else
						ParticleNormalSmokeStop();
					
					ParticleSteamExtinguishingStop();
		
					//sounds
					SoundFireStop();
					SoundFireHeavyStart();
					break;
				}
				case FireplaceFireState.END_FIRE:
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
		
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();
		
					ParticleFireEndStart();
					
					//sounds
					SoundFireStop();
					SoundFireNoFireStart();
					break;
				}
				case FireplaceFireState.EXTINGUISHING_FIRE:		//TODO add steam particles when extinguishing
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
		
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();	
		
					ParticleFireEndStop();
					
					ParticleSteamExtinguishingStart();
					
					//sounds
					SoundFireStop();
					SoundFireExtinguishingStart();
					break;			
				}
				case FireplaceFireState.EXTINGUISHED_FIRE:		//TODO add steam particles when fireplace is extinguished
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
		
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();	
		
					ParticleFireEndStop();
					
					ParticleSteamExtinguishingStop();
					
					if ( CanShowSmoke() ) 
						ParticleSteamEndStart();
					else
						ParticleSteamEndStop();
					
					//sounds
					SoundFireStop();
					SoundFireExtinguishedStart();
					SoundFireNoFireStart();
					break;
				}
				case FireplaceFireState.REIGNITED_FIRE:
				{
					//particles
					ParticleFireStartStart();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
		
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();	
		
					ParticleFireEndStop();
					
					ParticleSteamEndStop();
					
					//sounds
					SoundFireStop();
					break;
				}
				case FireplaceFireState.NO_FIRE:
				{
					//particles
					ParticleFireStartStop();
					
					ParticleSmallFireStop();
					ParticleSmallSmokeStop();
		
					ParticleNormalFireStop();
					ParticleNormalSmokeStop();	
		
					ParticleFireEndStop();
					
					ParticleSteamEndStop();
					ParticleSteamExtinguishingStop();
					
					//sounds
					SoundFireStop();
					break;
				}
			}
			
			m_LastFireState = fire_state;
		}
	}
	
	protected void StopAllParticlesAndSounds()
	{
		// go through all fireplace particles
		if ( m_ParticleFireStart )
			StopParticle( m_ParticleFireStart );
		if ( m_ParticleSmallFire )
			StopParticle( m_ParticleSmallFire );
		if ( m_ParticleNormalFire )
			StopParticle( m_ParticleNormalFire );
		if ( m_ParticleSmallSmoke )
			StopParticle( m_ParticleSmallSmoke );
		if ( m_ParticleNormalSmoke )
			StopParticle( m_ParticleNormalSmoke );
		if ( m_ParticleFireEnd )
			StopParticle( m_ParticleFireEnd );
		if ( m_ParticleSteamEnd )
			StopParticle( m_ParticleSteamEnd );
		if ( m_ParticleSteamExtinguishing )
			StopParticle( m_ParticleSteamExtinguishing );

		// go through all fireplace sounds
		if ( m_SoundFireLoop )
			StopSoundSet( m_SoundFireLoop );
		if ( m_SoundFire )
			StopSoundSet( m_SoundFire );
	}

	bool CanShowSmoke()
	{
		return true;
	}
	
	//Fireplace fire intensity
	void RefreshFireState()
	{
		float temperature = GetTemperature();
		
		//if it's burning
		if (IsBurning())
		{
			//Start fire
			if (temperature <= PARAM_MIN_FIRE_TEMPERATURE)
			{
				if ( GetFireState() != FireplaceFireState.START_FIRE )
				{
					SetFireState( FireplaceFireState.START_FIRE );
				}
			}
			//Small fire
			else if (temperature <= PARAM_SMALL_FIRE_TEMPERATURE)
			{
				if ( GetFireState() != FireplaceFireState.SMALL_FIRE )
				{
					SetFireState( FireplaceFireState.SMALL_FIRE );
				}
			}
			//Normal fire
			else if (temperature > PARAM_SMALL_FIRE_TEMPERATURE)
			{
				if ( GetFireState() != FireplaceFireState.NORMAL_FIRE )
				{
					SetFireState( FireplaceFireState.NORMAL_FIRE );
				}
			}			
		}
	}

	//================================================================
	// PARTICLES & SOUNDS
	//================================================================
	//returns true if particle started, false if not
	protected bool PlayParticle( out Particle particle, int particle_type, vector pos, bool worldpos = false )
	{
		if ( !particle && GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{
			if ( !worldpos )
			{
				particle = ParticleManager.GetInstance().PlayOnObject( particle_type, this, pos );
			}
			else
			{
				particle = ParticleManager.GetInstance().PlayInWorld( particle_type, pos );
			}
			
			return true;
		}
		
		return false;
	}
	
	//returns true if particle stopped, false if not
	protected bool StopParticle( out Particle particle )
	{
		if ( particle && GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{
			particle.Stop();
			particle = NULL;
			
			return true;
		}
		
		return false;
	}
	
	//start fire
	protected void ParticleFireStartStart()
	{
		if ( IsOven() )
		{
			PlayParticle( m_ParticleFireStart, PARTICLE_OVEN_FIRE_START, GetFireEffectPosition() );
		}
		else
		{
			PlayParticle( m_ParticleFireStart, PARTICLE_FIRE_START, GetFireEffectPosition() );
		}
	}
	
	protected void ParticleFireStartStop()
	{
		StopParticle( m_ParticleFireStart );
	}
	
	//small fire
	protected void ParticleSmallFireStart()
	{
		if ( IsOven() )
		{
			PlayParticle( m_ParticleSmallFire, PARTICLE_OVEN_FIRE, GetFireEffectPosition() );
		}
		else
		{
			PlayParticle( m_ParticleSmallFire, PARTICLE_SMALL_FIRE, GetFireEffectPosition() );	
		}
	}
	
	protected void ParticleSmallFireStop()
	{
		StopParticle( m_ParticleSmallFire );
	}	
	
	//normal fire
	protected void ParticleNormalFireStart()
	{
		if ( IsOven() )
		{
			PlayParticle( m_ParticleNormalFire, PARTICLE_OVEN_FIRE, GetFireEffectPosition() );
		}
		else
		{
			PlayParticle( m_ParticleNormalFire, PARTICLE_NORMAL_FIRE, GetFireEffectPosition() );
		}
	}
	
	protected void ParticleNormalFireStop()
	{
		StopParticle( m_ParticleNormalFire );
	}
	
	//small smoke
	protected void ParticleSmallSmokeStart()
	{
		PlayParticle( m_ParticleSmallSmoke, PARTICLE_SMALL_SMOKE, GetSmokeEffectPosition() );
				
		//calculate air resistance
		float actual_height;
		if ( GetCeilingHeight( actual_height ) )
		{
			float air_resistance = GetAirResistanceForSmokeParticles( actual_height );
			
			//alter smoke height
			m_ParticleSmallSmoke.SetParameter( -1, EmitorParam.AIR_RESISTANCE, air_resistance );
		}
					
		if ( IsRoofAbove() )
		{
			m_ParticleSmallSmoke.SetParameter( -1, EmitorParam.WIND, false );
		}
	}
	
	protected void ParticleSmallSmokeStop()
	{
		StopParticle( m_ParticleSmallSmoke );
	}
	
	//normal smoke
	protected void ParticleNormalSmokeStart()
	{
		PlayParticle( m_ParticleNormalSmoke, PARTICLE_NORMAL_SMOKE, GetSmokeEffectPosition() );
		
		//calculate air resistance
		float actual_height;
		if ( GetCeilingHeight( actual_height ) )
		{		
			float air_resistance = GetAirResistanceForSmokeParticles( actual_height );
			
			//alter smoke height
			m_ParticleNormalSmoke.SetParameter( -1, EmitorParam.AIR_RESISTANCE, air_resistance );
		}
		
		if ( IsRoofAbove() )
		{
			m_ParticleNormalSmoke.SetParameter( -1, EmitorParam.WIND, false );
		}
	}
	
	protected void ParticleNormalSmokeStop()
	{
		StopParticle( m_ParticleNormalSmoke );
	}
	
	//fire end
	protected void ParticleFireEndStart()
	{
		if ( IsOven() )
		{
			PlayParticle( m_ParticleFireEnd, PARTICLE_OVEN_FIRE_END, GetFireEffectPosition() );
		}
		else
		{
			PlayParticle( m_ParticleFireEnd, PARTICLE_FIRE_END, GetFireEffectPosition() );
		}
	}
	
	protected void ParticleFireEndStop()
	{
		StopParticle( m_ParticleFireEnd );
	}
	
	protected void ParticleFireWindyNoIgniteStart()
	{
		if ( IsOven() )
		{
			PlayParticle( m_ParticleFireEnd, PARTICLE_OVEN_FIRE_END, GetFireEffectPosition() );
		}
		else
		{
			PlayParticle( m_ParticleFireEnd, PARTICLE_NO_IGNITE_WIND, GetFireEffectPosition() );
		}
	}
	
	//steam extinguishing
	protected void ParticleSteamExtinguishingStart()
	{
		PlayParticle( m_ParticleSteamExtinguishing, PARTICLE_STEAM_EXTINGUISHING, GetFireEffectPosition() );
	}
	
	protected void ParticleSteamExtinguishingStop()
	{
		StopParticle(m_ParticleSteamExtinguishing);
	}	

	//steam end
	protected void ParticleSteamEndStart()
	{
		PlayParticle( m_ParticleSteamEnd, PARTICLE_STEAM_END, GetFireEffectPosition() );
	}
	
	protected void ParticleSteamEndStop()
	{
		StopParticle( m_ParticleSteamEnd );
	}
	
	protected void ParticleWetNoIgniteStart()
	{
		PlayParticle( m_ParticleSteamEnd, PARTICLE_STEAM_END, GetFireEffectPosition() );
	}
	
	protected void ParticleWetNoIgniteStop()
	{
		StopParticle( m_ParticleSteamEnd );
	}

	bool GetCeilingHeight( out float actual_height )
	{
		vector from = this.GetPosition();
		vector to = this.GetPosition();
		from[1] = from[1] + 1.0;
		to[1] = to[1] + MIN_CEILING_HEIGHT;
		vector contactPos;
		vector contactDir;
		int contactComponent;
		
		bool hit = DayZPhysics.RaycastRV( from, to, contactPos, contactDir, contactComponent, NULL, NULL, this );
		actual_height = vector.Distance( from, contactPos ) + 1.0;
		
		return hit;
	}
	
	float GetAirResistanceForSmokeParticles( float actual_height )
	{
		float air_res;
		actual_height = Math.Clamp( actual_height, 0, 36 );
		
		air_res = ( 6 - actual_height ) * 0.33;
		air_res = Math.Clamp( air_res, 0, 2 );
		
		return air_res;
	}
	
	//Particle Positions
	//Get local fire and smoke effect position
	protected vector GetFireEffectPosition()
	{
		return Vector( 0, 0.05, 0 );
	}
	
	protected vector GetSmokeEffectPosition()
	{
		return Vector( 0, 0.05, 0 );
	}

	//================================================================
	// SOUNDS
	//================================================================
	//Burning
	//Start
	protected void SoundFireLightStart()
	{
		PlaySoundSetLoop( m_SoundFireLoop, SOUND_FIRE_LIGHT, 1.0, 1.0 );
	}
	
	protected void SoundFireHeavyStart()
	{
		PlaySoundSetLoop( m_SoundFireLoop, SOUND_FIRE_HEAVY, 1.0, 2.0 );
	}

	protected void SoundFireNoFireStart()
	{
		PlaySoundSetLoop( m_SoundFireLoop, SOUND_FIRE_NO_FIRE, 2.0, 2.0 );
	}
	
	protected void SoundFireExtinguishedStart()
	{
		PlaySoundSet( m_SoundFire, SOUND_FIRE_EXTINGUISHED, 0.1, 0.1 );
	}	
	
	protected void SoundFireWetNoIgniteStart()
	{
		PlaySoundSet( m_SoundFire, SOUND_FIRE_EXTINGUISHED, 0.1, 0.1 );
	}	

	protected void SoundFireExtinguishingStart()
	{
		PlaySoundSetLoop( m_SoundFireLoop, SOUND_FIRE_EXTINGUISHING, 1.0, 0.5 );
	}
	
	protected void SoundFireWindyNoIgniteStart()
	{
		PlaySoundSet( m_SoundFire, "ExtinguishByWind_SoundSet", 0, 0 );
	}	
	
	//Stop
	protected void SoundFireStop()
	{
		StopSoundSet( m_SoundFireLoop );
	}
	
	//================================================================
	// FUEL / KINDLING
	//================================================================
	//Add to fire consumables
	protected void AddToFireConsumables( ItemBase item )
	{
		float energy = GetFireConsumableTypeEnergy ( item );
		m_FireConsumables.Insert( item, new FireConsumable ( item, energy ) );
		
		CalcAndSetQuantity();
	}

	protected float GetFireConsumableTypeEnergy( ItemBase item )
	{
		//Kindling
		ref FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( item.Type() );
		if ( fire_consumable_type )
		{
			return fire_consumable_type.GetEnergy();
		}
		
		return 0;
	}

	//Remove from fire consumables
	protected void RemoveFromFireConsumables( FireConsumable fire_consumable )
	{
		if ( fire_consumable )
		{
			m_FireConsumables.Remove( fire_consumable.GetItem() );
			delete fire_consumable;
		}
		
		CalcAndSetQuantity();
	}
	
	protected FireConsumable GetFireConsumableByItem( ItemBase item )
	{
		return m_FireConsumables.Get( item );
	}

	//Set fuel / kindling to consume
	//Sets the item with the lowest energy value as item that will be consumed next
	//Returns reference to set fire consumable
	protected FireConsumable SetItemToConsume()
	{
		if ( m_FireConsumables.Count() == 0 )
		{
			m_ItemToConsume = NULL;
		}
		else
		{
			float energy = 0;
			
			for ( int i = 0; i < m_FireConsumables.Count(); ++i )
			{
				ItemBase key = m_FireConsumables.GetKey( i );
				ref FireConsumable fire_consumable = m_FireConsumables.Get( key );
				energy = fire_consumable.GetEnergy();
				
				if ( i == 0 )
				{
					m_ItemToConsume = fire_consumable;
				}
				else
				{
					if ( fire_consumable.GetEnergy() < m_ItemToConsume.GetEnergy() )
					{
						m_ItemToConsume = fire_consumable;
					}	
				}
			}
		}
		
		//refresh visual
		RefreshFireplaceVisuals();

		return m_ItemToConsume;
	}

	protected FireConsumable GetItemToConsume()
	{
		return m_ItemToConsume;
	}

	//Spend item that is used as consumable for fire (fuel, kindling)
	//if 'amount == 0', the whole quantity will be consumed (quantity -= 1 )
	//debug
	//int m_debug_fire_consume_time = 0;
	protected void SpendFireConsumable( float amount )
	{
		//spend item
		ref FireConsumable fire_consumable = GetItemToConsume();
		
		if ( !fire_consumable )
		{
			//Set new item to consume
			fire_consumable = SetItemToConsume();
		}
		
		if ( fire_consumable )
		{
			ItemBase item = fire_consumable.GetItem();
			
			//set remaining energy
			fire_consumable.SetRemainingEnergy( fire_consumable.GetRemainingEnergy() - amount );
			
			//Debug
			/*
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			string message = "item type = " + item.GetType() + " m_RemainingEnergy = " + fire_consumable.GetRemainingEnergy().ToString() + " quantity = " + item.GetQuantity().ToString() + " amount = " + amount.ToString();
			player.MessageAction ( message );
			*/
			
			if ( fire_consumable.GetRemainingEnergy() <= 0 || amount == 0 )
			{
				//set ashes
				if ( !HasAshes() )
				{
					SetAshesState( true );
				}	
				
				if ( item.IsInherited( ItemBook ) || item.IsInherited( Paper )|| item.IsInherited( GiftWrapPaper ) )
				{
					//Debug
					//Print( "Item consumed = " + item.GetType() + " time = " + m_debug_fire_consume_time.ToString() );
					//m_debug_fire_consume_time = 0;
					//
					
					RemoveFromFireConsumables( fire_consumable );
					item.Delete();
				}
				else
				{
					//if there is nothing left, delete and remove old consumable, set new fire consumable
					if ( item.GetQuantity() <= 1 )
					{
						//remove item from fuel items
						RemoveFromFireConsumables( fire_consumable );
					}
					else
					{
						fire_consumable.SetRemainingEnergy( fire_consumable.GetEnergy() );
					}

					//Debug
					//Print( "Item consumed = " + item.GetType() + " time = " + m_debug_fire_consume_time.ToString() );
					//m_debug_fire_consume_time = 0;
					//
					
					item.AddQuantity( -1 );
					SetItemToConsume();			//set item after each quantity decrease
				}
			}
		}
		
		//debug
		//m_debug_fire_consume_time += TIMER_HEATING_UPDATE_INTERVAL;
		//
		
		CalcAndSetQuantity();
	}

	//GetKindlingCount
	//Returns count of all kindling type items (define in 'm_KindlingTypes') attached to fireplace
	protected int GetKindlingCount()
	{
		int attachments_count = GetInventory().AttachmentCount();
		int fire_consumables_count = m_FireConsumables.Count();
		int kindling_count = 0;
		
		for ( int i = 0; i < attachments_count; i++ )
		{
			ItemBase item = ItemBase.Cast( GetInventory().GetAttachmentFromIndex( i ) );
			
			for ( int j = 0; j < fire_consumables_count; j++ )
			{
				ref FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( item.Type() );

				if ( fire_consumable_type && fire_consumable_type.IsKindling() )
				{
					kindling_count++;
					break;
				}
			}
		}
		
		return kindling_count;
	}

	//GetFuelCount
	//Returns count of all fuel type items (define in 'm_FuelTypes') attached to fireplace
	protected int GetFuelCount()
	{
		int attachments_count = GetInventory().AttachmentCount();
		int fire_consumables_count = m_FireConsumables.Count();
		int fuel_count = 0;
		
		for ( int i = 0; i < attachments_count; i++ )
		{
			ItemBase item = ItemBase.Cast( GetInventory().GetAttachmentFromIndex( i ) );
			
			for ( int j = 0; j < fire_consumables_count; j++ )
			{
				ref FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( item.Type() );

				if ( fire_consumable_type && !fire_consumable_type.IsKindling() )
				{
					fuel_count++;
					break;
				}
			}
		}
		
		return fuel_count;
	}

	//returns if item attached to fireplace is kindling
	protected bool IsKindling( ItemBase item )
	{
		ref FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( item.Type() );
		if ( fire_consumable_type )
		{
			return fire_consumable_type.IsKindling();
		}
		
		return false;
	}

	//returns if item attached to fireplace is fuel
	protected bool IsFuel( ItemBase item )
	{
		ref FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( item.Type() );
		if ( fire_consumable_type )
		{
			return !fire_consumable_type.IsKindling();
		}
		
		return false;
	}

	//Has attachments of given type
	bool IsItemTypeAttached( typename item_type )
	{
		return GetAttachmentByType( item_type ) != null;
	}

	//Has attachments of required quantity
	bool IsItemAttachedQuantity( typename item_type, float quantity )
	{
		ItemBase item = ItemBase.Cast( GetAttachmentByType( item_type ) );
		
		return ( item.GetQuantity() >= quantity );
	}

	//Has last attached item
	bool HasLastAttachment()
	{
		return ( GetInventory().AttachmentCount() == 1 );
	}

	//Has last fuel/kindling attached
	bool HasLastFuelKindlingAttached()
	{
		int fuel_kindling_count = GetFuelCount() + GetKindlingCount();

		return ( fuel_kindling_count == 1 );
	}

	
	//================================================================
	// FIRE STATE
	//================================================================
	//Is fireplace burning?
	bool IsBurning()
	{
		return m_IsBurning;
	}
	
	//Set burning state
	protected void SetBurningState(bool is_burning)
	{
		m_IsBurning = is_burning;
	}

	//Are ashes present in fireplace?
	bool HasAshes()
	{
		return m_HasAshes;
	}

	//Set ashes state
	protected void SetAshesState(bool has_ashes)
	{
		m_HasAshes = has_ashes;

		if (m_HasAshes)
		{
			SetBurntFirewood();		//set burnt wood visual
		}
	}

	//Is in oven state
	bool IsOven()
	{
		return m_IsOven;
	}		
	bool HasStoneCircle()
	{
		return m_HasStoneCircle;
	}
	
	bool HasStones()
	{
		EntityAI attached_item = GetAttachmentByType(ATTACHMENT_STONES);

		return attached_item && attached_item.GetQuantity() > 0;
	}
	
	int GetAttachedStonesCount()
	{
		if (HasStones())
		{
			return GetAttachmentByType(ATTACHMENT_STONES).GetQuantity();
		}
		
		return 0;
	}

	void SetOvenState( bool is_oven )
	{
		if ( m_IsOven != is_oven )
		{
			m_IsOven = is_oven;
			m_RoofAbove = is_oven;
			
			GetInventory().SetSlotLock( InventorySlots.GetSlotIdFromString("Stones") , is_oven);
			
			Synchronize();
		}
	}
	void SetStoneCircleState( bool has_stonecircle )
	{
		if ( m_HasStoneCircle != has_stonecircle )
		{
			m_HasStoneCircle = has_stonecircle;
			
			GetInventory().SetSlotLock( InventorySlots.GetSlotIdFromString("Stones") , has_stonecircle);
			
			Synchronize();
		}
	}
	
	//Temperature loss MP
	protected float GetTemperatureLossMP()
	{
		return m_TemperatureLossMP;
	}	
	
	protected void SetTemperatureLossMP( float value )
	{
		m_TemperatureLossMP = value;
	}
	
	//Fuel burn rate MP
	protected float GetFuelBurnRateMP()
	{
		return m_FuelBurnRateMP;
	}	
	
	protected void SetFuelBurnRateMP( float value )
	{
		m_FuelBurnRateMP = value;
	}
	
	//Get fire state 
	protected int GetFireState()
	{
		return m_FireState;
	}

	//Sets fire state
	protected void SetFireState( FireplaceFireState state )
	{
		if ( m_FireState != state )
		{
			m_FireState = state;
			
			Synchronize();
		}
	}
		
	void SetExtinguishingState()
	{
		SetFireState( FireplaceFireState.EXTINGUISHING_FIRE );
	}

	//================================================================
	// FIRE PROCESS
	//================================================================
	
	//Start the fire process
	// 1. start heating
	// 2. heating
	// 3. stop heating
	void StartFire( bool force_start = false )
	{
		//stop cooling process if active
		if ( m_CoolingTimer )
		{
			m_CoolingTimer.Stop();
			m_CoolingTimer = NULL;
		}
		
		//set under roof flag (was fire ignited under roof?)
		if (MiscGameplayFunctions.IsUnderRoof(this))
		{
			m_RoofAbove = true;
		}
		
		//start fire
		if ( !IsBurning() || force_start )	
		{
			//set fuel to consume
			SetItemToConsume();
			
			//set fire state
			SetBurningState( true );
			
			m_UTSource.SetActive(true);
			//start heating
			StartHeating();
			
			//Update navmesh
			if ( !IsFireplaceIndoor() )
			{
				SetAffectPathgraph( false, true );
				GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( GetGame().UpdatePathgraphRegionByObject, 100, false, this );
			}
		}
		
		Synchronize();
	}

	//Start heating
	protected void StartHeating()
	{
		//visual
		SetObjectMaterial( 0, MATERIAL_FIREPLACE_GLOW );
		
		if ( IsWindy() && !IsRoofAbove() && !IsOven() && !IsBarrelWithHoles() )
		{
			StopFire();
			return;
		}
		
		if ( GetWet() > PARAM_BURN_WET_THRESHOLD )		
		{
			StopFire( FireplaceFireState.EXTINGUISHED_FIRE );
			return;
		}
		
		//create area damage
		if ( IsBaseFireplace() && !IsOven() )
		{
			CreateAreaDamage();
		}
		
		m_HeatingTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
		m_HeatingTimer.Run( TIMER_HEATING_UPDATE_INTERVAL, this, "Heating", NULL, true );	
		
		//Setup the noise parameters on fire start
		m_NoisePar = new NoiseParams();
		if ( IsRoofAbove() || IsOven() || IsFireplaceIndoor() ) //If we have a roof, we are probably inside
			m_NoisePar.LoadFromPath("CfgVehicles FireplaceBase NoiseFireplaceSpecial");
		else
			m_NoisePar.LoadFromPath("CfgVehicles FireplaceBase NoiseFireplaceBase");
	}

	//Do heating
	protected void Heating()
	{
		//!DEBUG
		/*
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.MessageAction ( "Heating..." );
		*/
		
		float temperature = GetTemperature();
		float temperature_modifier = 0;
		
		//check burning conditions
		if ( GetWet() > PARAM_BURN_WET_THRESHOLD )		//wetness condition
		{
			StopFire( FireplaceFireState.EXTINGUISHED_FIRE );
			return; 
		}
		else
		{
			if ( m_FireConsumables.Count() == 0 )		//fire consumable condition
			{
				StopFire();
				return; 				
			}
		}

		//DEBUG
		/*
		string s_message = "-> fire amount = " + PARAM_FIRE_CONSUM_RATE_AMOUNT.ToString() + " burn rate = " + GetFuelBurnRateMP().ToString() + " temp loss = " + GetTemperatureLossMP().ToString();
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.MessageAction (s_message);
		Print ( s_message );
		*/
		
		//spend actual fire consumable
		float amount = ( PARAM_FIRE_CONSUM_RATE_AMOUNT * GetFuelBurnRateMP() ) * TIMER_HEATING_UPDATE_INTERVAL;
		SpendFireConsumable( amount );
		
		//set wetness if raining and alter temperature modifier (which will lower temperature increase because of rain)
		float rain = GetGame().GetWeather().GetRain().GetActual();
		if ( rain >= PARAM_BURN_WET_THRESHOLD && ( IsRainingAbove() && !MiscGameplayFunctions.IsUnderRoof( this ) ) )
		{
			//set wet to fireplace
			AddWetnessToFireplace(PARAM_WET_INCREASE_COEF * rain);
			
			//set temperature modifier
			temperature_modifier = PARAM_TEMPERATURE_INCREASE * rain;
		}
		//subtract wetness when heating and not raining above
		else
		{
			AddWetnessToFireplace(-PARAM_WET_HEATING_DECREASE_COEF);
		}

		// temperature via UniversalTemperatureSource
		m_UTSLFireplace.SetFuelCount(GetFuelCount());
		m_UTSLFireplace.SetCurrentTemperature(GetTemperature() + PARAM_TEMPERATURE_INCREASE - temperature_modifier);
		
		//check fire state
		if ( GetFireState() != FireplaceFireState.EXTINGUISHING_FIRE )
		{
			RefreshFireState();
		}
		
		//damage cargo items
		BurnItemsInFireplace();
		
		//manage cooking equipment (this applies only for case of cooking pot on a tripod)
		if (HasCookingStand() && m_CookingEquipment)
		{
			float cook_equip_temp = m_CookingEquipment.GetTemperature();
			
			if ( cook_equip_temp >= PARAM_COOKING_TEMP_THRESHOLD )
			{
				//start cooking
				CookWithEquipment();
			}
			
			//DEBUG
			/*
			PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
			string s_message = "cooking equipment temp = " + ftoa(cook_equip_temp) + " | PARAM_COOKING_TEMP_THRESHOLD = " + ftoa(PARAM_COOKING_TEMP_THRESHOLD);
			if (player != NULL) {player.MessageAction (s_message);}
			Print ( s_message );
			*/
			//set temperature to cooking equipment
			cook_equip_temp = cook_equip_temp + PARAM_COOKING_EQUIP_TEMP_INCREASE;
			if ( temperature <= PARAM_SMALL_FIRE_TEMPERATURE )
			{
				cook_equip_temp = Math.Clamp ( cook_equip_temp, PARAM_ITEM_HEAT_MIN_TEMP, PARAM_SMALL_FIRE_TEMPERATURE );
			}
			else
			{
				cook_equip_temp = Math.Clamp ( cook_equip_temp, PARAM_ITEM_HEAT_MIN_TEMP, PARAM_COOKING_EQUIP_MAX_TEMP );
			}

			m_CookingEquipment.SetTemperature( cook_equip_temp );
		}

		float cook_item_temp;
		int i;
		// manage cooking on direct cooking slots
		if ( DirectCookingSlotsInUse() )
		{
			for ( i = 0; i < DIRECT_COOKING_SLOT_COUNT; i++ )
			{
				if ( m_DirectCookingSlots[i] )
				{
					cook_item_temp = m_DirectCookingSlots[i].GetTemperature();
					if ( cook_item_temp >= PARAM_COOKING_TEMP_THRESHOLD )
					{
						CookOnDirectSlot( m_DirectCookingSlots[i], cook_item_temp, temperature );
					}
				}
			}
		}

		// manage smoking slots
		if ( SmokingSlotsInUse() )
		{
			for ( i = 0; i < SMOKING_SLOT_COUNT; i++ )
			{
				if ( m_SmokingSlots[i] )
				{
					SmokeOnSmokingSlot( m_SmokingSlots[i], cook_item_temp, temperature );
				}
			}
		}
		
		//Make noise for AI, only at night
		if ( GetGame().GetWorld().IsNight() && m_CanNoise )
		{
			NoiseSystem noise = GetGame().GetNoiseSystem();
			if ( noise && m_NoisePar )
			{
				noise.AddNoisePos( this, GetPosition(), m_NoisePar);
			}
			m_CanNoise = false;
		}
		else
			m_CanNoise = true;
		
		Synchronize();
	}

	//Stop the fire process
	// 1. start cooling
	// 2. cooling
	// 3. stop cooling
	void StopFire( FireplaceFireState fire_state = FireplaceFireState.END_FIRE )
	{
		//Debug
		//PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
		//if (player != NULL) {player.MessageAction ( "Stop fire..." );}
		//Print("Stop fire...");
		
		//Stop heating
		StopHeating();
		
		//spend item
		SpendFireConsumable( 0 );
		
		//turn fire off
		SetBurningState( false );
		
		//start cooling
		StartCooling();
		
		//Refresh fire visual
		SetFireState( fire_state );
		
		//Update navmesh
		if ( !IsFireplaceIndoor() )
		{
			SetAffectPathgraph( false, false );
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( GetGame().UpdatePathgraphRegionByObject, 100, false, this );
		}
		
		Synchronize();
	}
	
	//Stop heating
	protected void StopHeating()
	{
		//Debug
		//PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
		//if (player != NULL) {player.MessageAction ( "Stop heating..." );}
		//Print("Stop heating...");
		
		//Stop heating
		if ( !m_HeatingTimer ) //Check in case we stop fire as it starts for feedback
			return;
		
		m_HeatingTimer.Stop();
		m_HeatingTimer = NULL;
	}	

	//Start cooling
	protected void StartCooling()
	{
		//Debug
		//PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
		//if (player != NULL) {player.MessageAction ( "Start cooling..." );}
		//Print("Start cooling...");
		
		//stop heating timer if active
		if ( m_HeatingTimer )
		{
			m_HeatingTimer.Stop();
			m_HeatingTimer = NULL;
		}
		
		//Start cooling
		m_CoolingTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
		m_CoolingTimer.Run( TIMER_COOLING_UPDATE_INTERVAL, this, "Cooling", NULL, true );
	}

	//Start cooling       
	protected void Cooling()
	{
		//Debug
		/*
		PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
		player.MessageAction ( "Cooling..." );
		Print("Cooling...");
		*/
		
		float wetness = GetWet();
		float temperature = GetTemperature();
		float temperature_modifier = 0;
		
		if ( this && !IsBurning() && temperature >= 10 )
		{
			//check wetness
			//set wetness if raining and alter temperature modifier (which will lower temperature increase because of rain)
			float rain = GetGame().GetWeather().GetRain().GetActual();
			if ( IsRainingAbove() && !IsRoofAbove() )
			{
				//set wet to fireplace
				AddWetnessToFireplace(PARAM_WET_INCREASE_COEF * rain);

				//set temperature modifier
				temperature_modifier = temperature_modifier + ( PARAM_TEMPERATURE_DECREASE * rain );
			}
			else		//subtrackt wetness
			{
				AddWetnessToFireplace(-PARAM_WET_COOLING_DECREASE_COEF);
			}
			
			//calculate already obtained wetness (e.g. extinguished by water)
			temperature_modifier = temperature_modifier + ( PARAM_TEMPERATURE_DECREASE * wetness );

			// temperature via UniversalTemperatureSource
			m_UTSLFireplace.SetFuelCount(GetFuelCount());
			m_UTSLFireplace.SetCurrentTemperature(GetTemperature() - PARAM_TEMPERATURE_DECREASE - temperature_modifier );
			
			//damage cargo items
			BurnItemsInFireplace();
			
			//cook with equipment if temperature of equipment is still high
			if ( m_CookingEquipment )
			{
				float cook_equip_temp = m_CookingEquipment.GetTemperature();
				
				if ( cook_equip_temp >= PARAM_COOKING_TEMP_THRESHOLD )
				{
					//cook
					CookWithEquipment();
				}
			}

			float cook_item_temp;
			int i;

			// manage cooking on direct cooking slots
			if ( DirectCookingSlotsInUse() )
			{
				for ( i = 0; i < DIRECT_COOKING_SLOT_COUNT; i++ )
				{
					if ( m_DirectCookingSlots[i] )
					{
						cook_item_temp = m_DirectCookingSlots[i].GetTemperature();
						if ( cook_item_temp >= PARAM_COOKING_TEMP_THRESHOLD )
						{
							CookOnDirectSlot( m_DirectCookingSlots[i], cook_item_temp, temperature );
						}
					}
				}
			}

			// manage smoking slots
			if ( SmokingSlotsInUse() )
			{
				for ( i = 0; i < SMOKING_SLOT_COUNT; i++ )
				{
					if ( m_SmokingSlots[i] )
					{
						SmokeOnSmokingSlot( m_SmokingSlots[i], cook_item_temp, temperature );
					}
				}
			}

			//Debug
			//PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
			//if (player != NULL) {player.MessageAction ( temperature.ToString() );}
		}
		else
		{
			//stop cooling
			if (Math.AbsFloat(temperature) < 10)
			{
				StopCooling();
				m_UTSource.SetActive(false);
			}
		}
	}

	protected void StopCooling()
	{
		//Debug
		//PlayerBase player = ( PlayerBase ) GetGame().GetPlayer();
		//if (player != NULL) {player.MessageAction ( "Stop cooling..." );}
		//Print("Stop cooling...");
		
		//stop all fire visuals
		SetFireState( FireplaceFireState.NO_FIRE );	
		SetObjectMaterial( 0, MATERIAL_FIREPLACE_NOGLOW );
		
		//Stop cooling
		m_CoolingTimer.Stop();
		m_CoolingTimer = NULL;
		
		//destroy area damage
		DestroyAreaDamage();

		//remove cookware audio visuals
		if ( GetCookingEquipment() )
		{
			Bottle_Base cooking_pot = Bottle_Base.Cast( GetCookingEquipment() );
			if ( cooking_pot )
				cooking_pot.RemoveAudioVisualsOnClient();
		}
		if ( DirectCookingSlotsInUse() )
		{
			for ( int i = 0; i < DIRECT_COOKING_SLOT_COUNT; i++ )
			{
				if ( m_DirectCookingSlots[i] )
				{
					Bottle_Base cookingpot = Bottle_Base.Cast( m_DirectCookingSlots[i] );
					if ( cookingpot )
						cookingpot.RemoveAudioVisualsOnClient();

					FryingPan fryingpan = FryingPan.Cast( m_DirectCookingSlots[i] );
					if ( fryingpan )
						fryingpan.RemoveAudioVisualsOnClient();

					Edible_Base itsfood = Edible_Base.Cast( m_DirectCookingSlots[i] );
					if ( itsfood )
						itsfood.MakeSoundsOnClient( false );
				}
			}
		}
	}
	
	//================================================================
	// COOKING
	//================================================================
	protected void CookWithEquipment()
	{
		if (m_CookingProcess == null)
		{
			m_CookingProcess = new Cooking();
		}

		m_CookingProcess.CookWithEquipment(m_CookingEquipment);
	}

	Cooking GetCookingProcess()
	{
		if (m_CookingProcess == null)
		{
			m_CookingProcess = new Cooking();
		}
		
		return m_CookingProcess;
	}
	
	protected void CookOnDirectSlot(ItemBase slot_item, float temp_equip, float temp_ext)
	{
		if (m_CookingProcess == null)
		{
			m_CookingProcess = new Cooking();
		}
		
		m_CookingProcess.CookWithEquipment(slot_item);
		
		switch (slot_item.Type())
		{
		case ATTACHMENT_CAULDRON:
		case ATTACHMENT_COOKING_POT:
		case ATTACHMENT_FRYING_PAN:
			float cookEquipTemp = temp_equip + PARAM_COOKING_EQUIP_TEMP_INCREASE;
			if ( temp_ext <= PARAM_SMALL_FIRE_TEMPERATURE )
			{
				cookEquipTemp = Math.Clamp(cookEquipTemp, PARAM_ITEM_HEAT_MIN_TEMP, PARAM_SMALL_FIRE_TEMPERATURE);
			}
			else
			{
				cookEquipTemp = Math.Clamp(cookEquipTemp, PARAM_ITEM_HEAT_MIN_TEMP, PARAM_COOKING_EQUIP_MAX_TEMP);
			}
			slot_item.SetTemperature(cookEquipTemp);
		break;
		}
	}
	
	protected void SmokeOnSmokingSlot( ItemBase slot_item, float temp_equip, float temp_ext )
	{
		if ( m_CookingProcess == NULL )
		{
			m_CookingProcess = new Cooking();
		}
		
		// smoking slots accept only individual meat/fruit/veg items
		Edible_Base ingr = Edible_Base.Cast( slot_item );
		if ( ingr )
		{
			m_CookingProcess.SmokeItem( ingr, FireplaceBase.TIMER_HEATING_UPDATE_INTERVAL * FireplaceBase.SMOKING_SPEED );
		}
	}
	
	//================================================================
	// FIRE VICINITY
	//================================================================
	//apply damage to all items, raise temperature to only specific items
	protected void BurnItemsInFireplace()
	{
		//! cargo
		CargoBase cargo = GetInventory().GetCargo();
		for (int i = 0; i < cargo.GetItemCount(); i++)
		{
			ItemBase item = ItemBase.Cast(cargo.GetItem(i));
			
			//set damage
			AddDamageToItemByFireEx(item, false, false);

			if (item.GetHealth("", "Health") <= 0 && !item.IsKindOf("Grenade_Base"))
			{
				item.Delete();
			}
			
			//add temperature
			AddTemperatureToItemByFire(item);
			
			//remove wetness
			AddWetnessToItem(item, -PARAM_WET_HEATING_DECREASE_COEF);
		}
		
		//! attachments
		for (int j = 0; j < GetInventory().AttachmentCount(); ++j)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(j));
			
			//set damage
			AddDamageToItemByFireEx(attachment, false, true);
			
			//add temperature
			AddTemperatureToItemByFire(attachment);
		
			//remove wetness
			AddWetnessToItem(attachment, -PARAM_WET_HEATING_DECREASE_COEF);
		}
	}

	//add temperature to item by fire
	protected void AddTemperatureToItemByFire(ItemBase item)
	{
		if (item.GetTemperatureMax() >= PARAM_ITEM_HEAT_MIN_TEMP)
		{
			float temperature = item.GetTemperature() + PARAM_ITEM_HEAT_TEMP_INCREASE_COEF;
			temperature = Math.Clamp(temperature, PARAM_ITEM_HEAT_MIN_TEMP, PARAM_MAX_ITEM_HEAT_TEMP_INCREASE);
			item.SetTemperature(temperature);
		}
	}

	//! DEPRECATED
	protected void AddTemperatureToFireplace(float amount) {};
	
	//add damage to item by fire
	protected void AddDamageToItemByFireEx(ItemBase item, bool can_be_ruined, bool pAttachment)
	{
		if (!item)
		{
			return;		
		}

		if (GetTemperature() <= PARAM_ITEM_HEAT_MIN_TEMP)
		{
			return;
		}

		//! fireplace cargo only
		if (!pAttachment)
		{
			//! cargo item attachments
			GameInventory itemInventory = item.GetInventory();
			if (itemInventory)
			{
				for (int i = 0; i < itemInventory.AttachmentCount(); ++i)
				{
					itemInventory.GetAttachmentFromIndex(i).AddHealth("", "", -PARAM_BURN_DAMAGE_COEF);
				}
			}

			item.AddHealth("", "", -PARAM_BURN_DAMAGE_COEF);
		}
		else
		{
			float timerCoef = 1.0;
			if (m_HeatingTimer && m_HeatingTimer.IsRunning())
			{
				timerCoef = TIMER_HEATING_UPDATE_INTERVAL;
			}
			else if (m_CoolingTimer && m_CoolingTimer.IsRunning())
			{
				timerCoef = TIMER_COOLING_UPDATE_INTERVAL;		
			}

			switch (item.Type())
			{
			case ATTACHMENT_TRIPOD:
			case ATTACHMENT_COOKINGSTAND:
			case ATTACHMENT_COOKING_POT:
			case ATTACHMENT_CAULDRON:
			case ATTACHMENT_FRYING_PAN:
				item.DecreaseHealth(GameConstants.FIRE_ATTACHMENT_DAMAGE_PER_SECOND * timerCoef, false);
			break;
			}
	
			//! fuel & kindling		
			if (IsFuel(item) || IsKindling(item))
			{
				if (item.GetHealthLevel() < GameConstants.STATE_BADLY_DAMAGED)
				{
					item.DecreaseHealth(PARAM_BURN_DAMAGE_COEF, false);
				}
			}
			
			//! cookable
			if (item.CanBeCooked() && item.GetHealthLevel() >= GameConstants.STATE_BADLY_DAMAGED)
			{
				Edible_Base edible_item = Edible_Base.Cast( item );
				edible_item.ChangeFoodStage(FoodStageType.BURNED);
			}
		}
	}
	
	// DEPRECATED
	protected void AddDamageToItemByFire(ItemBase item, bool can_be_ruined)
	{
		item.DecreaseHealth( PARAM_BURN_DAMAGE_COEF, false );

		if ( item.CanBeCooked() & item.GetHealthLevel() >= GameConstants.STATE_BADLY_DAMAGED )
		{
			Edible_Base edible_item = Edible_Base.Cast( item );
			edible_item.ChangeFoodStage( FoodStageType.BURNED );
		}
	}
	
	//add wetness on item
	protected void AddWetnessToItem(ItemBase item, float amount)
	{
		float wetness = item.GetWet();
		wetness = wetness + amount;
		wetness = Math.Clamp(wetness, 0, 1); //wetness <0-1>
		item.SetWet(wetness);
	}
	
	//add wetness on fireplace
	void AddWetnessToFireplace(float amount)
	{
		//add wetness
		float wetness = GetWet();
		wetness = wetness + amount;
		wetness = Math.Clamp(wetness, 0, 1); //wetness <0-1>
		SetWet(wetness);
		
		//decrease temperature
		if ( amount > 0 )
		{
			float temperature = GetTemperature();
			temperature = temperature * ( 1 - ( wetness * 0.5 ) );
			temperature = Math.Clamp( temperature, PARAM_MIN_FIRE_TEMPERATURE, PARAM_NORMAL_FIRE_TEMPERATURE );
			// temperature via UniversalTemperatureSource
			m_UTSLFireplace.SetFuelCount(GetFuelCount());
			m_UTSLFireplace.SetCurrentTemperature(temperature);
		}
	}	
	
	//! DEPRECATED
	protected void TransferHeatToNearPlayers() {}
	
	//Create and Destroy damage radius around the fireplace when starting/stopping burning process
	void CreateAreaDamage()
	{
		//destroy area damage if some already exists
		DestroyAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AreaDamageLoopedDeferred( this );
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetExtents("-0.30 0 -0.30", "0.30 0.75 0.30");
		m_AreaDamage.SetLoopInterval( 0.5 );
		m_AreaDamage.SetDeferDuration( 0.5 );
		m_AreaDamage.SetHitZones( { "Head","Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot" } );
		m_AreaDamage.SetAmmoName( "FireDamage" );
		m_AreaDamage.Spawn();
	}
	
	void DestroyAreaDamage()
	{
		if ( m_AreaDamage ) 
		{
			m_AreaDamage.Destroy();
		}
	}

	//! DEPRECATED
	bool IsFacingFireplace(PlayerBase player)
	{
		ErrorEx("Deprecated method", ErrorExSeverity.INFO);

		return false;
	}

	//================================================================
	// ACTIONS
	//================================================================
	// --- Actions / Action conditions
	//Check if there is some kindling attached
	bool HasAnyKindling()
	{
		return GetKindlingCount() > 0;
	}

	//Check if the weather is too windy
	static bool IsWindy()
	{
		//check wind
		float wind_speed = GetGame().GetWeather().GetWindSpeed();
		float wind_speed_threshold = GetGame().GetWeather().GetWindMaximumSpeed() * FireplaceBase.IGNITE_WIND_THRESHOLD;

		return ( wind_speed >= wind_speed_threshold );
	}
	
	bool GetIgniteFailure()
	{
		return m_NoIgnite;
	}
	
	void SetIgniteFailure( bool failure )
	{
		m_NoIgnite = failure;
	}

	//Check if the fireplace is too wet to be ignited
	static bool IsEntityWet( notnull EntityAI entity_ai )
	{
		return ( entity_ai.GetWet() >= FireplaceBase.PARAM_MAX_WET_TO_IGNITE );
	}
	
	bool IsWet()
	{
		return FireplaceBase.IsEntityWet( this );
	}

	//Check if there is any roof above fireplace
	bool IsRoofAbove()
	{
		return m_RoofAbove;
	}
	
	//check if fireplace is opened
	override bool IsOpen()
	{
		return true;
	}
	
	//Check if there is enough space for smoke
	bool IsCeilingHighEnoughForSmoke()
	{
		return !MiscGameplayFunctions.IsUnderRoof( this, FireplaceBase.MIN_CEILING_HEIGHT );
	}

	//Check if it's raining and there is only sky above fireplace
	static bool IsRainingAboveEntity( notnull EntityAI entity_ai )
	{
		return ( GetGame() && ( GetGame().GetWeather().GetRain().GetActual() >= FireplaceBase.PARAM_IGNITE_RAIN_THRESHOLD ) );
	}
	
	bool IsRainingAbove()
	{
		return FireplaceBase.IsRainingAboveEntity( this );
	}

	//Check there is water surface bellow fireplace
	static bool IsEntityOnWaterSurface( notnull EntityAI entity_ai )
	{
		vector fireplace_pos = entity_ai.GetPosition();	
		if ( GetGame().SurfaceIsSea( fireplace_pos[0], fireplace_pos[2] ) )
		{
			return true;
		}
		else if ( GetGame().SurfaceIsPond( fireplace_pos[0], fireplace_pos[2] ) )
		{
			return true;
		}
		else
		{
			string surface_type;
			GetGame().SurfaceGetType3D( fireplace_pos[0], fireplace_pos[1] + 1.0, fireplace_pos[2], surface_type );
			return surface_type.Contains("water");
		}
		
		return false;
	}
	bool IsOnWaterSurface()
	{
		return FireplaceBase.IsEntityOnWaterSurface( this );
	}
	
	//check if the surface is interior
	static bool IsEntityOnInteriorSurface( notnull EntityAI entity_ai )
	{
		string surface_type;
		vector fireplace_pos = entity_ai.GetPosition();
		GetGame().SurfaceGetType3D( fireplace_pos[0], fireplace_pos[1] + 1.0, fireplace_pos[2], surface_type );
		return ( ( GetGame().ConfigGetInt( "CfgSurfaces " + surface_type + " interior" ) ) == 1 );
	}
	bool IsOnInteriorSurface()
	{
		return FireplaceBase.IsEntityOnInteriorSurface( this );
	}
	
	//Checks if has not additional items in it
	override bool IsEmpty()
	{
		return ( GetInventory().GetCargo().GetItemCount() == 0 && GetInventory().AttachmentCount() == 0 );
	}
	
	//Checks if it has no items in cargo
	bool IsCargoEmpty()
	{
		return ( GetInventory().GetCargo().GetItemCount() == 0 );
	}
	
	/*
	bool HasTripodAttached()
	{
	 	"CookingTripod"
	}
	*/
	
	bool HasCookingStand()
	{
		return GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("CookingTripod")) != null;
	}
	
	//Check if object is in animation phase
	bool IsInAnimPhase( string anim_phase )
	{
		return GetAnimationPhase(anim_phase) == 0;
	}
	
	bool IsSpaceFor(vector size)
	{
		array<Object> objs = {};
		if (GetGame().IsBoxCollidingGeometry(GetWorldPosition() + Vector(0, size[1] * 0.5 + 0.1, 0), GetDirection().VectorToAngles(), size, ObjIntersect.View, ObjIntersect.Geom, {this}, objs))
		{
			foreach (Object obj : objs)
			{
				if (dBodyGetInteractionLayer(obj) == PhxInteractionLayers.ITEM_LARGE)
					return false;
			}
		}
		return true;
	}
	
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		if (!otherDestination || otherDestination.GetParent() == this)
		{
			return false;
		}
		return true;
	}
	
	
	
	//Action condition for building oven
	bool CanBuildOven()
	{	
		return !IsOven() && !IsBurning() && (GetAttachedStonesCount() >= MIN_STONES_TO_BUILD_OVEN) && !FindAttachmentBySlotName("CookingTripod") && !HasStoneCircle() && IsSpaceForOven();
	}
	
	bool IsSpaceForOven()
	{
		const float size = 0.6;		
		return IsSpaceFor(Vector(size, size, size));
	}
	
	//Action condition for stone circle
	bool CanBuildStoneCircle()
	{
		return !HasStoneCircle() && !IsOven() && !IsBurning() && (GetAttachedStonesCount() >= MIN_STONES_TO_BUILD_CIRCLE) && IsSpaceForCircle();
	}
	
	bool IsSpaceForCircle()
	{
		return IsSpaceFor(Vector(0.9, 0.1, 0.9));
	}
	
	//Action condition for dismantling oven
	bool CanDismantleOven()
	{
		return ( IsOven() && !IsBurning() && !DirectCookingSlotsInUse() && !SmokingSlotsInUse() && GetTemperature() <= MAX_TEMPERATURE_TO_DISMANTLE_OVEN );
	}
	//Action condition for dismantling stone circle
	bool CanDismantleStoneCircle()
	{
		return ( HasStoneCircle() && !IsOven() && !IsBurning() );
	}
	
	bool CanCookOnStick()
	{
		return ( GetTemperature() >= PARAM_COOKING_TEMP_THRESHOLD );
	}
	
	//Can extinguish fire
	bool CanExtinguishFire()
	{
		return IsBurning();
	}
	
	FireplaceLight GetLightEntity()
	{
		return m_Light;
	}
	
	void SetLightEntity( FireplaceLight light )
	{
		m_Light = light;
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( GetGame().IsServer() )
		{
			//remove grass
			Object cc_object = GetGame().CreateObjectEx( OBJECT_CLUTTER_CUTTER , position, ECE_PLACE_ON_SURFACE );
			cc_object.SetOrientation( orientation );
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( GetGame().ObjectDelete, 1000, false, cc_object );
			
			SetIsPlaceSound( true );
		}
	}
	
	override string GetPlaceSoundset()
	{
		return "placeFireplace_SoundSet";
	}
	
	//================================================================
	// QUANITTY
	//================================================================
	// calculates and sets total energy based on possible (fuel/kinidling) item attachments
	protected void CalcAndSetTotalEnergy()
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			m_TotalEnergy = 0;
			
			for ( int i = 0; i < m_FireConsumableTypes.Count(); ++i )
			{
				typename key = m_FireConsumableTypes.GetKey( i );
				FireConsumableType fire_consumable_type = m_FireConsumableTypes.Get( key );
				//string qt_config_path = "CfgVehicles" + " " + fire_consumable_type.GetItemType().ToString() + " " + "varQuantityMax";
				string qt_config_path = string.Format("CfgVehicles %1 varQuantityMax", fire_consumable_type.GetItemType().ToString());
				//string sm_config_path = "CfgSlots" + " " + "Slot_" + fire_consumable_type.GetAttSlot() + " " + "stackMax";
				string sm_config_path = string.Format("CfgSlots Slot_%1 stackMax", fire_consumable_type.GetAttSlot());
				if ( GetGame().ConfigIsExisting( qt_config_path ) )
				{
					float quantity_max = GetGame().ConfigGetFloat( qt_config_path );
				}
				
				if ( GetGame().ConfigIsExisting( sm_config_path ) )
				{
					float stack_max = GetGame().ConfigGetFloat( sm_config_path );
				}			
				
				//debug
				//Print( fire_consumable_type.GetItemType().ToString() + " quantity_max = " + quantity_max.ToString() + " [" + (quantity_max*fire_consumable_type.GetEnergy()).ToString()  + "] | stack_max = " + stack_max.ToString() + " [" + (stack_max*fire_consumable_type.GetEnergy()).ToString() + "]" );
				
				if ( stack_max > 0 )
				{
					m_TotalEnergy += stack_max * fire_consumable_type.GetEnergy();
				}
				else
				{
					m_TotalEnergy += quantity_max * fire_consumable_type.GetEnergy();
				}
			}
			
			//debug
			//Print( "Total energy = " + m_TotalEnergy.ToString() );
		}
	}
	
	// calculates and sets current quantity based on actual (fuel/kinidling) item attachments
	protected void CalcAndSetQuantity()
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			float remaining_energy;
			
			for ( int i = 0; i < m_FireConsumables.Count(); ++i )
			{
				ItemBase key = m_FireConsumables.GetKey( i );
				FireConsumable fire_consumable = m_FireConsumables.Get( key );
				
				float quantity = fire_consumable.GetItem().GetQuantity();
				
				if ( quantity > 0 )
				{
					remaining_energy += ( ( quantity - 1 ) * fire_consumable.GetEnergy() ) + fire_consumable.GetRemainingEnergy();
					//Print( fire_consumable.GetItem().GetType() + " remaining energy = " + ( ( ( quantity - 1 ) * fire_consumable.GetEnergy() ) + fire_consumable.GetRemainingEnergy() ).ToString() );
				}
				else
				{
					remaining_energy += fire_consumable.GetRemainingEnergy();
					//Print( fire_consumable.GetItem().GetType() + " remaining energy = " + ( fire_consumable.GetRemainingEnergy().ToString() ) );
				}
			}
			
			SetQuantity( remaining_energy / m_TotalEnergy * GetQuantityMax() );
		}
	}
	
	override void OnAttachmentQuantityChanged( ItemBase item )
	{
		super.OnAttachmentQuantityChanged( item );
		
		CalcAndSetQuantity();
	}
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if ( !super.CanReleaseAttachment( attachment ) )
			return false;
		
		ItemBase item = ItemBase.Cast( attachment );

		//kindling items
		if ( IsKindling ( item ) || IsFuel( item ) )
		{
			return !IsBurning();
		}
		
		return true;
	}
	
	void LockOvenAttachments(bool lock)
	{
		//Print("LockOvenAttachments");
		//string path_cooking_equipment = "" + CFG_VEHICLESPATH + " " + GetType() + " GUIInventoryAttachmentsProps CookingEquipment attachmentSlots";
		string path_cooking_equipment = string.Format("%1 %2 GUIInventoryAttachmentsProps CookingEquipment attachmentSlots", CFG_VEHICLESPATH, GetType());
		//string path_direct_cooking = "" + CFG_VEHICLESPATH + " " + GetType() + " GUIInventoryAttachmentsProps DirectCooking attachmentSlots";
		string path_direct_cooking = string.Format("%1 %2 GUIInventoryAttachmentsProps DirectCooking attachmentSlots", CFG_VEHICLESPATH, GetType());
		if ( GetGame().ConfigIsExisting(path_cooking_equipment) && GetGame().ConfigIsExisting(path_direct_cooking) )
		{
			array<string> arr_cooking_equipment = new array<string>;
			array<string> arr_direct_cooking = new array<string>;
			GetGame().ConfigGetTextArray(path_cooking_equipment,arr_cooking_equipment);
			GetGame().ConfigGetTextArray(path_direct_cooking,arr_direct_cooking);
			for ( int i = 0; i < arr_cooking_equipment.Count(); i++ )
			{
				if ( lock != GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(arr_cooking_equipment[i])) )
				{
					GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(arr_cooking_equipment[i]),lock);
					//Print("attachment lock: " + arr_cooking_equipment[i] + " " + lock);
				}
			}
			
			for ( i = 0; i < arr_direct_cooking.Count(); i++ )
			{
				if ( lock == GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(arr_direct_cooking[i])) )
				{
					GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(arr_direct_cooking[i]),!lock);
					//Print("attachment lock: " + arr_direct_cooking[i] + " " + !lock);
				}
			}
		}
		m_OvenAttachmentsLockState = lock;
	}
	override bool DisassembleOnLastDetach()
	{
		return true;
	}
	
	override void OnRPC( PlayerIdentity sender, int rpc_type, ParamsReadContext ctx ) 
	{
		super.OnRPC( sender, rpc_type, ctx );
		
		ref Param1<bool> p = new Param1<bool>(false);
				
		if (ctx.Read(p))
		{
			bool failure = p.param1;
		}
		
		switch ( rpc_type )
		{
			case FirePlaceFailure.WIND:
			
				if ( failure )
				{
					ParticleFireWindyNoIgniteStart();
					SoundFireStop();
					SoundFireWindyNoIgniteStart();
				}
			
			break;
			
			case FirePlaceFailure.WET:
				
				if ( failure )
				{
					ParticleWetNoIgniteStart();
					SoundFireStop();
					SoundFireWetNoIgniteStart();
				}
			
			break;
		}
	}
	
	
	//================================================================
	// DEBUG
	//================================================================
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
			ItemBase firewood = ItemBase.Cast(GetInventory().CreateInInventory("Firewood"));
 			firewood.SetQuantity(firewood.GetQuantityMax());
			
			ItemBase sticks = ItemBase.Cast(GetInventory().CreateInInventory("WoodenStick"));
 			sticks.SetQuantity(sticks.GetQuantityMax());
			
			ItemBase stone = ItemBase.Cast(GetInventory().CreateInInventory("Stone"));
 			stone.SetQuantity(stone.GetQuantityMax());
			
			GetInventory().CreateInInventory("Rag");

			SpawnEntityOnGroundPos("PetrolLighter", GetPosition());
	}
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Ignite";
		button2 = "Extinguish";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		switch (button_index)
		{
			case 1:
				OnIgnitedThis(null);
				break;
			case 2:
				StopFire();
				break;
		}
	}
}