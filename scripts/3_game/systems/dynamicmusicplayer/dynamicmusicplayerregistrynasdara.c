class DynamicMusicPlayerRegistryNasdara : DynamicMusicPlayerRegistry
{
	void DynamicMusicPlayerRegistryNasdara()
	{
		m_WorldName = "nasdara";
	}
	
	override protected void RegisterTracksMenu()
	{
		m_TracksMenu = new array<ref DynamicMusicTrackData>();

		RegisterTrackMenu("Music_nasdara_menu_SoundSet", true); 
		RegisterTrackMenu("Music_Menu_2_SoundSet");
		RegisterTrackMenu("Music_Menu_3_SoundSet");
		RegisterTrackMenu("Music_Menu_4_SoundSet");
	}

	override protected void RegisterTracksLocationStatic()
	{
		super.RegisterTracksLocationStatic();

		array<ref TVectorArray> Border_Crossing;
		Border_Crossing = {
			{"3607.849121 277.643585 4849.083008", "4024.146484 276.382538 5141.240234"}, //Border_Crossing
			{"3802.113037 279.579987 5120.352539", "4079.197998 272.378845 5424.841797"},
		};
		
		array<ref TVectorArray> Desert;
		Desert = {
			{"13003.801758 169.269516 1139.779297", "16000.135742 185.763336 14388.526367"}, //Desert
			{"12014.322266 195.797928 7492.764648", "14764.032227 220.287476 11580.726563"},
		};

		array<ref TVectorArray> Mountains;
		Mountains = {
			{"4060.177490 330.632111 7355.109375", "4636.962891 317.837250 8239.078125"}, //Mountains
		};

		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Border_crossing_SoundSet", Border_Crossing, DynamicMusicPlayerTimeOfDay.ANY, true);

		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Desert_SoundSet", Desert, DynamicMusicPlayerTimeOfDay.ANY, true);

		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Mountains_SoundSet", Mountains, DynamicMusicPlayerTimeOfDay.ANY, true);
		
		//____________________________________________CITIES ___________________________________________
		
		
		array<ref TVectorArray> Monarchy_palace; //Monarchy_palace
		Monarchy_palace = {
			{"10002.068359 177.365158 8136.884766", "10282.649414 184.977798 8291.402344"},
		};			
		
		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Monarchy_palace_SoundSet", Monarchy_palace, DynamicMusicPlayerTimeOfDay.ANY, true);

		
//____________________________________________ Industrial zones ___________________________________________
		array<ref TVectorArray> Oil_refineries;
		Oil_refineries = {
			{"10461.745117 168.674225 5786.726563", "10926.111328 171.078613 6245.745117"}, //Oil Oil refineries
		};
		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Oil_refineries_SoundSet", Oil_refineries, DynamicMusicPlayerTimeOfDay.ANY, true);

		array<ref TVectorArray> Failed_project;
		Failed_project = {
			{"6528.913086 249.314224 9810.271484", "7553.573730 249.665131 10580.478516"}, //Failed project
		};
		RegisterTrackLocationStaticMultiRectangle("Music_nasdara_Failed_project_SoundSet", Failed_project, DynamicMusicPlayerTimeOfDay.ANY, true);

	}
//_______________________________________________daytime music_____________________________________________
		override protected void RegisterTracksTime()
	{
   	 	m_TracksTime = new array<ref DynamicMusicTrackData>();
    	//DAY
    	RegisterTrackTime("Music_nasdara_time_day_1_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
   		RegisterTrackTime("Music_nasdara_time_day_2_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_3_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_4_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_5_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_6_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_7_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_8_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackTime("Music_nasdara_time_day_9_SoundSet", DynamicMusicPlayerTimeOfDay.DAY);
		//DUSK
		RegisterTrackTime("Music_nasdara_time_dusk_1_SoundSet", DynamicMusicPlayerTimeOfDay.DUSK);
		RegisterTrackTime("Music_nasdara_time_dusk_2_SoundSet", DynamicMusicPlayerTimeOfDay.DUSK);
		//DAWN
		RegisterTrackTime("Music_nasdara_time_dawn_1_SoundSet", DynamicMusicPlayerTimeOfDay.DAWN);
		RegisterTrackTime("Music_nasdara_time_dawn_2_SoundSet", DynamicMusicPlayerTimeOfDay.DAWN);
		//NIGHT
		RegisterTrackTime("Music_nasdara_time_night_1_SoundSet", DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackTime("Music_nasdara_time_night_2_SoundSet", DynamicMusicPlayerTimeOfDay.NIGHT);
	}
};