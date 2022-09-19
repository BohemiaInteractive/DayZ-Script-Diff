// Mostly used for better readability
enum eZoneType
{
	STATIC = 1,
	DYNAMIC = 2
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
	vector 					m_Position; 							// World position of Area
	
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
	
	// Other values and storage
	string 					m_TriggerType = "ContaminatedTrigger"; 	// The trigger class used by this zone
	CylinderTrigger			m_Trigger; 								// The trigger used to determine if player is inside toxic area

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
		
		// We sync our data
		SetSynchDirty();
		
		// Now that everything is ready, we finalize setup
		InitZone();
	}

	void Tick() {};
	
		
	// Through this we will evaluate the resize of particles
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		Tick();
	}
	
	void InitZone()
	{
		//Debug.Log("------------------------------------------");
		//Debug.Log( "We have created the zone : " + m_Name );
		
		m_Position = GetWorldPosition();
		
		if ( !GetGame().IsDedicatedServer() )
		{
			InitZoneClient();
		}
		
		if ( GetGame().IsServer() )
		{
			InitZoneServer();
		}
		
		//Debug.Log("------------------------------------------");
	}
	
	// The following methods are to be overriden to execute specifc logic
	// Each method is executed where it says it will so no need to check for server or client ;) 
	void InitZoneServer() {};
	
	void InitZoneClient() {};
	
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
#ifdef NO_GUI
		return; // do not place any particles if there is no GUI
#endif
		if (partId == 0)
		{
			Error("[WARNING] :: [EffectArea PlaceParticles] :: no particle defined, skipping area particle generation" );
			return;
		}
		// Determine if we snap first layer to ground
		bool snapFirstLayer = true; 
		if ( m_Type == eZoneType.STATIC && pos[1] != GetGame().SurfaceRoadY( pos[0], pos[2] ) )
			snapFirstLayer = false;
		
		// BEGINNING OF SAFETY NET
		// We want to prevent divisions by 0
		if ( radius == 0 )
		{
			// In specific case of radius, we log an error and return as it makes no sense
			Error("[WARNING] :: [EffectArea PlaceParticles] :: Radius of contaminated zone is set to 0, this should not happen");
			return;
		}
		
		if ( outerToggle && radius == outerOffset )
		{
			Error("[WARNING] :: [EffectArea PlaceParticles] :: Your outerOffset is EQUAL to your Radius, this will result in division by 0");
			return;
		}
		
		// Inner spacing of 0 would cause infinite loops as no increment would happen
		if ( innerSpacing == 0 )
			innerSpacing = 1;
		
		// END OF SAFETY NET
		
		int partCounter = 0; // Used for debugging, allows one to know how many emitters are spawned in zone
		int numberOfEmitters = 1; // We always have the central emitter
		
		//Debug.Log("We have : " + nbRings + " rings");
		//Debug.Log("We have : " + m_VerticalLayers + " layers");
		
		float angle = 0; // Used in for loop to know where we are in terms of angle spacing ( RADIANS )
		
		ParticlePropertiesArray props = new ParticlePropertiesArray();
		
		// We also populate vertically, layer 0 will be snapped to ground, subsequent layers will see particles floating and relevant m_VerticalOffset
		for ( int k = 0; k <= m_VerticalLayers; k++ )
		{
			vector partPos = pos;
			// We prevent division by 0
			// We don't want to tamper with ground layer vertical positioning
			if ( k != 0 )
			{
				partPos[1] = partPos[1] + ( m_VerticalOffset * k );
			}
			
			// We will want to start by placing a particle at center of area
			props.Insert(ParticleProperties(partPos, ParticlePropertiesFlags.PLAY_ON_CREATION, null, vector.Zero, this));
			partCounter++;
			
			// For each concentric ring, we place a particle emitter at a set offset
			for ( int i = 1; i <= nbRings + outerToggle; i++ )
			{
				//Debug.Log("We are on iteration I : " + i );
				
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
					// Use accumulated angle for vector direction
					
					float sinAngle = Math.Sin( angle );
					float cosAngle = Math.Cos( angle );
				
					partPos = vector.RotateAroundZero( temp, vector.Up, cosAngle, sinAngle );
					partPos += pos;
					
					// We snap first layer to ground if specified
					if ( k == 0 && snapFirstLayer == true )
						partPos[1] = GetGame().SurfaceY( partPos[0], partPos[2] );
					else if ( k == 0 && snapFirstLayer == false )
						partPos[1] = partPos[1] - m_NegativeHeight;
					
					// We check the particle is indeed in the trigger to make it consistent
					if ( partPos[1] <= pos[1] + m_PositiveHeight && partPos[1] >= pos[1] - m_NegativeHeight )
					{
						// Place emitter at vector end ( coord )
						props.Insert(ParticleProperties(partPos, ParticlePropertiesFlags.PLAY_ON_CREATION, null, GetGame().GetSurfaceOrientation( partPos[0], partPos[2] ), this));
						
						++partCounter;
					}

					// Increase accumulated angle
					angle += angleIncrement;
				}
				
				angle = 0; // We reset our accumulated angle for the next ring
			}
		}
		
		m_ToxicClouds.Reserve(partCounter);
		
		ParticleManager gPM = ParticleManager.GetInstance();
		
		array<ParticleSource> createdParticles = gPM.CreateParticlesByIdArr(partId, props, partCounter);
		if (createdParticles.Count() != partCounter)
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
		
		//Debug.Log("Emitter count : " + partCounter );
	}
	
	void OnParticleAllocation(ParticleManager pm, array<ParticleSource> particles)
	{
		foreach (ParticleSource p : particles)
		{
			if (p.GetOwner() == this) // Safety, since it can be unrelated particles when done through event
				m_ToxicClouds.Insert(p);
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
	
	void CreateTrigger( vector pos, int radius )
	{
		// The trigger pos is based on lwer end, but we want to stretch downwards
		pos[1] = pos[1] - m_NegativeHeight;
		
		// Create new trigger of specified type
		if ( Class.CastTo( m_Trigger, GetGame().CreateObjectEx( m_TriggerType, pos, ECE_NONE ) ) )
		{
			// We finalize trigger dimension setup
			m_Trigger.SetCollisionCylinder( radius, ( m_NegativeHeight + m_PositiveHeight ) );
			
			// If the trigger is lower in hierarchy and can see it's local effects customized, we pass the new parameters
			if ( m_Trigger.IsInherited( EffectTrigger ) )
			{
				//Debug.Log("We override area local effects");
				EffectTrigger.Cast( m_Trigger ).SetLocalEffects( m_AroundParticleID, m_TinyParticleID, m_PPERequesterIdx );
			}
			
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
			GetGame().ObjectDelete( m_Trigger );
		}
		
		// We stop playing particles on this client when the base object is deleted ( out of range for example )
		if ( (GetGame().IsClient() || !GetGame().IsMultiplayer())  && m_ToxicClouds )
		{
			foreach ( Particle p : m_ToxicClouds )
			{
				p.Stop();
			}
		}
		
		super.EEDelete( parent );
	}
}