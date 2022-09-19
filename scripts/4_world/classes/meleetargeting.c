class MeleeTargetData
{	
	Object Obj;
	vector HitPos;
	int HitComponent;
	
	void MeleeTargetData(Object o, vector p, int c)
	{
		Obj = o;
		HitPos = p;
		HitComponent = c;
	}
}

class MeleeTargetSettings
{
	vector ConeOrigin;
	float ConeLength;
	float ConeHalfAngle;
	float ConeHalfAngleRad;
	float ConeMinHeight;
	float ConeMaxHeight;
	
	vector ConeLeftPoint;
	vector ConeRightPoint;
	
	vector RayStart;
	vector RayEnd;
	vector Dir;
	vector XZDir;
	float MaxDist;
	
	EntityAI Attacker;
	ref array<typename> TargetableObjects
	
	void MeleeTargetSettings(vector coneOrigin, float coneLength, float coneHalfAngle, float coneMinHeight, float coneMaxHeight, vector rayStart, vector dir, float maxDist, EntityAI pToIgnore, array<typename> targetableObjects)
	{
		ConeOrigin 			= coneOrigin;
		ConeLength 			= coneLength;
		ConeHalfAngle 		= coneHalfAngle;
		ConeHalfAngleRad 	= Math.DEG2RAD * coneHalfAngle;
		ConeMinHeight 		= coneMinHeight;
		ConeMaxHeight 		= coneMaxHeight;
		
		RayStart 			= rayStart;
		RayEnd 				= rayStart + Math.SqrFloat(coneLength) * dir;
		
		Dir 				= dir; 
		
		XZDir 				= dir;
		XZDir[1] 			= 0;
		XZDir.Normalize();
		
		MaxDist 			= maxDist;
		
		Attacker 			= pToIgnore;
		TargetableObjects 	= targetableObjects;
		
		// Calculate cone points
		Math3D.ConePoints(ConeOrigin, ConeLength, ConeHalfAngleRad, -Math.Atan2(XZDir[0], XZDir[2]), ConeLeftPoint, ConeRightPoint);
	}
}

class ComponentResult
{
	vector ComponentPos;
	float ComponentAngle;
	float ComponentDistance2;
	int ComponentIdx;
}

class MeleeTargeting
{
	const static PhxInteractionLayers MELEE_TARGET_OBSTRUCTION_LAYERS = PhxInteractionLayers.BUILDING|PhxInteractionLayers.DOOR|PhxInteractionLayers.VEHICLE|PhxInteractionLayers.ROADWAY|PhxInteractionLayers.TERRAIN|PhxInteractionLayers.ITEM_SMALL|PhxInteractionLayers.ITEM_LARGE|PhxInteractionLayers.FENCE;
	
	MeleeTargetData GetMeleeTargetEx(MeleeTargetSettings settings, out array<Object> allTargets = null, array<string> blacklistedDamageZones = null)
	{
		MeleeTargetData ret;
		
		// Easier access to MeleeTargetSettings variables
		vector 	coneOrigin 					= settings.ConeOrigin;
		float 	coneLength 					= settings.ConeLength;
		float 	coneHalfAngle 				= settings.ConeHalfAngle;
		float	radAngle 					= settings.ConeHalfAngleRad;
		float 	coneMinHeight 				= settings.ConeMinHeight;
		float 	coneMaxHeight 				= settings.ConeMaxHeight;
		
		vector coneLeft 					= settings.ConeLeftPoint;
		vector coneRight 					= settings.ConeRightPoint;
		
		vector rayStart 					= settings.RayStart;
		vector rayEnd 						= settings.RayEnd;
		vector dir 							= settings.Dir;
		vector xzDir 						= settings.XZDir;
		float maxDist 						= settings.MaxDist;
		
		EntityAI pToIgnore 					= settings.Attacker;
		array<typename> targetableObjects 	= settings.TargetableObjects;
		
		// Calculate box size
		float boxWidth = vector.Distance(coneLeft, coneRight);		
		float boxHeight = coneMaxHeight - coneMinHeight;
		
		vector boxSize = Vector(boxWidth, boxHeight, coneLength);
		
		// Calculate box center
		vector centerHeight = Vector(0, Math.Lerp(coneMinHeight, coneMaxHeight, 0.5), 0);
		vector offset = xzDir * coneLength * 0.5;
		vector boxCenter = coneOrigin + offset + centerHeight;
		
		// Gather all targets
		BoxCollidingParams params = new BoxCollidingParams();
		params.SetParams(boxCenter, xzDir.VectorToAngles(), boxSize, ObjIntersect.Fire, ObjIntersect.Fire, true);
		array<ref BoxCollidingResult> results = new array<ref BoxCollidingResult>;
		array<Object> toIgnore = { pToIgnore };
		if (GetGame().IsBoxCollidingGeometryProxy(params, toIgnore, results))
		{
			//
			float 	retVal 	= float.MAX;
			float 	tgAngle = Math.Tan(radAngle);	
		
			#ifdef DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DRAW_RANGE) && DiagMenu.GetBool(DiagMenuIDs.DM_MELEE_DEBUG_ENABLE))
				Debug.DrawLine(rayStart, rayEnd, COLOR_GREEN, ShapeFlags.ONCE);
			#endif
			
			// Find the most suitable target
			foreach (BoxCollidingResult bResult : results)
			{
				Object obj = bResult.obj;
				
				// Check for targetable objects
				if (!obj.IsAnyInherited(targetableObjects) || !obj.IsAlive())
				{
					continue;
				}
				
				// Ready the defaults for the object
				vector targetPos = obj.GetPosition();							
				float targetAngle = Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(coneOrigin, dir, targetPos));	
				float targetDistance2 = vector.DistanceSq(targetPos, Math3D.NearestPoint(rayStart, rayEnd, targetPos));	
				int hitComponent = -1;

				float csSum = float.MAX;
				
				// Find the most suitable component
				ComponentResult result;
				if (FindMostSuitableComponentEx(obj, bResult, settings, csSum, result, blacklistedDamageZones))
				{
					targetPos 		= result.ComponentPos;
					targetAngle 	= result.ComponentAngle;
					targetDistance2 = result.ComponentDistance2;
					hitComponent 	= result.ComponentIdx;
				}

				// ProxyInfo
				if (bResult.proxyInfo)
				{
					foreach (BoxCollidingResult pInfo : bResult.proxyInfo)
					{
						if (FindMostSuitableComponentEx(obj, pInfo, settings, csSum, result, blacklistedDamageZones))
						{
							targetPos 		= result.ComponentPos;							
							targetAngle 	= result.ComponentAngle;	
							targetDistance2 = result.ComponentDistance2;	
							hitComponent 	= result.ComponentIdx;
						}
					}
				}
				
				// No suitable component found
				if (hitComponent == -1 || csSum == float.MAX)
				{
					continue;
				}

				// Check if it is a better fit than what has been found previously
				float sum  = targetDistance2;
				if (sum < retVal)
				{
					ret 	= new MeleeTargetData(obj, targetPos, hitComponent);
					retVal 	= sum;
				}
	
				allTargets.Insert(obj);
			}
		}
		
		return ret;
	}
	
	MeleeTargetData GetMeleeTarget(MeleeTargetSettings settings, out array<Object> allTargets = null)
	{
		return GetMeleeTargetEx(settings, allTargets);
	}
	

	bool FindMostSuitableComponentEx(Object obj, BoxCollidingResult bResult, MeleeTargetSettings settings, out float sum, out ref ComponentResult result, array<string> blacklistedDamageZones)
	{
		foreach (ComponentInfo cInfo : bResult.componentInfo)
		{
			ComponentResult cResult = new ComponentResult;
			
			if (!EvaluateComponentEx(obj, cInfo, settings, cResult, blacklistedDamageZones))
			{
				continue;
			}
			
			// Smallest number is a winner
			float cSum = cResult.ComponentDistance2;
			if (cSum < sum)
			{
				sum = cSum;				
				result = cResult;				
			}
		}
		
		return result != null;
	}

	bool FindMostSuitableComponent(Object obj, BoxCollidingResult bResult, MeleeTargetSettings settings, out float sum, out ref ComponentResult result)
	{
		return FindMostSuitableComponentEx(obj, bResult, settings, sum, result, null);
	}
	
	bool EvaluateComponentEx(Object obj, ComponentInfo cInfo, MeleeTargetSettings settings, out ref ComponentResult result, array<string> blacklistedDamageZones)
	{
		//! check if the component is on blacklist, if so, continue in lookup
		foreach (string zoneName: blacklistedDamageZones)
		{
			if (obj.GetDamageZoneNameByComponentIndex(cInfo.component) == zoneName)
			{
				return false;
			}
		}

		vector componentPos = cInfo.componentCenter;
		
		// Too far away! (fast reject)
		float componentMaxDist2 = Math.SqrFloat(settings.MaxDist + cInfo.componentRadius * obj.GetScale());
		vector nearestPoint = Math3D.NearestPoint(settings.RayStart, settings.RayEnd, componentPos);
		float componentDistance2 = vector.DistanceSq(componentPos, nearestPoint);
		if (componentDistance2 > componentMaxDist2)
		{
			return false;
		}
		
		// Here some more accurate check could be placed that would adjust the componentPos
		
		// Outside of the cone angle!
		float componentAngle = Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(settings.ConeOrigin, settings.XZDir, componentPos));	
		if (componentAngle > settings.ConeHalfAngle)
		{
			return false;
		}
			
		// Obstructed!
		if (IsMeleeTargetObstructed(settings.RayStart, componentPos))
		{
			return false;
		}

		// We found something, fill it in!
		result.ComponentPos = componentPos;
		result.ComponentAngle = componentAngle;
		result.ComponentDistance2 = componentDistance2;
		result.ComponentIdx = cInfo.component;
		
		return true;
	}
	
	bool EvaluateComponent(Object obj, ComponentInfo cInfo, MeleeTargetSettings settings, out ref ComponentResult result)
	{
		return EvaluateComponentEx(obj, cInfo, settings, result, {});
	}
	
	bool IsMeleeTargetObstructed(vector rayStart, vector rayEnd)
	{
		if (rayStart == rayEnd)
		{
			return true; // Not possible to trace when this happens (zero length raycast)
		}
		
		Object hitObject;
		vector hitPos, hitNormal;
		float hitFraction;

		return DayZPhysics.RayCastBullet( rayStart, rayEnd, MELEE_TARGET_OBSTRUCTION_LAYERS, null, hitObject, hitPos, hitNormal, hitFraction);
	}
}