class DynamicMusicPlayerRegistry
{
	ref map<EDynamicMusicPlayerCategory, ref DynamicMusicPlayerSettings> m_SettingsByCategory;
	
	ref array<ref DynamicMusicTrackData> m_TracksMenu;
	ref array<ref DynamicMusicTrackData> m_TracksCredits;
	ref array<ref DynamicMusicTrackData> m_TracksTime;
	ref array<ref DynamicMusicTrackData> m_TracksLocationStatic;
	ref array<ref DynamicMusicTrackData> m_TracksLocationStaticPrioritized;
	ref array<ref DynamicMusicTrackData> m_TracksLocationDynamic;
	
	protected string m_WorldName = "";
	
	void DynamicMusicPlayerRegistry()
	{
		PreloadData();
	}
	
	protected void PreloadData()
	{
		m_WorldName.ToLower();

		RegisterCategorySettings();

		RegisterTracksMenu();
		RegisterTracksCredits();
		RegisterTracksTime();
		RegisterTracksLocationStatic();
		RegisterTracksLocationDynamic();
	}
	
	//! --------------------------------------------------------------------------------
	protected void RegisterCategorySettings()
	{
		if (!m_SettingsByCategory)
			m_SettingsByCategory = new map<EDynamicMusicPlayerCategory, ref DynamicMusicPlayerSettings>();

		RegisterGlobalSettings();
		RegisterMenuSettings();
		RegisterCreditsSettings();
		RegisterTimeSettings();
		RegisterLocationStaticSettings();
		RegisterLocationDynamicSettings();
	}
	
	//! --------------------------------------------------------------------------------	
	private void RegisterGlobalSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 3.0;
		settings.m_MaxWaitTimeSeconds = 5.0;
		
		m_SettingsByCategory[EDynamicMusicPlayerCategory.NONE] = settings;
	}
	
	private void RegisterMenuSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 1.0;
		settings.m_MaxWaitTimeSeconds = 3.0;
		
		settings.m_PreviousTrackFadeoutSeconds = 2.0;
		
		m_SettingsByCategory[EDynamicMusicPlayerCategory.MENU] = settings;
	}

	private void RegisterCreditsSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 1.0;
		settings.m_MaxWaitTimeSeconds = 3.0;
		
		settings.m_PreviousTrackFadeoutSeconds = 2.0;
		
		m_SettingsByCategory[EDynamicMusicPlayerCategory.CREDITS] = settings;
	}
	
	private void RegisterTimeSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 360.0; //360 = 6 min
		settings.m_MaxWaitTimeSeconds = 600.0; //600 = 10 min
		
		m_SettingsByCategory[EDynamicMusicPlayerCategory.TIME] = settings;
	}
	
	private void RegisterLocationStaticSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 360.0; //360 = 6 min
		settings.m_MaxWaitTimeSeconds = 600.0; //600 = 10 min

		m_SettingsByCategory[EDynamicMusicPlayerCategory.LOCATION_STATIC] = settings;
		
		settings.m_PreviousTrackFadeoutSeconds = 15.0;
		m_SettingsByCategory[EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY] = settings;
	}
	
	private void RegisterLocationDynamicSettings()
	{
		DynamicMusicPlayerSettings settings = new DynamicMusicPlayerSettings();
		settings.m_MinWaitTimeSeconds = 3.0;
		settings.m_MaxWaitTimeSeconds = 5.0;

		settings.m_PreviousTrackFadeoutSeconds = 20.0;
		
		m_SettingsByCategory[EDynamicMusicPlayerCategory.LOCATION_DYNAMIC] = settings;
	}
	
//======================================================================== MENU =================================================================
//===============================================================================================================================================	
	protected void RegisterTracksMenu()
	{
		m_TracksMenu = new array<ref DynamicMusicTrackData>();

		RegisterTrackMenu("Music_Menu_SoundSet", true); 
		RegisterTrackMenu("Music_Menu_2_SoundSet");
		RegisterTrackMenu("Music_Menu_3_SoundSet");
		RegisterTrackMenu("Music_Menu_4_SoundSet");
	}
//===============================================================================================================================================	
	protected void RegisterTracksCredits()
	{
		m_TracksCredits = new array<ref DynamicMusicTrackData>();

		RegisterTrackCredits("Music_Menu_subtitles_remake_SoundSet");
	}
//____________________________________________Day Time setup___________________________________________

	protected void RegisterTracksTime()
	{
		m_TracksTime = new array<ref DynamicMusicTrackData>();

		//DAY
		RegisterTrackTime("Music_time_day_1_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_2_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_3_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_4_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_5_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_6_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_7_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_time_day_8_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		//NIGHT
		RegisterTrackTime("Music_time_night_1_SoundSet", DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackTime("Music_time_night_2_SoundSet", DynamicMusicPlayerTimeOfDay.NIGHT);
		//DAWN
		RegisterTrackTime("Music_time_dawn_1_SoundSet", DynamicMusicPlayerTimeOfDay.DAWN);
		RegisterTrackTime("Music_time_dawn_2_SoundSet", DynamicMusicPlayerTimeOfDay.DAWN);
		//DUSK
		RegisterTrackTime("Music_time_dusk_1_SoundSet", DynamicMusicPlayerTimeOfDay.DUSK);
		RegisterTrackTime("Music_time_dusk_2_SoundSet", DynamicMusicPlayerTimeOfDay.DUSK);
		//UNSORTED
		//RegisterTrackTime("Music_time_based_new_6_SoundSet", DynamicMusicPlayerTimeOfDay.ANY);
	}

	protected void RegisterTracksLocationStatic()
	{
		m_TracksLocationStatic = new array<ref DynamicMusicTrackData>();
		m_TracksLocationStaticPrioritized = new array<ref DynamicMusicTrackData>();
	}

	protected void RegisterTracksLocationDynamic()
	{
		m_TracksLocationDynamic = new array<ref DynamicMusicTrackData>();

		RegisterTrackLocationDynamic("Music_loc_contaminated_day_SoundSet", DynamicMusicLocationTypes.CONTAMINATED_ZONE, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationDynamic("Music_loc_contaminated_night_SoundSet", DynamicMusicLocationTypes.CONTAMINATED_ZONE, DynamicMusicPlayerTimeOfDay.NIGHT);
	}

	//! --------------------------------------------------------------------------------
	protected void RegisterTrackMenu(string soundSetName, bool hasPriority = false, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_Category 	= EDynamicMusicPlayerCategory.MENU;
		track.m_SoundSet 	= soundSetName;
		track.m_TimeOfDay 	= timeOfDay;
		track.m_HasPriority	= hasPriority;

		m_TracksMenu.Insert(track);
	}
	
	//! --------------------------------------------------------------------------------
	protected void RegisterTrackCredits(string soundSetName, bool hasPriority = false)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_Category 	= EDynamicMusicPlayerCategory.CREDITS;
		track.m_SoundSet 	= soundSetName;
		track.m_HasPriority	= hasPriority;

		m_TracksCredits.Insert(track);
	}
	
	protected void RegisterTrackTime(string soundSetName, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_Category 	= EDynamicMusicPlayerCategory.TIME;
		track.m_SoundSet 	= soundSetName;
		track.m_TimeOfDay 	= timeOfDay;

		m_TracksTime.Insert(track);
	}
	
	protected void RegisterTrackLocationStatic(string soundSetName, vector start, vector end, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY, bool runImmediately = false)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_SoundSet 	= soundSetName;
		track.m_TimeOfDay 	= timeOfDay;
		track.m_Shape		= DynamicMusicLocationShape.BOX;
		
		track.InsertLocation(start, end);
		
		if (!runImmediately)
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC;
			m_TracksLocationStatic.Insert(track);
		}
		else
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY;
			m_TracksLocationStaticPrioritized.Insert(track);
		}
	}
	
	protected void RegisterTrackLocationStaticMultiRectangle(string soundSetName, array<ref TVectorArray> locationBoundaries, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY, bool runImmediately = false)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_SoundSet 	= soundSetName;
		track.m_TimeOfDay 	= timeOfDay;
		track.m_Shape		= DynamicMusicLocationShape.BOX;
		
		track.locationBoundaries = locationBoundaries;
		
		if (!runImmediately)
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC;
			m_TracksLocationStatic.Insert(track);
		}
		else
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY;
			m_TracksLocationStaticPrioritized.Insert(track);
		}
		
	}
	
	protected void RegisterTrackLocationStaticPoints(string soundSetName, array<vector> vertices, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY, bool runImmediately = false)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_SoundSet 	= soundSetName;
		track.m_TimeOfDay 	= timeOfDay;
		track.m_Shape		= DynamicMusicLocationShape.POLYGON;
		
		track.vertices = vertices;
		
		if (!runImmediately)
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC;
			m_TracksLocationStatic.Insert(track);
		}
		else
		{
			track.m_Category = EDynamicMusicPlayerCategory.LOCATION_STATIC_PRIORITY;
			m_TracksLocationStaticPrioritized.Insert(track);
		}
	}
	
	protected void RegisterTrackLocationDynamic(string soundSetName, int locationType = DynamicMusicLocationTypes.NONE, int timeOfDay = DynamicMusicPlayerTimeOfDay.ANY)
	{
		DynamicMusicTrackData track = new DynamicMusicTrackData();
		track.m_Category 		= EDynamicMusicPlayerCategory.LOCATION_DYNAMIC;
		track.m_SoundSet 		= soundSetName;
		track.m_LocationType	= locationType;
		track.m_TimeOfDay 		= timeOfDay;

		m_TracksLocationDynamic.Insert(track);
	}
}
