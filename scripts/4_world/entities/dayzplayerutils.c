// *************************************************************************************
// ! DayZPlayerUtils - some utils used for dayz player - static functions
// *************************************************************************************
class DayZPlayerUtils
{
	//--------------------------------------------------
	// Debug Draw
	
	//! enables debug draw in functions 
	static proto native void 		EnableDebugDraw(bool pEnable);
	static proto native void 		DrawDebugText(string text, vector pos, float size);

	//! clear info - new draw starts
	static proto native void 		DrawStartFrame();	
	//! draws debug box (color=red)
	static proto native void 		DrawDebugBox(vector pos, float size, int color);


	//--------------------------------------------------
	// animation override

	//! overrides total animation translation
	static proto native bool		DebugOverrideAnimationTranslation(string pAnimName, vector pTranslation);	
	
	//! overrides total animation rotation
	static proto native bool		DebugOverrideAnimationRotation(string pAnimName, vector pRotation);	
	
	//! overrides total animation speed
	static proto native bool		DebugOverrideAnimationSpeed(string pAnimName, float pSpeed);	


	//--------------------------------------------------
	// 

	//! pLimits in form   Xvalue, Ymin, Ymax,   Xvalue1, Ymin1, Ymax1, .... 
	//! sample 	0, 0, 5, 	1, 2, 3, 	2, 0, 1

	//! for X <= 0 it clamps Y to Rangle (0,5) 	
	//! for X == 0.5 it clamps Y to Rangle (1,4) 	
	//! for X == 1 it clamps Y to Rangle (2,3) 	
	//! for X == 1.5 it clamps Y to Rangle (1,2) 	
	//! for X >= 2 it clamps Y to Rangle (0,1) 	
	
	static proto float 				LinearRangeClamp(float pValueX, float pValueY, float pLimits[]);




	//--------------------------------------------------
	// From Physics World

	//! returns entities  overlapping/touching in AABB box -
	static proto native void 		PhysicsGetEntitiesInBox(vector min, vector max, out array<EntityAI> entList);




	//--------------------------------------------------
	// From Scene

	//! returns entities overlapping/touching in AABB box - 
	static proto native void 		SceneGetEntitiesInBox(vector min, vector max, out array<EntityAI> entList);



	//--------------------------------------------------
	// Custom player functions

	/**@fn		GetEntitiesInCone
	 * @brief	returns entities in height-extended 2D cone
	 * @param[in]	angle 	\p 	angle in degrees
	 * @param[in]	minHeigh	\p cone 3D-extension Y min
	 * @param[in]	maxHeight	\p cone 3D-extension Y max
	 * @note		the detection hits points/faces from ANY non-edit model geometry, including resolution LODs, regardless of animation hiding (scaling). That is why certain animated objects, like flags in folded state, get detected from significant distance away!
	 **/
	static proto native void 		GetEntitiesInCone(vector pos, vector dir, float angle, float dist, float minHeigh, float maxHeight, out array<Object> entList);



	//! returns fight target
	static Object 				GetMeleeTarget(vector pos, vector dir, float angle, float dist, float minHeight, float maxHeight, EntityAI pToIgnore, array<typename> targetableObjects, out array<Object> allTargets = NULL)
	{
		// Print("In: GetFightTarget");
		InitCachedEntList();

		GetEntitiesInCone(pos, dir, angle, dist, minHeight, maxHeight, m_CachedEntList);
		
		Object ret		= NULL;
		Object obj 		= NULL;

		float 		retVal 	= dist * 2;		// max value 
		float 		tgAngle = Math.Tan(Math.DEG2RAD * angle);

		// DrawStartFrame();
		
		foreach(auto ent: m_CachedEntList)
		{		
			if (ent == pToIgnore)
			{
				continue;
			}
			
			Class.CastTo(obj, ent);
			if(obj)
			{
				// check for targetable objects
				if( !obj.IsAnyInherited(targetableObjects) )
					{ continue; }
				
				if( !obj.IsAlive() )
					{ continue; }
			}

			vector entpos = ent.GetPosition();

			vector diff 		= entpos - pos;
			float  cDistSq 		= diff.LengthSq();
			if (cDistSq > dist*dist)	// out of distance
			{
				//Print("out of distance");
				continue;
			}

			float  frontDist 	= diff[0]*dir[0] + diff[2]*dir[2];
			if (frontDist < 0.1) 	// behind the pos/dist half plane or further than dist
			{
				//Print("behind the pos/dist half plane or further than dist");
				continue;
			}
		
			vector project  = pos + dir * frontDist;
			vector posdiff = Vector(project[0] - entpos[0], 0, project[2] - entpos[2]);
			float sideDist = posdiff.LengthSq();

			if (sideDist > tgAngle)	// out of cone
			{
				//Print("out of cone");
				continue;
			}

			float  sum  	= frontDist + sideDist;
			if (sum < retVal)
			{
				ret 	= ent;
				retVal 	= sum;
			}
			else
			{
				//Print("sum !< retVal");
			}

			// string txt 		= project.ToString() + ": " + sum.ToString(); 
			// DrawDebugBox(project,0.01,0xffff0000);
			// DrawDebugBox(entpos,0.01,0xffff0000);
			// DrawDebugText(txt, project, 1.0);
			allTargets.Insert(ent);
		}

		return ret;
	}
	
	static proto native bool PlayerCanChangeStance(DayZPlayer pPlayer, int pTargetStance, bool forceCheck = false);
	
	//! inventory DayZPlayer utils
	
	/**@fn		FindMagazinesForAmmo
	 * @brief	searches inventory for all magazines that can hold specified ammo type
	 * @param[in]	player 	\p 	DayZPlayer
	 * @param[in]	ammoTypeName	\p	type name of the cartridge
	 * @param[out]	mags	\p	array filled with suitable magazines
	 * @returns		true if found at least one
	 **/
	static proto native bool FindMagazinesForAmmo(DayZPlayer player, string ammoTypeName, out array<Magazine> mags);
	
	/*static bool HandleEjectMagazine (DayZPlayer player, Weapon weapon, int muzzleIndex)
	{
		int slotId = weapon.GetSlotFromMuzzleIndex(muzzleIndex);
		EntityAI att = weapon.FindAttachment(slotId);
		if (att)
		{
			InventoryLocation loc = new InventoryLocation;
			if (player.FindFreeLocationFor(att, loc))
			{
				return weapon.EjectMagazine(muzzleIndex, loc);
			}
		}
		return false;
	}*/

	static Magazine SelectStoreCartridge(DayZPlayer player, Weapon_Base weapon, int muzzleIndex, Magazine exclude_mag, float damage, string magTypeName)
	{
		if (damage < 1.0)
		{
			// find suitable heap / mag (but not the excluded one)
			ref array<Magazine> mags = new array<Magazine>;
			if (DayZPlayerUtils.FindMagazinesForAmmo(player, magTypeName, mags))
			{
				int sz = mags.Count();
				for (int i = 0; i < sz; ++i)
				{
					Magazine mag_i = mags.Get(i);
					if (mag_i != exclude_mag && mag_i.CanAddCartridges(1))
					{
						return mag_i;
					}
				}
			}
			
			// create a new one in inventory
			InventoryLocation inv_loc = new InventoryLocation;
			if (player.GetInventory().FindFirstFreeLocationForNewEntity(magTypeName, FindInventoryLocationType.ANY, inv_loc))
			{
				EntityAI eai_inv = GetGame().SpawnEntity(magTypeName, inv_loc, ECE_IN_INVENTORY, RF_DEFAULT);
				if (eai_inv && eai_inv.IsInherited(Magazine))
				{
					Magazine mag_inv;
					if (Class.CastTo(mag_inv, eai_inv))
					{
						mag_inv.ServerSetAmmoCount(0);
						return mag_inv;
					}
				}
			}
		}

		vector pos = player.GetPosition();
		EntityAI eai_gnd = player.SpawnEntityOnGroundPos(magTypeName, pos);
		if (eai_gnd && eai_gnd.IsInherited(Magazine))
		{
			Magazine mag_gnd;
			if (Class.CastTo(mag_gnd, eai_gnd))
			{
				mag_gnd.ServerSetAmmoCount(0);
				return mag_gnd;
			}
		}

		return NULL;
	}

	static bool HandleDropMagazine(DayZPlayer player, Magazine mag)
	{
		vector m4[4];
		Math3D.MatrixIdentity4(m4);
		GameInventory.PrepareDropEntityPos(player, mag, m4);
		InventoryLocation il_mag_next = new InventoryLocation;
		il_mag_next.SetGround(mag, m4);
		InventoryLocation il_mag_curr = new InventoryLocation;
		if (mag.GetInventory().GetCurrentInventoryLocation(il_mag_curr))
		{
			return GameInventory.LocationSyncMoveEntity(il_mag_curr, il_mag_next);
		}
		else
		{
			Error("DayZPlayerUtils::HandleDropMagazine - cannot get current inv location of mag=" + mag);
			return false;
		}
	}
	
	static bool HandleDropCartridge(DayZPlayer player, float damage, string cartTypeName, string magTypeName)
	{
		vector pos = player.GetPosition();
		EntityAI eai_gnd = player.SpawnEntityOnGroundPos(magTypeName, pos);
		if (eai_gnd && eai_gnd.IsInherited(Magazine))
		{
			Magazine mag_gnd;
			if (Class.CastTo(mag_gnd, eai_gnd))
			{
				mag_gnd.ServerSetAmmoCount(0);
				if (mag_gnd.ServerStoreCartridge(damage, cartTypeName))
				{
					return true;
				}
			}
		}
		return false;
	}

	static bool HandleStoreCartridge(DayZPlayer player, Weapon_Base weapon, int muzzleIndex, float damage, string cartTypeName, string magTypeName, bool CanDrop = true)
	{
		if (damage < 1.0)
		{
			// find suitable heap / mag
			ref array<Magazine> mags = new array<Magazine>;
			if (DayZPlayerUtils.FindMagazinesForAmmo(player, magTypeName, mags))
			{	
				int health_lvl = -1;	
				float test_heatlh = 1 - damage;		
				int sz = mags.Count();
				for (int i = 0; i < sz; ++i)
				{
					Magazine mag_i = mags.Get(i);
					 // add small amout for 
					
					if( health_lvl == -1)
					{
						if (mag_i.CanAddCartridges(1) /*&& Math.AbsFloat(1 - mag_i.GetHealthLevelValue(mag_i.GetHealthLevel()) - damage) < 0.01*/)
						{
							int num_health_lvl = mag_i.GetNumberOfHealthLevels();
							for( int j = 1; j < num_health_lvl; j++)
							{
								if (mag_i.GetHealthLevelValue(j) < test_heatlh )
								{
									health_lvl = j - 1;
									break;
								}
							}
						}
					}
					
					if( mag_i.GetHealthLevel() == health_lvl )
					{
						if (mag_i.ServerStoreCartridge(damage, cartTypeName))
							return true;
					}
				}
			}
				
			// create a new one in inventory
			InventoryLocation inv_loc = new InventoryLocation;
			if (player.GetInventory().FindFirstFreeLocationForNewEntity(magTypeName, FindInventoryLocationType.ANY, inv_loc))
			{
				EntityAI eai_inv = GetGame().SpawnEntity(magTypeName, inv_loc, ECE_IN_INVENTORY, RF_DEFAULT);
				if (eai_inv && eai_inv.IsInherited(Magazine))
				{
					Magazine mag_inv;
					if (Class.CastTo(mag_inv, eai_inv))
					{
						mag_inv.ServerSetAmmoCount(0);
						mag_inv.SetHealth("", "", (1 - damage) * mag_inv.GetMaxHealth());
						if (mag_inv.ServerStoreCartridge(damage, cartTypeName))
							return true;
					}
				}
			}
		
			// drop on ground
			if ( CanDrop )
				return HandleDropCartridge(player, damage, cartTypeName, magTypeName);
			
		}
		return false;
	}
	
	/**@fn		InitComponentCollisions
	 * @brief	initializes table of component collisions
	 * @param[in]	player 	\p 	DayZPlayer
	 * @param[in]	boxes	\p	array filled with box @see ComponentCollisionBox
	 * @param[in]	capsules	\p	array filled with capsules @see ComponentCollisionCapsule
	 * @returns		true if success
	 **/
	static proto native bool InitComponentCollisions(Human player, array<ref ComponentCollisionBox> boxes, array<ref ComponentCollisionCapsule> capsules);

	/**@fn		IsComponentCollisionInitialized
	 * @returns		true if already initialized
	 **/
	static proto native bool IsComponentCollisionInitialized();

	/**@fn		ClearComponentCollisions
	 **/
	static proto native void ClearComponentCollisions();

	static proto native vector GetMemoryPointPositionBoneRelative(DayZPlayer pPlayer, int pBoneIndex, int pPointIndex);
	
	static void InitPlayerComponentCollisions(Human player)
	{
		if (IsComponentCollisionInitialized())
			Error("DayZPlayerUtils.InitComponentCollisions: already initialized!");

		array<ref ComponentCollisionBox> b = new array<ref ComponentCollisionBox>;
		b.Insert(new ComponentCollisionBox(0.40, 0.31, 0.31, "Pelvis", "Spine2"));
		b.Insert(new ComponentCollisionBox(0.40, 0.31, 0.31, "Spine3", "Neck"));

		array<ref ComponentCollisionCapsule> c = new array<ref ComponentCollisionCapsule>;
		c.Insert(new ComponentCollisionCapsule(0.11, "Neck1", "Head"));
		c.Insert(new ComponentCollisionCapsule(0.09, "LeftArm", "LeftArmRoll"));
		c.Insert(new ComponentCollisionCapsule(0.08, "LeftForeArm", "LeftHand"));
		c.Insert(new ComponentCollisionCapsule(0.09, "RightArm", "RightArmRoll"));
		c.Insert(new ComponentCollisionCapsule(0.08, "RightForeArm", "RightHand"));
		c.Insert(new ComponentCollisionCapsule(0.11, "LeftUpLeg", "LeftUpLegRoll"));
		c.Insert(new ComponentCollisionCapsule(0.10, "LeftLeg", "LeftFoot"));
		c.Insert(new ComponentCollisionCapsule(0.11, "RightUpLeg", "RightUpLegRoll"));
		c.Insert(new ComponentCollisionCapsule(0.10, "RightLeg", "RightFoot"));

		DayZPlayerUtils.InitComponentCollisions(player, b, c);
	}
	
	static int ConvertStanceMaskToStanceIdx(int stanceMask)
	{
		switch (stanceMask)
		{
			case stanceMask & DayZPlayerConstants.STANCEMASK_PRONE:
				return DayZPlayerConstants.STANCEIDX_PRONE;
			
			case stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH:
				return DayZPlayerConstants.STANCEIDX_CROUCH;
			
			case stanceMask & DayZPlayerConstants.STANCEMASK_ERECT:
				return DayZPlayerConstants.STANCEIDX_ERECT;
			
			case stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE:
				return DayZPlayerConstants.STANCEIDX_RAISEDPRONE;
			
			case stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH:
				return DayZPlayerConstants.STANCEIDX_RAISEDCROUCH;
			
			case stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT:
				return DayZPlayerConstants.STANCEIDX_RAISEDERECT;
		}
		
		return -1;
	}
	
	static EWaterLevels CheckWaterLevel(DayZPlayer pPlayer, out vector waterLevel)
	{
		SHumanCommandSwimSettings swimData = pPlayer.GetDayZPlayerType().CommandSwimSettingsW();
		vector 	pp = pPlayer.GetPosition();
		waterLevel = HumanCommandSwim.WaterLevelCheck(pPlayer, pp);
		
		if (waterLevel[1] < swimData.m_fToCrouchLevel)
		{
			return EWaterLevels.LEVEL_LOW;
		}
		else if (waterLevel[1] >= swimData.m_fToCrouchLevel && waterLevel[1] < swimData.m_fToErectLevel)
		{
			return EWaterLevels.LEVEL_CROUCH;
		}
		else// if (waterLevel[1] >= swimData.m_fToErectLevel)
		{
			//! if total water depth >= 1.5m && character is 1.5m in water 
			if (waterLevel[0] >= swimData.m_fWaterLevelIn && waterLevel[1] >= swimData.m_fWaterLevelIn)
			{
				return EWaterLevels.LEVEL_SWIM_START;
			}
			else
			{
				return EWaterLevels.LEVEL_ERECT;
			}
		}
	}
	
	//------------------------------------------------	
	// private data	


	private static ref array<Object> 	m_CachedEntList;


	//! cannot be instantiated 
	private void DayZPlayerUtils()	{};

	//!
	private static 	void InitCachedEntList()
	{
		if (m_CachedEntList == NULL)
		{
			m_CachedEntList = new array<Object>;	
		}

		m_CachedEntList.Clear();
	}
}

class ComponentCollisionBox
{
	vector m_Offset;
	string m_BoneName0;
	string m_BoneName1;

	void ComponentCollisionBox(float x, float y, float z, string b0, string b1)
	{
		m_Offset[0] = x;
		m_Offset[1] = y;
		m_Offset[2] = z;
		m_BoneName0 = b0;
		m_BoneName1 = b1;
	}
};

class ComponentCollisionCapsule
{
	float m_Radius;
	string m_BoneName0;
	string m_BoneName1;

	void ComponentCollisionCapsule(float r, string b0, string b1)
	{
		m_Radius = r;
		m_BoneName0 = b0;
		m_BoneName1 = b1;
	}
};