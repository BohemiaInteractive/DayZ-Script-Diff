//#define DMP_DEBUG_PRINT
//#define DMP_DEBUG_SETTINGS

class DynamicMusicLocationTypes
{
	const int NONE = -1;
	const int CONTAMINATED_ZONE = 0;
	const int UNDERGROUND = 1;
}

class DynamicMusicLocationShape
{
	const int BOX = 0;
	const int POLYGON = 1;
}

class DynamicMusicLocationDynamicData
{
	int m_Type = DynamicMusicLocationTypes.NONE;

	//! rectangle coords (2d only)
	vector m_Min = vector.Zero;
	vector m_Max = vector.Zero;
	
	static array<vector> GetRectangularCoordsFromSize(vector origin, float size)
	{
		vector min = Vector(origin[0] - size, origin[1], origin[2] - size);
		vector max = Vector(origin[0] + size, origin[1], origin[2] + size);
		
		return {min, max};
	}
}

class DynamicMusicPlayerSettings
{
	float m_MinWaitTimeSeconds = 3.0;
	float m_MaxWaitTimeSeconds = 5.0;

	float m_PreviousTrackFadeoutSeconds = 30;
}

class DynamicMusicTrackData
{
	bool m_HasPriority		= false;
	int m_TimeOfDay 		= -1;
	int m_LocationType 		= DynamicMusicLocationTypes.NONE;
	int m_Shape				= DynamicMusicLocationShape.BOX;
	string m_SoundSet;

	EDynamicMusicPlayerCategory m_Category;
	
	ref array<ref array<vector>> locationBoundaries = new array<ref array<vector>>();
	ref array<vector> vertices = new array<vector>();
	
	void InsertLocation(vector min, vector max)
	{
		locationBoundaries.Insert({min, max});
	}
}

class DynamicMusicPlayerTrackHistoryLookupType
{
	const int ANY 		= 0;
	const int BUFFER 	= 1;
}

class DynamicMusicPlayer
{
	#ifdef DMP_DEBUG_SETTINGS
	protected const float TICK_TIME_OF_DATE_UPDATE_SECONDS 			= 10.0;
	protected const float TICK_LOCATION_CACHE_UPDATE_SECONDS		= 10.0;
	protected const float TICK_LOCATION_UPDATE_SECONDS 				= 5.0;
	protected const float TICK_PRIORITY_LOCATION_UPDATE_SECONDS 	= 2.0;
	#else
	protected const float TICK_TIME_OF_DATE_UPDATE_SECONDS 			= 300.0;
	protected const float TICK_LOCATION_CACHE_UPDATE_SECONDS		= 120.0;
	protected const float TICK_LOCATION_UPDATE_SECONDS 				= 120.0;
	protected const float TICK_PRIORITY_LOCATION_UPDATE_SECONDS 	= 30.0;
	#endif
	
	protected const float TICK_FADEOUT_PROCESSOR_SECONDS 			= 0.2;

	protected const int TRACKS_BUFFER_HISTORY_SIZE 	= 2;
	protected const float LOCATION_DISTANCE_MAX 	= 500;
	
	protected float m_TickTimeOfDateElapsed;
	protected float m_TickLocationCacheUpdateElapsed;
	protected float m_TickLocationUpdateElapsed;
	protected float m_TickPriorityLocationUpdateElapsed;
	protected float m_TickFadeOutProcessingElapsed;

	protected int m_ActualTimeOfDay
	protected EDynamicMusicPlayerCategory m_CategorySelected;
	protected DynamicMusicTrackData m_CurrentTrack;
	protected ref DynamicMusicPlayerRegistry m_DynamicMusicPlayerRegistry;

	protected ref map<int, ref DynamicMusicLocationDynamicData> m_LocationsDynamic			//! map of dynamically registered locations during runtime
 
	private ref array<ref DynamicMusicTrackData> m_TracksLocationStaticCached;				//! static + filtered by the distance between player and center of zone
	private ref array<ref DynamicMusicTrackData> m_TracksLocationStaticPrioritizedCached;	//! static prio + filtered by the distance between player and center of zone
	
	protected ref array<ref DynamicMusicTrackData> m_TracksLocationMatchedPlayerInside;
	
	protected AbstractWave m_SoundPlaying;
	private bool m_WaitingForPlayback;
	private int m_RequestedPlaybackMode	//! gets the playback mode as set in sounds menu; 0 - all; 1 - menu only
	
	private ref map<EDynamicMusicPlayerCategory, ref SimpleCircularBuffer<int>> m_LastPlayedTrackBufferPerCategory;
	
	private vector m_PlayerPosition;
	
	private float m_FadeoutTimeElapsed;
	private float m_FadeoutTimeRequested;
	private bool m_FadeoutInProgress;
	
	private bool m_Created;
	
	void DynamicMusicPlayer(DynamicMusicPlayerRegistry configuration)
	{
		m_DynamicMusicPlayerRegistry = configuration;
		m_RequestedPlaybackMode = g_Game.GetProfileOptionInt(EDayZProfilesOptions.AMBIENT_MUSIC_MODE);

		m_ActualTimeOfDay 	= DynamicMusicPlayerTimeOfDay.ANY;
		m_CategorySelected 	= EDynamicMusicPlayerCategory.NONE;

		m_LastPlayedTrackBufferPerCategory = new map<EDynamicMusicPlayerCategory, ref SimpleCircularBuffer<int>>;
		
		m_LastPlayedTrackBufferPerCategory[EDynamicMusicPlayerCategory.MENU] = new SimpleCircularBuffer<int>(TRACKS_BUFFER_HISTORY_SIZE, -1);
		m_LastPlayedTrackBufferPerCategory[EDynamicMusicPlayerCategory.TIME] = new SimpleCircularBuffer<int>(TRACKS_BUFFER_HISTORY_SIZE, -1);

		m_LastPlayedTrackBufferPerCategory[EDynamicMusicPlayerCategory.LOCATION_STATIC] 			= new SimpleCircularBuffer<int>(TRACKS_BUFFER_HISTORY_SIZE, -1);
		m_LastPlayedTrackBufferPerCategory[EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY] 	= new SimpleCircularBuffer<int>(TRACKS_BUFFER_HISTORY_SIZE, -1);
		m_LastPlayedTrackBufferPerCategory[EDynamicMusicPlayerCategory.LOCATION_DYNAMIC] 			= new SimpleCircularBuffer<int>(TRACKS_BUFFER_HISTORY_SIZE, -1);
		
		m_LocationsDynamic = new map<int, ref DynamicMusicLocationDynamicData>();
		m_TracksLocationStaticCached = new array<ref DynamicMusicTrackData>();
		m_TracksLocationStaticPrioritizedCached = new array<ref DynamicMusicTrackData>();
		
		m_TracksLocationMatchedPlayerInside = new array<ref DynamicMusicTrackData>();

		SetTimeOfDate();
		
		//! fadeout settings
		m_FadeoutTimeElapsed 	= 0.0;
		m_FadeoutTimeRequested 	= 0.0;
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RefreshTracksCache, 5000);
		
		DayZProfilesOptions.m_OnIntOptionChanged.Insert(OnProfileOptionChanged);
		
		m_Created = true;
	}
	
	void OnUpdate(float timeslice)
	{
		if (m_DynamicMusicPlayerRegistry == null)
			return;

		m_TickTimeOfDateElapsed 				+= timeslice;
		m_TickLocationCacheUpdateElapsed		+= timeslice;
		m_TickLocationUpdateElapsed 			+= timeslice;
		m_TickPriorityLocationUpdateElapsed 	+= timeslice;
		m_TickFadeOutProcessingElapsed 			+= timeslice;
		
		//! handle fadeouts
		if (m_FadeoutInProgress && m_TickFadeOutProcessingElapsed >= TICK_FADEOUT_PROCESSOR_SECONDS)
		{
			m_FadeoutTimeElapsed += TICK_FADEOUT_PROCESSOR_SECONDS;
			m_TickFadeOutProcessingElapsed = 0.0;
			if (m_FadeoutTimeElapsed >= m_FadeoutTimeRequested)
			{
				m_FadeoutTimeElapsed = 0.0;
				m_FadeoutTimeRequested = 0.0;
				m_FadeoutInProgress = false;
				OnFadeoutFinished(m_CategorySelected);
			}
			else
				ProcessFadeOut();
		}
		else
		{
			if (m_CategorySelected != EDynamicMusicPlayerCategory.MENU)
			{
				//! caching of locations based on distance from player (<= LOCATION_DISTANCE_MAX)
				if (m_TickLocationCacheUpdateElapsed >= TICK_LOCATION_CACHE_UPDATE_SECONDS)
				{
					m_TickLocationCacheUpdateElapsed = 0.0;
					RefreshTracksCache();
				}
	
				if (m_TickPriorityLocationUpdateElapsed >= TICK_PRIORITY_LOCATION_UPDATE_SECONDS)
				{
					if (g_Game.GetPlayer())
					{
						m_PlayerPosition = g_Game.GetPlayer().GetPosition();
						m_PlayerPosition[1] = 0.0;
					}

					m_TickPriorityLocationUpdateElapsed = 0.0;
					
					//! no playback at all OR playback of non-prioritized category
					if ((IsPlaybackActive() && !IsPriotitizedCategorySelected()) || !IsPlaybackActive())
					{
						if (PlayerInsideOfLocationFilter(m_LocationsDynamic))
							OnLocationMatched(EDynamicMusicPlayerCategory.LOCATION_DYNAMIC, true);
						else if (PlayerInsideOfLocationFilter(m_TracksLocationStaticPrioritizedCached))
							OnLocationMatched(EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY, true);
					}
				}

				if (m_TickLocationUpdateElapsed >= TICK_LOCATION_UPDATE_SECONDS)
				{
					m_TickLocationUpdateElapsed = 0.0;
					
					if (!IsPlaybackActive())
					{
						if (PlayerInsideOfLocationFilter(m_TracksLocationStaticCached))
							OnLocationMatched(EDynamicMusicPlayerCategory.LOCATION_STATIC, false);
					}
				}
				
				if (m_TickTimeOfDateElapsed >= TICK_TIME_OF_DATE_UPDATE_SECONDS)
				{
					m_TickTimeOfDateElapsed = 0.0;
					SetTimeOfDate();
					
					//! works as default category selector
					if (!IsPlaybackActive() || !IsPriotitizedCategorySelected())
						SetCategory(EDynamicMusicPlayerCategory.TIME, false);
				}
			}
			else //! menu only
			{
				if (!IsPlaybackActive())
					DetermineTrackByCategory(m_CategorySelected);
			}
		
		}

		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.SOUNDS_DYNAMIC_MUSIC_PLAYER_STATS))
		{
			DisplayDebugStats(true);
			DisplayStaticLocations(true);
		}
		#endif
	}

	void SetCategory(EDynamicMusicPlayerCategory category, bool forced)
	{
		if (m_DynamicMusicPlayerRegistry == null)
			return;

		m_CategorySelected = category;
		
		OnCategorySet(category, forced);
	}
	
	void RegisterDynamicLocation(notnull Entity caller, int locationType, float locationSize)
	{
		int id = caller.GetID();
		if (!m_LocationsDynamic.Contains(id))
		{
			array<vector> minMax = DynamicMusicLocationDynamicData.GetRectangularCoordsFromSize(caller.GetPosition(), locationSize);
			DynamicMusicLocationDynamicData location = new DynamicMusicLocationDynamicData();
			location.m_Type = locationType;
			location.m_Min = minMax[0];
			location.m_Max = minMax[1];
			m_LocationsDynamic.Insert(id, location);
		}
	}

	void UnregisterDynamicLocation(notnull Entity caller)
	{
		m_LocationsDynamic.Remove(caller.GetID());
	}
	
	void OnGameEvent(EventType eventTypeId, Param params)
	{
		if (eventTypeId == MPSessionPlayerReadyEventTypeID)
		{
			SetTimeOfDate();
			SetCategory(EDynamicMusicPlayerCategory.TIME, false);
		}
	}
	
	protected bool IsPriotitizedCategorySelected()
	{
		return m_CategorySelected == EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY || m_CategorySelected == EDynamicMusicPlayerCategory.LOCATION_DYNAMIC;
	}
	
	protected void DetermineTrackByCategory(EDynamicMusicPlayerCategory category)
	{
		if (m_CategorySelected != EDynamicMusicPlayerCategory.MENU && m_RequestedPlaybackMode == 1)
			return;

		if (IsPlaybackActive())
			return;
		
		switch (category)
		{
			case EDynamicMusicPlayerCategory.MENU:
				if (SetSelectedTrackFromCategory(category, m_DynamicMusicPlayerRegistry.m_TracksMenu, DynamicMusicPlayerTrackHistoryLookupType.BUFFER))
					break;
			
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DetermineTrackByCategory, 5000, false, category);
				break;
			
			case EDynamicMusicPlayerCategory.TIME:
				if (SetSelectedTrackFromCategory(category, m_DynamicMusicPlayerRegistry.m_TracksTime, DynamicMusicPlayerTrackHistoryLookupType.BUFFER))
					break;

				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DetermineTrackByCategory, 5000, false, category);
				break;
			case EDynamicMusicPlayerCategory.LOCATION_STATIC:
				if (SetSelectedTrackFromCategory(category, m_TracksLocationMatchedPlayerInside))
					break;

				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DetermineTrackByCategory, 5000, false, category);
				break;
			case EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY:
				if (SetSelectedTrackFromCategory(category, m_TracksLocationMatchedPlayerInside))
					break;

				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DetermineTrackByCategory, 5000, false, category);
				break;
			case EDynamicMusicPlayerCategory.LOCATION_DYNAMIC:
				if (SetSelectedTrackFromCategory(category, m_DynamicMusicPlayerRegistry.m_TracksLocationDynamic))
					break;
			
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DetermineTrackByCategory, 5000, false, category);
			
				break;
		}
	}
	
	protected bool IsPlaybackActive()
	{
		return m_SoundPlaying || m_WaitingForPlayback;
	}

	//! Events
	//! --------------------------------------------------------------------------------
	
	protected void OnProfileOptionChanged(EDayZProfilesOptions option, int value)
	{
		if (option == EDayZProfilesOptions.AMBIENT_MUSIC_MODE)
		{
			m_RequestedPlaybackMode = value;
			
			if (value == 1 && m_CategorySelected != EDynamicMusicPlayerCategory.MENU)
			{
				StopTrack();
				ResetWaitingQueue();
			}
		}
	}
	
	protected void OnCategorySet(EDynamicMusicPlayerCategory category, bool forced)
	{
		#ifdef DIAG_DEVELOPER
		DMPDebugPrint(string.Format(
			"OnCategorySet() - category: %1, forced: %2",
			EnumTools.EnumToString(EDynamicMusicPlayerCategory, category),
			forced),
		);
		#endif

		DetermineTrackByCategory(category);
	}
	
	protected void OnTrackEnded()
	{
		#ifdef DIAG_DEVELOPER
		if (m_CurrentTrack)
			DMPDebugPrint(string.Format("Track END - %1", m_CurrentTrack.m_SoundSet));
		#endif

		m_SoundPlaying = null;
		m_CurrentTrack = null;
		m_WaitingForPlayback = false;
	}
	
	protected void OnTrackStopped()
	{
		//! stopped only by fadeouts
		#ifdef DIAG_DEVELOPER
		if (m_CurrentTrack)
			DMPDebugPrint(string.Format("Track STOP - %1", m_CurrentTrack.m_SoundSet));
		#endif

		m_SoundPlaying = null;
		m_CurrentTrack = null;
		m_WaitingForPlayback = false;
	}

	protected void OnNextTrackSelected(DynamicMusicTrackData track, float waitTime)
	{
		m_WaitingForPlayback = true;
		m_CurrentTrack = track;
		
		if (m_Created)
			m_Created = false;

		#ifdef DIAG_DEVELOPER
		DMPDebugPrint(string.Format(
			"WaitTime set to %1s, deferring playback of \"%2\"",
			(int)waitTime,
			track.m_SoundSet),
		);
		m_DebugWaitTime = waitTime;
		#endif

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PlayTrack, (int)waitTime * 1000, false, track);
	}
	
	protected void OnLocationMatched(EDynamicMusicPlayerCategory category, bool isPriorityLocation)
	{
		#ifdef DIAG_DEVELOPER
		string messagePriority;
		if (isPriorityLocation)
			messagePriority = "(with priority)";
		DMPDebugPrint(string.Format("Location matched %1", messagePriority));
		#endif

		if (isPriorityLocation)
		{
			if (!IsPriotitizedCategorySelected())
			{
				m_CategorySelected = category;
				if (m_WaitingForPlayback)
					ResetWaitingQueue();
				
				if (m_SoundPlaying)
					FadeoutTrack(GetPreviousTrackFadeoutSeconds(category));

				SetCategory(category, isPriorityLocation);
			}
			else
				SetCategory(category, true); //! play prio location track (no fadeout)
		}
		else
			SetCategory(category, false); //! play location track (no fadeout) 
	}
	
	protected void OnFadeoutFinished(EDynamicMusicPlayerCategory category)
	{
		if (m_SoundPlaying)
			m_SoundPlaying.GetEvents().Event_OnSoundWaveEnded.Remove(OnTrackEnded);

		StopTrack();
		SetCategory(category, IsPriotitizedCategorySelected());
	}
	//! --------------------------------------------------------------------------------
	
	private void PlayTrack(DynamicMusicTrackData track)
	{
		SoundParams soundParams			= new SoundParams(track.m_SoundSet);
		if (soundParams.IsValid())
		{
			SoundObjectBuilder soundBuilder	= new SoundObjectBuilder(soundParams);
			SoundObject soundObject			= soundBuilder.BuildSoundObject();
			soundObject.SetKind(WaveKind.WAVEMUSIC);
	
			m_SoundPlaying = GetGame().GetSoundScene().Play2D(soundObject, soundBuilder);
			if (m_SoundPlaying)
			{
				m_SoundPlaying.Loop(false);
				m_SoundPlaying.Play();
	
				//! register callbacks
				m_SoundPlaying.GetEvents().Event_OnSoundWaveEnded.Insert(OnTrackEnded);
				m_SoundPlaying.GetEvents().Event_OnSoundWaveStopped.Insert(OnTrackStopped);
				
				m_WaitingForPlayback = false;
			}
		}
		else //! invalid sound set is used
		{
			m_WaitingForPlayback = false;
			m_CurrentTrack 		 = null;
		}
		
	}
	
	private void StopTrack()
	{
		if (m_SoundPlaying)
			m_SoundPlaying.Stop();
	}
	
	private void ResetWaitingQueue()
	{		
		if (m_WaitingForPlayback)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(PlayTrack);
			m_WaitingForPlayback = false;
			m_CurrentTrack = null;
		}
	}
	
	private void FadeoutTrack(float fadeoutSeconds)
	{
		if (m_FadeoutInProgress)
			return;

		if (m_CurrentTrack && m_SoundPlaying)
		{
			#ifdef DIAG_DEVELOPER
			DMPDebugPrint(string.Format("Stopping currently played track %1", m_CurrentTrack.m_SoundSet));
			DMPDebugPrint(string.Format("-- Setting fadeout to %1", fadeoutSeconds));
			#endif
			m_FadeoutInProgress = true;
			m_FadeoutTimeRequested = fadeoutSeconds;
		}
	}

	private void ProcessFadeOut()
	{
		if (m_SoundPlaying)
		{
			float volume = 1 - (m_FadeoutTimeElapsed / m_FadeoutTimeRequested);
			m_SoundPlaying.SetFadeOutFactor(volume);
		}
	}

	private bool PlayerInsideOfLocationFilter(array<ref DynamicMusicTrackData> locations)
	{
		m_TracksLocationMatchedPlayerInside.Clear();
		
		if (locations.Count() > 0)
		{
			foreach (DynamicMusicTrackData track : locations)
			{
				switch (track.m_Shape)
				{
					case DynamicMusicLocationShape.BOX:
						foreach (int locationId, array<vector> bounds : track.locationBoundaries)
						{
							if (Math.IsPointInRectangle(bounds[0], bounds[1], m_PlayerPosition))
							{
								if (m_TracksLocationMatchedPlayerInside.Find(track) == INDEX_NOT_FOUND)
									m_TracksLocationMatchedPlayerInside.Insert(track);
			
								#ifdef DIAG_DEVELOPER
								DMPDebugPrint(string.Format("Player inside location <%1, %2>", bounds[0], bounds[1]));
								#endif
							}
						}
						break;
					case DynamicMusicLocationShape.POLYGON:
						if (Math2D.IsPointInPolygonXZ(track.vertices, m_PlayerPosition))
						{
							if (m_TracksLocationMatchedPlayerInside.Find(track) == INDEX_NOT_FOUND)
								m_TracksLocationMatchedPlayerInside.Insert(track);					

							#ifdef DIAG_DEVELOPER
							DMPDebugPrint(string.Format("Player inside polygon location at <%1>", m_PlayerPosition));
							#endif	
						}
						break;
				}
			}
		}
		
		return m_TracksLocationMatchedPlayerInside.Count() > 0;
	}

	private bool PlayerInsideOfLocationFilter(map<int, ref DynamicMusicLocationDynamicData> locations)
	{
		if (locations.Count() > 0)
		{
			foreach (int locationId, DynamicMusicLocationDynamicData location : locations)
			{
				if (Math.IsPointInRectangle(location.m_Min, location.m_Max, m_PlayerPosition))
				{
					#ifdef DIAG_DEVELOPER
					DMPDebugPrint(string.Format("Player inside location <%1, %2>", location.m_Min, location.m_Max));
					#endif
					return true;
				}
			}
		}

		return false;
	}
	
	private bool SetSelectedTrackFromCategory(EDynamicMusicPlayerCategory category, notnull array<ref DynamicMusicTrackData> tracklist, int historyLookupType = DynamicMusicPlayerTrackHistoryLookupType.ANY)
	{
		if (tracklist.Count() == 0)
			return true;

		array<ref DynamicMusicTrackData> filteredTracks = new array<ref DynamicMusicTrackData>();
		foreach (DynamicMusicTrackData filteredTrack : tracklist)
		{
			if (filteredTrack.m_TimeOfDay == m_ActualTimeOfDay || filteredTrack.m_TimeOfDay == DynamicMusicPlayerTimeOfDay.ANY)
				filteredTracks.Insert(filteredTrack);
		}

		float trackIndex;
		//! currently prioritize main menu track on DMP start
		if (m_Created && category == EDynamicMusicPlayerCategory.MENU)
			trackIndex = SelectRandomTrackIndexFromCategoryPriorityFlagFirst(category, filteredTracks);
		else
			trackIndex = SelectRandomTrackIndexFromCategory(category, historyLookupType, filteredTracks);

		if (trackIndex > INDEX_NOT_FOUND)
		{
			m_LastPlayedTrackBufferPerCategory[category].Add(trackIndex);
			OnNextTrackSelected(filteredTracks[trackIndex], GetWaitTimeForCategory(category));

			return true;
		}
		
		return false;
	}
	
	private int SelectRandomTrackIndexFromCategoryPriorityFlagFirst(EDynamicMusicPlayerCategory category, notnull array<ref DynamicMusicTrackData> tracks)
	{
		//! main menu priority tracks first
		if (category == EDynamicMusicPlayerCategory.MENU)
		{
			array<int> priorityFlagIndices = new array<int>();
			
			foreach (int i, DynamicMusicTrackData track : tracks)
			{

				if (!track.m_HasPriority)
					continue;

				priorityFlagIndices.Insert(i);
			}

			if (priorityFlagIndices.Count() > 0)
				return priorityFlagIndices[priorityFlagIndices.GetRandomIndex()];

			//! fallback in case there is no priority track
			return tracks.GetRandomIndex();
		}

		return INDEX_NOT_FOUND;
	}
	
	private int SelectRandomTrackIndexFromCategory(EDynamicMusicPlayerCategory category, int lookupType, notnull array<ref DynamicMusicTrackData> tracks)
	{
		int count = tracks.Count();
		if (count > 0)
		{
			int index = Math.RandomInt(0, count);

			switch (lookupType)
			{
				case DynamicMusicPlayerTrackHistoryLookupType.ANY:
					return index;

				case DynamicMusicPlayerTrackHistoryLookupType.BUFFER:
					// fallback - num of track is smaller than actual history size;
					if (count <= TRACKS_BUFFER_HISTORY_SIZE)
						return index;

					if (m_LastPlayedTrackBufferPerCategory[category].GetValues().Find(index) == INDEX_NOT_FOUND)
						return index;
				
					return INDEX_NOT_FOUND;
			}
		}
		
		return INDEX_NOT_FOUND;
	}
	
	private void SetTimeOfDate()
	{
		if (g_Game.GetMission())
		{
			m_ActualTimeOfDay = g_Game.GetMission().GetWorldData().GetDaytime();
			return;
		}

		m_ActualTimeOfDay = DynamicMusicPlayerTimeOfDay.DAY;
	}
	
	protected float GetWaitTimeForCategory(EDynamicMusicPlayerCategory category)
	{
		return Math.RandomFloatInclusive(GetMinWaitTimePerCategory(category), GetMaxWaitTimePerCategory(category));
	}
	
	private float GetMinWaitTimePerCategory(EDynamicMusicPlayerCategory category)
	{
		float waitTime = m_DynamicMusicPlayerRegistry.m_SettingsByCategory[category].m_MinWaitTimeSeconds;
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.DYNAMIC_MUSIC_PLAYER))
		{
			float timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			if (timeAccel > 0)
				return waitTime / FeatureTimeAccel.GetFeatureTimeAccelValue();
		}
		#endif
		return waitTime;		
	}
	
	private float GetMaxWaitTimePerCategory(EDynamicMusicPlayerCategory category)
	{
		float waitTime = m_DynamicMusicPlayerRegistry.m_SettingsByCategory[category].m_MaxWaitTimeSeconds;
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.DYNAMIC_MUSIC_PLAYER))
		{
			float timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			if (timeAccel > 0)
				return waitTime / FeatureTimeAccel.GetFeatureTimeAccelValue();
		}
		#endif
		return waitTime;
	}
	
	private float GetPreviousTrackFadeoutSeconds(EDynamicMusicPlayerCategory category)
	{
		return m_DynamicMusicPlayerRegistry.m_SettingsByCategory[category].m_PreviousTrackFadeoutSeconds;
	}
	
	private void RefreshTracksCache()
	{
		if (m_DynamicMusicPlayerRegistry)
		{
			m_TracksLocationStaticCached.Clear();
			foreach (DynamicMusicTrackData track : m_DynamicMusicPlayerRegistry.m_TracksLocationStatic)
			{
				if (track.m_Shape == DynamicMusicLocationShape.BOX)
				{
					foreach (array<vector> bounds : track.locationBoundaries)
					{
						if (vector.Distance(m_PlayerPosition, Math.CenterOfRectangle(bounds[0], bounds[1])) > LOCATION_DISTANCE_MAX)
							continue;
					}
				}
	
				m_TracksLocationStaticCached.Insert(track);
			}
	
			m_TracksLocationStaticPrioritizedCached.Clear();
			foreach (DynamicMusicTrackData trackPrio : m_DynamicMusicPlayerRegistry.m_TracksLocationStaticPrioritized)
			{
				if (trackPrio.m_Shape == DynamicMusicLocationShape.BOX)
				{
					foreach (array<vector> boundsPrio : trackPrio.locationBoundaries)
					{
						if (vector.Distance(m_PlayerPosition, Math.CenterOfRectangle(boundsPrio[0], boundsPrio[1])) > LOCATION_DISTANCE_MAX)
							continue;
					}
				}
				
				m_TracksLocationStaticPrioritizedCached.Insert(trackPrio);
			}
		}
	}
	
	#ifdef DIAG_DEVELOPER
	private ref array<Shape> m_DebugShapesLocations 		= new array<Shape>();
	private ref array<Shape> m_DebugShapesLocationsVertices = new array<Shape>();
	private float m_DebugWaitTime = 0;

	private void DisplayDebugStats(bool enabled)
	{
		int windowPosX = 10;
		int windowPosY = 200;

		DbgUI.Begin("DMP - Overall stats", windowPosX, windowPosY);
		if (enabled)
		{
			bool isPlaybackActive = m_SoundPlaying != null;
			DbgUI.Text(string.Format("Day/Night: %1", DynamicMusicPlayerTimeOfDay.ToString(m_ActualTimeOfDay)));
			DbgUI.Text("Playback:");
			DbgUI.Text(string.Format("  active: %1", isPlaybackActive.ToString()));
			DbgUI.Text(string.Format("  waiting: %1", m_WaitingForPlayback.ToString()));
			DbgUI.Text(string.Format("Selected Category: %1", EnumTools.EnumToString(EDynamicMusicPlayerCategory, m_CategorySelected)));

			if (m_CategorySelected != EDynamicMusicPlayerCategory.MENU)
			{
				DbgUI.Text("Update timers:");
				DbgUI.Text(string.Format("  TimeOfDay: %1(%2)", TICK_TIME_OF_DATE_UPDATE_SECONDS, TICK_TIME_OF_DATE_UPDATE_SECONDS - (int)m_TickTimeOfDateElapsed));
				DbgUI.Text(string.Format("  Location: %1(%2)", TICK_LOCATION_UPDATE_SECONDS, TICK_LOCATION_UPDATE_SECONDS - (int)m_TickLocationUpdateElapsed));
				DbgUI.Text(string.Format("  PriorityLocation: %1(%2)", TICK_PRIORITY_LOCATION_UPDATE_SECONDS, TICK_PRIORITY_LOCATION_UPDATE_SECONDS - (int)m_TickPriorityLocationUpdateElapsed));
				DbgUI.Text(string.Format("  Location Cache: %1(%2)", TICK_LOCATION_CACHE_UPDATE_SECONDS, TICK_LOCATION_CACHE_UPDATE_SECONDS - (int)m_TickLocationCacheUpdateElapsed));
			}

			if (m_CategorySelected != EDynamicMusicPlayerCategory.MENU)
			{			
				DbgUI.Text("Player:");
				DbgUI.Text(string.Format("  position: %1", m_PlayerPosition.ToString()));
				DbgUI.Text(string.Format("  matched num tracks(location): %1", m_TracksLocationMatchedPlayerInside.Count()));
			}
			
			DbgUI.Text("Tracks counts:");
			if (m_CategorySelected == EDynamicMusicPlayerCategory.MENU)
				DbgUI.Text(string.Format("  Menu: %1", m_DynamicMusicPlayerRegistry.m_TracksMenu.Count()));
			else
			{
				DbgUI.Text(string.Format("  Time: %1", m_DynamicMusicPlayerRegistry.m_TracksTime.Count()));
				DbgUI.Text(string.Format("  Static[cache]: %1", m_TracksLocationStaticCached.Count()));
				DbgUI.Text(string.Format("  Static(prio)[cache]: %1", m_TracksLocationStaticPrioritizedCached.Count()));
				DbgUI.Text(string.Format("  Dynamic(prio): %1", m_DynamicMusicPlayerRegistry.m_TracksLocationDynamic.Count()));
			}
		}
		DbgUI.End();
		
		DbgUI.Begin("DMP - Current track", windowPosX, windowPosY+380);
		if (enabled && m_CurrentTrack)
		{
			string isPlaying = "waiting";
			if (m_SoundPlaying != null)
				isPlaying = "playing";

			DbgUI.Text(string.Format("State: %1", isPlaying));
			if (m_WaitingForPlayback)
			{
				DbgUI.Text(string.Format("Wait time: %1s (%2s)", (int)m_DebugWaitTime, (int)(g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).GetRemainingTime(PlayTrack) * 0.001)));
			}
			DbgUI.Text(string.Format("Sound set: %1", m_CurrentTrack.m_SoundSet));
			DbgUI.Text(string.Format("Category: %1", EnumTools.EnumToString(EDynamicMusicPlayerCategory, m_CurrentTrack.m_Category)));
			DbgUI.Text(string.Format("Time of day: %1", DynamicMusicPlayerTimeOfDay.ToString(m_CurrentTrack.m_TimeOfDay)));
		}
		DbgUI.End();
		
		DbgUI.Begin("DMP - Controls", windowPosX + 500, windowPosY);
		if (enabled)
		{
			if (DbgUI.Button("Stop"))
				StopTrack();

			if (DbgUI.Button("Reset Waiting"))
				ResetWaitingQueue();
			
			DbgUI.Text("Set Category:\n");
			if (DbgUI.Button("Time"))
				SetCategory(EDynamicMusicPlayerCategory.TIME, false);
			if (DbgUI.Button("Location"))
				SetCategory(EDynamicMusicPlayerCategory.LOCATION_STATIC, false);
			
			DbgUI.Text("Reset Timers\n");
			if (DbgUI.Button("Timer ALL"))
			{
				m_TickTimeOfDateElapsed = TICK_TIME_OF_DATE_UPDATE_SECONDS - 1;
				m_TickLocationUpdateElapsed = TICK_LOCATION_UPDATE_SECONDS - 1;
				m_TickPriorityLocationUpdateElapsed = TICK_PRIORITY_LOCATION_UPDATE_SECONDS - 1;
			}
			
			if (DbgUI.Button("Timer Daytime"))
				m_TickTimeOfDateElapsed = TICK_TIME_OF_DATE_UPDATE_SECONDS - 1;
			if (DbgUI.Button("Timer Location"))
				m_TickLocationUpdateElapsed = TICK_LOCATION_UPDATE_SECONDS - 1;
			if (DbgUI.Button("Timer Location(prio)"))
				m_TickPriorityLocationUpdateElapsed = TICK_PRIORITY_LOCATION_UPDATE_SECONDS - 1;
			
		}
		DbgUI.End();
	}
	
	private void DisplayStaticLocations(bool enabled)
	{
		if (enabled)
		{
			vector locationMin;
			vector locationMax;
			vector position = g_Game.GetCurrentCameraPosition();
			
			foreach (DynamicMusicTrackData track : m_TracksLocationStaticCached)
			{
				foreach (array<vector> bounds : track.locationBoundaries)
				{
					locationMin = bounds[0];
					locationMax = bounds[1];
				
					if (vector.Distance(position, Math.CenterOfRectangle(locationMin, locationMax)) > 2000)
						continue;
		
					Debug.CleanupDrawShapes(m_DebugShapesLocations);
		
					locationMax[1] = locationMin[1] + 200.0; //! force the height of box for debug
					locationMin[1] = locationMin[1] - 50.0;
					m_DebugShapesLocations.Insert(Debug.DrawBoxEx(locationMin, locationMax, Colors.PURPLE, ShapeFlags.NOZWRITE|ShapeFlags.ONCE));
				}
				
				Debug.CleanupDrawShapes(m_DebugShapesLocationsVertices);
				DrawPolygonLocation(track);
			}
			
			foreach (DynamicMusicTrackData trackPrio : m_TracksLocationStaticPrioritizedCached)
			{
				foreach (array<vector> boundsPrio : trackPrio.locationBoundaries)
				{
					locationMin = boundsPrio[0];
					locationMax = boundsPrio[1];

					if (vector.Distance(position, Math.CenterOfRectangle(locationMin, locationMax)) > 2000)
						continue;

					Debug.CleanupDrawShapes(m_DebugShapesLocations);

					locationMax[1] = locationMin[1] + 200.0; //! force the height of box for debug
					locationMin[1] = locationMin[1] - 50.0;
					m_DebugShapesLocations.Insert(Debug.DrawBoxEx(locationMin, locationMax, Colors.RED, ShapeFlags.NOZWRITE|ShapeFlags.ONCE));
				}
				
				Debug.CleanupDrawShapes(m_DebugShapesLocationsVertices);
				DrawPolygonLocation(trackPrio);
			}
			
			foreach (DynamicMusicLocationDynamicData locationDynamic : m_LocationsDynamic)
			{
				locationMin = locationDynamic.m_Min;
				locationMax = locationDynamic.m_Max;
				
				if (vector.Distance(position, Math.CenterOfRectangle(locationMin, locationMax)) > 2000)
					continue;

				Debug.CleanupDrawShapes(m_DebugShapesLocations);

				locationMax[1] = locationMin[1] + 200.0; //! force the height of box for debug
				locationMin[1] = locationMin[1] - 50.0;
				m_DebugShapesLocations.Insert(Debug.DrawBoxEx(locationMin, locationMax, Colors.YELLOW, ShapeFlags.NOZWRITE|ShapeFlags.ONCE));
			}
		}
		else
		{
			Debug.CleanupDrawShapes(m_DebugShapesLocations);
			Debug.CleanupDrawShapes(m_DebugShapesLocationsVertices);
		}
	}
	
	private void DrawPolygonLocation(notnull DynamicMusicTrackData track)
	{
		vector first, current, last;
		
		int count =  track.vertices.Count();
		foreach (int i, vector vertexPos : track.vertices)
		{
			vertexPos[1] = vertexPos[1] + 0.5;
			current = vertexPos;

			if (i == 0)
				first = vertexPos;
			else
				m_DebugShapesLocationsVertices.Insert(Debug.DrawLine(last, current, COLOR_WHITE, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.ONCE));
			
			last = current;
		}
		
		m_DebugShapesLocationsVertices.Insert(Debug.DrawLine(current, first, COLOR_WHITE, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.ONCE));
	}
	
	private void DMPDebugPrint(string message)
	{
		#ifdef DMP_DEBUG_PRINT
		Debug.Log(message);
		#endif
	}
	
	//!DEPRECATED
	private void CleanupDebugShapes(array<Shape> shapesArr)
	{
		Debug.CleanupDrawShapes(shapesArr);
	}

	#endif
}


//! for backward compatibility
class DynamicMusicPlayerTimeOfDay : WorldDataDaytime {}
