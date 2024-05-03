class DynamicMusicPlayerRegistryChernarus : DynamicMusicPlayerRegistry
{
	void DynamicMusicPlayerRegistryChernarus()
	{
		m_WorldName = "chernarusplus";
	}

	override protected void RegisterTracksLocationStatic()
	{
		super.RegisterTracksLocationStatic();

		//===================================================================================CHERNARUS=============================================================
		array<ref TVectorArray> militaryVybor;
		militaryVybor = {
			{"4358.785156 329.249908 9393.362305", "4691.308594 338.790955 9858.332031"},
			{"4703.648438 339.014618 9472.273438", "5177.216309 337.153534 9840.411133"},
			{"5278.045410 337.444946 9748.135742", "5418.036133 326.337585 9936.634766"},
			{"4519.662109 339.314636 10287.443359", "4819.628418 336.735168 10708.831055"},
			{"3974.624512 339.000000 10690.816406", "4651.811523 305.450165 11117.966797"},
			{"3811.918213 332.862915 10733.521484", "3929.790283 330.932190 10915.667969"},
			{"4371.768066 339.070038 9867.461914", "4907.290039 338.975433 10272.441406"},
			{"4134.639160 343.073975 10173.080078", "4495.437012 339.012451 10670.143555"},
			{"4922.312012 339.247559 9853.164063", "5023.402832 336.858826 10160.547852"},
		};
		
		array<ref TVectorArray> militaryTisy;
		militaryTisy = {
			{"1258.536011 442.738800 13465.949219", "1915.419189 424.005707 14172.190430"},
		};
		
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_vybor_day1_SoundSet", militaryVybor, DynamicMusicPlayerTimeOfDay.DAY, true);
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_vybor_day2_SoundSet", militaryVybor, DynamicMusicPlayerTimeOfDay.DAY, true);
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_vybor_night_SoundSet", militaryVybor, DynamicMusicPlayerTimeOfDay.NIGHT, true);

		RegisterTrackLocationStaticMultiRectangle("Music_loc_tisy_day1_SoundSet", militaryTisy, DynamicMusicPlayerTimeOfDay.DAY, true);
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_tisy_day2_SoundSet", militaryTisy, DynamicMusicPlayerTimeOfDay.DAY, true);
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_tisy_night1_SoundSet", militaryTisy, DynamicMusicPlayerTimeOfDay.NIGHT, true);
		//RegisterTrackLocationStaticMultiRectangle("Music_loc_tisy_night2_SoundSet", militaryTisy, DynamicMusicPlayerTimeOfDay.NIGHT, true);

		//____________________________________________ChernarusPlus CITIES ___________________________________________
		
		
		array<ref TVectorArray> cityChernohorsk;
		cityChernohorsk = {
			{"5531.164551 60.421795 2400.743408", "5955.600586 70.326355 2722.855225"},
			{"5344.257813 56.594906 2691.400391", "5977.086426 68.508591 2746.809082"},
			{"6248.862793 7.192856 2252.695313", "6827.800293 8.673743 2784.460938"},
			{"6067.001465 22.094645 2861.009277", "6232.067383 42.554207 3084.179199"},
			{"5992.256348 68.691139 3149.254883", "6246.539551 33.110256 3428.477783"},
			{"6479.552734 60.493557 3519.554688", "6893.115234 59.336658 3721.031250"},
			{"6889.792969 39.345371 3448.400635", "6959.889648 46.281517 3593.536377"},
			{"6433.864746 24.808739 3058.289795", "6535.033691 17.129761 3138.408447"},
			{"6304.493164 35.797741 3162.674561", "6356.386230 27.198618 3310.456299"},
			{"6653.054688 12.841890 2789.732666", "6550.689941 13.968339 3032.881104"},
			{"6255.447266 19.912449 2793.263672", "6543.220703 9.361076 2989.793945"},
			{"6838.630371 50.846977 3411.763916", "6882.331055 45.267555 3509.374756"},
		};
		
		array<ref TVectorArray> cityZelenogorsk;
		cityZelenogorsk = {
			{"2555.150879 193.533386 4996.022461", "2899.415771 217.191833 5516.551270"},
		};
		
		array<ref TVectorArray> cityNovajaPetrovka;
		cityNovajaPetrovka = {
			{"3214.297363 203.606903 12668.121094", "3530.579102 206.588135 13236.137695"},
			{"3538.789307 191.809509 13000.791992", "3788.247559 197.426666 13210.142578"},
		};

		array<ref TVectorArray> citySeverograd;
		citySeverograd = {
			{"7525.991211 133.878799 12476.297852", "8183.814453 115.107689 12864.666016"},
			{"8193.934570 113.964493 12672.379883", "8531.380859 121.646416 12957.698242"},
		};
		
		array<ref TVectorArray> cityNovodmytrovsk;
		cityNovodmytrovsk = {
			{"10535.823242 51.623943 14182.674805", "11536.229492 56.157513 14534.586914"},
			{"11255.142578 76.195442 14561.146484", "11657.960938 93.220894 14944.044922"},
			{"11551.236328 37.834187 14210.252930", "11855.493164 47.213768 14479.544922"},
		};

		array<ref TVectorArray> citySvetlojarsk;
		citySvetlojarsk = {		
			{"13697.645508 24.200567 13274.035156", "14053.754883 2.847323 13429.905273"},
			{"13765.426758 29.103195 13113.474609", "14162.206055 3.360178 13288.216797"},
			{"14006.562500 17.743019 13020.282227", "14212.198242 -3.626604 13113.509766"},
		};
		
		array<ref TVectorArray> cityBerezeno;
		cityBerezeno = {
			{"12029.467773 28.862682 9448.350586", "12500.858398 11.261748 9829.152344"},
			{"12588.754883 6.049923 9611.257813", "12915.958984 5.983838 9883.449219"},
			{"12754.895508 6.000570 9937.926758", "13191.422852 5.745190 10269.451172"},
			{"11680.861328 80.134071 9065.595703", "12102.810547 41.999142 9264.718750"},
			{"11842.054688 68.244682 8932.847656", "12158.123047 49.225658 9101.944336"},
			{"12582.141602 6.469708 9439.373047", "12741.715820 5.900000 9558.228516"},
			{"12020.833984 40.551315 9349.837891", "12114.671875 30.644287 9413.709961"},
		};
		
		array<ref TVectorArray> cityElectrozavodsk;
		cityElectrozavodsk = {
			{"10108.923828 19.349075 2254.205566", "10658.163086 28.823973 2596.111572"},
			{"9941.821289 6.559545 2087.425537", "10658.828125 6.528403 2253.264648"},
			{"10402.891602 7.446366 1960.392090", "10664.541016 5.829778 2200.995361"},
		};

		
		/*
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityChernohorsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityChernohorsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityChernohorsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityChernohorsk, DynamicMusicPlayerTimeOfDay.NIGHT);

		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityZelenogorsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityZelenogorsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityZelenogorsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityZelenogorsk, DynamicMusicPlayerTimeOfDay.NIGHT);

		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityNovajaPetrovka, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityNovajaPetrovka, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityNovajaPetrovka, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityNovajaPetrovka, DynamicMusicPlayerTimeOfDay.NIGHT);
		
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", citySeverograd, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", citySeverograd, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", citySeverograd, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", citySeverograd, DynamicMusicPlayerTimeOfDay.NIGHT);
		
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityNovodmytrovsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityNovodmytrovsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityNovodmytrovsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityNovodmytrovsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", citySvetlojarsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", citySvetlojarsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", citySvetlojarsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", citySvetlojarsk, DynamicMusicPlayerTimeOfDay.NIGHT);

		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityBerezeno, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityBerezeno, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityBerezeno, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityBerezeno, DynamicMusicPlayerTimeOfDay.NIGHT);
		
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_1_SoundSet", cityElectrozavodsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_day_2_SoundSet", cityElectrozavodsk, DynamicMusicPlayerTimeOfDay.DAY);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_1_SoundSet", cityElectrozavodsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		RegisterTrackLocationStaticMultiRectangle("Music_loc_city_night_2_SoundSet", cityElectrozavodsk, DynamicMusicPlayerTimeOfDay.NIGHT);
		*/
		
//____________________________________________ChernarusPlus Industrial zones Novodmytrovsk___________________________________________

		/*
		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"11854.528320 29.882481 14179.353516", "12478.435547 56.856274 14385.267578"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"11854.528320 29.882481 14179.353516", "12478.435547 56.856274 14385.267578"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"11854.528320 29.882481 14179.353516", "12478.435547 56.856274 14385.267578"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"11854.528320 29.882481 14179.353516", "12478.435547 56.856274 14385.267578"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);
		
//____________________________________________ChernarusPlus Industrial zones Chernogorsk___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"6933.279297 -7.615602 2519.416016", "7373.195801 13.949765 2970.550293"},
				{"7438.569336 5.645970 2927.914063", "8084.084473 6.028780 3287.097412"},
				{"7200.540527 6.000000 3192.891602", "7474.596191 5.915507 3381.304199"},
				{"7378.830078 4.619025 2598.827881", "7663.363281 5.787320 2912.731445"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"6933.279297 -7.615602 2519.416016", "7373.195801 13.949765 2970.550293"},
				{"7438.569336 5.645970 2927.914063", "8084.084473 6.028780 3287.097412"},
				{"7200.540527 6.000000 3192.891602", "7474.596191 5.915507 3381.304199"},
				{"7378.830078 4.619025 2598.827881", "7663.363281 5.787320 2912.731445"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"6933.279297 -7.615602 2519.416016", "7373.195801 13.949765 2970.550293"},
				{"7438.569336 5.645970 2927.914063", "8084.084473 6.028780 3287.097412"},
				{"7200.540527 6.000000 3192.891602", "7474.596191 5.915507 3381.304199"},
				{"7378.830078 4.619025 2598.827881", "7663.363281 5.787320 2912.731445"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"6933.279297 -7.615602 2519.416016", "7373.195801 13.949765 2970.550293"},
				{"7438.569336 5.645970 2927.914063", "8084.084473 6.028780 3287.097412"},
				{"7200.540527 6.000000 3192.891602", "7474.596191 5.915507 3381.304199"},
				{"7378.830078 4.619025 2598.827881", "7663.363281 5.787320 2912.731445"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Industrial zones Electrozavodsk___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"9866.310547 -15.653315 1599.896851", "10385.212891 6.046751 2074.640625"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"9866.310547 -15.653315 1599.896851", "10385.212891 6.046751 2074.640625"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"9866.310547 -15.653315 1599.896851", "10385.212891 6.046751 2074.640625"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"9866.310547 -15.653315 1599.896851", "10385.212891 6.046751 2074.640625"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Industrial zones Solnechnyj___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"12946.036133 16.273359 6957.259766", "13165.247070 5.991191 7206.662109"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"12946.036133 16.273359 6957.259766", "13165.247070 5.991191 7206.662109"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"12946.036133 16.273359 6957.259766", "13165.247070 5.991191 7206.662109"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"12946.036133 16.273359 6957.259766", "13165.247070 5.991191 7206.662109"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Industrial zones Severograd___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"8464.271484 137.924744 13101.271484", "8780.803711 161.241776 13672.717773"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"8464.271484 137.924744 13101.271484", "8780.803711 161.241776 13672.717773"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"8464.271484 137.924744 13101.271484", "8780.803711 161.241776 13672.717773"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"8464.271484 137.924744 13101.271484", "8780.803711 161.241776 13672.717773"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Traffic Jam 1 ___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"798.668945 5.972593 1913.950195", "897.092590 5.879053 2004.056396"},
				{"907.711365 5.772083 1993.015137", "987.267517 5.936453 2091.598145"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"798.668945 5.972593 1913.950195", "897.092590 5.879053 2004.056396"},
				{"907.711365 5.772083 1993.015137", "987.267517 5.936453 2091.598145"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"798.668945 5.972593 1913.950195", "897.092590 5.879053 2004.056396"},
				{"907.711365 5.772083 1993.015137", "987.267517 5.936453 2091.598145"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"798.668945 5.972593 1913.950195", "897.092590 5.879053 2004.056396"},
				{"907.711365 5.772083 1993.015137", "987.267517 5.936453 2091.598145"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Traffic Jam 2 ___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"239.559509 293.526276 9213.648438", "341.622986 294.961273 9320.047852"},
				{"276.458130 295.051697 9357.187500", "410.040009 297.106293 9480.652344"},
				{"417.146698 294.604492 9445.519531", "584.194885 300.853577 9496.502930"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"239.559509 293.526276 9213.648438", "341.622986 294.961273 9320.047852"},
				{"276.458130 295.051697 9357.187500", "410.040009 297.106293 9480.652344"},
				{"417.146698 294.604492 9445.519531", "584.194885 300.853577 9496.502930"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"239.559509 293.526276 9213.648438", "341.622986 294.961273 9320.047852"},
				{"276.458130 295.051697 9357.187500", "410.040009 297.106293 9480.652344"},
				{"417.146698 294.604492 9445.519531", "584.194885 300.853577 9496.502930"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"239.559509 293.526276 9213.648438", "341.622986 294.961273 9320.047852"},
				{"276.458130 295.051697 9357.187500", "410.040009 297.106293 9480.652344"},
				{"417.146698 294.604492 9445.519531", "584.194885 300.853577 9496.502930"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Traffic Jam 3 ___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"3284.781006 319.386780 14875.200195", "3374.985840 332.917664 15291.269531"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"3284.781006 319.386780 14875.200195", "3374.985840 332.917664 15291.269531"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"3284.781006 319.386780 14875.200195", "3374.985840 332.917664 15291.269531"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"3284.781006 319.386780 14875.200195", "3374.985840 332.917664 15291.269531"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

//____________________________________________ChernarusPlus Prisone ___________________________________________

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"2403.832031 -32.429996 1190.847412", "2676.229004 -8.220685 1427.551514"},
				{"2684.682861 -19.024158 1118.522705", "2906.357666 0.624884 1350.529175"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"2403.832031 -32.429996 1190.847412", "2676.229004 -8.220685 1427.551514"},
				{"2684.682861 -19.024158 1118.522705", "2906.357666 0.624884 1350.529175"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"2403.832031 -32.429996 1190.847412", "2676.229004 -8.220685 1427.551514"},
				{"2684.682861 -19.024158 1118.522705", "2906.357666 0.624884 1350.529175"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_vybor_night_SoundSet", //========> PLACEHOLDER
			{
				{"2403.832031 -32.429996 1190.847412", "2676.229004 -8.220685 1427.551514"},
				{"2684.682861 -19.024158 1118.522705", "2906.357666 0.624884 1350.529175"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);
		*/
		

//____________________________________________ Coast ___________________________________________
		
		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_coast_edit_1_SoundSet",
			{
				{"14479.316406 0.205862 13284.150391", "14915.232422 50.594284 13482.948242"},
				{"14915.232422 50.594284 13482.948242", "15195.464844 11.634924 13724.858398"},
				{"13762.034180 19.417923 11866.500000", "14291.090820 2.186958 12787.103516"},
				{"12719.478516 18.769157 8456.476563", "12982.782227 0.244197 9286.715820"},
				{"13360.052734 15.667940 4366.248047", "13576.785156 -1.456465 5987.672363"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_coast_edit_1_SoundSet",
			{
				{"12846.285156 4.942527 3677.016113", "13750.902344 2.102134 4208.992188"},
				{"8210.556641 -7.580429 2349.267822", "8594.726563 28.589079 2847.871338"},
				{"8480.291992 -53.893250 1986.895752", "9335.819336 56.478188 2335.644775"},
				{"4718.038086 -9.668183 1929.492188", "5822.236816 8.946076 2202.721191"},
				{"3749.452148 1.088811 2194.605957", "4241.458984 6.000000 2389.696777"},
				{"2299.152588 -6.205252 1592.642212", "3265.470947 11.660863 2304.694580"},
				{"977.166687 5.160747 1850.284424", "1563.582642 15.414894 2440.919434"},
				{"36.032646 -3.334796 1394.060669", "550.728577 6.195624 1748.442993"},
			},
			DynamicMusicPlayerTimeOfDay.DAY,
		);
		
		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_coast_edit_1_SoundSet",
			{
				{"14479.316406 0.205862 13284.150391", "14915.232422 50.594284 13482.948242"},
				{"14915.232422 50.594284 13482.948242", "15195.464844 11.634924 13724.858398"},
				{"13762.034180 19.417923 11866.500000", "14291.090820 2.186958 12787.103516"},
				{"12719.478516 18.769157 8456.476563", "12982.782227 0.244197 9286.715820"},
				{"13360.052734 15.667940 4366.248047", "13576.785156 -1.456465 5987.672363"},		
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);

		RegisterTrackLocationStaticMultiRectangle(
			"Music_loc_coast_edit_1_SoundSet",
			{
				{"12846.285156 4.942527 3677.016113", "13750.902344 2.102134 4208.992188"},
				{"8210.556641 -7.580429 2349.267822", "8594.726563 28.589079 2847.871338"},
				{"8480.291992 -53.893250 1986.895752", "9335.819336 56.478188 2335.644775"},
				{"44718.038086 -9.668183 1929.492188", "5822.236816 8.946076 2202.721191"},
				{"3749.452148 1.088811 2194.605957", "4241.458984 6.000000 2389.696777"},
				{"2299.152588 -6.205252 1592.642212", "3265.470947 11.660863 2304.694580"},
				{"977.166687 5.160747 1850.284424", "1563.582642 15.414894 2440.919434"},
				{"36.032646 -3.334796 1394.060669", "550.728577 6.195624 1748.442993"},
			},
			DynamicMusicPlayerTimeOfDay.NIGHT,
		);
	}

}
