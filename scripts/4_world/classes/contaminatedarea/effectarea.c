// #define EFFECT_AREA_VISUAL_DEBUG

// Mostly used for better readability
enum eZoneType
{
	STATIC = 1,
	DYNAMIC = 2
}

enum EEffectAreaType
{
	UNDEFINED	= 1,
	HOT_SPRING 	= 2,
	VOLCANIC 	= 4
}

// Class used for parameter transfer, all of the params are shadows of EffectArea member variables
class EffectAreaParams
{
	string 	m_ParamName 				= "Default setup";
	string 	m_ParamTriggerType 			= "ContaminatedTrigger";
	float 	m_ParamRadius 				= 100;
	float 	m_ParamPosHeight 			= 25;
	float 	m_ParamNegHeight 			= 10;
	int 	m_ParamInnerRings 			= 1;
	int 	m_ParamInnerSpace 			= 35;
	bool 	m_ParamOuterToggle 			= true;
	int 	m_ParamOuterSpace 			= 20;
	int 	m_ParamOuterOffset 			= -5;
	int 	m_ParamVertLayers 			= 0;
	int 	m_ParamVerticalOffset 		= 10;
	int		m_ParamEffectInterval		= 0;
	int		m_ParamEffectDuration		= 0;
	int		m_ParamEffectModifier		= 0;
	/*
	int 	m_ParamPartId 				= ParticleList.CONTAMINATED_AREA_GAS_BIGASS;
	int 	m_ParamAroundPartId 		= ParticleList.CONTAMINATED_AREA_GAS_AROUND;
	int 	m_ParamTinyPartId 			= ParticleList.CONTAMINATED_AREA_GAS_TINY;
	*/
	int 	m_ParamPartId 				= 0;
	int 	m_ParamAroundPartId 		= 0;
	int 	m_ParamTinyPartId 			= 0;
	
	string 	m_ParamPpeRequesterType 	= "PPERequester_ContaminatedAreaTint";
}

// Base class for contaminated and other "Effect" areas
class EffectArea : House
{
	// Area Data
	string					m_Name = "Default setup"; 				// The user defined name of the area
	int						m_Type = eZoneType.STATIC; 				// If the zone is static or dynamic
	vector 					m_Position; 							// World position of area snapped to ground on creation (see: EffectAreaLoader)
	vector 					m_PositionTrigger; 						// World position adjusted according to trigger pivot (pivot is cylinder center)
	int 					m_EffectInterval;						// If non persisent effect: determines intervals between effect activation
	int						m_EffectDuration;						// If non persisent effect: determines duration of effect
	bool 					m_EffectModifier;						// Flag for modification of internal behavior of the effect
	
	// Trigger Data
	float					m_Radius = 100; 						// Radius of the Contaminated Area
	float					m_PositiveHeight = 25; 					// Distance between center and maximum height
	float					m_NegativeHeight = 10; 					// Distance between center and minimum height
	
	// PCG parameters
	// Inner Particle data
	int						m_InnerRings = 1; 						// The amount of inner rings one wants
	int						m_InnerSpacing = 35; 					// Distance between two particles placed on inner rings of the area
	// Outer particle data
	bool					m_OuterRingToggle = true; 				// Allow disabling outer ring if undesired
	int						m_OuterRingOffset = -5; 				// Distance between the outermost ring of particles and area radius
	int						m_OuterSpacing = 20; 					// Distance between two particles placed on the outer most ring of the area
	// Verticality handling
	int						m_VerticalLayers = 0; 					// Used to add multiple layers vertically and set vertical spacing ( 0 don't do anything )
	int						m_VerticalOffset = 10; 					// Used to determine vertical offset between two vertical layers
	
	// Particles and visual effects
	int						m_ParticleID = ParticleList.CONTAMINATED_AREA_GAS_BIGASS;
	int						m_AroundParticleID = ParticleList.CONTAMINATED_AREA_GAS_AROUND;
	int						m_TinyParticleID = ParticleList.CONTAMINATED_AREA_GAS_TINY;
	string 					m_PPERequesterType;
	int 					m_PPERequesterIdx = -1;
	int						m_EffectsPriority;						// When multiple areas overlap, only the area with the highest priority will play its effects
	
	const int 				PARTICLES_MAX = 1000;					// Better safe than sorry
	
	// Other values and storage
	string 					m_TriggerType = "ContaminatedTrigger"; 	// The trigger class used by this zone
	EffectTrigger			m_Trigger; 								// The trigger used to determine if player is inside toxic area

	ref array<Particle> 	m_ToxicClouds; 							// All static toxic clouds in ContaminatedArea

	// ----------------------------------------------
	// 				INITIAL SETUP
	// ----------------------------------------------
	
	void EffectArea()
	{
		RegisterNetSyncVariableFloat("m_Radius", 0, 0, 2);
		RegisterNetSyncVariableFloat("m_PositiveHeight", 0, 0, 2);
		RegisterNetSyncVariableFloat("m_NegativeHeight", 0, 0, 2);
		
		RegisterNetSyncVariableInt("m_InnerRings");
		RegisterNetSyncVariableInt("m_InnerSpacing");
		RegisterNetSyncVariableInt("m_OuterRingOffset");
		RegisterNetSyncVariableInt("m_OuterSpacing");
		RegisterNetSyncVariableInt("m_VerticalLayers");
		RegisterNetSyncVariableInt("m_VerticalOffset");
		
		RegisterNetSyncVariableInt("m_ParticleID");
		/*
		RegisterNetSyncVariableInt("m_AroundParticleID");
		RegisterNetSyncVariableInt("m_TinyParticleID");
		RegisterNetSyncVariableInt("m_PPERequesterIdx");
		*/
		RegisterNetSyncVariableBool("m_OuterRingToggle");
	}
	
	void ~EffectArea()
	{
		
	}

	void SetupZoneData( EffectAreaParams params ) 
	{
		// A lot of branching, allowing to use default values on specified params
		if ( params.m_ParamName != "" )
			m_Name = params.m_ParamName;
		if ( params.m_ParamTriggerType != "" )
			m_TriggerType = params.m_ParamTriggerType;
		
		if ( params.m_ParamRadius > 0 )
			m_Radius = params.m_ParamRadius;
		if ( params.m_ParamPosHeight > -1 )
			m_PositiveHeight = params.m_ParamPosHeight;
		if ( params.m_ParamNegHeight > -1 )
			m_NegativeHeight = params.m_ParamNegHeight;
		
		m_InnerRings = params.m_ParamInnerRings;
		if ( params.m_ParamInnerSpace > -1 )
			m_InnerSpacing = params.m_ParamInnerSpace;
		
		m_OuterRingToggle = params.m_ParamOuterToggle;
		if ( params.m_ParamOuterSpace > -1 )
			m_OuterSpacing = params.m_ParamOuterSpace;
		m_OuterRingOffset = params.m_ParamOuterOffset;
		
		if ( params.m_ParamVertLayers > 0 )
			m_VerticalLayers = params.m_ParamVertLayers;
		if ( params.m_ParamVerticalOffset > 0 )
			m_VerticalOffset = params.m_ParamVerticalOffset;
		
		m_EffectInterval = params.m_ParamEffectInterval;
		m_EffectDuration = params.m_ParamEffectDuration;
		m_EffectModifier = params.m_ParamEffectModifier;
		
		m_ParticleID = params.m_ParamPartId;
		m_AroundParticleID = params.m_ParamAroundPartId;
		m_TinyParticleID = params.m_ParamTinyPartId;
		
		if ( params.m_ParamPpeRequesterType != "" )
		{
			m_PPERequesterType = params.m_ParamPpeRequesterType;
			m_PPERequesterIdx = GetRequesterIndex(m_PPERequesterType);			
		}
		// We get the PPE index for future usage and synchronization
		
		// DEVELOPER NOTE :
		// If you cannot register a new requester, add your own indexation and lookup methods to get an index and synchronize it
		// EXAMPLE : m_PPERequesterIdx = MyLookupMethod()
		
		#ifdef ENABLE_LOGGING
		Debug.Log(">>>> SetupZoneData: Finished: " + m_Name);
		#endif
		
		// We sync our data
		SetSynchDirty();
	}

	void Tick()
	{
		#ifdef DIAG_DEVELOPER
		#ifdef EFFECT_AREA_VISUAL_DEBUG
		CleanupDebugShapes(m_DebugTargets);
		#endif
		#endif
	}
	
		
	// Through this we will evaluate the resize of particles
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		Tick();
	}
	
	void InitZone()
	{
	//	Debug.Log("------------------------------------------");
	//	Debug.Log("InitZone: " + m_Name);
		
		m_Position 				= GetPosition();
		m_PositionTrigger		= m_Position;
		m_PositionTrigger[1] 	= m_Position[1] + ((m_PositiveHeight - m_NegativeHeight) * 0.5); // Cylinder trigger pivot correction
		
		if (!g_Game.IsDedicatedServer())
			InitZoneClient();
		
		if (g_Game.IsServer())
			InitZoneServer();
		
	//	Debug.Log("------------------------------------------");
	}
	
	// The following methods are to be overriden to execute specifc logic
	void InitZoneServer();
	void InitZoneClient();
	
	// ----------------------------------------------
	// 				INTERACTION SETUP
	// ----------------------------------------------
	
	override bool CanPutInCargo( EntityAI parent )
	{
		return false;
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}
	
	override bool DisableVicinityIcon()
    {
        return true;
    }
	
	override bool CanBeTargetedByAI( EntityAI ai )
	{
		return false;
	}
	
	// ----------------------------------------------
	// 				PARTICLE GENERATION
	// ----------------------------------------------
	// Used to position all particles procedurally
	void PlaceParticles( vector pos, float radius, int nbRings, int innerSpacing, bool outerToggle, int outerSpacing, int outerOffset, int partId )
	{
	//	Debug.Log("PlaceParticles: " + pos);
		
	#ifdef NO_GUI
		return; // do not place any particles if there is no GUI
	#endif	
		if (partId == 0)
		{
			Error("[WARNING] :: [EffectArea PlaceParticles] :: no particle defined, skipping area particle generation" );
			return;
		}
		if ( radius == 0 )
		{
			Error("[WARNING] :: [EffectArea PlaceParticles] :: Radius of contaminated zone is set to 0, this should not happen");
			return;
		}
		if ( outerToggle && radius == outerOffset )
		{
			Error("[WARNING] :: [EffectArea PlaceParticles] :: Your outerOffset is EQUAL to your Radius, this will result in division by 0");
			return;
		}
		
		int partCount = 0;	// Number of spawned emitters
		
		ParticlePropertiesArray props = new ParticlePropertiesArray();
		
		// Inner spacing of 0 would cause infinite loops as no increment would happen
		if (innerSpacing == 0)
			innerSpacing = 1;
			
		// For each concentric ring, we place a particle emitter at a set offset
		for ( int i = 0; i <= nbRings + outerToggle; ++i )
		{
			if (i == 0)						// Skipping 0, we want to start by placing a particle at center of area
			{
				SpawnParticles(props, pos, pos, partCount);
			}	
			else
			{
				// We prepare the variables to use later in calculation
				float angleIncrement; 		// The value added to the offset angle to place following particle
				float ab; 					// Length of a side of triangle used to calculate particle positionning
				vector temp = vector.Zero; 	// Vector we rotate to position next spawn point
				
				// The particle density is not the same on the final ring which will only happen if toggled
				// Toggle uses bool parameter treated as int, thus i > nbRings test ( allows to limit branching )
				if ( i > nbRings )
				{
					ab = radius - outerOffset; // We want to leave some space to better see area demarcation
					
					// We calculate the rotation angle depending on particle spacing and distance from center
					angleIncrement = Math.Acos( 1 - ( ( outerSpacing * outerSpacing ) / ( 2 * Math.SqrInt(ab) ) ) );
					temp[2] = temp[2] + ab;
					
					//Debug.Log("Radius of last circle " + i + " is : " + ab);
				}
				else
				{
					ab = ( radius / ( nbRings + 1 ) ) * i; // We add the offset from one ring to another
					
					// We calculate the rotation angle depending on particle spacing and distance from center
					angleIncrement = Math.Acos( 1 - ( ( innerSpacing * innerSpacing ) / ( 2 * Math.SqrInt(ab) ) ) );
					temp[2] = temp[2] + ab;
					
					//Debug.Log("Radius of inner circle " + i + " is : " + ab);
				}

				for ( int j = 0; j <= ( Math.PI2 / angleIncrement ); j++ )
				{
					// Determine position of particle emitter
					// Use offset of current ring for vector length

					float sinAngle 	= Math.Sin(angleIncrement * j);
					float cosAngle 	= Math.Cos(angleIncrement * j);

					vector partPos = vector.RotateAroundZero( temp, vector.Up, cosAngle, sinAngle );
					partPos += pos;

					SpawnParticles(props, pos, partPos, partCount);
				}
			}			
		}
		
		InsertParticles(props, partCount, partId);
	}
	
	// Fill the radius with particle emitters using the Circle packing in a circle method
	void FillWithParticles(vector pos, float areaRadius, float outwardsBleed, float partSize, int partId)
	{
	//	Debug.Log("FillWithParticles: " + pos);
		
	#ifdef NO_GUI
		return; // do not place any particles if there is no GUI
	#endif
		if (partId == 0)
			return;
 
		if (partSize <= 0)
			partSize = 1;
		
		int 	partCount 	= 0;								// Number of spawned emitters
		int 	ringCount	= 0;								// Number of area rings
		float	ringDist 	= 0;								// Distance between rings

		float 	radiusMax	= areaRadius + outwardsBleed; 		// Visual radius of the area
		float	radiusPart	= partSize / 2;						// Particle radius
				
		bool 	centerPart	= true;								// Spawn central particle?
		
		ParticlePropertiesArray props = new ParticlePropertiesArray();	

		// Debug.Log("Area radius: " + radiusMax + "m, Particle radius: " + radiusPart + "m");
		
		if (radiusMax > radiusPart * 1.5)						// Area has to be larger than particle, plus some margin
		{
			if (radiusMax < radiusPart * 2.5)					// Area fits one ring of particles, but no center particle (minus some overlap margin)
			{
				ringDist	= radiusMax - radiusPart;			// Snap the particles to outer edge
				ringCount 	= 1;
				centerPart 	= false;
			}
			else 												// Area fits all
			{
				radiusMax  -= radiusPart;						// Snap the particles to outer edge
				ringCount 	= Math.Ceil(radiusMax / partSize);	// Get number of inner rings
				ringDist 	= radiusMax / ringCount;			// Adjust ring distance after rounding
			}	
		}
							
		// Debug.Log("We have : " + ringCount + " rings, " + ringDist + "m apart, center: " + centerPart);
		// Debug.Log("We have : " + m_VerticalLayers + " layers, " + m_VerticalOffset + "m apart");
		
		// For each concentric ring, we place a particle emitter at a set offset
		for (int ring = 0; ring <= ringCount; ++ring)
		{
			if (ring == 0 && centerPart)						// We start by placing particle at center of area
			{
				SpawnParticles(props, pos, pos, partCount);
			}	
			else if (ring > 0)
			{
				float ringRadius = ringDist * ring;
				float circumference = 2 * Math.PI2 * ringRadius;
				
				int count = Math.Floor(circumference / partSize);				// Get number of particles on ring (roughly)
				float angleInc = Math.PI2 / count;								// Get angle between particles on ring
			
				for (int i = 0; i < count; ++i)		// Insert particles around the ring
				{
					vector partPos = pos;
					float x = ringRadius * Math.Sin(angleInc * i);
					float z = ringRadius * Math.Cos(angleInc * i);
					
					partPos[0] = partPos[0] + x;
					partPos[2] = partPos[2] + z;
					
					SpawnParticles(props, pos, partPos, partCount);
				}				
			}
		}
		
		InsertParticles(props, partCount, partId);
 	}
	
	protected void SpawnParticles(ParticlePropertiesArray props, vector centerPos, vector partPos, inout int count)
	{
		partPos[1] = g_Game.SurfaceY(partPos[0], partPos[2]);	// Snap particles to ground
		
		// We also populate vertically, layer 0 will be snapped to ground, subsequent layers will see particles floating by m_VerticalOffset
		for (int layer = 0; layer <= m_VerticalLayers; ++layer)
		{
			partPos[1] = partPos[1] + (m_VerticalOffset * layer);
		
			// We check that spawned particle is inside the trigger	
			if (count < PARTICLES_MAX && Math.IsInRange(partPos[1], centerPos[1] - m_NegativeHeight, centerPos[1] + m_PositiveHeight))
			{
				props.Insert(ParticleProperties(partPos, ParticlePropertiesFlags.PLAY_ON_CREATION, null, g_Game.GetSurfaceOrientation( partPos[0], partPos[2] ), this));
				++count;
			}
		}
	}	
	
	private void InsertParticles(ParticlePropertiesArray props, int count, int partId)
	{
		m_ToxicClouds.Reserve(count);
		
		ParticleManager gPM = ParticleManager.GetInstance();
		
		array<ParticleSource> createdParticles = gPM.CreateParticlesByIdArr(partId, props, count);
		if (createdParticles.Count() != count)
		{
			if (gPM.IsFinishedAllocating())
			{
				ErrorEx(string.Format("Not enough particles in pool for EffectArea: %1", m_Name));
				OnParticleAllocation(gPM, createdParticles);
			}
			else
			{
				gPM.GetEvents().Event_OnAllocation.Insert(OnParticleAllocation);
			}
		}
		else
		{
			OnParticleAllocation(gPM, createdParticles);
		}
		
		// Debug.Log("Emitter count: " + count);
	}	
		
	void OnParticleAllocation(ParticleManager pm, array<ParticleSource> particles)
	{
		foreach (ParticleSource p : particles)
		{
			if (p.GetOwner() == this) // Safety, since it can be unrelated particles when done through event
			{
				m_ToxicClouds.Insert(p);
			}
		}
	}
	
	int GetRequesterIndex(string type)
	{
		typename t = type.ToType();
		if (!t)
			return - 1;
		PPERequesterBase req = PPERequesterBank.GetRequester(t);
		if (req)
			return req.GetRequesterIDX();
		return -1;
	}
	
	
	// ----------------------------------------------
	// 				TRIGGER SETUP
	// ----------------------------------------------
	void CreateTrigger(vector pos, int radius)
	{
		#ifdef DIAG_DEVELOPER
		#ifdef EFFECT_AREA_VISUAL_DEBUG 
		Shape dbgShape;
		CleanupDebugShapes(m_DebugTargets);
		#endif
		#endif
		
		// Create new trigger of specified type
		if (Class.CastTo(m_Trigger, g_Game.CreateObjectEx(m_TriggerType, pos, ECE_NONE)))
		{
			// We finalize trigger dimension setup
			float centerHeightCorrection = (m_PositiveHeight - m_NegativeHeight) * 0.5;
			
			m_Trigger.SetCollisionCylinderTwoWay(radius, -(m_NegativeHeight + centerHeightCorrection), (m_PositiveHeight - centerHeightCorrection));
			m_Trigger.SetPosition(pos);
			m_Trigger.Update();
			
			#ifdef DIAG_DEVELOPER
			#ifdef EFFECT_AREA_VISUAL_DEBUG
			/*
			vector cubePos = pos;
			cubePos[0] = cubePos[0] + radius;
			cubePos[1] = cubePos[1] + (m_PositiveHeight - centerHeightCorrection);
			cubePos[2] = cubePos[2] + radius;
			m_DebugTargets.Insert(Debug.DrawCube(cubePos, 0.5, 0x1fff0000));
			*/
			
			vector colliderPosDebug = pos;
			//! upper limit
			colliderPosDebug[1] = pos[1] + (m_PositiveHeight - centerHeightCorrection);
			m_DebugTargets.Insert(Debug.DrawSphere(colliderPosDebug, 0.15, 0x1f0000ff, ShapeFlags.NOZWRITE));
			//m_DebugTargets.Insert(Debug.DrawLine(cubePos, colliderPosDebug, 0x1fff0000, ShapeFlags.NOZWRITE)); // connector
			//! center
			m_DebugTargets.Insert(Debug.DrawSphere(pos, 0.15, 0x1fff0000, ShapeFlags.NOZWRITE));
			//m_DebugTargets.Insert(Debug.DrawLine(cubePos, pos, 0x1fff0000, ShapeFlags.NOZWRITE)); // connector
			
			//! bottom limit
			colliderPosDebug[1] = pos[1] - (m_NegativeHeight + centerHeightCorrection);
			m_DebugTargets.Insert(Debug.DrawSphere(colliderPosDebug, 0.15, 0x1f00ff00, ShapeFlags.NOZWRITE));
			//m_DebugTargets.Insert(Debug.DrawLine(cubePos, colliderPosDebug, 0x1fff0000, ShapeFlags.NOZWRITE)); // connector
			
			float triggerHeight = (m_PositiveHeight + m_NegativeHeight);
			m_DebugTargets.Insert(Debug.DrawCylinder(pos, radius, triggerHeight, 0x1f0000ff, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE));
			#endif
			#endif
			
			// If the trigger is lower in hierarchy and can see it's local effects customized, we pass the new parameters
			if ( m_Trigger.IsInherited(EffectTrigger))
			{
				//Debug.Log("We override area local effects");
				EffectTrigger.Cast(m_Trigger).SetLocalEffects(m_AroundParticleID, m_TinyParticleID, m_PPERequesterIdx);
			}
			m_Trigger.Init(this, m_EffectsPriority);
			//Debug.Log("We created the trigger at : " + m_Trigger.GetWorldPosition() );
		}
	}
	
	// ----------------------------------------------
	// 				AREA DELETION
	// ----------------------------------------------
	
	override void EEDelete( EntityAI parent )
	{
		if ( m_Trigger )
		{
			g_Game.ObjectDelete( m_Trigger );
		}
		
		// We stop playing particles on this client when the base object is deleted ( out of range for example )
		if ( (g_Game.IsClient() || !g_Game.IsMultiplayer())  && m_ToxicClouds )
		{
			foreach ( Particle p : m_ToxicClouds )
			{
				p.Stop();
			}
		}
		
		super.EEDelete( parent );
	}
	
	void OnPlayerEnterServer(PlayerBase player, EffectTrigger trigger)
	{
		player.IncreaseEffectAreaCount();
	}
	void OnPlayerExitServer(PlayerBase player, EffectTrigger trigger)
	{
		player.DecreaseEffectAreaCount();
	}
	
	#ifdef DIAG_DEVELOPER
	#ifdef EFFECT_AREA_VISUAL_DEBUG
	protected ref array<Shape> m_DebugTargets = new array<Shape>();
	
	protected void CleanupDebugShapes(array<Shape> shapes)
	{
		foreach (Shape shape : shapes)
			Debug.RemoveShape(shape);

		shapes.Clear();
	}
	#endif
	#endif
}