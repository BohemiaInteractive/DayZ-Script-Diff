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
	
	void MainMenuStats( Widget root )
	{
		m_Root = root;
		
		m_TimeSurvived = m_Root.FindAnyWidget( "TimeSurvived" );
		m_TimeSurvivedValue = TextWidget.Cast( m_Root.FindAnyWidget( "TimeSurvivedValue" ) );
		
		m_PlayersKilled = m_Root.FindAnyWidget( "PlayersKilled" );
		m_PlayersKilledValue = TextWidget.Cast( m_Root.FindAnyWidget( "PlayersKilledValue" ) );
		
		m_InfectedKilled = m_Root.FindAnyWidget( "InfectedKilled" );
		m_InfectedKilledValue = TextWidget.Cast( m_Root.FindAnyWidget( "InfectedKilledValue" ) );
		
		m_DistanceTraveled = m_Root.FindAnyWidget( "DistanceTraveled" );
		m_DistanceTraveledValue = TextWidget.Cast( m_Root.FindAnyWidget( "DistanceTraveledValue" ) );
		
		m_LongRangeShot = m_Root.FindAnyWidget( "LongRangeShot" );
		m_LongRangeShotValue = TextWidget.Cast( m_Root.FindAnyWidget( "LongRangeShotValue" ) );
	}
		
	void ShowStats()
	{
		m_Root.Show( true );
		UpdateStats();
	}
	
	void HideStats()
	{
		m_Root.Show( false );
	}
	
	void UpdateStats()
	{
		PlayerBase player;
		MissionMainMenu mission_main_menu = MissionMainMenu.Cast( GetGame().GetMission() );
		
		#ifdef PLATFORM_WINDOWS
			player = mission_main_menu.GetIntroScenePC().GetIntroCharacter().GetCharacterObj();
		#endif
		#ifdef PLATFORM_CONSOLE
			player = mission_main_menu.GetIntroScenePC().GetIntroCharacter().GetCharacterObj();
		#endif
		
		if ( player )
		{
			float stat_value;
			string stat_text;
			
			m_TimeSurvivedValue.SetText( GetTimeString( player.StatGet( AnalyticsManagerServer.STAT_PLAYTIME ) ) );
			m_PlayersKilledValue.SetText( GetValueString( player.StatGet( AnalyticsManagerServer.STAT_PLAYERS_KILLED ) ) );
			m_InfectedKilledValue.SetText( GetValueString( player.StatGet( AnalyticsManagerServer.STAT_INFECTED_KILLED ) ) );
			m_DistanceTraveledValue.SetText( GetDistanceString( player.StatGet( AnalyticsManagerServer.STAT_DISTANCE ) ) );
			m_LongRangeShotValue.SetText( GetDistanceString( player.StatGet( AnalyticsManagerServer.STAT_LONGEST_SURVIVOR_HIT ), true ) );
		}
	}
	
	protected string GetTimeString( float total_time )
	{
		string day_symbol = "d";							//define symbols
		string hour_symbol = "h";
		string minute_symbol = "min";
		
		if ( total_time > 0 )
		{
			string time_string;		
			int time_seconds = total_time; 						//convert total time to int
			
			int days = time_seconds / 3600 / 24;
			int hours = time_seconds / 3600 % 24;
			int minutes = ( time_seconds % 3600 ) / 60;
			
			if ( days > 0 )
			{
				time_string += GetValueString( days ) + day_symbol;		//days
				time_string += " ";										//separator
			}
			
			if ( hours > 0 || days > 0 )
			{
				time_string += GetValueString( hours ) + hour_symbol;	//hours
				time_string += " ";										//separator
			}			

			if ( minutes >= 0 )
			{
				time_string += GetValueString( minutes ) + minute_symbol;	//minutes
			}			

			return time_string;
		}
		
		return "0" + " " + minute_symbol;
	}
	
	protected string GetDistanceString( float total_distance, bool meters_only = false )
	{
		string meter_symbol = "m";							//define symbols
		string kilometer_symbol = "km";
		
		if ( total_distance > 0 )
		{
			string distance_string;
			
			float kilometers = total_distance / 1000;
			kilometers = Math.Round( kilometers );
			if ( kilometers >= 10 && !meters_only )
			{
				distance_string = GetValueString( kilometers, true ) + " " + kilometer_symbol;		//kilometers
			}
			else
			{
				distance_string = GetValueString( total_distance ) + " " + meter_symbol;			//meters
			}
			
			return distance_string;
		}	
	
		return "0" + " " + meter_symbol;
	}
	
	protected string GetValueString( float total_value, bool show_decimals = false )
	{
		if ( total_value > 0 )
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
}