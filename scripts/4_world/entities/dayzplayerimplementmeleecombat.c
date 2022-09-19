enum EMeleeHitType
{
	NONE = -1,

	LIGHT,
	HEAVY,
	SPRINT,
	KICK,
	FINISHER_LIVERSTAB,
	FINISHER_NECKSTAB,
	FINISHER_GENERIC,

	WPN_HIT,
	WPN_HIT_BUTTSTOCK,
	WPN_STAB,
	WPN_STAB_FINISHER,
}

class DayZPlayerImplementMeleeCombat
{
	//! Target selection settings
	protected const float				TARGETING_ANGLE_NORMAL	= 30.0; //!< Second Pass: Half angle of cone
	protected const float				TARGETING_ANGLE_SPRINT	= 15.0; //!< Second Pass: Half angle of cone during sprint
	protected const float				TARGETING_MIN_HEIGHT	= -2.0; //!< Second Pass: How deep the cone goes in meters from player position
	protected const float				TARGETING_MAX_HEIGHT	= 2.0; 	//!< Second Pass: How high the cone goes in meters from player position
	protected const float 				TARGETING_RAY_RADIUS_EX	= 0.5; 	//!< Second Pass: Max distance from ray projected from player looking direction
	protected const float				TARGETING_RAY_RADIUS	= 0.25; //!< DEPRECATED: "HitZoneSelection"
	protected const float				TARGETING_RAY_DIST		= 5.0; 	//!< DEPRECATED: "HitZoneSelection"
	protected const float				TARGETING_RAY_DIST_SHORT= 2.0;	//!< DEPRECATED: "HitZoneSelection"

	protected const float 				RANGE_EXTENDER_NORMAL	= 0.65; //!< General range extension
	protected const float 				RANGE_EXTENDER_SPRINT	= 1.35; //!< General range extension while in sprint
	
	protected const string				DEFAULT_HIT_ZONE 		= "Torso"; //!< DEPRECATED: "HitZoneSelection"
	
	//! Target selecting "component"
	protected ref MeleeTargeting		m_MeleeTargeting; //!< Contains logic for Second Pass
	
	//! Targets - types
	protected Object					m_TargetObject; 		//!< Main target found during most recent TargetSelection
	protected EMeleeTargetType			m_TargetType;  			//!< DEPRECATED: Was added but never used..?
	protected ref array<Object> 		m_AllTargetObjects; 	//!< All potential targets found during most recent TargetSelection
	
	#ifdef DEVELOPER
	protected Object					m_PreviousTargetObject; 		//!< Main target found during most recent TargetSelection
	protected ref array<Object> 		m_AllPreviousTargetObjects; 	//!< All potential targets found during most recent TargetSelection
	#endif
	
	protected ref array<typename>		m_TargetableObjects; 	//!< Typenames of all directly/preferred targetable objects (1st Pass + 2nd Pass)
	protected ref array<typename>		m_NonAlignableObjects; 	//!< Typenames of objects that can be targeted, but are not a priority (3rd Pass)
	protected ref array<string>			m_BlacklistedDamageZones; //!< List of blacklisted damage zone names (cannot use indices due to the possible changes when p3d components are recalculated)
	
	//! Parent
	protected DayZPlayerImplement		m_DZPlayer; //!< Player executing the melee

	//! Weapons - cache
	protected InventoryItem				m_Weapon; 		//!< Weapon used during most recent Update
	protected int						m_WeaponMode; 	//!< WeaponMode used during most recent Update
	protected float						m_WeaponRange; 	//!< WeaponRange used during most recent Update
	
	//! Misc - cache
	protected bool 						m_ForceUntargetable;	//!< Forcing current target as untargetable
	protected bool 						m_SprintAttack; 		//!< If most recent attack was a sprint attack
	protected bool						m_WasHit; 				//!< If most recent attack was 
	
	protected vector 					m_RayStart; 	//!< Start position of most recent HitZoneSelectionRaycast
	protected vector	 				m_RayEnd; 		//!< End position of most recent HitZoneSelectionRaycast
	protected vector	 				m_RayEndShort; 	//!< DEPRECATED: "HitZoneSelection"
	
	protected EMeleeHitType				m_HitType; 	//!< Hit type of the most recent attack

	//! Hit result - cache
	protected int 						m_HitZoneIdx; 		//!< Most recent target HitZone index
	protected string					m_HitZoneName; 		//!< Most recent target HitZone name
	protected vector					m_HitPositionWS;	//!< Most recent target position
	
	#ifdef DEVELOPER
	protected int 						m_PreviousHitZoneIdx; 		//!< Most recent target HitZone index
	protected string					m_PreviousHitZoneName; 		//!< Most recent target HitZone name
	protected vector					m_PreviousHitPositionWS;	//!< Most recent target position
	#endif

	// ------------------------------------------------------------
	// CONSTRUCTOR
	// ------------------------------------------------------------

	void DayZPlayerImplementMeleeCombat(DayZPlayerImplement player)
	{
		Init(player);
	}
	
	void Init(DayZPlayerImplement player)
	{
		m_DZPlayer 		= player;
		
		m_MeleeTargeting = new MeleeTargeting;
		
		m_HitZoneName	= "";
		m_HitZoneIdx 	= -1;
		m_HitPositionWS = vector.Zero;
		
		m_SprintAttack 	= false;
		m_WasHit 		= false;
		
		m_TargetObject      = null;
		m_TargetType		= EMeleeTargetType.ALIGNABLE;
		m_AllTargetObjects 	= new array<Object>;
		#ifdef DEVELOPER
		m_AllPreviousTargetObjects = new array<Object>;
		#endif

		m_TargetableObjects = new array<typename>; //checks against CONFIG hierarchy
		m_TargetableObjects.Insert(DayZPlayer);
		m_TargetableObjects.Insert(DayZInfected);
		m_TargetableObjects.Insert(DayZAnimal);

		m_NonAlignableObjects = new array<typename>;
		m_NonAlignableObjects.Insert(Building);
		m_NonAlignableObjects.Insert(Car);
		m_NonAlignableObjects.Insert(CarWheel);
		m_NonAlignableObjects.Insert(CarDoor);
		m_NonAlignableObjects.Insert(TentBase);
		m_NonAlignableObjects.Insert(BaseBuildingBase);
		
		m_BlacklistedDamageZones = new array<string>;
		m_BlacklistedDamageZones.Insert("Brain");
	}

	void ~DayZPlayerImplementMeleeCombat() {}

	// ------------------------------------------------------------
	// PUBLIC
	// ------------------------------------------------------------

	EMeleeHitType GetHitType()
	{
		return m_HitType;
	}

	void SetHitZoneIdx(int pHitZone)
	{
		m_HitZoneIdx = pHitZone;
	}
	
	EntityAI GetTargetEntity()
	{
		return EntityAI.Cast(m_TargetObject);
	}

	void SetTargetObject(Object pTarget)
	{
		m_TargetObject = pTarget;
	}
	
	//! component idx
	int GetHitZoneIdx()
	{
		return m_HitZoneIdx;
	}
	
	vector GetHitPos()
	{
		return m_HitPositionWS;
	}

	void SetHitPos(vector pHitPos)
	{
		m_HitPositionWS = pHitPos;
	}
	
	int GetWeaponMode()
	{
		return m_WeaponMode;
	}
	
	void Reset(InventoryItem weapon, EMeleeHitType hitMask, bool wasHitEvent = false)
	{
		m_Weapon 					= weapon;
		m_HitType 					= hitMask;
		m_TargetType 				= EMeleeTargetType.ALIGNABLE;
		m_SprintAttack 				= hitMask == EMeleeHitType.SPRINT;
		m_WeaponMode 				= SelectWeaponMode(weapon);
		m_WeaponRange 				= GetWeaponRange(weapon, m_WeaponMode);
		m_WasHit 					= wasHitEvent;

		#ifdef DEVELOPER
		m_AllPreviousTargetObjects	= m_AllTargetObjects;
		#endif
		m_AllTargetObjects.Clear();
	}
	
	void ResetTarget()
	{
		#ifdef DEVELOPER
		m_PreviousTargetObject 	= m_TargetObject;
		m_PreviousHitPositionWS = m_HitPositionWS;
		m_PreviousHitZoneIdx 	= m_HitZoneIdx;
		m_PreviousHitZoneName 	= m_HitZoneName;
		#endif
		
		InternalResetTarget();
	}

	void Update(InventoryItem weapon, EMeleeHitType hitMask, bool wasHitEvent = false)
	{
		Reset(weapon, hitMask, wasHitEvent);

		#ifndef SERVER
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			//Error("DayZPlayerImplementMeleeCombat - ScriptInputUserData already posted");
			return;
		}
		
		//! Select target
		TargetSelection();

		//! Store target into input packet
		if (GetGame().IsMultiplayer())
		{
			ScriptInputUserData ctx = new ScriptInputUserData;
			ctx.Write(INPUT_UDT_MELEE_TARGET);
			ctx.Write(m_TargetObject);
			ctx.Write(m_HitPositionWS);
			ctx.Write(m_HitZoneIdx);
			ctx.Send();
		}
		#endif
	}
	
	void CheckMeleeItem()
	{
		if (m_Weapon)
		{
			ItemBase item = ItemBase.Cast(m_Weapon.ProcessMeleeItemDamage(GetWeaponMode()));
			
			if (item && item.GetHierarchyRootPlayer())
			{
				PlayerBase.Cast(item.GetHierarchyRootPlayer()).SetCheckMeleeItem(item);
			}
			else if (m_Weapon && m_Weapon.GetHierarchyRootPlayer())
			{
				PlayerBase.Cast(m_Weapon.GetHierarchyRootPlayer()).SetCheckMeleeItem(ItemBase.Cast(m_Weapon));
			}
		}
	}
	
	// ------------------------------------------------------------
	// protected
	// ------------------------------------------------------------

	protected int SelectWeaponMode(InventoryItem weapon)
	{
		if ( weapon )
		{
			//! melee with firearm
			if (weapon.IsInherited(Weapon))
			{
				switch (m_HitType)
				{
					case EMeleeHitType.WPN_HIT:
						return 0;
					break;
					case EMeleeHitType.WPN_HIT_BUTTSTOCK:
						return 1;
					break;
					case EMeleeHitType.WPN_STAB:
						return 2;
					break;
				}
			}
			else
			{
				//! melee weapon attacks - gets mode from the item
				switch (m_HitType)
				{
					case EMeleeHitType.LIGHT:
						return weapon.GetMeleeMode();
					break;
					case EMeleeHitType.HEAVY:
						return weapon.GetMeleeHeavyMode();
					break;
					case EMeleeHitType.SPRINT:
						return weapon.GetMeleeSprintMode();
					break;
				}
			}
		}

		//! bare hand melee mode selection
		switch (m_HitType)
		{
			case EMeleeHitType.HEAVY:
				return 1;
			break;
			case EMeleeHitType.SPRINT:
				return 2;
			break;
		}
		return 0; //! default bare-hand light attack
	}
	
	protected float GetWeaponRange(InventoryItem weapon, int weaponMode)
	{
		if ( weapon )
			return weapon.GetMeleeCombatData().GetModeRange(weaponMode);
		else
			return m_DZPlayer.GetMeleeCombatData().GetModeRange(weaponMode);
	}
		
	protected float GetRange()
	{
		return m_WeaponRange + RANGE_EXTENDER_NORMAL;
	}
	
	protected float GetAngle()
	{
		if (m_SprintAttack)
			return TARGETING_ANGLE_SPRINT;
		else
			return TARGETING_ANGLE_NORMAL;
	}
	
	protected void TargetSelection()
	{
		// Prepare common variables
		PlayerBase player = PlayerBase.Cast(m_DZPlayer);
		vector pos = m_DZPlayer.GetPosition();
		vector rayStart = m_DZPlayer.GetBonePositionWS(m_DZPlayer.GetBoneIndexByName("Head"));

		vector cameraPos, cameraRot, cameraDir;
		m_DZPlayer.GetCurrentCameraTransform(cameraPos, cameraDir, cameraRot);
		vector dir = MiscGameplayFunctions.GetHeadingVector(player);
		dir[1] = cameraDir[1];

		// Calculate max distances
		float dist = GetRange();
		float angle = GetAngle();
		float dist2 = Math.SqrFloat(dist);
		
		// There's generally 2 TargetSelection calls per animation
		// First to obtain target to move towards during animation
		// Second one is to see if that target is still in range before applying damage to it
		// m_WasHit means the hit event occured, so this is the second call
		if ( m_WasHit )
		{
			// See if the component is still in range
			if (CanObjectBeTargeted(m_TargetObject) && ( vector.DistanceSq(rayStart, m_TargetObject.GetDamageZonePos(m_HitZoneName)) <= dist2 ))
			{
				m_AllTargetObjects.Insert(m_TargetObject);
				// This result should still be cached, so no need to fill it in again
				return;
			}
		}
		
		// Find a new target
		InternalResetTarget();
		
		Object target;
		vector hitPos;
		int hitZone;
		
		// First pass - Prefer what the player is looking at (only aligneable ones, we will try for nonaligneable later)
		if (HitZoneSelectionRaycastHelper(hitPos, hitZone, target))
		{
			if (m_ForceUntargetable)
			{
				SetTarget(target, hitPos, hitZone);
				return;
			}

			if (CanObjectBeTargeted(target) && vector.DistanceSq(rayStart, hitPos) <= dist2)
			{
				m_AllTargetObjects.Insert(target);
				
				SetTarget(target, hitPos, hitZone);
				return;
			}
		}
		
		// Second pass - Try to obtain something aligneable closeby the player watching direction		
		MeleeTargetData targetData = m_MeleeTargeting.GetMeleeTargetEx(new MeleeTargetSettings(pos, dist*0.75, angle, TARGETING_MIN_HEIGHT, TARGETING_MAX_HEIGHT, rayStart, dir, TARGETING_RAY_RADIUS_EX, m_DZPlayer, m_TargetableObjects), m_AllTargetObjects, m_BlacklistedDamageZones);

		if (targetData)
		{
			SetTarget(targetData.Obj, targetData.HitPos, targetData.HitComponent);
			return;
		}
		
		// Third pass - We found no aligneable target, check if the target that we are directly looking at is nonalignable (big)
		if (CanObjectBeTargeted(target, true) && vector.DistanceSq(rayStart, hitPos) <= dist2)
		{
			m_AllTargetObjects.Insert(target);

			SetTarget(target, hitPos, hitZone);
			return;
		}
	}
	
	protected void InternalResetTarget()
	{
		m_TargetObject 	= null;
		m_HitPositionWS = vector.Zero;
		m_HitZoneIdx 	= -1;
		m_HitZoneName 	= "";
	}
	
	protected void SetTarget(Object obj, vector hitPos, int hitZone)
	{
		if (obj)
		{
			m_TargetObject 	= obj;
			m_HitPositionWS = hitPos;
			m_HitZoneIdx 	= hitZone;
			m_HitZoneName 	= m_TargetObject.GetDamageZoneNameByComponentIndex(m_HitZoneIdx);
		}
	}
	
	protected bool CanObjectBeTargeted(Object obj, bool checkNonAligneAble = false)
	{
		return obj && obj.IsAlive() && ( obj.IsAnyInherited(m_TargetableObjects) || (checkNonAligneAble && obj.IsAnyInherited(m_NonAlignableObjects)) );
	}
	
	protected bool HitZoneSelectionRaycastHelper(out vector hitPos, out int hitZone, out Object target)
	{
		return HitZoneSelectionRaycast(hitPos, hitZone, target, false);
		// Not sure if desired, as it can give some results that feel a little odd
		// But it will depend entirely on the player and situation..
		// I guess the crosshair is hidden while melee anyways though
		// || ( m_DZPlayer.IsInThirdPerson() && HitZoneSelectionRaycast(hitPos, hitZone, target, true);
	}
	
	protected bool HitZoneSelectionRaycast(out vector hitPos, out int hitZone, out Object target, bool useCamera)
	{
		PlayerBase player = PlayerBase.Cast(m_DZPlayer);
		
		vector pos;
		vector dir;
		vector playerDir;
		if (useCamera)
		{ // What the player camera is looking at (crosshair)
			vector cameraRotation;
			player.GetCurrentCameraTransform(pos, dir, cameraRotation);
			playerDir = dir;
		}
		else
		{ // What the player himself is looking at
			playerDir = MiscGameplayFunctions.GetHeadingVector(player);
			dir = GetGame().GetCurrentCameraDirection();
			MiscGameplayFunctions.GetHeadBonePos(player, pos);
		}
		
		//! Prevents targeting of objects behind player
		if (vector.Dot(dir, playerDir) < 0.5)
		{
			return false;
		}

		m_RayStart = pos;
		m_RayEnd = m_RayStart + GetRange() * dir;
		
		// raycast
		set<Object> hitObjects = new set<Object>;
		vector hitNormal;
		
		if ( DayZPhysics.RaycastRV(m_RayStart, m_RayEnd, hitPos, hitNormal, hitZone, hitObjects, null, player, false, false, ObjIntersectIFire) && hitObjects.Count() > 0 )
		{
			target = hitObjects[0];
			m_ForceUntargetable = false;

			//! Opponent is inside car - targeting range is shorter in that case
			PlayerBase playerTarget = PlayerBase.Cast(target);
			if (playerTarget && playerTarget.IsInVehicle())
			{
				if (vector.DistanceSq(pos, hitPos) > Math.SqrFloat(GetRange() * 0.5))
				{
					m_ForceUntargetable = true;
					target = null;
					hitPos = vector.Zero;
					hitZone = -1;
				}
			}

			return true;
		}
		
		return false;
	}

	//! DEPRECATED
	protected void HitZoneSelection()
	{
		Object cursorTarget = null;
		PlayerBase player = PlayerBase.Cast(m_DZPlayer);

		// ray properties 
		vector pos;
		vector cameraDirection = GetGame().GetCurrentCameraDirection();

		MiscGameplayFunctions.GetHeadBonePos(player, pos);
		m_RayStart = pos;
		m_RayEnd = pos + cameraDirection * TARGETING_RAY_DIST;
		m_RayEndShort = pos + cameraDirection * TARGETING_RAY_DIST_SHORT;

		// raycast
		set<Object> hitObjects = new set<Object>;
		int hitComponentIndex;
		float hitFraction;
		vector start, end, hitNormal, hitPosObstructed;
		PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.DEFAULT;
		
		if ( !DayZPhysics.RaycastRV(m_RayStart, m_RayEndShort, m_HitPositionWS, hitNormal, m_HitZoneIdx, hitObjects, null, player, false, false, ObjIntersectIFire) && !DayZPhysics.RaycastRV(m_RayStart, m_RayEnd, m_HitPositionWS, hitNormal, m_HitZoneIdx, hitObjects, null, player, false, false, ObjIntersectIFire, TARGETING_RAY_RADIUS) )
		{
			m_HitZoneIdx = -1;
			//Print("HitZoneSelection failed");
		}
		else if ( hitObjects.Count() > 0 )
		{
			cursorTarget = hitObjects.Get(0);

			//! make sure we are in range of the current weapon;				
			vector playerPos = m_DZPlayer.GetPosition();
			vector hitPos = m_HitPositionWS;
			//! 2d only
			playerPos[1] = 0;
			hitPos[1] = 0;

			//! just for building and transports (big objects)				
			if ( cursorTarget.IsAnyInherited(m_NonAlignableObjects) && vector.Distance(playerPos, hitPos) <= GetWeaponRange(m_Weapon, GetWeaponMode()))
			{
				//! if no object in cone, set this object from raycast for these special cases
				if (m_TargetObject == null)
				{
					m_TargetObject = cursorTarget;
				}
			}

			if (cursorTarget == m_TargetObject)
			{
				m_HitZoneName = cursorTarget.GetDamageZoneNameByComponentIndex(m_HitZoneIdx);
				//Print("hit object: " + m_TargetObject + " | component idx: " + m_HitZoneIdx + " | hitzone name: " + m_HitZoneName);
			}
			else
			{
				if (m_TargetObject == DayZInfected.Cast(m_TargetObject) || m_TargetObject == PlayerBase.Cast(m_TargetObject))
				m_HitZoneName = DEFAULT_HIT_ZONE; //Default to torso if no zone is targeted 
			}
		}
		else
		{
			m_HitZoneIdx = -1;
			//Print("HitZoneSelection failed");
		}
	}

	//! DEPRECATED - New function in MeleeTargeting
	protected bool IsObstructed(Object object)
	{
		// check direct visibility of object (obstruction check)
		PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.BUILDING|PhxInteractionLayers.DOOR|PhxInteractionLayers.VEHICLE|PhxInteractionLayers.ROADWAY|PhxInteractionLayers.TERRAIN|PhxInteractionLayers.ITEM_SMALL|PhxInteractionLayers.ITEM_LARGE|PhxInteractionLayers.FENCE;
		int hitComponentIndex;
		float hitFraction;
		vector start, end, hitNormal, hitPosObstructed;
		Object hitObject = null;
		PlayerBase player = PlayerBase.Cast(m_DZPlayer);

		if (object)
		{
			MiscGameplayFunctions.GetHeadBonePos(player, start);
			end = start + MiscGameplayFunctions.GetHeadingVector(player) * vector.Distance(player.GetPosition(), object.GetPosition());
			
			if ( end == start )
				return true; //! not possible to trace when this happens (zero length raycast)

			return DayZPhysics.RayCastBullet( start, end, collisionLayerMask, null, hitObject, hitPosObstructed, hitNormal, hitFraction);
		}

		return false;
	}
	
#ifdef DEVELOPER
	// ------------------------------------------------------------
	// DEBUG
	// ------------------------------------------------------------
	protected ref array<Shape> dbgConeShapes = new array<Shape>();
	protected ref array<Shape> dbgTargets = new array<Shape>();
	protected ref array<Shape> hitPosShapes = new array<Shape>();
	
	void Debug(InventoryItem weapon, EMeleeHitType hitType)
	{
		CleanAllDebugShapes();
		
		if (!DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DEBUG_ENABLE))
			return;

		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_CONTINUOUS) && (!GetGame().IsMultiplayer() || !GetGame().IsServer()))
			Update(weapon, hitType);

		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_SHOW_TARGETS))
			ShowDebugMeleeTarget();
		
		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DRAW_TARGETS))
			DrawDebugTargets();
		
		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DRAW_RANGE))
		{
			DrawDebugMeleeHitPosition();
			DrawDebugMeleeCone();
		}
		
		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DRAW_BLOCK_RANGE_AI))
			DrawDebugBlockCone(GameConstants.AI_MAX_BLOCKABLE_ANGLE, COLOR_GREEN);
		
		if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DRAW_BLOCK_RANGE_PVP))
			DrawDebugBlockCone(GameConstants.PVP_MAX_BLOCKABLE_ANGLE, COLOR_YELLOW);
	}

	//! shows target in DbgUI 'window'
	protected void ShowDebugMeleeTarget()
	{
		int windowPosX = 0;
		int windowPosY = 500;

		//DbgUI.BeginCleanupScope();
		DbgUI.Begin("Melee Target", windowPosX, windowPosY);
		HumanCommandMelee2 hmc2 = m_DZPlayer.GetCommand_Melee2();
		if (hmc2)		
		{
			DbgUI.Text("Current combo: " + hmc2.GetComboCount());
		}
		if (m_PreviousTargetObject)
		{
			DbgUI.Text("Previous Character: " + m_PreviousTargetObject.GetDisplayName());
			DbgUI.Text("Previous HitZone: " + m_PreviousHitZoneName + "(" + m_PreviousHitZoneIdx + ")");
			DbgUI.Text("Previous HitPosWS:" + m_PreviousHitPositionWS);
			DbgUI.Text("Previous Distance:" + vector.Distance(m_PreviousHitPositionWS, m_DZPlayer.GetPosition()));
		}
		if (m_TargetObject)
		{
			DbgUI.Text("Character: " + m_TargetObject.GetDisplayName());
			DbgUI.Text("HitZone: " + m_HitZoneName + "(" + m_HitZoneIdx + ")");
			DbgUI.Text("HitPosWS:" + m_HitPositionWS);
			DbgUI.Text("Distance:" + vector.Distance(m_HitPositionWS, m_DZPlayer.GetPosition()));
		}
		DbgUI.End();
		//DbgUI.EndCleanupScope();
	}

	//! shows debug sphere above the target
	protected void DrawDebugTargets()
	{
		DrawDebugTargetsHelper(m_AllPreviousTargetObjects, m_PreviousTargetObject, COLOR_RED_A, COLOR_YELLOW_A);
		DrawDebugTargetsHelper(m_AllTargetObjects, m_TargetObject, COLOR_RED, COLOR_YELLOW);
	}
	
	protected void DrawDebugTargetsHelper(array<Object> allTargets, Object target, int colorMainTarget, int colorTarget)
	{
		for (int i = 0; i < allTargets.Count(); ++i )
		{
			if ( m_TargetObject && allTargets.Count() )
			{
				Object obj = allTargets[i];
				vector w_pos = obj.GetPosition();
				// sphere pos tweaks
				vector w_pos_sphr = w_pos;
				w_pos_sphr[1] = w_pos_sphr[1] + 1.8;
				// line pos tweaks
				vector w_pos_lend = w_pos;
				w_pos_lend[1] = w_pos_lend[1] + 1.8;
					
				if ( obj == m_TargetObject )
				{
					dbgTargets.Insert( Debug.DrawSphere(w_pos_sphr, 0.05, colorMainTarget, ShapeFlags.NOOUTLINE) );
					dbgTargets.Insert( Debug.DrawLine(w_pos, w_pos_lend, colorMainTarget) );
				}
				else
				{
					dbgTargets.Insert( Debug.DrawSphere(w_pos_sphr, 0.05, colorTarget, ShapeFlags.NOOUTLINE) );
					dbgTargets.Insert( Debug.DrawLine(w_pos, w_pos_lend, colorTarget) );
				}
			}
		}
	}
	
	protected void DrawDebugMeleeHitPosition()
	{	
		if (m_PreviousTargetObject)
			hitPosShapes.Insert( Debug.DrawSphere(m_PreviousHitPositionWS, 0.15, COLOR_YELLOW_A, ShapeFlags.NOOUTLINE|ShapeFlags.TRANSP) );
		
		if (m_TargetObject)
			hitPosShapes.Insert( Debug.DrawSphere(m_HitPositionWS, 0.15, COLOR_YELLOW, ShapeFlags.NOOUTLINE|ShapeFlags.TRANSP) );			
	}
		
	protected void DrawDebugMeleeCone()
	{
		// cone settings
		float dist = GetRange();
		vector start = m_DZPlayer.GetPosition();
					
		vector normDir = MiscGameplayFunctions.GetHeadingVector(PlayerBase.Cast(m_DZPlayer));
		normDir[1] = 0;
		normDir.Normalize();
		float playerAngle = -Math.Atan2(normDir[0], normDir[2]);		
			
		dbgConeShapes.InsertArray(Debug.DrawCone(start, dist, GetAngle() * Math.DEG2RAD, playerAngle + Math.PI_HALF, COLOR_BLUE));
	}

	protected void DrawDebugBlockCone(float angle, int color)
	{
		// cone settings
		float dist = 3;
		vector start = m_DZPlayer.GetPosition();
					
		vector dir = GetGame().GetCurrentCameraDirection();
		dir[1] = 0;
		dir.Normalize();
		float playerAngle = -Math.Atan2(dir[0], dir[2]);
		
		dbgConeShapes.InsertArray(Debug.DrawCone(start, dist, angle * Math.DEG2RAD, playerAngle + Math.PI_HALF, color));
	}
	
	protected void CleanAllDebugShapes()
	{
		CleanupDebugShapes(dbgTargets);
		CleanupDebugShapes(dbgConeShapes);
		CleanupDebugShapes(hitPosShapes);
	}

	protected void CleanupDebugShapes(array<Shape> shapes)
	{
		for ( int it = 0; it < shapes.Count(); ++it )
			Debug.RemoveShape( shapes[it] );

		shapes.Clear();
	}
#endif
}