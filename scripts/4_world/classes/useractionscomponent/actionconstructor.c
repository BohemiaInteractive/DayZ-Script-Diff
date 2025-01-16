class ActionConstructor
{
	bool ConstructActions( out array<ref ActionBase> actionsArray, out map<typename, ActionBase> actionNameActionMap )
	{
		TTypenameArray action_array_names = new TTypenameArray;
		RegisterActions(action_array_names);
		Sort(action_array_names,action_array_names.Count());
		
		actionsArray = new array<ref ActionBase>;
		actionNameActionMap = new map<typename, ActionBase>;
		ActionBase new_action;
		
		for(int i = 0; i < action_array_names.Count(); i++)
		{
			new_action = ActionBase.Cast(action_array_names[i].Spawn());
			if(new_action)
			{
				new_action.CreateConditionComponents();
				new_action.SetID(actionsArray.Count());
				actionsArray.Insert(new_action);
				actionNameActionMap.Insert(action_array_names[i], new_action);
			}
		}
		return true;
	}
	
	void RegisterActions(TTypenameArray actions)
	{
		// SingleUse Actions	
		actions.Insert(ActionOpen);
		actions.Insert(ActionClose);
		actions.Insert(ActionConsumeSingle);
		actions.Insert(ActionForceConsumeSingle);
		actions.Insert(ActionMeasureTemperatureSelf);
		actions.Insert(ActionMeasureTemperatureTarget);
		actions.Insert(ActionDisinfectSelf);
		actions.Insert(ActionDisinfectTarget);
		actions.Insert(ActionPlugIn);
		actions.Insert(ActionTogglePlaceObject);
		actions.Insert(ActionTogglePlaceObjectDigging);
		actions.Insert(ActionTurnOnWhileInHands);
		actions.Insert(ActionTurnOffWhileInHands);
		actions.Insert(ActionPlantSeed);
		actions.Insert(ActionAttachSeeds);
		actions.Insert(ActionDisinfectPlantBit);
		actions.Insert(ActionTurnOnHeatpack);
		actions.Insert(ActionTurnOnTransmitter);	
		actions.Insert(ActionTurnOffTransmitter);
		actions.Insert(ActionWashHandsItemContinuous);
		actions.Insert(ActionPlaceFireplaceIndoor);
		actions.Insert(ActionPlaceOvenIndoor);
		actions.Insert(ActionCreateIndoorFireplace);
		actions.Insert(ActionCreateGreenhouseGardenPlot);
		actions.Insert(ActionCreateIndoorOven);
		actions.Insert(ActionAttach);
		actions.Insert(ActionAttachOnProxy);
		actions.Insert(ActionAttachOnTrap);
		actions.Insert(ActionPlugTargetIntoThis);
		actions.Insert(ActionEmptySeedsPack);
		actions.Insert(ActionInjectSelf);
		actions.Insert(ActionInjectTarget);
		actions.Insert(ActionInjectEpinephrineSelf);
		actions.Insert(ActionInjectEpinephrineTarget);
		actions.Insert(ActionInjectMorphineSelf);
		actions.Insert(ActionInjectMorphineTarget);
		actions.Insert(ActionAttachOnSelection);
		actions.Insert(ActionTurnOnWeaponFlashlight);
		actions.Insert(ActionTurnOffWeaponFlashlight);
		actions.Insert(ActionNextCombinationLockDial);
		actions.Insert(ActionAttachToConstruction);
		actions.Insert(ActionClapBearTrapWithThisItem);
		actions.Insert(ActionDisarmMine);
		actions.Insert(ActionPlaceFireplaceIntoBarrel);
		actions.Insert(ActionStopEngine);
		actions.Insert(ActionStopEngineBoat);
		actions.Insert(ActionUnfoldMap);
		actions.Insert(ActionFoldMap);
		actions.Insert(ActionDropItem);
		actions.Insert(ActionDropItemSimple);
		actions.Insert(ActionSwitchSeats);
		actions.Insert(ActionTurnOnHeadtorch);
		actions.Insert(ActionTurnOffHeadtorch);
		actions.Insert(ActionTurnOnHelmetFlashlight);
		actions.Insert(ActionTurnOffHelmetFlashlight);
		actions.Insert(ActionToggleNVG);
		actions.Insert(ActionAttachOnTentProxy);
		actions.Insert(ActionTriggerRemotely);
		actions.Insert(ActionDisarmExplosive);
		actions.Insert(ActionDisarmExplosiveWithRemoteDetonator);
		actions.Insert(ActionDisarmExplosiveWithRemoteDetonatorUnpaired);
		actions.Insert(ActionEatPillFromBottle);
		actions.Insert(ActionEatTabletFromWrapper);

		// Continuous use actions
		actions.Insert(ActionWorldCraft);
		actions.Insert(ActionCraftImprovisedHandsCover);
		actions.Insert(ActionCraftImprovisedHeadCover);
		actions.Insert(ActionCraftImprovisedFeetCover);
		actions.Insert(ActionCraftImprovisedFaceCover);
		actions.Insert(ActionCraftImprovisedTorsoCover);
		actions.Insert(ActionCraftImprovisedLegsCover);
		actions.Insert(ActionCraftImprovisedEyePatch);
		actions.Insert(ActionCraftRopeBelt);
		actions.Insert(ActionDeCraftRopeBelt);
		actions.Insert(ActionCraftBoneKnife);
		actions.Insert(ActionCraftArmband);
		actions.Insert(ActionCraftBoneKnifeEnv);
		actions.Insert(ActionCraftStoneKnifeEnv);
		actions.Insert(ActionSkinning);
		actions.Insert(ActionPlaceObject);
		actions.Insert(ActionDeployObject);
		actions.Insert(ActionDeployHuntingTrap);
		actions.Insert(ActionMineTree);
		actions.Insert(ActionMineBush);
		actions.Insert(ActionMineRock);
		actions.Insert(ActionMineRock1H);
		actions.Insert(ActionDigGardenPlot);
		actions.Insert(ActionDismantleGardenPlot);
		actions.Insert(ActionWaterPlant);
		actions.Insert(ActionFertilizeSlot);
		actions.Insert(ActionDisinfectPlant);
		actions.Insert(ActionFillObject);
		actions.Insert(ActionDefibrilateSelf);
		actions.Insert(ActionDefibrilateTarget);
		actions.Insert(ActionTestBloodSelf);
		actions.Insert(ActionTestBloodTarget);
		actions.Insert(ActionGiveBloodSelf);
		actions.Insert(ActionGiveBloodTarget);
		actions.Insert(ActionGiveSalineSelf);
		actions.Insert(ActionGiveSalineTarget);
		actions.Insert(ActionCollectBloodSelf);
		actions.Insert(ActionCollectBloodTarget);
		actions.Insert(ActionCollectSampleSelf);
		actions.Insert(ActionCollectSampleTarget);
		actions.Insert(ActionCoverHeadSelf);
		actions.Insert(ActionUncoverHeadSelf);
		actions.Insert(ActionCoverHeadTarget);
		actions.Insert(ActionShave);
		actions.Insert(ActionSplintSelf);
		actions.Insert(ActionSplintTarget);
		actions.Insert(ActionForceDrink);
		actions.Insert(ActionForceFeed);
		actions.Insert(ActionForceFeedSmall);
		actions.Insert(ActionEatBig);
		actions.Insert(ActionEatMeat);
		actions.Insert(ActionEatFruit);
		actions.Insert(ActionDrink);
		actions.Insert(ActionDrinkCookingPot);
		actions.Insert(ActionBandageSelf);
		actions.Insert(ActionBandageTarget);
		actions.Insert(ActionRestrainTarget);
		actions.Insert(ActionRestrainSelf);
		actions.Insert(ActionUnrestrainTarget);
		actions.Insert(ActionUnrestrainSelf);
		actions.Insert(ActionFillBottleBase);
		actions.Insert(ActionFillBottleSnow);
		actions.Insert(ActionUpgradeTorchFromGasPump);
		actions.Insert(ActionRefuelTorch);
		actions.Insert(ActionTuneFrequency);
		actions.Insert(ActionTuneRadioStation);
		actions.Insert(ActionIgniteFireplaceByAir);
		actions.Insert(ActionLightItemOnFire);
		actions.Insert(ActionLightItemOnFireWithBlowtorch);
		actions.Insert(ActionExtinguishFireplaceByLiquid);
		actions.Insert(ActionExtinguishFireplaceByExtinguisher);
		actions.Insert(ActionSewSelf);
		actions.Insert(ActionSewTarget);
		actions.Insert(ActionBuryAshes);
		actions.Insert(ActionCookOnStick);
		actions.Insert(ActionFillGeneratorTank);
		actions.Insert(ActionSawPlanks);
		actions.Insert(ActionBuryBody);
		actions.Insert(ActionEatCan);
		actions.Insert(ActionEatSmallCan);
		actions.Insert(ActionForceFeedCan);
		actions.Insert(ActionWaterGardenSlot);
		actions.Insert(ActionWringClothes);
		actions.Insert(ActionLoadMagazine);
		actions.Insert(ActionEmptyMagazine);
		actions.Insert(ActionBuildPart);
		actions.Insert(ActionActionBuildPartNoTool);
		actions.Insert(ActionDismantlePart);
		actions.Insert(ActionDestroyPart);
		actions.Insert(ActionCPR);
		actions.Insert(ActionDrinkPondContinuous);
		actions.Insert(ActionEatSnowContinuous);
		actions.Insert(ActionDrinkThroughContinuous);
		actions.Insert(ActionEat);
		actions.Insert(ActionEatSmall);
		actions.Insert(ActionMineTreeBark);
		actions.Insert(ActionDigWorms);
		actions.Insert(ActionShaveTarget);
		actions.Insert(ActionDrinkWellContinuous);
		actions.Insert(ActionUncoverHeadTarget);
		actions.Insert(ActionLockDoors);
		actions.Insert(ActionUnlockDoors);
		actions.Insert(ActionUnpackBox);
		actions.Insert(ActionWashHandsWater);
		actions.Insert(ActionWashHandsSnow);
		actions.Insert(ActionWashHandsWell);
		actions.Insert(ActionDrinkCan);
		actions.Insert(ActionBuildOven);
		actions.Insert(ActionDismantleOven);
		actions.Insert(ActionBuildStoneCircle);
		actions.Insert(ActionDismantleStoneCircle);
		actions.Insert(ActionFillFuel);
		actions.Insert(ActionDrainLiquid);
		actions.Insert(ActionPourLiquid);
		actions.Insert(ActionStartEngine);
		actions.Insert(ActionStartEngineBoat);
		actions.Insert(ActionRaiseMegaphone);
		actions.Insert(ActionActivateTrap);
		actions.Insert(ActionEmptyBottleBase);
		actions.Insert(ActionEmptyCookingPot);
		actions.Insert(ActionViewOptics);
		actions.Insert(ActionToggleNVMode);
		actions.Insert(ActionViewBinoculars);
		actions.Insert(ActionFoldBaseBuildingObject);
		actions.Insert(ActionMountBarbedWire);
		actions.Insert(ActionUnmountBarbedWire);
		actions.Insert(ActionDialCombinationLock);
		actions.Insert(ActionDialCombinationLockOnTarget);
		actions.Insert(ActionFillCoolant);
		actions.Insert(ActionFillOil);
		actions.Insert(ActionGagSelf);
		actions.Insert(ActionGagTarget);
		actions.Insert(ActionUngagSelf);
		actions.Insert(ActionUngagTarget);
		actions.Insert(ActionDigInStash);
		actions.Insert(ActionDigOutStash);
		actions.Insert(ActionMineBushByHand);
		actions.Insert(ActionBreakLongWoodenStick);
		actions.Insert(ActionFishingNew);
		actions.Insert(ActionRepairTent);
		actions.Insert(ActionRepairTentPart);
		actions.Insert(ActionRepairPart);
		actions.Insert(ActionRepairCarPart);
		actions.Insert(ActionRepairCarPartWithBlowtorch);
		actions.Insert(ActionRepairCarChassis);
		actions.Insert(ActionRepairBoatChassis);
		actions.Insert(ActionRepairCarChassisWithBlowtorch);
		actions.Insert(ActionRepairCarEngineWithBlowtorch);
		actions.Insert(ActionRepairBoatEngine);
		actions.Insert(ActionRepairShelter);
		actions.Insert(ActionRepairItemWithBlowtorch);
		actions.Insert(ActionLockAttachment);
		actions.Insert(ActionUnpackGift);
		actions.Insert(ActionPackGift);
		actions.Insert(ActionAttachExplosivesTrigger);
		actions.Insert(ActionArmExplosive);
		actions.Insert(ActionTurnValve);
		actions.Insert(ActionTurnValveUndergroundReservoir);
		actions.Insert(ActionEatCereal);
		actions.Insert(ActionUnlockShippingContainer);
		
		// Interact actions
		actions.Insert(ActionPullOutPlug);
		actions.Insert(ActionUnplugThisByCord);
		actions.Insert(ActionTurnOnWhileOnGround);
		actions.Insert(ActionTurnOffWhileOnGround);
		actions.Insert(ActionTurnOnPowerGenerator);
		actions.Insert(ActionTurnOffPowerGenerator);
		actions.Insert(ActionTurnOnSpotlight);
		actions.Insert(ActionTurnOffSpotlight);
		actions.Insert(ActionRemoveSeed);
		actions.Insert(ActionRemovePlant);
		actions.Insert(ActionHarvestCrops);
		actions.Insert(ActionFoldObject);
		actions.Insert(ActionRepositionPluggedItem);
		actions.Insert(ActionTurnOnTransmitterOnGround);
		actions.Insert(ActionTurnOffTransmitterOnGround);
		actions.Insert(ActionTuneFrequencyOnGround);
		actions.Insert(ActionToggleTentOpen);
		actions.Insert(ActionPackTent);
		actions.Insert(ActionOpenBarrel);
		actions.Insert(ActionCloseBarrel);
		actions.Insert(ActionOpenBarrelHoles);
		actions.Insert(ActionCloseBarrelHoles);
		actions.Insert(ActionOpenDoors);
		actions.Insert(ActionCloseDoors);
		actions.Insert(ActionLockedDoors);
		actions.Insert(ActionEnterLadder);
		actions.Insert(ActionExitLadder);
		actions.Insert(ActionGetInTransport);
		actions.Insert(ActionTakeItem);
		actions.Insert(ActionDetachPowerSourceFromPanel);
		actions.Insert(ActionSwapItemToHands);
		actions.Insert(ActionTakeItemToHands);
		actions.Insert(ActionOpenCarDoors);
		actions.Insert(ActionCloseCarDoors);
		actions.Insert(ActionOpenCarDoorsOutside);
		actions.Insert(ActionCloseCarDoorsOutside);
		actions.Insert(ActionGetOutTransport);
		actions.Insert(ActionAnimateSeats);
		actions.Insert(ActionSidePlateInteract);
		actions.Insert(ActionDetach);
		actions.Insert(ActionOpenFence);
		actions.Insert(ActionCloseFence);
		actions.Insert(ActionNextCombinationLockDialOnTarget);
		actions.Insert(ActionCheckPulseTarget);
		actions.Insert(ActionUnpin);
		actions.Insert(ActionPin);
		actions.Insert(ActionRaiseFlag);
		actions.Insert(ActionLowerFlag);
		actions.Insert(ActionDeconstructShelter);
		actions.Insert(ActionBuildShelter);
		actions.Insert(ActionDetachFromTarget);
		actions.Insert(ActionDetachFromTarget_SpecificSlot_WoodenLogs);
		actions.Insert(ActionDetachFromTarget_SpecificSlot_WoodenPlanks);
		actions.Insert(ActionDetachFromTarget_SpecificSlot_MetalSheets);
		actions.Insert(ActionDetachFromTarget_SpecificSlotsCategory_Barrel);
		actions.Insert(ActionDetachFromTarget_SpecificSlotsCategory_WoodenCrate);
		actions.Insert(ActionTakeArrow);
		actions.Insert(ActionTakeArrowToHands);
		actions.Insert(ActionOperatePanelPowerStation);
		
		//vehicle actions
		actions.Insert(ActionSwitchLights);
		actions.Insert(ActionCarHornShort);
		actions.Insert(ActionCarHornLong);
		actions.Insert(ActionPushCar);
		actions.Insert(ActionPushBoat);
		
		#ifdef DIAG_DEVELOPER 
		//Instant action - debug
		actions.Insert(ActionDebug);
		#endif
			
		//Weapon
		actions.Insert(FirearmActionAttachMagazine);
		actions.Insert(FirearmActionLoadMultiBullet);
		actions.Insert(FirearmActionLoadBullet);
		
		actions.Insert(ActionLoadMagazineQuick);
		actions.Insert(FirearmActionAttachMagazineQuick);
		actions.Insert(FirearmActionMechanicManipulate);
		actions.Insert(FirearmActionUnjam);
		
		actions.Insert(FirearmActionDetachMagazine);		
		
		actions.Insert(FirearmActionLoadBulletQuick);
		actions.Insert(FirearmActionLoadMultiBulletQuick);
		actions.Insert(FirearmActionLoadMultiBulletRadial);

		actions.Insert(ActionPullBodyFromTransport);
		
		actions.Insert(ActionSetAlarmClock);
		actions.Insert(ActionTurnOnAlarmClock);
		actions.Insert(ActionTurnOffAlarmClock);
		
		actions.Insert(ActionAttachWithSwitch);
		
		actions.Insert(ActionSetKitchenTimer);
		actions.Insert(ActionResetKitchenTimer);
		actions.Insert(ActionUseUndergroundPanel);
		actions.Insert(ActionUseUndergroundLever);
		actions.Insert(ActionUnrestrainTargetHands);
		
		actions.Insert(ActionCraftBolts);
		actions.Insert(ActionCraftBoltsFeather);
		
		actions.Insert(ActionDeCraftWitchHoodCoif);
		
		actions.Insert(ActionDeCraftDrysackBag);
		
		actions.Insert(ActionDummyContinuousRelease);
	}
};