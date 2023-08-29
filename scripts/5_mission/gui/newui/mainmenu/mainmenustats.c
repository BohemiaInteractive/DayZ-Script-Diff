class MainMenuStats extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	
	protected Widget m_TimeSurvived;
	protected TextWidget m_TimeSurvivedValue;
	
	protected Widget m_PlayersKilled;
	protected TextWidget m_PlayersKilledValue;
	
	protected Widget m_InfectedKilled;
	protected TextWidget m_InfectedKilledValue;
	
	protected Widget m_DistanceTraveled;
	protected TextWidget m_DistanceTraveledValue;
	
	protected Widget m_LongRangeShot;
	protected TextWidget m_LongRangeShotValue;
	
	private ref FullTimeData m_TimeSurvivedFull;
	
	void MainMenuStats(Widget root)
	{
		m_Root = root;
		
		m_TimeSurvivedFull = new FullTimeData();
		
		m_TimeSurvived = m_Root.FindAnyWidget("TimeSurvived");
		m_TimeSurvivedValue = TextWidget.Cast(m_Root.FindAnyWidget("TimeSurvivedValue"));
		
		m_PlayersKilled = m_Root.FindAnyWidget("PlayersKilled");
		m_PlayersKilledValue = TextWidget.Cast(m_Root.FindAnyWidget("PlayersKilledValue"));
		
		m_InfectedKilled = m_Root.FindAnyWidget("InfectedKilled");
		m_InfectedKilledValue = TextWidget.Cast(m_Root.FindAnyWidget("InfectedKilledValue"));
		
		m_DistanceTraveled = m_Root.FindAnyWidget("DistanceTraveled");
		m_DistanceTraveledValue = TextWidget.Cast(m_Root.FindAnyWidget("DistanceTraveledValue"));
		
		m_LongRangeShot = m_Root.FindAnyWidget("LongRangeShot");
		m_LongRangeShotValue = TextWidget.Cast(m_Root.FindAnyWidget("LongRangeShotValue"));
	}
		
	void ShowStats()
	{
		m_Root.Show(true);
		UpdateStats();
	}
	
	void HideStats()
	{
		m_Root.Show(false);
	}
	
	void UpdateStats()
	{
		PlayerBase player;
		MissionMainMenu missionMainMenu = MissionMainMenu.Cast(GetGame().GetMission());

		if (missionMainMenu && missionMainMenu.GetIntroScenePC())
		{
			#ifdef PLATFORM_WINDOWS
			player = missionMainMenu.GetIntroScenePC().GetIntroCharacter().GetCharacterObj();
			#endif
			#ifdef PLATFORM_CONSOLE
			player = missionMainMenu.GetIntroScenePC().GetIntroCharacter().GetCharacterObj();
			#endif
		
			if (player)
			{
				TimeConversions.ConvertSecondsToFullTime(player.StatGet(AnalyticsManagerServer.STAT_PLAYTIME), m_TimeSurvivedFull);			
				m_TimeSurvivedValue.SetText(m_TimeSurvivedFull.FormatedNonZero());
	
				m_PlayersKilledValue.SetText(GetValueString(player.StatGet(AnalyticsManagerServer.STAT_PLAYERS_KILLED)));
				m_InfectedKilledValue.SetText(GetValueString(player.StatGet(AnalyticsManagerServer.STAT_INFECTED_KILLED)));
				m_DistanceTraveledValue.SetText(GetDistanceString(player.StatGet(AnalyticsManagerServer.STAT_DISTANCE)));
				m_LongRangeShotValue.SetText(GetDistanceString(player.StatGet(AnalyticsManagerServer.STAT_LONGEST_SURVIVOR_HIT), true));
			}
		}
	}
	
	protected string GetDistanceString( float total_distance, bool meters_only = false )
	{
		if (total_distance > 0)
		{
			string distanceString;
			
			float kilometers = total_distance * 0.001;
			kilometers = Math.Round(kilometers);
			if ( kilometers >= 10 && !meters_only )
			{
				distanceString = GetValueString(kilometers, true) + " #STR_distance_unit_abbrev_kilometer_0";
			}
			else
			{
				distanceString = GetValueString(total_distance) + " #STR_distance_unit_abbrev_meter_0";
			}
			
			return distanceString;
		}	
	
		return "0" + " #STR_distance_unit_abbrev_meter_0";
	}
	
	protected string GetValueString(float total_value, bool show_decimals = false)
	{
		if (total_value > 0)
		{
			string out_string;
			
			int total_value_int = total_value;
			string number_str = total_value_int.ToString();
			
			//number
			if ( total_value >= 1000 )
			{
				int count;		
				int first_length = number_str.Length() % 3;		//calculate position of the first separator
				if ( first_length > 0 )
				{
					count = 3 - first_length;
				}
				
				for ( int i = 0; i < number_str.Length(); ++i )
				{
					out_string += number_str.Get( i );
					count ++;
					
					if ( count >= 3 )
					{
						out_string += " ";						//separator
						count = 0;
					}
				}
			}
			else
			{
				out_string = number_str;
			}
			
			//decimals
			if ( show_decimals )
			{
				string total_value_str = total_value.ToString();
				int decimal_idx = total_value_str.IndexOf( "." );
			
				if ( decimal_idx > -1 )
				{
					out_string.TrimInPlace();
					out_string += total_value_str.Substring( decimal_idx, total_value_str.Length() - decimal_idx );
				}
			}

			return out_string;
		}
		
		return "0";
	}

	//!	
	//! DEPRECATED
	//!
	protected string GetTimeString(float total_time);
}