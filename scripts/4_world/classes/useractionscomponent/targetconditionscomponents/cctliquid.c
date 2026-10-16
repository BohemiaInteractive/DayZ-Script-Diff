class CCTLiquid : CCTBase
{
	protected static const float HIT_POS_TOLERANCE = 1.0;
	protected static const float HIT_POS_TOLERANCE_SQ = HIT_POS_TOLERANCE * HIT_POS_TOLERANCE;
	protected static const float SEA_FALLBACK_DISTANCE_BUFFER = 1.5;
	protected static const float SYNTHETIC_POND_SAMPLE_STEP = 1.0;
	protected static const float SYNTHETIC_POND_SURFACE_TOLERANCE = 0.35;
	protected static const float SYNTHETIC_POND_FALLBACK_SURFACE_TOLERANCE = 1.75;
	protected static const float SYNTHETIC_CURSOR_DISTANCE_BUFFER = 1.5;
	protected static const float SEA_CURSOR_STICK_DISTANCE = 0.75;
	protected static const int SEA_CURSOR_STICK_TIME_MS = 350;
	protected static const float SYNTHETIC_CURSOR_CACHE_AIM_TOLERANCE = 1.75;
	protected static const int SYNTHETIC_CURSOR_CACHE_MAX_AGE_MS = 350;
	protected static const float SEA_HIT_RAW_Y_TOLERANCE = 1.25;
	protected static const float SEA_CURSOR_MIN_DOWN_DOT = 0.035;
	protected static const float LIQUID_SURFACE_RAYCAST_OFFSET = 0.2;
	
	protected vector m_LastSyntheticHitCursor;
	protected bool m_HasLastSyntheticHitCursor;
	protected int m_LastSyntheticHitCursorTime;
	
	protected float m_MaxVerticalReach;
	protected float m_MaxActionDistance;
	protected int m_AllowedLiquidSource;
	protected ref SurfaceDetectionParameters m_SurfaceDetectionParams;
	protected ref SurfaceDetectionParameters m_SurfaceDetectionParamsNoWater;
	protected int m_LiquidType = LIQUID_NONE;
	protected int m_WaterSourceType = EWaterSourceObjectType.NONE;
	protected int m_LiquidSourceType = ELiquidSourceObjectType.NONE;
	protected bool m_AllowSeaLevelCursorFallback;
	
	#ifdef DIAG_DEVELOPER
	protected bool m_DebugSyntheticCursor;
	protected ref array<Shape> m_DebugSyntheticCursorShapes;
	#endif
	
	void CCTLiquid(int allowedLiquidSource, float maximalTargetDistance = UAMaxDistances.DEFAULT, float maxVerticalReach = UAMaxDistances.DEFAULT, SurfaceDetectionType surfaceDetectionType = SurfaceDetectionType.Roadway, bool allowSeaLevelCursorFallback = false)
	{
		m_LastSyntheticHitCursor = vector.Zero;
		m_HasLastSyntheticHitCursor = false;
		m_LastSyntheticHitCursorTime = 0;
		
		m_MaxActionDistance = maximalTargetDistance;
		m_MaxVerticalReach = maxVerticalReach;
		m_AllowedLiquidSource = allowedLiquidSource;
		m_AllowSeaLevelCursorFallback = allowSeaLevelCursorFallback;
		
		m_SurfaceDetectionParams = new SurfaceDetectionParameters();
		m_SurfaceDetectionParams.type = surfaceDetectionType;
		m_SurfaceDetectionParams.includeWater = true;
		m_SurfaceDetectionParams.syncMode = UseObjectsMode.NoWait;
		m_SurfaceDetectionParams.rsd = RoadSurfaceDetection.CLOSEST;
		
		m_SurfaceDetectionParamsNoWater = new SurfaceDetectionParameters();
		m_SurfaceDetectionParamsNoWater.type = surfaceDetectionType;
		m_SurfaceDetectionParamsNoWater.includeWater = false;
		m_SurfaceDetectionParamsNoWater.syncMode = UseObjectsMode.NoWait;
		m_SurfaceDetectionParamsNoWater.rsd = RoadSurfaceDetection.CLOSEST;
		
		#ifdef DIAG_DEVELOPER
		m_DebugSyntheticCursor = DiagMenu.GetBool(DiagMenuIDs.SYNTHETIC_FISHING_CURSOR);
		m_DebugSyntheticCursorShapes = new array<Shape>();
		#endif
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target)
			return false;
	
		m_LiquidType = LIQUID_NONE;
		m_WaterSourceType = EWaterSourceObjectType.NONE;
		m_LiquidSourceType = ELiquidSourceObjectType.NONE;
	
		vector playerPos = player.GetPosition();
		Object targetObj = target.GetObject();
		vector objectPosition = vector.Zero;
		bool isValidInteract = false;
	
		//! Pre-calculate object validity once.
		if (targetObj)
		{
			m_WaterSourceType = targetObj.GetWaterSourceObjectType();
			m_LiquidSourceType = targetObj.GetLiquidSourceObjectType();
	
			if (targetObj.IsWell())
			{
				TIntArray componentIndices = new TIntArray();
				targetObj.GetActionComponentsForSelectionName(targetObj.GetViewGeometryLevel(), UAMisc.LIQUID_SOURCE_SELECTION, componentIndices);
	
				if (componentIndices.Count() > 0)
					isValidInteract = componentIndices.Find(target.GetComponentIndex()) != -1;
				else
					isValidInteract = true;
			}
			else
			{
				objectPosition = targetObj.GetPosition();
				isValidInteract = IsValidTargetObject(playerPos, objectPosition, m_WaterSourceType, m_LiquidSourceType, target.IsProxy());
			}
		}
	
		vector hitPosition = target.GetCursorHitPos();
	
		#ifdef DIAG_DEVELOPER
		m_DebugSyntheticCursor = DiagMenu.GetBool(DiagMenuIDs.SYNTHETIC_FISHING_CURSOR);
		#endif
	
		//! Fishing/synthetic/sea-normalized cursor path
		//! For fishing or other synthetic cursor actions, do not fall through to generic surface checks. If the synthetic cursor cannot be resolved, the action should not be available.
		if (m_AllowSeaLevelCursorFallback && !targetObj)
		{
			vector resolvedHitPosition;
			int resolvedLiquidType;
			
			if (TryResolveSyntheticCursor(player, playerPos, hitPosition, resolvedHitPosition, resolvedLiquidType))
			{
				hitPosition = resolvedHitPosition;
				target.SetCursorHitPos(hitPosition);
		
				m_LiquidType = resolvedLiquidType;
				return IsValidLiquid(m_LiquidType);
			}
		}
	
		if (isValidInteract && targetObj)
		{
			m_LiquidType = targetObj.GetLiquidSourceType();
			return IsValidLiquid(m_LiquidType);
		}
	
		string surfaceType;
		if (CheckSurfaceLiquid(player, targetObj, hitPosition, playerPos, isValidInteract, surfaceType))
		{
			return IsValidLiquid(m_LiquidType);
		}
	
		if (!targetObj && surfaceType == string.Empty && m_LiquidType == LIQUID_NONE)
		{
			if (!CheckSeaLevelFallback(hitPosition, playerPos))
			{
				#ifdef DIAG_DEVELOPER
				DbgUI.Begin("", 10, 200);
				if (m_DebugSyntheticCursor)
				{
					DbgUI.Text(string.Format("CCTLiquid CheckSeaLevelFallback rejected: reason=could not create sea hit point | hitPosition=%1 | playerPos=%2", hitPosition, playerPos));
				}
				DbgUI.End();
				#endif
			}
		}
	
		return IsValidLiquid(m_LiquidType);
	}
	
	protected bool IsValidLiquid(int liquid)
	{
		return (liquid & m_AllowedLiquidSource) != 0;
	}
	
	protected bool IsValidTargetObject(vector playerPos, vector objectPosition, EWaterSourceObjectType waterSourceType, ELiquidSourceObjectType liquidSourceType, bool isProxy)
	{
		//! Early exit for invalid types
		if ((waterSourceType == EWaterSourceObjectType.NONE && liquidSourceType == ELiquidSourceObjectType.NONE) || isProxy)
			return false;
		
		//! Check if position is valid
		if (IsNearlyZero(objectPosition))
			return false;
		
		//! Single distance calculation
		if (vector.Distance(playerPos, objectPosition) > m_MaxActionDistance)
			return false;
		
		//! Check vertical distance
		float heightDiff = Math.AbsFloat(objectPosition[1] - playerPos[1]);
		return heightDiff <= m_MaxVerticalReach;
	}
	
	protected bool CheckSurfaceLiquid(PlayerBase player, Object targetObj, vector hitPosition, vector playerPos, bool isValidInteract, out string surfaceType)
	{
		surfaceType = string.Empty;
		
		//! Valid object source wins, do not also check surface.
		if (isValidInteract)
			return false;
		
		//! Preserve ValidateDistances() range check for surface-liquid hits.
		if (IsNearlyZero(hitPosition))
			return false;

		m_SurfaceDetectionParams.position = hitPosition;
		SurfaceDetectionResult surfaceResult = new SurfaceDetectionResult();
		if (!g_Game.GetSurface(m_SurfaceDetectionParams, surfaceResult))
			return false;
			
		if (!surfaceResult)
			return false;
		
		SurfaceInfo surfaceInfo = surfaceResult.surface;
		if (!surfaceInfo)
			return false;
		
		surfaceType = surfaceInfo.GetSurfaceType();
		if (surfaceType == string.Empty)
			return false;
		
		float surfaceHeight = surfaceResult.height;
		float heightDiffSurface = Math.AbsFloat(hitPosition[1] - surfaceHeight);
		float playerToSurfaceHeight = Math.AbsFloat(playerPos[1] - surfaceHeight);
		
		int liquidType = g_Game.CorrectLiquidType(surfaceInfo.GetLiquidType());
		if (liquidType == LIQUID_NONE)
			return false;
		
		vector liquidSurfacePos = hitPosition;
		liquidSurfacePos[1] = surfaceHeight;
		
		//!  Do not allow water merely below grass/terrain.
		if (!IsCursorActuallyOnLiquidSurface(hitPosition, liquidSurfacePos))
			return false;
		
		vector surfacePos = liquidSurfacePos;
		surfacePos[1] = surfacePos[1] + LIQUID_SURFACE_RAYCAST_OFFSET;
		
		if (vector.Distance(playerPos, surfacePos) > m_MaxActionDistance)
			return false;
		
		if (targetObj)
		{
			if (heightDiffSurface > 0.25)
				return false;
		
			if (!HasClearPathToLiquidSurface(player, surfacePos))
				return false;
		}
		else
		{
			if (!HasClearPathToLiquidSurface(player, surfacePos))
				return false;
		}
		
		if (playerToSurfaceHeight > m_MaxVerticalReach)
			return false;
		
		m_LiquidType = liquidType;
		return true;
	}
	
	protected bool HasClearPathToLiquidSurface(PlayerBase player, vector surfacePos)
	{
		vector rayStart;
		MiscGameplayFunctions.GetHeadBonePos(player, rayStart);
		RaycastRVParams rayInput = new RaycastRVParams(rayStart, surfacePos, player);
		rayInput.flags = CollisionFlags.FIRSTCONTACT;
		rayInput.type = ObjIntersectFire;
		rayInput.sorted = true;
		rayInput.ignore = player;
	
		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		if (!DayZPhysics.RaycastRVProxy(rayInput, results))
			return true;
		
		float surfaceDist = vector.Distance(rayStart, surfacePos);
		foreach (RaycastRVResult result : results)
		{
			if (!result)
				continue;
			
			Object hitObj = result.obj;
			if (!hitObj)
				continue;
			
			float hitDist = vector.Distance(rayStart, result.pos);
			//! Something solid is between player and the liquid point.
			if (hitDist < surfaceDist)
				return false;
		}
	
		return true;
	}
	
	protected bool CheckSeaLevelFallback(vector hitPosition, vector playerPos)
	{
		vector normalizedSeaHit;
		if (!TryGetNormalizedSeaHitPoint(playerPos, hitPosition, normalizedSeaHit))
			return false;
		
		m_LiquidType = LIQUID_SALTWATER;
		return true;
	}
	
	protected bool TryGetLiquidSurfaceAt(vector probePosition, out vector surfacePosition, out int liquidType, out string surfaceType)
	{
		surfacePosition = vector.Zero;
		liquidType = LIQUID_NONE;
		surfaceType = string.Empty;
		
		m_SurfaceDetectionParams.position = probePosition;
		SurfaceDetectionResult surfaceResult = new SurfaceDetectionResult();
		if (!g_Game.GetSurface(m_SurfaceDetectionParams, surfaceResult))
			return false;
		
		if (!surfaceResult)
			return false;
	
		SurfaceInfo surfaceInfo = surfaceResult.surface;
		if (!surfaceInfo)
			return false;
	
		surfaceType = surfaceInfo.GetSurfaceType();
		if (surfaceType == string.Empty)
			return false;
	
		liquidType = g_Game.CorrectLiquidType(surfaceInfo.GetLiquidType());
		if (liquidType == LIQUID_NONE)
			return false;
	
		surfacePosition = probePosition;
		surfacePosition[1] = surfaceResult.height;
	
		return true;
	}
	
	protected bool TryBuildSeaLevelHitPosition(PlayerBase player, vector playerPos, out vector seaHitPosition)
	{
		seaHitPosition = vector.Zero;
		
		//! Server validates the synced cursor position. It should not invent one from camera data.
		if (g_Game.IsHeadlessOrDedicatedServer())
			return false;
		
		float seaLevel = g_Game.SurfaceGetSeaLevel();
		float seaLevelMax = g_Game.SurfaceGetSeaLevelMax();
	
		if (playerPos[1] > seaLevelMax + m_MaxVerticalReach)
			return false;
	
		vector cameraPos = g_Game.GetCurrentCameraPosition();
		vector cameraDir = g_Game.GetCurrentCameraDirection();
	
		if (cameraPos == vector.Zero || cameraDir == vector.Zero)
			return false;
		
		cameraDir.Normalize();
	
		float safeDistance = m_MaxActionDistance - SEA_FALLBACK_DISTANCE_BUFFER;
		if (safeDistance <= 1.0)
			safeDistance = m_MaxActionDistance * 0.8;
	
		//! Dynamic minimum downward angle. If the ray is too close to horizontal, tiny camera sway causes huge sea cursor jumps.
		float cameraToSeaHeight = cameraPos[1] - seaLevel;
		if (cameraToSeaHeight <= 0.0)
			return false;
		
		//! Only reject near-horizontal aim. Do not scale this directly with height, because the real intersection check below decides whether the sea-plane hit is within range.
		float minStableDownDot = SEA_CURSOR_MIN_DOWN_DOT;
		float maxAllowedCameraDirY = -minStableDownDot;
		
		if (cameraDir[1] > maxAllowedCameraDirY)
		{
			#ifdef DIAG_DEVELOPER
			DbgUI.Begin("", 10, 200);
			if (m_DebugSyntheticCursor)
			{
				DbgUI.Text(string.Format("CCTLiquid TryBuildSeaLevelHitPosition rejected: reason=camera too shallow | cameraDirY=%1 | minStableDownDot=%2 | height=%3 | safeDist=%4", cameraDir[1], minStableDownDot, cameraToSeaHeight, safeDistance));
			}
			DbgUI.End();
			#endif
		
			return false;
		}
		
		//! Real intersection of camera ray with sea plane.
		float t = (seaLevel - cameraPos[1]) / cameraDir[1];
		if (t <= 0)
			return false;
	
		vector candidate = cameraPos + cameraDir * t;
		candidate[1] = seaLevel;
	
		float safeDistanceSq = safeDistance * safeDistance;
	
		//! If the real cursor/sea intersection is beyond allowed range, do not clamp a closer fake point.
		if (vector.DistanceSq(playerPos, candidate) > safeDistanceSq)
		{
			#ifdef DIAG_DEVELOPER
			DbgUI.Begin("", 20, 200);
			if (m_DebugSyntheticCursor)
			{
				DbgUI.Text(string.Format("CCTLiquid TryBuildSeaLevelHitPosition rejected: reason=Real sea intersection too far | player=%1 | candidate=%2 | dist=%3 | safeDist=%4", playerPos.ToString(), candidate.ToString(), vector.Distance(playerPos, candidate), safeDistance));
			}
			DbgUI.End();
			#endif
			return false;
		}
	
		if (!IsSeaHitPositionValid(playerPos, candidate))
			return false;
		
		vector seaSurfacePos = candidate;
		seaSurfacePos[1] = seaSurfacePos[1] + LIQUID_SURFACE_RAYCAST_OFFSET;
		
		if (!HasClearPathToLiquidSurface(player, seaSurfacePos))
			return false;
	
		//! Apply small deadband/sticky cache.
		candidate = ApplySyntheticHitCursorStability(playerPos, candidate, LIQUID_SALTWATER);
		if (!IsSeaHitPositionValid(playerPos, candidate))
			return false;
		
		vector cachedOrStableSeaSurfacePos = candidate;
		cachedOrStableSeaSurfacePos[1] = cachedOrStableSeaSurfacePos[1] + LIQUID_SURFACE_RAYCAST_OFFSET;
		
		if (!HasClearPathToLiquidSurface(player, cachedOrStableSeaSurfacePos))
			return false;
	
		seaHitPosition = candidate;
		return true;
	}
	
	protected bool IsSeaHitPositionValid(vector playerPos, vector hitPosition)
	{
		vector normalizedHit;
		if (!TryGetNormalizedSeaHitPoint(playerPos, hitPosition, normalizedHit))
			return false;
	
		return true;
	}
	
	protected bool TryBuildPondHitPosition(PlayerBase player, vector playerPos, out vector pondHitPosition)
	{
		#ifdef DIAG_DEVELOPER
		float debugBestDelta = 999999.0;
		int debugSampleCount = 0;
		int debugValidKnownSurfaceCount = 0;
		string debugPondFailReason = "no pond samples";
		#endif
		
		pondHitPosition = vector.Zero;
	
		//! Client only. Server validates synced cursor position.
		if (g_Game.IsHeadlessOrDedicatedServer())
			return false;
		
		vector cameraPos = g_Game.GetCurrentCameraPosition();
		vector cameraDir = g_Game.GetCurrentCameraDirection();
	
		if (cameraPos == vector.Zero || cameraDir == vector.Zero)
			return false;
	
		cameraDir.Normalize();
	
		//! If looking upward/horizontal, we are probably not aiming at a pond surface.
		if (cameraDir[1] > 0.15)
			return false;
	
		float safeDistance = m_MaxActionDistance - SYNTHETIC_CURSOR_DISTANCE_BUFFER;
		if (safeDistance <= 1.0)
			safeDistance = m_MaxActionDistance * 0.8;
	
		vector bestSurfacePos = vector.Zero;
		int bestLiquidType = LIQUID_NONE;
		float bestDelta = 999999.0;
		vector pondSurfacePos = vector.Zero;
		
		for (float dist = 1.0; dist <= safeDistance; dist += SYNTHETIC_POND_SAMPLE_STEP)
		{
			#ifdef DIAG_DEVELOPER
			debugSampleCount++;
			#endif
			
			vector probePos = cameraPos + cameraDir * dist;
			
			vector surfacePos;
			int liquidType;
			string surfaceType;
		
			if (!TryGetLiquidSurfaceAt(probePos, surfacePos, liquidType, surfaceType))
			{
				#ifdef DIAG_DEVELOPER
				debugPondFailReason = "TryGetLiquidSurfaceAt failed";
				#endif
				continue;
			}
		
			if (!IsAllowedPondLiquidSurface(playerPos, surfacePos, liquidType))
			{
				#ifdef DIAG_DEVELOPER
				debugPondFailReason = "known pond surface invalid";
				#endif
				continue;
			}
			
			#ifdef DIAG_DEVELOPER
			debugValidKnownSurfaceCount++;
			#endif
		
			float rayToSurfaceDelta = Math.AbsFloat(probePos[1] - surfacePos[1]);
			if (rayToSurfaceDelta < bestDelta)
			{
				bestDelta = rayToSurfaceDelta;
				bestSurfacePos = surfacePos;
				bestLiquidType = liquidType;
				
				#ifdef DIAG_DEVELOPER
				debugBestDelta = bestDelta;
				debugPondFailReason = "best pond surface too far from camera ray";
				#endif
			}
		
			//! The ray is close enough to the liquid surface.
			if (rayToSurfaceDelta <= SYNTHETIC_POND_SURFACE_TOLERANCE)
			{
				surfacePos = ApplySyntheticHitCursorStability(playerPos, surfacePos, liquidType);
			
				if (!IsValidPondHitPosition(playerPos, surfacePos, liquidType))
					continue;
			
				pondSurfacePos = surfacePos;
				pondSurfacePos[1] = pondSurfacePos[1] + LIQUID_SURFACE_RAYCAST_OFFSET;
			
				if (!HasClearPathToLiquidSurface(player, pondSurfacePos))
					continue;
			
				pondHitPosition = surfacePos;
				return true;
			}
		}
	
		//! Last chance: use closest sampled surface if it was very close.
		if (!IsNearlyZero(bestSurfacePos) && bestDelta <= SYNTHETIC_POND_FALLBACK_SURFACE_TOLERANCE)
		{
			bestSurfacePos = ApplySyntheticHitCursorStability(playerPos, bestSurfacePos, bestLiquidType);
			if (IsValidPondHitPosition(playerPos, bestSurfacePos, bestLiquidType))
			{
				pondSurfacePos = bestSurfacePos;
				pondSurfacePos[1] = pondSurfacePos[1] + LIQUID_SURFACE_RAYCAST_OFFSET;
				
				if (!HasClearPathToLiquidSurface(player, pondSurfacePos))
					return false;
				
				pondHitPosition = bestSurfacePos;
				return true;
			}
		}
		
		#ifdef DIAG_DEVELOPER
		DbgUI.Begin("", 50, 240);
		if (m_DebugSyntheticCursor)
		{
			DbgUI.Text(string.Format("CCTLiquid TryBuildPondHitPosition failed: reason=%1 | samples=%2 | validSurfaces=%3 | bestDelta=%4 | tolerance=%5 | relaxedTolerance=%6", debugPondFailReason, debugSampleCount, debugValidKnownSurfaceCount, debugBestDelta, SYNTHETIC_POND_SURFACE_TOLERANCE, SYNTHETIC_POND_FALLBACK_SURFACE_TOLERANCE));
		}
		DbgUI.End();
		#endif
	
		return false;
	}
	
	protected bool IsValidPondHitPosition(vector playerPos, vector hitPosition, int liquidType = LIQUID_NONE)
	{
		vector surfacePos;
		string surfaceType;
		int validatedLiquidType;
		
		return TryValidatePondHitPosition(playerPos, hitPosition, surfacePos, validatedLiquidType, surfaceType, liquidType);
	}
	
	protected bool TryGetNormalizedSeaHitPoint(vector playerPos, vector hitPosition, out vector normalizedHit)
	{
		normalizedHit = vector.Zero;
	
		if (IsNearlyZero(hitPosition))
			return false;
	
		if (!g_Game.SurfaceIsSea(hitPosition[0], hitPosition[2]))
			return false;
	
		float seaLevel = g_Game.SurfaceGetSeaLevel();
		float seaLevelMax = g_Game.SurfaceGetSeaLevelMax();
	
		//! Raw Y sanity check. Do not require exact sea Y here because synced cursor Y can differ.
		float hitToSeaHeight = Math.AbsFloat(hitPosition[1] - seaLevel);
		if (hitToSeaHeight > SEA_HIT_RAW_Y_TOLERANCE)
			return false;
	
		if (hitPosition[1] > seaLevelMax + m_MaxVerticalReach)
			return false;
	
		normalizedHit = hitPosition;
		normalizedHit[1] = seaLevel;
	
		//! Validate real fishing distance against normalized sea point.
		if (vector.Distance(playerPos, normalizedHit) > m_MaxActionDistance)
			return false;
	
		//! Preserve ValidateDistances() vertical behavior for normalized sea hits.
		float playerToSeaHeight = Math.AbsFloat(playerPos[1] - seaLevel);
		if (playerToSeaHeight > m_MaxVerticalReach)
			return false;
	
		return true;
	}
	
	protected bool TryBuildSyntheticCursor(PlayerBase player, vector playerPos, out vector syntheticHitPosition, out int syntheticLiquidType)
	{
		syntheticHitPosition = vector.Zero;
		syntheticLiquidType = LIQUID_NONE;
	
		//! Client only. Server validates synced cursor position.
		if (g_Game.IsHeadlessOrDedicatedServer())
			return false;
	
		if ((m_AllowedLiquidSource & LIQUID_SALTWATER) != 0)
		{
			if (TryBuildSeaLevelHitPosition(player, playerPos, syntheticHitPosition))
			{
				syntheticLiquidType = LIQUID_SALTWATER;
				return true;
			}
		}
	
		if ((m_AllowedLiquidSource & LIQUID_FRESHWATER) != 0)
		{
			if (TryBuildPondHitPosition(player, playerPos, syntheticHitPosition))
			{
				//! Let final validation detect exact liquid type.
				syntheticLiquidType = LIQUID_FRESHWATER;
				return true;
			}
		}
	
		return false;
	}
	
	protected bool TryResolveSyntheticCursor(PlayerBase player, vector playerPos, vector currentHitPosition, out vector resolvedHitPosition, out int resolvedLiquidType)
	{
		resolvedHitPosition = currentHitPosition;
		resolvedLiquidType = LIQUID_NONE;
	
		//! First validate the existing/synced cursor.
		if (TryValidateHitCursor(player, playerPos, currentHitPosition, resolvedHitPosition, resolvedLiquidType))
			return true;
		
		//! If there is a real cursor hit but it is not valid liquid, do not synthesize water. This prevents looking at ground/terrain near a pond from creating a synthetic cursor.
		if (currentHitPosition != vector.Zero)
		{
			ClearSyntheticCursorCache();
		
			#ifdef DIAG_DEVELOPER
			if (m_DebugSyntheticCursor)
				DebugDrawSyntheticCursor(player, currentHitPosition, vector.Zero, false, "current non-liquid hit prevents synthetic cursor", LIQUID_NONE);
			else
				ClearDebugCursorShapes();
			#endif
		
			return false;
		}
	
		//! Server stops here. It should not invent a cursor from camera state.
		if (g_Game.IsHeadlessOrDedicatedServer())
			return false;
	
		//! Client fallback: build synthetic cursor only when current cursor is invalid.
		vector syntheticHitPosition;
		int syntheticLiquidType;
		
		if (!TryBuildSyntheticCursor(player, playerPos, syntheticHitPosition, syntheticLiquidType))
		{
			//! Do not immediately drop a previously valid cursor. A single frame can fail because the camera ray is shallow or surface detection missed.
			if (TryUseCachedSyntheticCursor(player, playerPos, currentHitPosition, resolvedHitPosition, resolvedLiquidType))
			{
				#ifdef DIAG_DEVELOPER
				if (m_DebugSyntheticCursor)
					DebugDrawSyntheticCursor(player, currentHitPosition, resolvedHitPosition, true, "accepted cached synthetic cursor", resolvedLiquidType);
				else
					ClearDebugCursorShapes();
				#endif
		
				return true;
			}
		
			ClearSyntheticCursorCache();
		
			#ifdef DIAG_DEVELOPER
			if (m_DebugSyntheticCursor)
				DebugDrawSyntheticCursor(player, currentHitPosition, syntheticHitPosition, false, "synthetic cursor failed", LIQUID_NONE);
			else
				ClearDebugCursorShapes();
			#endif
		
			return false;
		}
	
		//! Validate the generated cursor once.
		if (!TryValidateHitCursor(player, playerPos, syntheticHitPosition, resolvedHitPosition, resolvedLiquidType))
		{
			ClearSyntheticCursorCache();
	
			#ifdef DIAG_DEVELOPER
			if (m_DebugSyntheticCursor)
				DebugDrawSyntheticCursor(player, currentHitPosition, syntheticHitPosition, false, "synthetic cursor invalid after build", syntheticLiquidType);
			else
				ClearDebugCursorShapes();
			#endif
	
			return false;
		}
	
		#ifdef DIAG_DEVELOPER
		if (m_DebugSyntheticCursor)
			DebugDrawSyntheticCursor(player, currentHitPosition, resolvedHitPosition, true, "accepted synthetic cursor", resolvedLiquidType);
		else
			ClearDebugCursorShapes();
		#endif
	
		return true;
	}
	
	protected bool TryValidatePondHitPosition(vector playerPos,	vector hitPosition,	out vector surfacePos, out int liquidType, out string surfaceType, int expectedLiquidType = LIQUID_NONE)
	{
		surfacePos = vector.Zero;
		liquidType = LIQUID_NONE;
		surfaceType = string.Empty;
	
		if (IsNearlyZero(hitPosition))
			return false;
	
		if (vector.Distance(playerPos, hitPosition) > m_MaxActionDistance)
			return false;
		
		if (!g_Game.SurfaceIsPond(hitPosition[0], hitPosition[2]))
			return false;
	
		int detectedLiquidType;
		if (!TryGetLiquidSurfaceAt(hitPosition, surfacePos, detectedLiquidType, surfaceType))
			return false;
	
		//! Use detected liquid as the source of truth.
		liquidType = detectedLiquidType;
		
		//! If caller expected a specific liquid mask/type, require compatibility.
		if (expectedLiquidType != LIQUID_NONE && (liquidType & expectedLiquidType) == 0)
			return false;
	
		if (!IsAllowedPondLiquidSurface(playerPos, surfacePos, liquidType))
			return false;
		
		float hitToSurfaceHeight = Math.AbsFloat(hitPosition[1] - surfacePos[1]);
		if (hitToSurfaceHeight > SYNTHETIC_POND_SURFACE_TOLERANCE)
			return false;
	
		return true;
	}
	
	protected void ClearSyntheticCursorCache()
	{
		m_LastSyntheticHitCursor = vector.Zero;
		m_HasLastSyntheticHitCursor = false;
		m_LastSyntheticHitCursorTime = 0;
	}
	
	protected vector ApplySyntheticHitCursorStability(vector playerPos, vector candidate, int liquidType)
	{
		int now = g_Game.GetTime();
		if (m_HasLastSyntheticHitCursor)
		{
			int age = now - m_LastSyntheticHitCursorTime;
	
			if (age <= SEA_CURSOR_STICK_TIME_MS)
			{
				float stickDistSq = SEA_CURSOR_STICK_DISTANCE * SEA_CURSOR_STICK_DISTANCE;
				float deltaSq = vector.DistanceSq(candidate, m_LastSyntheticHitCursor);
	
				if (deltaSq <= stickDistSq)
				{
					if (liquidType == LIQUID_SALTWATER && IsSeaHitPositionValid(playerPos, m_LastSyntheticHitCursor))
					{
						m_LastSyntheticHitCursorTime = now;
						return m_LastSyntheticHitCursor;
					}
	
					if (liquidType != LIQUID_SALTWATER && IsValidPondHitPosition(playerPos, m_LastSyntheticHitCursor, liquidType))
					{
						m_LastSyntheticHitCursorTime = now;
						return m_LastSyntheticHitCursor;
					}
				}
			}
		}
	
		m_LastSyntheticHitCursor = candidate;
		m_HasLastSyntheticHitCursor = true;
		m_LastSyntheticHitCursorTime = now;
	
		return candidate;
	}
	
	protected bool TryValidateHitCursor(PlayerBase player, vector playerPos, vector hitPosition, out vector resolvedHitPosition, out int resolvedLiquidType)
	{
		resolvedHitPosition = hitPosition;
		resolvedLiquidType = LIQUID_NONE;
		
		if (IsNearlyZero(hitPosition))
			return false;
	
		//! Saltwater path. This normalizes synced sea hits like <x, 0.546, z> back onto the sea plane.
		if ((m_AllowedLiquidSource & LIQUID_SALTWATER) != 0)
		{
			vector normalizedSeaHit;
			if (TryGetNormalizedSeaHitPoint(playerPos, hitPosition, normalizedSeaHit))
			{
				if (!HasClearPathToLiquidSurface(player, normalizedSeaHit))
					return false;
				
				if (!IsCursorActuallyOnLiquidSurface(hitPosition, normalizedSeaHit))
					return false;
				
				resolvedHitPosition = normalizedSeaHit;
				resolvedLiquidType = LIQUID_SALTWATER;
				return true;
			}
		}
	
		//! Freshwater / pond path.
		if ((m_AllowedLiquidSource & LIQUID_FRESHWATER) != 0)
		{
			vector surfacePos;
			string surfaceType;
			int detectedLiquidType;
	
			if (TryValidatePondHitPosition(playerPos, hitPosition, surfacePos, detectedLiquidType, surfaceType))
			{
				if (!HasClearPathToLiquidSurface(player, surfacePos))
					return false;
				
				if (!IsCursorActuallyOnLiquidSurface(hitPosition, surfacePos))
					return false;
				
				resolvedHitPosition = surfacePos;
				resolvedLiquidType = detectedLiquidType;
				return true;
			}
		}
	
		return false;
	}
	
	protected bool TryUseCachedSyntheticCursor(PlayerBase player, vector playerPos, vector currentHitPosition, out vector resolvedHitPosition, out int resolvedLiquidType)
	{
		resolvedHitPosition = vector.Zero;
		resolvedLiquidType = LIQUID_NONE;
	
		if (!m_HasLastSyntheticHitCursor)
			return false;
	
		int now = g_Game.GetTime();
		int age = now - m_LastSyntheticHitCursorTime;
	
		//! Cache is only a short grace fallback, not a persistent targeting source.
		if (age > SYNTHETIC_CURSOR_CACHE_MAX_AGE_MS)
			return false;
	
		//! Important: cached cursor must still match current aim.
		if (!IsCachedSyntheticCursorRelevant(currentHitPosition, m_LastSyntheticHitCursor))
			return false;
	
		if (TryValidateHitCursor(player, playerPos, m_LastSyntheticHitCursor, resolvedHitPosition, resolvedLiquidType))
		{
			m_LastSyntheticHitCursorTime = now;
			return true;
		}
	
		return false;
	}
	
	protected bool IsPositionNearCurrentCameraRay(vector position, float maxPerpendicularDistance)
	{
		if (position == vector.Zero)
			return false;
	
		vector cameraPos = g_Game.GetCurrentCameraPosition();
		vector cameraDir = g_Game.GetCurrentCameraDirection();
	
		if (cameraPos == vector.Zero || cameraDir == vector.Zero)
			return false;
	
		cameraDir.Normalize();
	
		vector cameraToPosition = position - cameraPos;
		float projectedDistance = vector.Dot(cameraToPosition, cameraDir);
	
		//! Cached point must be in front of camera.
		if (projectedDistance <= 0.0)
			return false;
	
		float safeDistance = m_MaxActionDistance - SYNTHETIC_CURSOR_DISTANCE_BUFFER;
	
		if (safeDistance <= 1.0)
			safeDistance = m_MaxActionDistance * 0.8;
	
		//! Do not allow using a cached point far beyond the active aim range.
		if (projectedDistance > safeDistance + maxPerpendicularDistance)
			return false;
	
		vector closestPointOnRay = cameraPos + cameraDir * projectedDistance;
		float maxPerpendicularDistanceSq = maxPerpendicularDistance * maxPerpendicularDistance;
	
		return vector.DistanceSq(position, closestPointOnRay) <= maxPerpendicularDistanceSq;
	}
	
	protected bool IsCachedSyntheticCursorRelevant(vector currentHitPosition, vector cachedHitPosition)
	{
		if (cachedHitPosition == vector.Zero)
			return false;
	
		float aimToleranceSq = SYNTHETIC_CURSOR_CACHE_AIM_TOLERANCE * SYNTHETIC_CURSOR_CACHE_AIM_TOLERANCE;
	
		//! If there is a current real hit, cached point must be near that hit. If it is not, do not let an old water point keep the action alive.
		if (currentHitPosition != vector.Zero)
		{
			return vector.DistanceSq(currentHitPosition, cachedHitPosition) <= aimToleranceSq;
		}
	
		//! If there is no real hit, compare cached point to current camera ray.
		return IsPositionNearCurrentCameraRay(cachedHitPosition, SYNTHETIC_CURSOR_CACHE_AIM_TOLERANCE);
	}
	
	protected bool IsAllowedPondLiquidSurface(vector playerPos, vector surfacePos, int liquidType)
	{
		if (liquidType == LIQUID_NONE || liquidType == LIQUID_SALTWATER)
			return false;
	
		if ((liquidType & m_AllowedLiquidSource) == 0)
			return false;
	
		if (!g_Game.SurfaceIsPond(surfacePos[0], surfacePos[2]))
			return false;
	
		if (vector.Distance(playerPos, surfacePos) > m_MaxActionDistance)
			return false;
	
		float playerToSurfaceHeight = Math.AbsFloat(playerPos[1] - surfacePos[1]);
		if (playerToSurfaceHeight > m_MaxVerticalReach)
			return false;
	
		return true;
	}
		
	protected bool TryGetNonWaterSurfaceAt(vector probePosition, out vector surfacePosition)
	{
		surfacePosition = vector.Zero;
		
		m_SurfaceDetectionParamsNoWater.position = probePosition;
		m_SurfaceDetectionParamsNoWater.rsd = RoadSurfaceDetection.CLOSEST;
		SurfaceDetectionResult surfaceResult = new SurfaceDetectionResult();
		if (!g_Game.GetSurface(m_SurfaceDetectionParamsNoWater, surfaceResult))
			return false;
	
		if (!surfaceResult)
			return false;
	
		SurfaceInfo surfaceInfo = surfaceResult.surface;
		if (!surfaceInfo)
			return false;
	
		string surfaceType = surfaceInfo.GetSurfaceType();
		if (surfaceType == string.Empty)
			return false;
		
		int surfaceLiquid = surfaceInfo.GetLiquidType();
		//! Check if we have the same or an other liquid surface
		if (surfaceLiquid > LIQUID_NONE)
			return false;
	
		surfacePosition = probePosition;
		surfacePosition[1] = surfaceResult.height;
	
		return true;
	}
	
	protected bool IsCursorActuallyOnLiquidSurface(vector hitPosition, vector liquidSurfacePos)
	{
		if (IsNearlyZero(hitPosition))
			return false;
	
		if (IsNearlyZero(liquidSurfacePos))
			return false;
		
		if (hitPosition[1] >= liquidSurfacePos[1])
			return false;
		
		vector nonWaterSurfacePos;
		if (TryGetNonWaterSurfaceAt(hitPosition, nonWaterSurfacePos))
		{
			//! If terrain/grass/road surface is higher, the player is aiming at ground over/near water, not the water itself.
			if (nonWaterSurfacePos[1] > liquidSurfacePos[1])
				return false;
		}
	
		return true;
	}
	
	override bool CanContinue(PlayerBase player, ActionTarget target)
	{
		return Can(player, target); //! Re-validate during action
	}
		
	int GetLiquidType()
	{
		return m_LiquidType;
	}
	
	int GetWaterSourceType()
	{
		return m_WaterSourceType;
	}
	
	int GetLiquidSourceType()
	{
		return m_LiquidSourceType;
	}
	
	bool IsNearlyZero(vector value)
	{
		return vector.DistanceSq(value, vector.Zero) < 0.0001;
	}
	
	#ifdef DIAG_DEVELOPER
	protected void ClearDebugCursorShapes()
	{
		if (!m_DebugSyntheticCursorShapes)
			return;
	
		for (int i = 0; i < m_DebugSyntheticCursorShapes.Count(); i++)
		{
			Shape shape = m_DebugSyntheticCursorShapes[i];
			if (shape)
				Debug.RemoveShape(shape);
		}
	
		m_DebugSyntheticCursorShapes.Clear();
	}
	
	protected void DebugDrawSyntheticCursor(PlayerBase player, vector originalHitPosition, vector cursorHitPosition, bool accepted, string reason, int liquidType = LIQUID_NONE)
	{
		ClearDebugCursorShapes();
	
		if (!player)
			return;
	
		vector playerPos = player.GetPosition();
	
		vector headPos;
		MiscGameplayFunctions.GetHeadBonePos(player, headPos);
	
		int colorAccepted = ARGB(255, 0, 255, 64);
		int colorRejected = ARGB(255, 255, 32, 32);
		int colorLine = ARGB(255, 0, 180, 255);
		int colorOriginal = ARGB(255, 255, 255, 0);
	
		int finalColor;
		if (accepted)
			finalColor = colorAccepted;
		else
			finalColor = colorRejected;
	
		//! Main synthetic cursor point
		if (!IsNearlyZero(cursorHitPosition))
		{
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawSphere(cursorHitPosition, 0.35, finalColor, ShapeFlags.NOZBUFFER));
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawLine(headPos, cursorHitPosition, colorLine));
	
			//! Small cross on the water plane
			vector x1 = cursorHitPosition;
			vector x2 = cursorHitPosition;
			vector z1 = cursorHitPosition;
			vector z2 = cursorHitPosition;
	
			x1[0] = x1[0] - 0.5;
			x2[0] = x2[0] + 0.5;
			z1[2] = z1[2] - 0.5;
			z2[2] = z2[2] + 0.5;
	
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawLine(x1, x2, finalColor));
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawLine(z1, z2, finalColor));
		}
	
		//! Original hit position, if any
		if (!IsNearlyZero(originalHitPosition))
		{
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawSphere(originalHitPosition, 0.2, colorOriginal, ShapeFlags.NOZBUFFER));
			m_DebugSyntheticCursorShapes.Insert(Debug.DrawLine(playerPos, originalHitPosition, colorOriginal));
		}
	
		float dist = 0.0;
		bool isSea = false;
		bool isPond = false;

		if (!IsNearlyZero(cursorHitPosition))
		{
			dist = vector.Distance(playerPos, cursorHitPosition);
			isSea = g_Game.SurfaceIsSea(cursorHitPosition[0], cursorHitPosition[2]);
			isPond = g_Game.SurfaceIsPond(cursorHitPosition[0], cursorHitPosition[2]);
		}

		DbgUI.Begin("", 30, 200);
		{
			DbgUI.Text(string.Format("CCTLiquid TryResolveSyntheticCursor: accepted=%1 | reason=%2 | liquid=%3 | player=%4 | originalHit=%5 | resolvedHit=%6 | dist=%7 | isSea=%8 | isPond=%9",	accepted, reason, liquidType, playerPos.ToString(), originalHitPosition.ToString(), cursorHitPosition.ToString(), dist, isSea, isPond));
		}
		DbgUI.End();
	}
	#endif
}
