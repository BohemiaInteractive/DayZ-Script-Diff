class VicinityItemManager
{
	private const float UPDATE_FREQUENCY 				= 0.25;
	private const float VICINITY_DISTANCE				= 0.5;
	private const float VICINITY_ACTOR_DISTANCE			= 2.0;
	private const float VICINITY_LARGE_ACTOR_DISTANCE 	= 3.0;
	private const float VICINITY_CONE_DISTANCE			= 2.0;
	private const float VICINITY_CONE_REACH_DISTANCE	= 2.0;
	private const float VICINITY_CONE_ANGLE 			= 30;
	private const float VICINITY_CONE_RADIANS 			= 0.5;
	private const string CE_CENTER 						= "ce_center";
	private const float HEIGHT_OFFSET 					= 0.2;
	private const int OBJECT_OBSTRUCTION_WEIGHT			= 10000; //in grams
	private const float CONE_HEIGHT_MIN 				= -0.5;
	private const float CONE_HEIGHT_MAX 				= 3.0;

	private ref array<EntityAI> m_VicinityItems 		= new array<EntityAI>();
	private ref array<CargoBase> m_VicinityCargos 		= new array<CargoBase>();
	private float m_RefreshCounter;
	private static ref VicinityItemManager s_Instance;
	
	static VicinityItemManager GetInstance ()
	{
		if (!s_Instance)
			s_Instance = new VicinityItemManager();

		return s_Instance;
	}
	
	void Init()
	{
	}
	
	array<EntityAI> GetVicinityItems()
	{
		return m_VicinityItems;
	}
	
	void AddVicinityItems(Object object)
	{
		EntityAI entity = EntityAI.Cast(object);
		if (entity)
		{
			if (m_VicinityItems.Find(entity) == INDEX_NOT_FOUND && GameInventory.CheckManipulatedObjectsDistances(entity, GetGame().GetPlayer(), VICINITY_CONE_REACH_DISTANCE + 1.0))
			{
				m_VicinityItems.Insert(entity);
			}
		}
	}

	array<CargoBase> GetVicinityCargos()
	{
		return m_VicinityCargos;
	}
	
	void AddVicinityCargos(CargoBase object)
	{
		if (m_VicinityCargos.Find(object) == INDEX_NOT_FOUND)
		{
			m_VicinityCargos.Insert(object);
		}
	}
	
	void ResetRefreshCounter()
	{
		m_RefreshCounter = 0;
	}
	
	void Update(float delta_time)
	{
		m_RefreshCounter += delta_time;
		
		if (m_RefreshCounter >= UPDATE_FREQUENCY)
		{
			RefreshVicinityItems();
			m_RefreshCounter = 0;
		}
	}
	
	bool ExcludeFromContainer_Phase1(Object actor_in_radius)
	{
		EntityAI entity;
		if (!Class.CastTo(entity, actor_in_radius))
			return true;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (entity == player)
			return true;
		if (entity.IsParticle())
			return true;
		if (entity.IsScriptedLight())
			return true;
		if (entity.IsBeingPlaced())
			return true;
		if (entity.IsHologram())
			return true;
		if (entity.IsMan() || entity.IsZombie() || entity.IsZombieMilitary())
		{
			//visibility cone check
			vector entityPosition = entity.GetPosition();
			
			if (entity && entity.IsMan())
			{
				PlayerBase vicinityPlayer = PlayerBase.Cast(entity);
				if (vicinityPlayer)
				{
					entityPosition = vicinityPlayer.GetBonePositionWS(vicinityPlayer.GetBoneIndexByName("spine3"));
				}
			}
			else if (entity && (entity.IsZombie() || entity.IsZombieMilitary()))
			{
				ZombieBase zombie = ZombieBase.Cast(entity);
				if (zombie)
				{
					entityPosition = zombie.GetBonePositionWS(zombie.GetBoneIndexByName("spine3"));
				}
			}
			
			vector entityDirection = player.GetPosition() - entityPosition;
			entityDirection.Normalize();
			entityDirection[1] = 0; //ignore height
			
			vector playerDirection =  MiscGameplayFunctions.GetHeadingVector(player);
			playerDirection.Normalize();
			playerDirection[1] = 0; //ignore height
			
			float dotRadians = vector.Dot(playerDirection, entityDirection);
			if (dotRadians > -0.5)
				return true;
		}

		return false;
	}
	
	bool ExcludeFromContainer_Phase2(Object object_in_radius)
	{
		EntityAI entity;

		if (!Class.CastTo(entity, object_in_radius))
			return true;
		if (entity == PlayerBase.Cast(GetGame().GetPlayer()))
			return true;
		if (entity.IsParticle())
			return true;
		if (entity.IsScriptedLight())
			return true;
		if (entity.IsBeingPlaced())
			return true;
		if (entity.IsHologram())
			return true;
		
		ItemBase item;
		if (!Class.CastTo(item, object_in_radius))
			return true;
		if (!item.IsTakeable())
			return true;
		
		return false;
	}
	
	bool ExcludeFromContainer_Phase3(Object object_in_cone)
	{
		EntityAI entity;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

		//Print("---object in cone: " + object_in_cone);
		if (!Class.CastTo(entity, object_in_cone))
			return true;
		if (entity == player)
			return true;
		if (entity.IsParticle())
			return true;
		if (entity.IsScriptedLight())
			return true;
		if (entity.IsBeingPlaced())
			return true;
		if (entity.IsHologram())
			return true;
		
		ItemBase item;
		if (!Class.CastTo(item, object_in_cone) && !object_in_cone.IsTransport() && !PASBroadcaster.Cast(object_in_cone))
			return true;
		if (item && !item.IsTakeable())
			return true;

		return false;
	}
	
	bool CanIgnoreDistanceCheck(EntityAI entity_ai)
	{
		return MiscGameplayFunctions.CanIgnoreDistanceCheck(entity_ai);
	}
	
	//per frame call
	void RefreshVicinityItems()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.GetActionManager() && player.GetActionManager().GetRunningAction())
			return;
		
		array<Object> objectsInVicinity 	= new array<Object>();
		array<CargoBase> proxyCargos 		= new array<CargoBase>();
		array<Object> filteredObjects 		= new array<Object>();
		array<Object> allFoundObjects 		= new array<Object>();
		vector playerPosition 				= player.GetPosition();
		vector headingDirection 			= MiscGameplayFunctions.GetHeadingVector(player);		
		vector playerHeadPos;
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);
		
		if (m_VicinityItems)
			m_VicinityItems.Clear();
		
		if (m_VicinityCargos)
			m_VicinityCargos.Clear();
		
		//1. GetAll actors in VICINITY_ACTOR_DISTANCE
//		DebugActorsSphereDraw( VICINITY_ACTOR_DISTANCE );
		GetGame().GetObjectsAtPosition3D(playerPosition, VICINITY_ACTOR_DISTANCE, objectsInVicinity, proxyCargos);
		
		// no filtering for cargo (initial implementation)		
		foreach (CargoBase cargoObject : proxyCargos)
			AddVicinityCargos(cargoObject);
		
		//filter unnecessary and duplicate objects beforehand
		foreach (Object actorInRadius : objectsInVicinity)
		{
			if (allFoundObjects.Find(actorInRadius) == INDEX_NOT_FOUND)
			{
				allFoundObjects.Insert(actorInRadius);
			}
			
			if (ExcludeFromContainer_Phase1(actorInRadius))
				continue;
				
			if (filteredObjects.Find(actorInRadius) == INDEX_NOT_FOUND)
			{
				filteredObjects.Insert(actorInRadius);
			}
		}
		
		if (objectsInVicinity) 
		{
			objectsInVicinity.Clear();
		}
		
		//2. GetAll Objects in VICINITY_DISTANCE
		GetGame().GetObjectsAtPosition3D(playerPosition, VICINITY_DISTANCE, objectsInVicinity, proxyCargos);
//		DebugObjectsSphereDraw( VICINITY_DISTANCE );
		
		//filter unnecessary and duplicate objects beforehand
		foreach (Object objectInRadius : objectsInVicinity)
		{
			if (allFoundObjects.Find(objectInRadius) == INDEX_NOT_FOUND)
			{
				allFoundObjects.Insert(objectInRadius);
			}
			
			if (ExcludeFromContainer_Phase2(objectInRadius))
				continue;
			
			if (filteredObjects.Find(objectInRadius) == INDEX_NOT_FOUND)
			{
				filteredObjects.Insert(objectInRadius);
			}
		}
		
		if (objectsInVicinity)
		{
			objectsInVicinity.Clear();
		}
		
		//3. Add objects from GetEntitiesInCone
		DayZPlayerUtils.GetEntitiesInCone( playerPosition, headingDirection, VICINITY_CONE_ANGLE, VICINITY_CONE_REACH_DISTANCE, CONE_HEIGHT_MIN, CONE_HEIGHT_MAX, objectsInVicinity);
//		DebugConeDraw( playerPosition, VICINITY_CONE_ANGLE );

		RaycastRVParams rayInput;
		array<ref RaycastRVResult> raycastResults = new array<ref RaycastRVResult>();
		//filter unnecessary and duplicate objects beforehand
		foreach (Object objectInCone : objectsInVicinity)
		{
			if (allFoundObjects.Find(objectInCone) == INDEX_NOT_FOUND)
			{
				allFoundObjects.Insert(objectInCone);
			}
			
			if (ExcludeFromContainer_Phase3(objectInCone))
				continue;

			if (filteredObjects.Find(objectInCone) == INDEX_NOT_FOUND)
			{
				//Test distance to closest component first
				rayInput = new RaycastRVParams(playerHeadPos, objectInCone.GetPosition(), player, 0.1);
				DayZPhysics.RaycastRVProxy(rayInput,raycastResults);
				
				foreach (RaycastRVResult result : raycastResults)
				{
					if (vector.DistanceSq(result.pos, playerHeadPos) > VICINITY_CONE_REACH_DISTANCE * VICINITY_CONE_REACH_DISTANCE)
					{
						continue;
					}
					
					if (result.hierLevel > 0 && result.parent == objectInCone)
					{
						filteredObjects.Insert(objectInCone);
					}
					else if (result.hierLevel == 0 && result.obj == objectInCone)
					{
						filteredObjects.Insert(objectInCone);
					}
				}
			}
		}
		
		//4. Add large objects - particularly buildings and BaseBuildingBase
		BoxCollidingParams params = new BoxCollidingParams();
		vector boxEdgeLength = {
			VICINITY_LARGE_ACTOR_DISTANCE,
			VICINITY_LARGE_ACTOR_DISTANCE,
			VICINITY_LARGE_ACTOR_DISTANCE
		};

		params.SetParams(playerPosition, headingDirection.VectorToAngles(), boxEdgeLength * 2, ObjIntersect.View, ObjIntersect.Fire, true);
		array<ref BoxCollidingResult> results = new array<ref BoxCollidingResult>();
		if (GetGame().IsBoxCollidingGeometryProxy(params, {player}, results))
		{
			foreach (BoxCollidingResult bResult : results)
			{
				if (bResult.obj && (bResult.obj.CanObstruct() || bResult.obj.CanProxyObstruct()))
				{
					if (allFoundObjects.Find(bResult.obj) == INDEX_NOT_FOUND)
					{
						allFoundObjects.Insert(bResult.obj);
					}
				}
				
				if (bResult.parent && (bResult.parent.CanObstruct() || bResult.parent.CanProxyObstruct()))
				{
					if (allFoundObjects.Find(bResult.parent) == INDEX_NOT_FOUND)
					{
						allFoundObjects.Insert(bResult.parent);
					}
				}
			}
		}
		
		//5. Filter filtered objects with RayCast from the player ( head bone )
		array<Object> obstructingObjects = new array<Object>();
		MiscGameplayFunctions.FilterObstructingObjects(allFoundObjects, obstructingObjects);
		
		if (obstructingObjects.Count() > 0)
		{
			if (filteredObjects.Count() > 10)
			{
				vector rayStart;
				MiscGameplayFunctions.GetHeadBonePos(player, rayStart);
				
				array<Object> filteredObjectsGrouped = new array<Object>();
				MiscGameplayFunctions.FilterObstructedObjectsByGrouping(rayStart, VICINITY_CONE_DISTANCE, 0.3, filteredObjects, obstructingObjects, filteredObjectsGrouped, true, true, VICINITY_CONE_REACH_DISTANCE);
				
				foreach (Object object: filteredObjectsGrouped)
					AddVicinityItems(object);
			}
			else
			{
				foreach (Object filteredObjectClose: filteredObjects)
				{
					EntityAI entity;
					Class.CastTo(entity, filteredObjectClose);
					
					//distance check
					if (vector.DistanceSq(playerPosition, entity.GetPosition()) > VICINITY_CONE_REACH_DISTANCE * VICINITY_CONE_REACH_DISTANCE)
					{
					    if (!CanIgnoreDistanceCheck(entity))
					    {
					        continue;
					    }
					}
					
					if (!IsObstructed(filteredObjectClose))
					{
						AddVicinityItems(filteredObjectClose);
					}
				}	
			}
		}
		else
		{
			foreach (Object filteredObject: filteredObjects)
				AddVicinityItems(filteredObject);
		}
	}
	
	bool IsObstructed(Object filtered_object)
	{
		return MiscGameplayFunctions.IsObjectObstructed(filtered_object);
	}
	
#ifdef DIAG_DEVELOPER
	//Debug functions
	
	ref array<Shape> rayShapes = new array<Shape>();
	
	private void DebugActorsSphereDraw(float radius)
	{
		CleanupDebugShapes(rayShapes);
		
		rayShapes.Insert(Debug.DrawSphere( GetGame().GetPlayer().GetPosition(), radius, COLOR_GREEN, ShapeFlags.TRANSP|ShapeFlags.WIREFRAME));
	}
	
	private void DebugObjectsSphereDraw(float radius)
	{		
		rayShapes.Insert(Debug.DrawSphere(GetGame().GetPlayer().GetPosition(), radius, COLOR_GREEN, ShapeFlags.TRANSP|ShapeFlags.WIREFRAME));
	}
	
	private void DebugRaycastDraw(vector start, vector end)
	{
		rayShapes.Insert(Debug.DrawArrow(start, end, 0.5, COLOR_YELLOW));
	}
	
	private void DebugConeDraw(vector start, float cone_angle)
	{
		vector endL, endR;
		float playerAngle;
		float xL,xR,zL,zR;
		
		playerAngle = MiscGameplayFunctions.GetHeadingAngle(PlayerBase.Cast(GetGame().GetPlayer()));

		endL = start;
		endR = start;
		xL = VICINITY_CONE_REACH_DISTANCE * Math.Cos( playerAngle + Math.PI_HALF + cone_angle * Math.DEG2RAD ); // x
		zL = VICINITY_CONE_REACH_DISTANCE * Math.Sin( playerAngle + Math.PI_HALF + cone_angle * Math.DEG2RAD ); // z
		xR = VICINITY_CONE_REACH_DISTANCE * Math.Cos( playerAngle + Math.PI_HALF - cone_angle * Math.DEG2RAD ); // x
		zR = VICINITY_CONE_REACH_DISTANCE * Math.Sin( playerAngle + Math.PI_HALF - cone_angle * Math.DEG2RAD ); // z
		endL[0] = endL[0] + xL;
		endL[2] = endL[2] + zL;
		endR[0] = endR[0] + xR;
		endR[2] = endR[2] + zR;

		rayShapes.Insert(Debug.DrawLine(start, endL, COLOR_GREEN));
		rayShapes.Insert(Debug.DrawLine(start, endR, COLOR_GREEN)) ;
		rayShapes.Insert(Debug.DrawLine(endL, endR, COLOR_GREEN));
	}
	
	private void CleanupDebugShapes(array<Shape> shapesArr)
	{
		foreach (Shape shape : shapesArr)
			Debug.RemoveShape(shape);
		
		shapesArr.Clear();
	}
#endif
}
