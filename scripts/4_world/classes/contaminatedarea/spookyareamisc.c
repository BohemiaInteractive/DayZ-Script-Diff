//-------------------------------------------------------------
class SpookyEventWind : SpookyEventBase
{
	override protected void Init()
	{
		SetCoolDown(65);
		m_SoundSet = "SpookyArea_WhistlingWind_SoundSet";
	}
	
	override protected bool CanDo(PlayerBase player, TStringVectorMap surfaceTypes)
	{
		return player.IsSoundInsideBuilding();
	}
	
	override protected void Do(PlayerBase player)
	{
		//put additional code here
	}
}
//-------------------------------------------------------------
class SpookyEventWhisper : SpookyEventBase
{
	override protected void Init()
	{
		SetCoolDown(80);
		m_SoundSet = "SpookyArea_Whispering_SoundSet";
	}
	
	override protected void Do(PlayerBase player)
	{
		//put additional code here
	}

}
//-------------------------------------------------------------
class SpookyEventSteps : SpookyEventBase
{
	override protected void Init()
	{
		SetCoolDown(40);
		m_SoundSet = "SpookyArea_RunOnConcrete_SoundSet";
		m_Surfaces = {"stone", "gravel", "concrete", "wood", "asphalt", "tiles", "textile"};
	}
	
	override protected void Do(PlayerBase player)
	{
		//put additional code here
	}
}
//-------------------------------------------------------------
class SpookyEventRustle : SpookyEventBase
{
	override protected void Init()
	{
		SetCoolDown(60);
		m_SoundSet = "SpookyArea_IntenseFoliageRustle_SoundSet";
		m_Surfaces = {"grass", "dirt", "forest", "soil"};
	}
	
	override protected bool CanDo(PlayerBase player, TStringVectorMap surfaceTypes)
	{
		return !player.IsSoundInsideBuilding();
	}
	
	override protected void Do(PlayerBase player)
	{
		vector soundPos = GetSoundPos(player);
		
		string secondarySoundSet = "SpookyArea_Hedgehog_SoundSet";
		
		if (Math.RandomBool())
		{
			secondarySoundSet = "SpookyArea_Badger_Voice_SoundSet";
		}

		EffectSound sound =	SEffectManager.PlaySound(secondarySoundSet, soundPos);
		sound.SetAutodestroy( true );
	}
}


//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------

class SpookyEventBase
{
	//internal params, do not set manually
	protected float 			m_PerformedTimestamp;//internal, marks the time this event was last performed so that it can guruantee the cooldown period before playing it again
	protected int 				m_Cooldown;//how much time needs to elapse between this event can be performed again in seconds
	//the params bellow can be set in the 'Init' method
	protected string 			m_SoundSet;//if set, will play this soundset when performing the event
	protected ref TStringArray 	m_Surfaces;//if set, the player needs to detect this surface for the event to be considered valid
	protected vector			m_MatchingSurfacePos;//position of the first matching surface
	
	void SpookyEventBase()
	{
		Init();
	}

	protected void Init();
	
	
	protected vector GetMatchingSurfacePos(TStringArray surfaces, TStringVectorMap gatheredSurfaces)
	{
		for (int i = 0; i < gatheredSurfaces.Count(); i++)
		{
			string currSurface = gatheredSurfaces.GetKey(i);
			foreach (string s:surfaces)
			{
				if (currSurface.Contains(s))
					return gatheredSurfaces.Get(currSurface);
			}
		}
		
		return vector.Zero;
	}
	
	protected void SetCoolDown(float secs)
	{
		m_Cooldown = secs * 1000;
	}
	
	protected bool HasSurfaces()
	{
		return (m_Surfaces && m_Surfaces.Count() > 0);
	}
	
	protected bool CanDo(PlayerBase player, TStringVectorMap surfaceTypes)
	{
		return true;
	}
	
	protected void Do(PlayerBase player);
	
	//internal, do not override, use 'CanDo' instead
	bool CanPerform(PlayerBase player, float currentTime, TStringVectorMap surfaceTypes)
	{
		bool surfaceCheckResult = 1;
		
		if (HasSurfaces())
		{
			m_MatchingSurfacePos = GetMatchingSurfacePos(m_Surfaces, surfaceTypes);
			surfaceCheckResult =  m_MatchingSurfacePos != vector.Zero;
		}
		
			
		return ( currentTime > (m_PerformedTimestamp + m_Cooldown) && surfaceCheckResult && CanDo(player, surfaceTypes) );
	}
	
	//internal, do not override, use 'Do' instead
	void Perform(PlayerBase player, float currentTime, TStringVectorMap gatheredSurfaces)
	{
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.TRIGGER_DEBUG))
			Print("Performing " + this);
		#endif
		m_PerformedTimestamp = currentTime;
		
		if (m_SoundSet)
		{
			vector soundPos = GetSoundPos(player);
			EffectSound sound =	SEffectManager.PlaySound(m_SoundSet, soundPos);
			sound.SetAutodestroy( true );
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.TRIGGER_DEBUG))
				Debug.DrawSphere(soundPos , 0.15,Colors.YELLOW, ShapeFlags.NOZBUFFER);
			#endif
		}
			
		Do(player);
	}

	protected vector GetSoundPos(PlayerBase player)
	{
		vector soundPos;
		
		if (HasSurfaces())
		{
			soundPos = m_MatchingSurfacePos;
		}
		else
		{
			float distance 		= Math.RandomFloatInclusive(5,15);
			vector randomDir 	= vector.RandomDir2D() * distance;
			vector playerPos 	= player.GetPosition();
			soundPos 			= playerPos + randomDir;
		}
		
		return soundPos;
	}
}

//----------------------------------------------------------------------------------------------------------

class SpookyTriggerEventsHandler
{
	protected ref array<ref SpookyEventBase> 	m_SoundEvents;
	protected PlayerBase						m_Player;
	protected float 							m_TimeAccu;
	protected const float 						CONSECUTIVE_EVENTS_COOLDOWN = 20;//min delay in seconds before two events
	protected const float 						EVENT_CHECK_FREQUENCY = 2;//when not in cooldown, the rate at which we query the events
	protected const float						FIRST_EVENT_CHECK_DELAY = 15;//the delay between the first event check when we first enter the contaminated area
	protected const float 						SURFACE_CHECK_POINT_DISTANCE = 2;//additional checks for surface are performed at this distance from the player
	protected float 							m_NextEventCheck = FIRST_EVENT_CHECK_DELAY;
	
	void SpookyTriggerEventsHandler(notnull PlayerBase player)
	{
		if (!m_SoundEvents)
		{
			m_SoundEvents = new array<ref SpookyEventBase>();
			RegisterEvents();
		}
		
		m_Player = player;
	}
	
	void ~SpookyTriggerEventsHandler()
	{
		m_SoundEvents = null;
	}
	
	protected void RegisterEvents()
	{
		m_SoundEvents.Insert(new SpookyEventWind());
		m_SoundEvents.Insert(new SpookyEventWhisper());
		m_SoundEvents.Insert(new SpookyEventSteps());
		m_SoundEvents.Insert(new SpookyEventRustle());

	}
	
	void Update(float deltaTime)
	{
		m_TimeAccu += deltaTime;
		if (m_TimeAccu > m_NextEventCheck)
		{
			if (SelectEvent())
				m_NextEventCheck = CONSECUTIVE_EVENTS_COOLDOWN;
			else
				m_NextEventCheck = EVENT_CHECK_FREQUENCY;
			m_TimeAccu = 0;
		}
	}
	
	protected void GatherSurfaces(notnull TStringVectorMap gatheredGurfaces)
	{
		gatheredGurfaces.Clear();
		
		vector playerPos = m_Player.GetPosition();
		TVectorArray positions = new TVectorArray();
		
		positions.Insert(playerPos);
		positions.Insert(playerPos + ("0 0 1" * SURFACE_CHECK_POINT_DISTANCE));
		positions.Insert(playerPos + ("0 0 -1" * SURFACE_CHECK_POINT_DISTANCE));
		positions.Insert(playerPos + ("1 0 0" * SURFACE_CHECK_POINT_DISTANCE));
		positions.Insert(playerPos + ("-1 0 0" * SURFACE_CHECK_POINT_DISTANCE));
		
		foreach (vector pos : positions)
		{
			string surfaceType;
			g_Game.SurfaceGetType3D(pos[0],pos[1], pos[2], surfaceType);
			
			if (!gatheredGurfaces.Contains(surfaceType))
				gatheredGurfaces.Insert(surfaceType, pos);
		}
		
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.TRIGGER_DEBUG))
		{
			foreach (vector p:positions)
				Debug.DrawLine(p, p + "0 10 0", COLOR_BLUE,ShapeFlags.NOZBUFFER);
		}
		#endif
		
	}
	
	protected bool SelectEvent()
	{
		TStringVectorMap gatheredSurfaces = new TStringVectorMap();
		GatherSurfaces(gatheredSurfaces);//this can be optimized by calling this on demand from an event, as none events might be eligible at this point, so we might be doing this in vain
		
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.TRIGGER_DEBUG))
		{
			for(int i = 0; i < gatheredSurfaces.Count(); i++)
			{
				Print(gatheredSurfaces.GetKey(i));
				Print(gatheredSurfaces.Get(gatheredSurfaces.GetKey(i)));
				
			}
			Print("--------------------------------------------------------------------");
		}
		#endif
		
		array<ref SpookyEventBase> validEvents = new array<ref SpookyEventBase>();
		float currentTime = g_Game.GetTime();
		foreach (SpookyEventBase spookyEvent:m_SoundEvents)
		{
			if (spookyEvent.CanPerform(m_Player, currentTime, gatheredSurfaces))
				validEvents.Insert(spookyEvent);
		}

		//validEvents.Debug();
		SpookyEventBase selectedEvent;
		
		if (validEvents.Count() > 0)
		{
			int randIndex = Math.RandomIntInclusive(0, validEvents.Count() - 1);
			selectedEvent = validEvents[randIndex];
		}
		if (selectedEvent)
		{
			selectedEvent.Perform(m_Player, currentTime, gatheredSurfaces);
			return true;
		}
		return false;
		
	}

}

//!this entity gets attached to each player while present in the spooky area
class SpookyPlayerStalker : ScriptedEntity
{
	
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLConstant;
	

	override void EEInit()
	{		
		super.EEInit();
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
 			m_UTSSettings 							= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_Updateable				= true;
			m_UTSSettings.m_UpdateInterval			= 3;
			m_UTSSettings.m_TemperatureItemCap		= -20;
			m_UTSSettings.m_TemperatureCap			= -20;
			m_UTSSettings.m_RangeFull				= 2;
			m_UTSSettings.m_RangeMax				= 2;
			m_UTSSettings.m_ManualUpdate			= false;
			m_UTSSettings.m_IsWorldOverriden		= false;
			
			m_UTSLConstant							= new UniversalTemperatureSourceLambdaConstant();
			m_UTSource								= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLConstant);
			m_UTSource.SetActive(true);
		}

	}
	
}
