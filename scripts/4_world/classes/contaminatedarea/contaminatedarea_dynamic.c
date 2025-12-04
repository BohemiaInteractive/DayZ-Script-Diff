// The parameters for the explosion light when creating dynamic area
class ShellLight : PointLightBase
{
	protected float m_DefaultBrightness = 10;
	protected float m_DefaultRadius = 100;
	
	void ShellLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 1.0, 0.3);
		SetDiffuseColor(1.0, 1.0, 0.3);
		SetLifetime(0.15);
		SetDisableShadowsWithinRadius(-1);
		SetCastShadow( false );
	}
}

// The dynamic Contaminated area, using it's own default settings
class ContaminatedArea_Dynamic : ContaminatedArea_DynamicBase
{
	protected vector 		m_OffsetPos; 				// This will be the position at which we spawn all future airborne FX
	protected ref Timer 	m_StartupTimer;
	protected ref Timer 	m_FXTimer;
	protected FlareLight 	m_FlareLight;
	protected ShellLight	m_ShellLight; 				// Light used upon ariborne shell detonation
	
	// Constants used for startup events
	const int 				AIRBORNE_EXPLOSION_DELAY 	= 20;
	const int 				AREA_SETUP_DELAY 			= 10;
	const float 			AIRBORNE_FX_OFFSET 			= 50;
	const float 			ARTILLERY_SHELL_SPEED		= 100; // Units per second
	
	// Item Spawning upon area creation, the 4 arrays bellow have to have the same amount of elements
	const ref array<string> 	SPAWN_ITEM_TYPE 		= {"Grenade_ChemGas"};//item classnames
	const ref array<int>		SPAWN_ITEM_COUNT 		= {Math.RandomIntInclusive(2,5)};//how many of each type
	const ref array<float> 		SPAWN_ITEM_RAD_MIN 		= {5};//min distance the item will be spawned from the area position(epicenter)
	const ref array<float> 		SPAWN_ITEM_RAD_MAX 		= {15};//max distance the item will be spawned from the area position(epicenter)
	
	override void EEOnCECreate()
	{
		// We get the PPE index for future usage and synchronization ( we must do it here for dynamic as it is not read through file )
		m_PPERequesterIdx = GetRequesterIndex(m_PPERequesterType);
		
		// If this is the first initialization, we delay it in order to play cool effects
		if (m_DecayState == eAreaDecayStage.INIT)
		{
			vector areaPos = GetPosition();
			m_OffsetPos = areaPos;
			m_OffsetPos[1] = m_OffsetPos[1] + AIRBORNE_FX_OFFSET;
			vector closestPoint = areaPos;
			
			// play artillery sound, sent to be played for everyone on server
			array<vector> artilleryPoints = GetGame().GetMission().GetWorldData().GetArtyFiringPos();
			int index = 0;
			foreach (int i, vector artilleryPoint : artilleryPoints)
			{
				int dist = 0;
				int temp = vector.DistanceSq(artilleryPoint, areaPos);
				if (temp < dist || dist == 0)
				{
					dist = temp;
					index = i;
				}
			}
			
			closestPoint = artilleryPoints.Get(index);
			
			// We calculate the delay depending on distance from firing position to simulate shell travel time
			float delay = vector.Distance(closestPoint, areaPos);
			delay = delay / ARTILLERY_SHELL_SPEED;
			delay += AIRBORNE_EXPLOSION_DELAY; // We add the base, minimum time ( no area can start before this delay )
			
			Param3<vector, vector, float> pos = new Param3<vector, vector, float>(closestPoint, areaPos, delay);
			array<ref Param> params = new array<ref Param>();
			// We send the message with this set of coords
			params.Insert(pos);
			GetGame().RPC(null, ERPCs.RPC_SOUND_ARTILLERY_SINGLE, params, true);
			
			m_FXTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
			m_FXTimer.Run(delay, this, "PlayFX");	
			
			delay += AREA_SETUP_DELAY; // We have an additional delay between shell detonation and finalization of area creation
			// setup zone
			m_StartupTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
			m_StartupTimer.Run(delay, this, "InitZone");
		}
		
		SetSynchDirty();
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		switch (m_DecayState)
		{
			case eAreaDecayStage.START:
				PlayExplosionLight();
			break;
		}
	}

	override void Tick()
	{
		if ( GetRemainingTime() < GetFinishDecayLifetime() )
		{
			// The second state of decay, further reduction of particle density and size
			SetDecayState( eAreaDecayStage.DECAY_END );
		}
		else if ( GetRemainingTime() < GetStartDecayLifetime() )
		{
			// The first state of decay, slight reduction in particle density and size
			SetDecayState( eAreaDecayStage.DECAY_START );
		}
		
	}
	
	override void SetupZoneData(EffectAreaParams params)
	{
		params.m_ParamName			= string.Format("Dynamic area (%1)", m_Position.ToString());
		params.m_ParamPartId 		= ParticleList.CONTAMINATED_AREA_GAS_BIGASS;
		params.m_ParamAroundPartId 	= ParticleList.CONTAMINATED_AREA_GAS_AROUND;
		params.m_ParamTinyPartId 	= ParticleList.CONTAMINATED_AREA_GAS_TINY;
		params.m_ParamPosHeight 	= 7;
		params.m_ParamNegHeight 	= 10;
		params.m_ParamRadius 		= 120;
		params.m_ParamInnerRings 	= 1;
		params.m_ParamInnerSpace 	= 40;
		params.m_ParamOuterSpace 	= 30;
		params.m_ParamOuterOffset 	= 0;
		params.m_ParamTriggerType 	= "ContaminatedTrigger_Dynamic";
		
		super.SetupZoneData(params);
	}
	
	override void DeferredInit()
	{
		super.DeferredInit();

		// We make sure we have the particle array
		if (!m_ToxicClouds)
			m_ToxicClouds = new array<Particle>();
		
		m_Position = GetPosition();
		m_OffsetPos = m_Position;
		m_OffsetPos[1] = m_OffsetPos[1] + AIRBORNE_FX_OFFSET;
		
		SetupZoneData(new EffectAreaParams);
		
		// If a player arrives slightly later during the creation process we check if playing the flare FX is relevant
		if (m_DecayState == eAreaDecayStage.INIT)
			PlayFlareVFX();
		
		if ( m_DecayState == eAreaDecayStage.LIVE )
			InitZone(); // If it has already been created, we simply do the normal setup, no cool effects, force the LIVE state
		
		super.DeferredInit();
	}
	
	override void InitZoneServer()
	{
		SpawnItems();

		super.InitZoneServer();
	}
	
	void SpawnItems()
	{
		//Print("---------============ Spawning items at pos:"+m_Position);
		foreach (int j, string type : SPAWN_ITEM_TYPE)
		{
			//Print("----------------------------------");
			for (int i = 0; i < SPAWN_ITEM_COUNT[j]; ++i)
			{
				vector randomDir2d = vector.RandomDir2D();
				float randomDist = Math.RandomFloatInclusive(SPAWN_ITEM_RAD_MIN[j],SPAWN_ITEM_RAD_MAX[j]);
				vector spawnPos = m_Position + (randomDir2d * randomDist);
				InventoryLocation il = new InventoryLocation();
				vector mat[4];
				Math3D.MatrixIdentity4(mat);
				mat[3] = spawnPos;
				il.SetGround(NULL, mat);
				//Print("Spawning item:"+ type + " at position:" + il.GetPos());
				GetGame().CreateObjectEx(type, il.GetPos(), ECE_PLACE_ON_SURFACE);
			}
		}
	}
	
	override void CreateTrigger(vector pos, int radius)
	{
		super.CreateTrigger(pos, radius);
		
		// This handles the specific case of dynamic triggers as some additionnal parameters are present
		ContaminatedTrigger_Dynamic dynaTrigger = ContaminatedTrigger_Dynamic.Cast( m_Trigger );
		if ( dynaTrigger )
		{
			dynaTrigger.SetLocalEffects( m_AroundParticleID, m_TinyParticleID, m_PPERequesterIdx );
			dynaTrigger.SetAreaState( m_DecayState );
		}
	}
	
	void PlayFX()
	{
		if (GetGame().IsServer())
		{
			Param1<vector> pos = new Param1<vector>(vector.Zero); 	// The value to be sent through RPC
			array<ref Param> params = new array<ref Param>(); 		// The RPC params
			
			// We send the message with this set of coords
			pos.param1 = m_OffsetPos;
			params.Insert(pos);
			GetGame().RPC(null, ERPCs.RPC_SOUND_CONTAMINATION, params, true);
			
			// We go to the next stage
			SetDecayState(eAreaDecayStage.START);
			SetSynchDirty();
		}
	}
	
	void PlayExplosionLight()
	{
		m_ShellLight = ShellLight.Cast( ScriptedLightBase.CreateLight( ShellLight, m_OffsetPos ) );
	}
	
	void PlayFlareVFX()
	{
		if ( GetGame().IsClient() || ( GetGame().IsServer() && !GetGame().IsMultiplayer() ) )
		{
			// We spawn locally the dummy object which will be used to move and manage the particle
			DynamicArea_Flare dummy = DynamicArea_Flare.Cast( GetGame().CreateObjectEx( "DynamicArea_Flare", m_OffsetPos, ECE_SETUP | ECE_LOCAL ) );
			
			// We add some light to reinforce the effect
			m_FlareLight = FlareLightContamination.Cast(ScriptedLightBase.CreateLight( FlareLightContamination, m_OffsetPos ));
		}
	}
}