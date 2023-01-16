class GetOutTransportActionData : ActionData
{
	Car m_Car;
	vector m_StartLocation;
	float m_CarSpeed;
	bool m_WasJumpingOut = false;
	float m_DmgTaken = 0; 				// Damage taken by the player when jumping out of vehicle
	float m_ShockTaken = 0; 			// Shock inflicted to the player when jumping out of vehicle
}

class ActionGetOutTransport: ActionBase
{
	//For the two following variables -> The HIGHER the value, the LOWER the output
	int m_DmgFactor = 60; 				//value used to translate impact strength into actual damage (impact strength -> velocity squared)
	int m_ShockFactor = 15; 			//Value used to translate impact strength into actual shock

	//Variables used to determine the different speed levels for bleeding checks
	const int LOW_SPEED_VALUE = 20;
	const int HIGH_SPEED_VALUE = 30;
	
	void ActionGetOutTransport()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text		 = "#leave_vehicle";
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new GetOutTransportActionData();
		return action_data;
	}


	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
 		Transport trans = null;
		int crew_index 	= -1;

		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			trans = vehCommand.GetTransport();
			if (trans)
			{
				crew_index = trans.CrewMemberIndex(player);
				return crew_index >= 0 && trans.CrewCanGetThrough(crew_index) && trans.IsAreaAtDoorFree(crew_index);
			}
		}

		return false;
	}

	void ProcessGetOutActionData(Car car, GetOutTransportActionData got_action_data)
	{
		got_action_data.m_StartLocation = got_action_data.m_Player.GetPosition();
		got_action_data.m_Car = car;
		float speed = car.GetSpeedometerAbsolute();
		got_action_data.m_CarSpeed = speed;
		got_action_data.m_DmgTaken = (got_action_data.m_CarSpeed * got_action_data.m_CarSpeed) / m_DmgFactor; //When using multiplications, wrong value is returned
		got_action_data.m_ShockTaken = (got_action_data.m_CarSpeed * got_action_data.m_CarSpeed) / m_ShockFactor;
		got_action_data.m_WasJumpingOut = speed > 8.0;
	}

	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);

		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			
			if (trans)
			{
				GetOutTransportActionData got_action_data = GetOutTransportActionData.Cast(action_data);
				Car car;
				if (Class.CastTo(car, trans))
				{
					ProcessGetOutActionData( car, got_action_data );

					if ( !got_action_data.m_WasJumpingOut )
					{
						vehCommand.GetOutVehicle();
					}
					else
					{
						vehCommand.JumpOutVehicle();
					}
					
					GetDayZGame().GetBacklit().OnLeaveCar();		
					if ( action_data.m_Player.GetInventory() ) 
						action_data.m_Player.GetInventory().LockInventory(LOCK_FROM_SCRIPT);							
				}
			}
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			
			if (trans)
			{
				CarScript car;
				if (Class.CastTo(car, trans))
				{
					car.ForceUpdateLightsStart();
				}
			}
		}
	}
	
	void Unhide(PlayerBase player);

	override void OnUpdate(ActionData action_data)
	{
		if (action_data.m_State == UA_START)
		{
			if (!action_data.m_Player.GetCommand_Vehicle())
			{
				End(action_data);
			}
		}
	}
	
	override bool CanBeUsedInRestrain()
	{
		return true;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override int GetActionCategory()
	{
		return AC_INTERACT;
	}
	
	override void OnEnd( ActionData action_data )
	{
		if ( action_data.m_Player.GetInventory() ) 
			action_data.m_Player.GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
	}
	
	override void OnEndServer( ActionData action_data )
	{
		GetOutTransportActionData got_action_data = GetOutTransportActionData.Cast(action_data);
		
		if (got_action_data.m_WasJumpingOut)
		{
			got_action_data.m_Player.OnJumpOutVehicleFinish(got_action_data.m_CarSpeed);
			
			PlayerBase player = got_action_data.m_Player;
			
			ApplyJumpOutDmg(action_data);
			
			vector posMS = player.WorldToModel(player.GetPosition());
			player.DamageAllLegs(got_action_data.m_DmgTaken); //Additionnal leg specific damage dealing
			player.ProcessDirectDamage(DT_CUSTOM, player, "", "FallDamage", posMS, got_action_data.m_DmgTaken);
		}

		if ( got_action_data.m_Car )
		{
			CarScript car;
			if ( Class.CastTo(car, got_action_data.m_Car) )
			{
				car.ForceUpdateLightsEnd();
			}
		}
	}
	
	//Manage all jumping out of vehicle damage logic
	void ApplyJumpOutDmg(ActionData action_data)
	{
		GetOutTransportActionData got_action_data = GetOutTransportActionData.Cast(action_data);
		PlayerBase player = got_action_data.m_Player;
		
		//Get all currently equipped clothing
		// ---------------------------------------------

		ClothingBase trousers = 	ClothingBase.Cast(player.GetItemOnSlot("LEGS"));
		ClothingBase bag = 			ClothingBase.Cast(player.GetItemOnSlot("BACK"));
		ClothingBase vest = 		ClothingBase.Cast(player.GetItemOnSlot("VEST"));
		ClothingBase headGear =  	ClothingBase.Cast(player.GetItemOnSlot("HeadGear"));
		ClothingBase mask =  		ClothingBase.Cast(player.GetItemOnSlot("Mask"));
		ClothingBase shirt = 		ClothingBase.Cast(player.GetItemOnSlot("BODY"));
		ClothingBase shoes =  		ClothingBase.Cast(player.GetItemOnSlot("FEET"));
		ClothingBase gloves = 		ClothingBase.Cast(player.GetItemOnSlot("GLOVES"));

		//Array used to find all relevant information about currently equipped clothes
		array<ClothingBase> equippedClothes = new array<ClothingBase>;

		equippedClothes.Insert(trousers);
		equippedClothes.Insert(bag);
		equippedClothes.Insert(vest);
		equippedClothes.Insert(headGear);
		equippedClothes.Insert(mask);
		equippedClothes.Insert(shirt);
		equippedClothes.Insert(shoes);
		equippedClothes.Insert(gloves);

		// -----------------------------------------------

		//Lower shock taken if player uses a helmet
		headGear = ClothingBase.Cast(player.GetItemOnHead());
		HelmetBase helmet;
		if (Class.CastTo(helmet, headGear))
			got_action_data.m_ShockTaken *= 0.5;

		// -----------------------------------------------
		
		int randNum; //value used for probability evaluation
		randNum = Math.RandomInt(0, 100);
		if (got_action_data.m_CarSpeed < LOW_SPEED_VALUE)
		{
			if (randNum < 20)
				player.GiveShock(-got_action_data.m_ShockTaken); //To inflict shock, a negative value must be passed

			randNum = Math.RandomIntInclusive(0, PlayerBase.m_BleedingSourcesLow.Count() - 1);
			
			player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow[randNum]);
		}
		else if (got_action_data.m_CarSpeed >= LOW_SPEED_VALUE && got_action_data.m_CarSpeed < HIGH_SPEED_VALUE)
		{
			if (randNum < 50)
				player.GiveShock(-got_action_data.m_ShockTaken);

			randNum = Math.RandomInt(0, PlayerBase.m_BleedingSourcesUp.Count() - 1);
			
			player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp[randNum]);
		}
		else if (got_action_data.m_CarSpeed >= HIGH_SPEED_VALUE)
		{
			headGear = ClothingBase.Cast(player.GetItemOnHead());
			if (!Class.CastTo(helmet, headGear))
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection("Head");

			if (randNum < 75)
				player.GiveShock(-got_action_data.m_ShockTaken);
		}
		
		int nbClothes = 0;

		//Damage all currently equipped clothes
		for (int i = 0; i < equippedClothes.Count(); i++)
		{
			//If no item is equipped on slot, slot is ignored
			if (equippedClothes[i] == null)
				continue;

			equippedClothes[i].DecreaseHealth(got_action_data.m_DmgTaken, false);
			nbClothes++;
		}
	}
};
