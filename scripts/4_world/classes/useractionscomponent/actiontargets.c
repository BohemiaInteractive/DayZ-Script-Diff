//! objects in vicinity - extended with secondary object which is parent of that Object
class VicinityObjects
{
	private ref map<Object, Object> m_VicinityObjects;
	
	void VicinityObjects()
	{
		m_VicinityObjects = new map<Object, Object>;
	}

	//! stores VicinityObject to Hashmap - for storing of parent/child relationship
	void StoreVicinityObject(Object object, Object parent = null)
	{
		//! completely remove items that are being placed or are holograms
		ItemBase ib = ItemBase.Cast(object);
		if (ib && (ib.IsBeingPlaced() || ib.IsHologram()))
			return;

		//! ignores plain objects
		/*if(object && object.IsPlainObject())
		{
			Print("ERROR: VicinityObjects | StoreVicinityObject | IsPlainObject check fail");
			return;
		}*/
		
		if ( !m_VicinityObjects.Contains(object) )
		{
			//! init of VicinityObjects - object, parent(if exists)
			m_VicinityObjects.Set(object, parent);
		}
	}

	//! transform simple array of Objects to VicinityObjects hashmap
	void TransformToVicinityObjects(array<Object> objects)
	{
		foreach (Object obj : objects)
		{
			if (obj && obj.GetType() != "" && obj.CanBeActionTarget())
			{
				StoreVicinityObject(obj);
			}
		}
	}
	
	void ClearVicinityObjects()
	{
		m_VicinityObjects.Clear();
	}

	//! return simple array of Objects in Vicinity
	array< Object > GetVicinityObjects()
	{
		ref array<Object> vicinityObjects = new array<Object>;
		for (int i = 0; i < m_VicinityObjects.Count(); i++)
		{
			//! filters out non-takeable items (won't be shown in vicinity)
			ItemBase ib = ItemBase.Cast(GetObject(i));
			if (ib && !ib.IsTakeable())
				continue;

			vicinityObjects.Insert(GetObject(i));
		}
		
		return vicinityObjects;
	}
	
	//! return simple array of Objects in Vicinity
	array< Object > GetRawVicinityObjects()
	{
		ref array<Object> vicinityObjects = new array<Object>;
		for (int i = 0; i < m_VicinityObjects.Count(); i++)
		{
			vicinityObjects.Insert(GetObject(i));
		}
		
		return vicinityObjects;
	}

	//! returns VicinityObjects Key
	Object GetObject(int i)
	{
		return m_VicinityObjects.GetKey(i);
	}

	//! returns VicinityObjects Element
	Object GetParent(int i)
	{
		return m_VicinityObjects.GetElement(i);
	}
	
	int Count()
	{
		return m_VicinityObjects.Count();
	}
	
	void Remove(Object object)
	{
		m_VicinityObjects.Remove(object);
	}
	
	void Remove(array<Object> objects)
	{
		for (int i = 0; i < objects.Count(); i++)
		{
			m_VicinityObjects.Remove(objects[i]);
		}
	}
}

class ActionTarget
{
	void ActionTarget(Object object, Object parent, int componentIndex, vector cursorHitPos, float utility)
	{
		m_Object = object;
		m_Parent = parent;
		m_ComponentIndex = componentIndex;
		m_CursorHitPos = cursorHitPos;
		m_Utility = utility;
	} 

	Object GetObject()
		{ return m_Object; }

	Object GetParent()
		{ return m_Parent; }

	bool IsProxy()
	{
		if (m_Parent)
		 	return true;
		return false;
	}
	
	int GetComponentIndex()	
		{ return m_ComponentIndex; }
	
	float GetUtility()
		{ return m_Utility; }
	
	vector GetCursorHitPos()
		{ return m_CursorHitPos; }
	
	void SetCursorHitPos(vector cursor_position)
	{
		m_CursorHitPos = cursor_position;
	}
	
	void DbgPrintTargetDump()
	{
		Print(DumpToString());
	}
	
	string DumpToString()
	{
		string res = "ActionTarget dump = {";
		res = res + "m_Object: " + Object.GetDebugName(m_Object);
		res = res + "; m_Parent: " + Object.GetDebugName(m_Parent);
		res = res + "; m_ComponentIndex: " + m_ComponentIndex.ToString();
		res = res + "; m_CursorHitPos: " + m_CursorHitPos.ToString();
		res = res + "; m_Utility: " + m_Utility.ToString();
		res = res + "}";
		return res;
	}
	
	private Object m_Object;		// object itself
	private Object m_Parent;		// null or parent of m_Object
	private int m_ComponentIndex;	// p3d Component ID or -1
	private vector m_CursorHitPos;
	private float m_Utility;
};

class ActionTargets
{
	private ref map<int, ref array<RaycastRVResult>> m_SortedResults = new map<int, ref array<RaycastRVResult>>();
	
	void ActionTargets(PlayerBase player)
	{
		m_Player = player;
		m_VicinityObjects = new VicinityObjects;
		m_Targets = new array<ref ActionTarget>;
		
		m_Debug = false;
	}
	
	static array<Object> GetVicinityObjects()
	{
		return m_VicinityObjects.GetVicinityObjects();
	}
	
	void Clear()
	{
		m_Targets.Clear();
		m_SortedResults.Clear();
		m_HitPos = vector.Zero;
	}
	
	void Update()
	{
		//! clear state
		m_VicinityObjects.ClearVicinityObjects();
		Clear();

		Object firstValidRaycastedObject = null;
		array<Object> vicinityObjects = new array<Object>;
		
		//! camera & ray properties
		vector playerPos = m_Player.GetPosition();
		vector headingDirection = MiscGameplayFunctions.GetHeadingVector(m_Player);

		m_RayStart = g_Game.GetCurrentCameraPosition();
		m_RayEnd = m_RayStart + g_Game.GetCurrentCameraDirection() * c_RayDistance;

		RaycastRVParams rayInput = new RaycastRVParams(m_RayStart, m_RayEnd, m_Player);
		rayInput.flags = CollisionFlags.ALLOBJECTS;
		//rayInput.sorted = true;
		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		array<RaycastRVResult> sortedValidRaycastResults = new array<RaycastRVResult>();
		map<Object, RaycastRVResult> resultMap = new map<Object, RaycastRVResult>();
		
		if (DayZPhysics.RaycastRVProxy(rayInput, results))
		{
			array<int> sortedDistanceHelper = SortResultsDistance(results);
			int sortedResultsCount = m_SortedResults.Count();
			if (sortedResultsCount > 0 )
			{
				foreach (int sortedDist : sortedDistanceHelper) //distances already sorted
				{
					array<RaycastRVResult> sameDistResults = m_SortedResults.Get(sortedDist);
					
					foreach (RaycastRVResult res : sameDistResults)
					{
						if (res.obj && !res.obj.CanBeActionTarget())
							continue;
						
						//! if the cursor target is a proxy
						if (res.hierLevel > 0)
						{
							//! ignores attachments on player and objects with invalid IDs
							if (res.parent.IsMan() || res.obj.GetID() < 1)
								continue;
						}
						
						sortedValidRaycastResults.Insert(res);
						resultMap.Insert(res.obj, res);
						if (!firstValidRaycastedObject)
							firstValidRaycastedObject = res.obj;
						
						if (m_HitPos == vector.Zero)
							m_HitPos = res.pos;
						
						break;
					}
				}
			}
		}
		else
		{
			firstValidRaycastedObject = null;
			m_HitPos = vector.Zero;
		}
		
		//! spacial search
		DayZPlayerCamera camera = m_Player.GetCurrentCamera();
		if (camera && camera.GetCurrentPitch() <= -45) // Spatial search is a contributor to very heavy searching, limit it to when we are at least looking down
			DayZPlayerUtils.GetEntitiesInCone(playerPos, headingDirection, c_ConeAngle, c_MaxTargetDistance, c_ConeHeightMin, c_ConeHeightMax, vicinityObjects);
		
		//! removes player from the vicinity
		vicinityObjects.RemoveItem(m_Player);
		
		FilterDuplicateVicinityObjects(vicinityObjects, sortedValidRaycastResults);

		//! transformation of array of Objects to hashmap (VicinityObjects)
		m_VicinityObjects.TransformToVicinityObjects(vicinityObjects);
		
		//! removes Vicinity objects that are not directly visible from player position
		FilterObstructedObjectsEx(firstValidRaycastedObject, vicinityObjects);
		
		//! select & sort targets based on utility function
		int vicinityObjCount = m_VicinityObjects.Count();
		for (int i = 0; i < vicinityObjCount; ++i)
		{
			Object object = m_VicinityObjects.GetObject(i);
			Object parent;
			RaycastRVResult result;
			int targetComponent;
			vector hitpos;
			//proper raycast results will use their own data
			if (resultMap.Find(object, result))
			{
				parent = result.parent;
				hitpos = result.pos;
				targetComponent = result.component;
			}
			else
			{
				parent = m_VicinityObjects.GetParent(i);
				hitpos = m_HitPos; //substitute raycast hitpos as reference for the utility check
				targetComponent = -1; //invalid component
			}
			
			float utility = ComputeUtility(object, m_RayStart, m_RayEnd, firstValidRaycastedObject, hitpos);
			if (utility > 0)
			{
				ActionTarget at = new ActionTarget(object, parent, targetComponent, m_HitPos, utility);
				StoreTarget(at);
			}
		}

		//! action target for surface actions (lowest utility)
		if (m_HitPos == vector.Zero)
		{
			vector 		contact_pos, contact_dir, hitNormal;
			int 		contactComponent;
			float 		hitFraction;
			Object 		hitObject;
			
			m_RayEnd = m_RayStart + g_Game.GetCurrentCameraDirection() * c_RayDistance * 3;
			
			PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.ROADWAY|PhxInteractionLayers.TERRAIN|PhxInteractionLayers.WATERLAYER;
			DayZPhysics.RayCastBullet(m_RayStart,m_RayEnd,collisionLayerMask,null,hitObject,contact_pos,hitNormal,hitFraction);
			m_HitPos = contact_pos;
		}
		
		m_Targets.Insert(new ActionTarget(null, null, -1, m_HitPos, 0));

#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.MISC_ACTION_TARGETS_DEBUG))
		{
			ShowDebugActionTargets(true);
			DrawDebugActionTargets(true);
			DrawDebugCone(true);
			DrawDebugRay(true);
			DrawSelectionPos(DiagMenu.GetBool(DiagMenuIDs.MISC_ACTION_TARGETS_SELPOS_DEBUG));
		}
		else
		{
			ShowDebugActionTargets(false);
			DrawDebugActionTargets(false);
			DrawDebugCone(false);
			DrawDebugRay(false);
			DrawSelectionPos(false);
		}
#endif
	}
	
	//distance sorting, returns sorted unique distance tiers as ints (float*1000)
	protected array<int> SortResultsDistance(array<ref RaycastRVResult> results)
	{
		array<int> distHelper = new array<int>();
		foreach (RaycastRVResult res : results)
		{
			int distance = (int)(vector.DistanceSq(res.pos, m_RayStart) * 1000);
			array<RaycastRVResult> arr = m_SortedResults.Get(distance);
			if (!arr)
				arr = new array<RaycastRVResult>();
			
			arr.Insert(res);
			m_SortedResults.Set(distance, arr);
			
			if (distHelper.Find(distance) == -1)
				distHelper.Insert(distance);
		}
		
		distHelper.Sort();
		return distHelper;
	}
	
	private bool IsObstructed(Object object)
	{
		IsObjectObstructedCache cache = new IsObjectObstructedCache(m_RayStart, 1);
		return IsObstructedEx(object, cache);
	}
	
	private bool IsObstructedEx(Object object, IsObjectObstructedCache cache)
	{
		return MiscGameplayFunctions.IsObjectObstructedEx(object, cache);
	}
 	
	//! returns count of founded targets
	int GetTargetsCount()
		{ return m_Targets.Count(); }
	
	//! returns action target at index
	ActionTarget GetTarget(int index)
		{ return m_Targets.Get(index); }

	//! inserts action into sorted array based on utility
	private void StoreTarget(ActionTarget pActionTarget)
	{
		int index = FindIndexForStoring(pActionTarget.GetUtility());
		m_Targets.InsertAt(pActionTarget, index);
		//Print("StoreTarget; object: " + pActionTarget.GetObject() + " | parent: " + pActionTarget.GetParent() + " | idx: " + index);
	}

	//! binary search algorithm	
	private int FindIndexForStoring(float value)
	{
		int left = 0;
		int right = m_Targets.Count() - 1;
		while ( left <= right )
		{
			int middle = (left + right) / 2;
			float middleValue = m_Targets.Get(middle).GetUtility();
			
			if ( middleValue == value )
				return middle;
            else if ( middleValue < value )
				right = middle - 1;
            else
				left = middle + 1;
		}
		
		return left;
	}
	
	//! computes utility of target
	private float ComputeUtility(Object pTarget, vector pRayStart, vector pRayEnd, Object cursorTarget, vector hitPos)
	{
		//! out of reach
		if (vector.DistanceSq(hitPos, m_Player.GetPosition()) > c_MaxTargetDistance * c_MaxTargetDistance)
			return -1;

		if (pTarget)
		{
			if ( pTarget == cursorTarget )
			{
				//! ground and static objects
				if ( pTarget.GetType() == string.Empty )
					return 0.01;

				if ( pTarget.IsBuilding() )
					return 0.25;

				if ( pTarget.IsTransport() )
					return 0.25;
				
				//!basebuilding objects
				if (pTarget.CanUseConstruction())
					return 0.85;

				if ( pTarget.IsWell() )
					return 0.9;

				vector playerPosXZ = m_Player.GetPosition();
				vector hitPosXZ = hitPos;
				playerPosXZ[1] = 0;
				hitPosXZ[1] = 0;
				if ( vector.DistanceSq(playerPosXZ, hitPosXZ) <= c_MaxTargetDistance * c_MaxTargetDistance )
					return c_UtilityMaxValue;
			}

			if ( PlayerBase.Cast(pTarget) && PlayerBase.Cast(pTarget).IsInVehicle() ) // utility in vehicle should be below base vehicle val
				return 0.20;
			
			float distSqr = DistSqrPoint2Line(pTarget.GetPosition(), pRayStart, pRayEnd);
			return (c_UtilityMaxDistFromRaySqr - distSqr) / c_UtilityMaxDistFromRaySqr;
		}

		return -1;
	}

	//! distance between point and line
	private float DistSqrPoint2Line(vector pPoint, vector pL1, vector pL2)
	{
		vector v = pL2 - pL1;
		vector w = pPoint - pL1;
	
		float c1 = vector.Dot(w,v);
		float c2 = vector.Dot(v,v);

		if ( c1 <= 0 || c2 == 0 )
			return vector.DistanceSq(pPoint, pL1);

		float b = c1 / c2;	
		vector nearestPoint = pL1 + (v * b);
		return vector.DistanceSq(pPoint, nearestPoint);		
	}
	
	private void FilterDuplicateVicinityObjects(inout array<Object> vicinityObjectsOutput, array<RaycastRVResult> sortedRaycastResults)
	{
		//remove duplicates
		array<Object> tmp = new array<Object>();
		tmp.Copy(vicinityObjectsOutput);
		vicinityObjectsOutput.Clear();
		foreach (Object obj : tmp)
		{
			if (obj && vicinityObjectsOutput.Find(obj) == -1)
				vicinityObjectsOutput.Insert(obj);
		}
		
		//next append the raycast result objects
		foreach (RaycastRVResult raycastResult : sortedRaycastResults)
		{
			if (raycastResult.obj && vicinityObjectsOutput.Find(raycastResult.obj) == -1)
				vicinityObjectsOutput.Insert(raycastResult.obj);
		}
	}
	
	private void FilterObstructedObjectsEx(Object cursor_target, array<Object> vicinityObjects)
	{
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.MISC_ACTION_TARGETS_DEBUG))
			CleanupDebugShapes(obstruction);
		#endif

		array<Object> obstructingObjects = new array<Object>;
		MiscGameplayFunctions.FilterObstructingObjects(vicinityObjects, obstructingObjects);
		
		if ( obstructingObjects.Count() > 0 )
		{
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
					
			int numObstructed = 0;
			int mCount = m_VicinityObjects.Count();
			
			if (mCount > GROUPING_COUNT_THRESHOLD)
			{
				array<Object> filteredObjects = new array<Object>;
				MiscGameplayFunctions.FilterObstructedObjectsByGrouping(m_RayStart, c_MaxTargetDistance, c_DistanceDelta, m_VicinityObjects.GetRawVicinityObjects(), vicinityObjects, filteredObjects);
				m_VicinityObjects.ClearVicinityObjects();
				m_VicinityObjects.TransformToVicinityObjects(filteredObjects);
			}
			else
			{
				FilterObstructedObjects(cursor_target);
			}
		}
	}
	
	private void FilterObstructedObjects(Object cursor_target)
	{
		int numObstructed = 0;
		int mCount = m_VicinityObjects.Count();
		IsObjectObstructedCache cache = new IsObjectObstructedCache(m_RayStart, mCount);
		mCount--;
	
		//! check if targets are not obstructed (eg.: wall)
		for ( int i = mCount; i >= 0; --i )
		{
			Object object = m_VicinityObjects.GetObject(i);
			Object parent = m_VicinityObjects.GetParent(i);

			//! check for object obstruction(if the object is not a proxy - has no parent)
			if (object && !parent)
			{
				//! when the number of obstructed items is higher than OBSTRUCTED_COUNT_THRESHOLD
				//! remove do no run obstruction check and skip these items
				if (numObstructed > OBSTRUCTED_COUNT_THRESHOLD && object != cursor_target)
				{
					m_VicinityObjects.Remove(object);
					continue;
				}

				//! obstruction check
				if (object != cursor_target && IsObstructedEx(object, cache))
				{
					m_VicinityObjects.Remove(object);
					numObstructed++;
				}

				cache.ClearCache();
			}
		}
	}
	
#ifdef DIAG_DEVELOPER
	ref array<Shape> shapes = new array<Shape>();
	ref array<Shape> dbgConeShapes = new array<Shape>();
	ref array<Shape> rayShapes = new array<Shape>();
	ref array<Shape> obstruction = new array<Shape>();
	ref array<Shape> dbgPosShapes = new array<Shape>();
	
	void ShowDebugActionTargets(bool enabled)
	{
		int windowPosX = 0;
		int windowPosY = 50;

		Object obj;

		DbgUI.BeginCleanupScope();
		DbgUI.Begin("Action Targets", windowPosX, windowPosY);
		if ( enabled )
		{
			for ( int i = 0; i < GetTargetsCount(); i++ )
			{				
				obj = m_Targets.Get(i).GetObject();
				if ( obj )
				{
					float util = m_Targets.Get(i).GetUtility();
					int compIdx = m_Targets.Get(i).GetComponentIndex();
					string compName;
					array<string> compNames = new array<string>;
					compName = obj.GetActionComponentName(compIdx);
					obj.GetActionComponentNameList(compIdx, compNames);

					if ( compNames.Count() > 0 )
					{
						for ( int c = 0; c < compNames.Count(); c++ )
						{
							DbgUI.Text(obj.GetDisplayName() + " :: " + obj + " | util: " + util + " | compIdx: " + compIdx + " | compName: " + compNames[c] + "| wPos: " + obj.GetWorldPosition() );
						}
					}
					else
					{
						DbgUI.Text(obj.GetDisplayName() + " :: " + obj + " | util: " + util + " | compIdx: " + compIdx + " | compName: " + compName + "| wPos: " + obj.GetWorldPosition() );					
					}
				}
				else
					continue;
			}
		}
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}

	void DrawDebugActionTargets(bool enabled)
	{
		int s_id;
		vector w_pos;
		vector w_pos_sphr;
		vector w_pos_lend;
		Object obj;

		if ( enabled )
		{
			CleanupDebugShapes(shapes);
			
			for ( int i = 0; i < GetTargetsCount(); i++ )
			{
				obj = m_Targets.Get(i).GetObject();
				if ( obj )
				{
					w_pos = obj.GetPosition();
					// sphere pos tweaks
					w_pos_sphr = w_pos;
					w_pos_sphr[1] = w_pos_sphr[1] + 0.5;
					// line pos tweaks
					w_pos_lend = w_pos;
					w_pos_lend[1] = w_pos_lend[1] + 0.5;
					
					if ( i == 0 )
					{
						shapes.Insert( Debug.DrawSphere(w_pos_sphr, 0.03, COLOR_RED) );
						shapes.Insert( Debug.DrawLine(w_pos, w_pos_lend, COLOR_RED) );
					}
					else
					{
						shapes.Insert( Debug.DrawSphere(w_pos_sphr, 0.03, COLOR_YELLOW) );
						shapes.Insert( Debug.DrawLine(w_pos, w_pos_lend, COLOR_YELLOW) );
					}
				}
			}
		}
		else
			CleanupDebugShapes(shapes);
	}
	
	private void DrawDebugCone(bool enabled)
	{
		// "cone" settings
		vector start, end, endL, endR;
		float playerAngle;
		float xL,xR,zL,zR;
		
		if (enabled)
		{
			CleanupDebugShapes(dbgConeShapes);

			start = m_Player.GetPosition();
			playerAngle = MiscGameplayFunctions.GetHeadingAngle(m_Player);
			
			// offset position of the shape in height
			start[1] = start[1] + 0.2;

			endL = start;
			endR = start;
			xL = c_MaxTargetDistance * Math.Cos(playerAngle + Math.PI_HALF + c_ConeAngle * Math.DEG2RAD); // x
			zL = c_MaxTargetDistance * Math.Sin(playerAngle + Math.PI_HALF + c_ConeAngle * Math.DEG2RAD); // z
			xR = c_MaxTargetDistance * Math.Cos(playerAngle + Math.PI_HALF - c_ConeAngle * Math.DEG2RAD); // x
			zR = c_MaxTargetDistance * Math.Sin(playerAngle + Math.PI_HALF - c_ConeAngle * Math.DEG2RAD); // z
			endL[0] = endL[0] + xL;
			endL[2] = endL[2] + zL;
			endR[0] = endR[0] + xR;
			endR[2] = endR[2] + zR;

			dbgConeShapes.Insert( Debug.DrawLine(start, endL, COLOR_BLUE) );
			dbgConeShapes.Insert( Debug.DrawLine(start, endR, COLOR_BLUE) ) ;
			dbgConeShapes.Insert( Debug.DrawLine(endL, endR, COLOR_BLUE) );
		}
		else
			CleanupDebugShapes(dbgConeShapes);		
	}
	
	private void DrawSelectionPos(bool enabled)
	{
		if (enabled)
		{
			CleanupDebugShapes(dbgPosShapes);
			if (GetTargetsCount() > 0 && GetTarget(0).GetUtility() > -1 )
			{
				ActionTarget at = GetTarget(0);
				if (at.GetObject())
				{
					string compName = at.GetObject().GetActionComponentName(at.GetComponentIndex());
					vector modelPos = at.GetObject().GetSelectionPositionMS(compName);
					vector worldPos = at.GetObject().ModelToWorld(modelPos);
					dbgPosShapes.Insert( Debug.DrawSphere(worldPos, 0.25, Colors.PURPLE, ShapeFlags.NOZBUFFER) );
				}
			}
		}
		else
			CleanupDebugShapes(dbgPosShapes);
	}

	private void DrawDebugRay(bool enabled)
	{
		if (enabled)
		{
			CleanupDebugShapes(rayShapes);
			rayShapes.Insert( Debug.DrawSphere(m_HitPos, Math.Sqrt(c_UtilityMaxDistFromRaySqr), COLOR_BLUE_A, ShapeFlags.TRANSP) );
			rayShapes.Insert( Debug.DrawLine(m_RayStart, m_RayEnd, COLOR_BLUE) );
		}
		else
			CleanupDebugShapes(rayShapes);
	}

	private void CleanupDebugShapes(array<Shape> shapesArr)
	{
		for ( int it = 0; it < shapesArr.Count(); ++it )
		{
			Shape shape = shapesArr[it];
			Debug.RemoveShape( shape );
		}
		
		shapesArr.Clear();
	}
#endif
	
	//--------------------------------------------------------
	// Members
	//--------------------------------------------------------
	//! player owner
	private PlayerBase m_Player;
	
	//! selected & sorted targets by utility function
	private ref array<ref ActionTarget> m_Targets;
	
	//! objects in vicinity
	static private ref VicinityObjects m_VicinityObjects

	private bool m_Debug
	
	private vector m_RayStart;
	private vector m_RayEnd;
	private vector m_HitPos; //!first valid hitpos! Used only as rough reference value for conecasted results.
	
	//--------------------------------------------------------
	// Constants
	//--------------------------------------------------------
	//! searching properties
	private const float c_RayDistance = 5.0;
	private const float c_MaxTargetDistance = 3.0;
	private const float c_MaxActionDistance = UAMaxDistances.DEFAULT;
	private const float c_ConeAngle = 30.0;
	private const float c_ConeHeightMin = -0.5;
	private const float c_ConeHeightMax = 2.0;
	private const float c_DistanceDelta = 0.3;
	
	//! utility constants
	private const float c_UtilityMaxValue = 10000;
	private const float c_UtilityMaxDistFromRaySqr = 0.8 * 0.8;

	//! p3d 
	private const string CE_CENTER 					= "ce_center";
	private const float HEIGHT_OFFSET 				= 0.2;
	
	//! misc
	private const int OBSTRUCTED_COUNT_THRESHOLD	= 3;
	private const int GROUPING_COUNT_THRESHOLD		= 10;
	
	//! DEPRECATED
	vector CalculateRayStart();
};

class ObjectGroup
{
	ref array<Object> Objects = new array<Object>;
}