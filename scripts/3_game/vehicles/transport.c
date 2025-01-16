/*!
	Base native class of all vehicles in game.
*/
#ifdef FEATURE_NETWORK_RECONCILIATION

class TransportOwnerState : PawnOwnerState
{
	proto native void	SetWorldTransform(vector transform[4]);
	proto native void	GetWorldTransform(out vector transform[4]);

	proto native void	SetLinearVelocity(vector value);
	proto native void	GetLinearVelocity(out vector value);

	proto native void	SetAngularVelocity(vector value);
	proto native void	GetAngularVelocity(out vector value);
	
	proto native void	SetBuoyancySubmerged(float value);
	proto native float	GetBuoyancySubmerged();

#ifdef DIAG_DEVELOPER
	override event void	GetTransform(inout vector transform[4])
	{
		GetWorldTransform(transform);
	}
#endif
};

class TransportMove : PawnMove
{
	proto native void	SetWorldTransform(vector transform[4]);
	proto native void	GetWorldTransform(out vector transform[4]);

	proto native void	SetLinearVelocity(vector value);
	proto native void	GetLinearVelocity(out vector value);

	proto native void	SetAngularVelocity(vector value);
	proto native void	GetAngularVelocity(out vector value);
	
#ifdef DIAG_DEVELOPER
	override event void	GetTransform(inout vector transform[4])
	{
		GetWorldTransform(transform);
	}
#endif
};

//! Uses NetworkMoveStrategy.NONE
class Transport extends Pawn
#else
class Transport extends EntityAI
#endif
{
	//! Shared context across all vehicles for flipping
	private static ref VehicleFlippedContext m_FlippedContext;
	
	ref TIntArray m_SingleUseActions;
	ref TIntArray m_ContinuousActions;
	ref TIntArray m_InteractActions;
	
	protected bool m_EngineZoneReceivedHit;
	protected vector m_fuelPos;
	
	protected ref set<int> m_UnconsciousCrewMemberIndices;
	protected ref set<int> m_DeadCrewMemberIndices;
	
	void Transport()
	{
		RegisterNetSyncVariableBool("m_EngineZoneReceivedHit");
		
		if ( MemoryPointExists("refill") )
			m_fuelPos = GetMemoryPointPos("refill");
		else
			m_fuelPos = "0 0 0";
		
		m_UnconsciousCrewMemberIndices 	= new set<int>();
		m_DeadCrewMemberIndices 		= new set<int>();
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		SetEngineZoneReceivedHit(dmgZone == "Engine");
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}

	//!
	protected override event typename GetOwnerStateType()
	{
		return TransportOwnerState;
	}

	//!
	protected override event typename GetMoveType()
	{
		return TransportMove;
	}

	//! Synchronizes car's state in case the simulation is not running.
	proto native void Synchronize();

	//! Returns crew capacity of this vehicle.
	proto native int CrewSize();

	//! Returns crew member index based on action component index.
	//! -1 is returned when no crew position corresponds to given component index.
	proto native int CrewPositionIndex( int componentIdx );

	//! Returns crew member index based on player's instance.
	//! -1 is returned when the player is not isnide.
	proto native int CrewMemberIndex( Human player );

	//! Returns crew member based on position index.
	//! Null can be returned if no Human is present on the given position.
	proto native Human CrewMember( int posIdx );

	//! Returns the driver
	//! Null can be returned if no Human is present.
	proto native Human CrewDriver();

	//! Reads entry point and direction into vehicle on given position in model space.
	proto void CrewEntry( int posIdx, out vector pos, out vector dir );

	//! Reads entry point and direction into vehicle on given position in world space.
	proto void CrewEntryWS( int posIdx, out vector pos, out vector dir );

	//! Returns crew transformation indside vehicle in model space
	proto void CrewTransform( int posIdx, out vector mat[4] );

	//! Returns crew transformation indside vehicle in world space
	proto void CrewTransformWS( int posIdx, out vector mat[4] );

	//! Performs transfer of player from world into vehicle on given position.
	proto native void CrewGetIn( Human player, int posIdx );

	//! Performs transfer of player from vehicle into world from given position.
	proto native Human CrewGetOut( int posIdx );
	
	//! Handles death of player in vehicle and awakes its physics if needed
	proto native void CrewDeath( int posIdx );
	
	override bool IsTransport()
	{
		return true;
	}
	
	override bool IsIgnoredByConstruction()
	{
		return false;
	}
	
	override bool IsHealthVisible()
	{
		return true;
	}

	override bool ShowZonesHealth()
	{
		return true;
	}

	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
	
	float GetMomentum()
	{
		return GetVelocity(this).Length() * dBodyGetMass(this);
	}
	
	bool IsVitalSparkPlug()
	{
		return true;
	}
	
	string GetVehicleType()
	{
		return "VehicleTypeUndefined";
	}
	
	string GetActionCompNameFuel()
	{
		return "refill";
	}
	
	float GetActionDistanceFuel()
	{
		return 1.5;
	}
	
	vector GetRefillPointPosWS()
	{
		return ModelToWorld( m_fuelPos );
	}
	
	protected /*sealed*/ VehicleFlippedContext GetFlipContext()
	{
		if (!m_FlippedContext)
		{
			m_FlippedContext = new VehicleFlippedContext();
		}

#ifdef DIAG_DEVELOPER
		m_FlippedContext.Reset(DiagMenu.GetBool(DiagMenuIDs.VEHICLE_DRAW_FLIP_CONTEXT));
#endif
		
		return m_FlippedContext;
	}
	
	protected bool DetectFlippedUsingSurface(VehicleFlippedContext ctx, float angleTolerance)
	{
		vector corners[4];
		GetTightlyPackedCorners(ETransformationAxis.BOTTOM, corners);
		
		// compute the average position to find the lowest "center-most" position
		vector avgPosition = vector.Zero;		
		for (int i = 0; i < 4; i++)
		{			
			avgPosition = avgPosition + corners[i];
		}
		
		avgPosition = avgPosition * 0.25;
		
		// get depth of the water to determine if we should use the roadway surface normal or just up vector
		float depth = GetGame().GetWaterDepth(avgPosition);
		
		vector normal = vector.Up;
		vector dirUp = GetDirectionUp();
		
		bool testLand = depth < -1.0;
		
		// iterate over the corners to find the average normal
		if (testLand)
		{
			// trace roadway, incase the vehicle is on a rock, or bridge
			ctx.m_SurfaceParams.type = SurfaceDetectionType.Roadway;
			
			// ignore expensive water computation, we already know we are above land
			ctx.m_SurfaceParams.includeWater = false;
			
			// ignore this vehicle, it may have a roadway LOD
			ctx.m_SurfaceParams.ignore = this;
			
			// detect closest to the given point
			ctx.m_SurfaceParams.rsd = RoadSurfaceDetection.CLOSEST;
			
			for (i = 0; i < 4; i++)
			{
				ctx.m_SurfaceParams.position = corners[i];
				
				GetGame().GetSurface(ctx.m_SurfaceParams, ctx.m_SurfaceResult);
				
				corners[i][1] = ctx.m_SurfaceResult.height;
			}
			
			vector d0 = vector.Direction(corners[0], corners[1]);
			vector d1 = vector.Direction(corners[0], corners[2]);
			
			d0.Normalize();
			d1.Normalize();
			
			// cross product the two directions to get the normal vector of the land
			normal = d0 * d1;
		}
				
		bool isFlipped = vector.Dot(normal, dirUp) < Math.Cos(angleTolerance * Math.DEG2RAD);
		
#ifdef DIAG_DEVELOPER
		if (ctx.IsDebug())
		{
			int color = 0xFF00FF00;
			if (isFlipped)
				color = 0xFFFF0000;
			
			ctx.AddShape(Shape.Create(ShapeType.LINE, color, ShapeFlags.NOZBUFFER, corners[0], corners[0] + normal));
			ctx.AddShape(Shape.Create(ShapeType.LINE, color, ShapeFlags.NOZBUFFER, corners[1], corners[1] + normal));
			ctx.AddShape(Shape.Create(ShapeType.LINE, color, ShapeFlags.NOZBUFFER, corners[2], corners[2] + normal));
			ctx.AddShape(Shape.Create(ShapeType.LINE, color, ShapeFlags.NOZBUFFER, corners[3], corners[3] + normal));
		}
#endif
		
		return isFlipped;
	}
	
	//! Override based on vehicle implementation (Car, Boat, modded, etc.)
	protected bool DetectFlipped(VehicleFlippedContext ctx)
	{
		return false;
	}
	
	//! Don't override, may change to native for caching 'DetectFlipped' in the future based on active-ness (i.e. only updated when vehicle changes active state)
	/*sealed*/ bool IsFlipped()
	{
		VehicleFlippedContext ctx = GetFlipContext();
		ctx.m_bIsAction = false;
		ctx.m_ActionPlayer = null;
		return DetectFlipped(ctx);
	}
	
	//! Don't override, may change to native for caching 'DetectFlipped' in the future based on active-ness (i.e. only updated when vehicle changes active state)
	/*sealed*/ bool IsActionFlipped(Man player)
	{
		VehicleFlippedContext ctx = GetFlipContext();
		ctx.m_bIsAction = true;
		ctx.m_ActionPlayer = player;
		return DetectFlipped(ctx);
	}
	
	bool IsAnyCrewPresent()
	{
		for (int index = 0; index < CrewSize(); ++index)
		{
			if (CrewMember(index) != null)
				return true;
		}
		
		return false;
	}
	
	float GetTransportCameraDistance()
	{
		return 4.0;
	}
	
	void MarkCrewMemberUnconscious(int crewMemberIndex)
	{
		set<int> crewMemberIndicesCopy = new set<int>();
		crewMemberIndicesCopy.Copy(m_UnconsciousCrewMemberIndices);
		crewMemberIndicesCopy.Insert(crewMemberIndex);

		m_UnconsciousCrewMemberIndices = crewMemberIndicesCopy;
	}
	
	void MarkCrewMemberDead(int crewMemberIndex)
	{
		set<int> crewMemberIndicesCopy = new set<int>();
		crewMemberIndicesCopy.Copy(m_DeadCrewMemberIndices);
		crewMemberIndicesCopy.Insert(crewMemberIndex);

		m_DeadCrewMemberIndices = crewMemberIndicesCopy;
	}
	protected void HandleByCrewMemberState(ECrewMemberState state);

	vector GetTransportCameraOffset()
	{
		return "0 1.3 0";
	}
	
	int GetAnimInstance()
	{
#ifndef CFGMODS_DEFINE_TEST
		Error("GetAnimInstance() not implemented");
		return 0;
#else
		return 2;
#endif
	}

	int GetSeatAnimationType( int posIdx )
	{
#ifndef CFGMODS_DEFINE_TEST
		Error("GetSeatAnimationType() not implemented");
#endif
		return 0;
	}

	int Get3rdPersonCameraType()
	{
#ifndef CFGMODS_DEFINE_TEST
		Error("Get3rdPersonCameraType() not implemented");
		return 0;
#else
		return 31;
#endif
	}
	
	bool CrewCanGetThrough( int posIdx )
	{
#ifndef CFGMODS_DEFINE_TEST
		return false;
#else
		return true;
#endif
	}
	
	bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
#ifndef CFGMODS_DEFINE_TEST
		return false;
#else
		return true;
#endif
	}
	
	bool CanReachSeatFromDoors( string pSeatSelection, vector pFromPos, float pDistance = 1.0 )
	{
#ifndef CFGMODS_DEFINE_TEST
		return false;
#else
		return true;
#endif	
	}

	bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
#ifndef CFGMODS_DEFINE_TEST
		return false;
#else
		return true;
#endif
	}
	
	int GetSeatIndexFromDoor( string pDoorSelection )
	{
		//Potientially could be fixed some other way, currently follows the unfortunate pattern that CanReachDoorsFromSeat has created
		switch (pDoorSelection)
		{
			case "DoorsDriver":
				return 0;
				break;
			case "DoorsCoDriver":
				return 1;
				break;
			case "DoorsCargo1":
				return 2;
				break;
			case "DoorsCargo2":
				return 3;
				break;
		}
		return -1;
	}
	
	bool IsIgnoredObject( Object o )
	{
		if (!o)
			return false;

		//! If the player can't interact (hint: collide) with the object, then it is ignored
		int layer = dBodyGetInteractionLayer(o);
		bool interacts = dGetInteractionLayer(this, PhxInteractionLayers.CHARACTER, layer);
		if (!interacts)
		{
			return true;
		}

		DayZPlayer player;
		if (Class.CastTo(player, o))
		{
			//! Ignore any player that is currently in this vehicle
			HumanCommandVehicle hcv = player.GetCommand_Vehicle();
			if (hcv && hcv.GetTransport() == this)
			{
				return true;
			}
		}

		EntityAI e = EntityAI.Cast(o);			
		// CanBeSkinned means it is a dead entity which should not block the door
		return ( ( e && (e.IsZombie() || e.IsHologram()) ) || o.CanBeSkinned() || o.IsBush() || o.IsTree() );
	}
	
	void SetEngineZoneReceivedHit(bool pState)
	{
		m_EngineZoneReceivedHit = pState;
		SetSynchDirty();
	}
	
	bool HasEngineZoneReceivedHit()
	{
		return m_EngineZoneReceivedHit;
	}
	
	bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight, inout vector extents, out vector transform[4] )
	{
		GetTransform(transform);
		
		vector crewPos;
		vector crewDir;
		CrewEntry( currentSeat, crewPos, crewDir );
		
		vector entry[4];
		entry[2] = crewDir;
		entry[0] = vector.Up * crewDir;
		entry[1] = entry[2] * entry[0];
		entry[3] = crewPos;
		
		Math3D.MatrixMultiply4( transform, entry, transform );
		
		vector position = transform[3];
		vector orientation = Math3D.MatrixToAngles(transform);
		
		position[1] = position[1] + maxAllowedObjHeight + (extents[1] * 0.5);
		
		array<Object> excluded = new array<Object>;
		array<Object> collided = new array<Object>;
		
		excluded.Insert(this);
		
		GetGame().IsBoxColliding(position, orientation, extents, excluded, collided);
		
		orientation.RotationMatrixFromAngles(transform);
		transform[3] = position;
		
		foreach (Object o : collided)
		{
			EntityAI e = EntityAI.Cast(o);			
			if (IsIgnoredObject(o))
				continue;
			
			vector minmax[2];
			if (o.GetCollisionBox(minmax))
				return false;
		}

		return true;
	}

	bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		vector transform[4];
		vector extents;
		
		extents[0] = horizontalExtents;
		extents[1] = playerHeight;
		extents[2] = horizontalExtents;
		
		return IsAreaAtDoorFree( currentSeat, maxAllowedObjHeight, extents, transform );
	}
	
	Shape DebugFreeAreaAtDoor( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		int color = ARGB(20, 0, 255, 0);
		
		vector transform[4];
		vector extents;
		
		extents[0] = horizontalExtents;
		extents[1] = playerHeight;
		extents[2] = horizontalExtents;
		
		if (!IsAreaAtDoorFree( currentSeat, maxAllowedObjHeight, extents, transform ))
		{
			color = ARGB(20, 255, 0, 0);
		}
		
		Shape shape = Debug.DrawBox(-extents * 0.5, extents * 0.5, color);
		shape.SetMatrix(transform);
		return shape;
	}
};

class VehicleContactData
{
	vector m_LocalPos;
	IEntity m_Other;
	float m_Impulse;
	
	void SetData(vector localPos, IEntity other, float impulse)
	{
		m_LocalPos	= localPos;
		m_Other		= other;
		m_Impulse	= impulse;
	}
};

class VehicleFlippedContext
{
	bool m_bIsAction = false;
	Man m_ActionPlayer;
	
	ref SurfaceDetectionParameters m_SurfaceParams = new SurfaceDetectionParameters();
	ref SurfaceDetectionResult m_SurfaceResult = new SurfaceDetectionResult();
	
#ifdef DIAG_DEVELOPER	
	private ref array<Shape> m_DebugShapes;
	private bool m_bIsDebug;

	void VehicleFlippedContext()
	{
		m_DebugShapes = new array<Shape>();
	}
	
	void ~VehicleFlippedContext()
	{
		Reset();
	}
	
	void Reset(bool isDebug = false)
	{
		foreach (Shape shape : m_DebugShapes)
		{
			shape.Destroy();
		}
		
		m_DebugShapes.Clear();
		
		m_bIsDebug = isDebug;
	}
	
	void AddShape(Shape shape)
	{
		m_DebugShapes.Insert(shape);
	}
	
	bool IsDebug()
	{
		return m_bIsDebug;
	}
#endif
};
