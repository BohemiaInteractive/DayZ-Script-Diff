class GetOutTransportActionData : ActionData
{
	Car m_Car;
	vector m_StartLocation;
	float m_CarSpeed;
	bool m_WasJumpingOut 	= false;
	float m_DmgTaken 		= 0.0; 			// Damage taken by the player when jumping out of vehicle
	float m_ShockTaken 		= 0.0; 			// Shock inflicted to the player when jumping out of vehicle
}

class ActionGetOutTransport : ActionBase
{
	//For the two following variables -> The HIGHER the value, the LOWER the output
	int m_DmgFactor = 60; 				//value used to translate impact strength into actual damage (impact strength -> velocity squared)
	int m_ShockFactor = 15; 			//Value used to translate impact strength into actual shock

	//Variables used to determine the different speed levels for bleeding checks
	const int LOW_SPEED_VALUE 			= 20;
	const int HIGH_SPEED_VALUE			= 30;
	
	private const int HEALTH_LOW_SPEED_VALUE 	= 30;
	private const int HEALTH_HIGH_SPEED_VALUE 	= 70;
	
	void ActionGetOutTransport()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text		 = "#leave_vehicle";
	}
	
	override ActionData CreateActionData()
	{
		ActionData data = new GetOutTransportActionData();
		return data;
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				int crewIndex = trans.CrewMemberIndex(player);
				return crewIndex >= 0 && trans.CrewCanGetThrough(crewIndex) && trans.IsAreaAtDoorFree(crewIndex);
			}
		}

		return false;
	}

	void ProcessGetOutActionData(Car car, GetOutTransportActionData got_action_data)
	{
		got_action_data.m_StartLocation = got_action_data.m_Player.GetPosition();
		got_action_data.m_Car = car;
		float carSpeed = car.GetSpeedometerAbsolute();
		got_action_data.m_CarSpeed = carSpeed;
		got_action_data.m_DmgTaken = (carSpeed * carSpeed) / m_DmgFactor; //When using multiplications, wrong value is returned
		got_action_data.m_ShockTaken = (carSpeed * carSpeed) / m_ShockFactor;
		got_action_data.m_WasJumpingOut = carSpeed > 8.0;
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
				GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);

				Car car;
				if (Class.CastTo(car, trans))
					ProcessGetOutActionData(car, gotActionData);

				if (!gotActionData.m_WasJumpingOut)
					vehCommand.GetOutVehicle();
				else
					vehCommand.JumpOutVehicle();
				
				if (car)
					GetDayZGame().GetBacklit().OnLeaveCar();

				if (action_data.m_Player.GetInventory())
					action_data.m_Player.GetInventory().LockInventory(LOCK_FROM_SCRIPT);
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
					car.ForceUpdateLightsStart();
			}
		}
	}
	
	void Unhide(PlayerBase player);

	override void OnUpdate(ActionData action_data)
	{
		if (action_data.m_State == UA_START)
		{
			if (!action_data.m_Player.GetCommand_Vehicle())
				End(action_data);
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
	
	override void OnEnd(ActionData action_data)
	{
		if (action_data.m_Player.GetInventory()) 
			action_data.m_Player.GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
		
		if (gotActionData.m_WasJumpingOut)
		{
			float carSpeed = gotActionData.m_CarSpeed;
			gotActionData.m_Player.OnJumpOutVehicleFinish(carSpeed);
			
			ApplyJumpOutDmg(action_data);
			
			vector posMS = gotActionData.m_Player.WorldToModel(gotActionData.m_Player.GetPosition());
			gotActionData.m_Player.DamageAllLegs(gotActionData.m_DmgTaken); //Additionnal leg specific damage dealing

			float healthCoef = Math.InverseLerp(HEALTH_LOW_SPEED_VALUE, HEALTH_HIGH_SPEED_VALUE, carSpeed);
			healthCoef = Math.Clamp(healthCoef, 0.0, 1.0);
			gotActionData.m_Player.ProcessDirectDamage(DamageType.CUSTOM, gotActionData.m_Player, "", "FallDamageHealth", posMS, healthCoef);
		}

		if (gotActionData.m_Car)
		{
			CarScript car;
			if (Class.CastTo(car, gotActionData.m_Car))
				car.ForceUpdateLightsEnd();
		}
	}
	
	//Manage all jumping out of vehicle damage logic
	void ApplyJumpOutDmg(ActionData action_data)
	{
		GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
		PlayerBase player = gotActionData.m_Player;
		
		array<ClothingBase> equippedClothes = new array<ClothingBase>;
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("LEGS")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BACK")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("VEST")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("HeadGear")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("Mask")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BODY")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("FEET")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("GLOVES")));

		// -----------------------------------------------

		//Lower shock taken if player uses a helmet
		ItemBase headGear = ClothingBase.Cast(player.GetItemOnHead());
		HelmetBase helmet;
		if (Class.CastTo(helmet, headGear))
			gotActionData.m_ShockTaken *= 0.5;

		// -----------------------------------------------
		
		int randNum; //value used for probability evaluation
		randNum = Math.RandomInt(0, 100);
		if (gotActionData.m_CarSpeed < LOW_SPEED_VALUE)
		{
			if (randNum < 20)
				player.GiveShock(-gotActionData.m_ShockTaken); //To inflict shock, a negative value must be passed

			randNum = Math.RandomIntInclusive(0, PlayerBase.m_BleedingSourcesLow.Count() - 1);
			
			player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow[randNum]);
		}
		else if (gotActionData.m_CarSpeed >= LOW_SPEED_VALUE && gotActionData.m_CarSpeed < HIGH_SPEED_VALUE)
		{
			if (randNum < 50)
				player.GiveShock(-gotActionData.m_ShockTaken);

			randNum = Math.RandomInt(0, PlayerBase.m_BleedingSourcesUp.Count() - 1);
			
			player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp[randNum]);
		}
		else if (gotActionData.m_CarSpeed >= HIGH_SPEED_VALUE)
		{
			if (!headGear)
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection("Head");

			if (randNum < 75)
				player.GiveShock(-gotActionData.m_ShockTaken);
		}
		
		//Damage all currently equipped clothes
		foreach (ClothingBase cloth : equippedClothes)
		{
			//If no item is equipped on slot, slot is ignored
			if (cloth == null)
				continue;

			cloth.DecreaseHealth(gotActionData.m_DmgTaken, false);
		}
	}
}
