/**@class		FullTimeData
 * @brief		struct that keeps Time relevant information for future formatting
 **/
class FullTimeData
{
	int m_Days
	int m_Hours
	int m_Minutes
	int m_Seconds
	
	string FormatedAsTimestamp()
	{
		m_Hours += m_Days * 24;
		
		string h;
		string m;
		string s;
		
		if (m_Hours < 10)
			h = "0";
		
		if (m_Minutes < 10)
			m = "0";
		
		if (m_Seconds < 10)
			s = "0";
		
		return string.Format("%1%2:%3%4:%5%6", h, m_Hours, m, m_Minutes, s, m_Seconds);
	}
	
	string FormatedWithZero()
	{
		return string.Format("%1#STR_time_unit_abbrev_day_0 %2#STR_time_unit_abbrev_hour_0 %3#STR_time_unit_abbrev_minute_0 %4#STR_time_unit_abbrev_second_0", m_Days, m_Hours, m_Minutes, m_Seconds);
	}
	
	string FormatedNonZero()
	{
		string message;
		
		if (m_Days > 0)
			message += string.Format("%1#STR_time_unit_abbrev_day_0 ", m_Days);
		if (m_Hours > 0)
			message += string.Format("%1#STR_time_unit_abbrev_hour_0 ", m_Hours);
		if (m_Minutes > 0)
			message += string.Format("%1#STR_time_unit_abbrev_minute_0 ", m_Minutes);

		message += string.Format("%1#STR_time_unit_abbrev_second_0", m_Seconds);
		
		return message;
	}
}

class TimeConversions
{
	/**
	  	\brief transform time in seconds into FullTimeData struct
		@param timeInSeconds time in seconds
		@param fullTime time output as FullTimeData struct
	*/
	static void ConvertSecondsToFullTime(int timeInSeconds, out FullTimeData fullTime)
	{
	    fullTime.m_Days = timeInSeconds / (24 * 3600);
	 
	    timeInSeconds = timeInSeconds % (24 * 3600);
	    fullTime.m_Hours = timeInSeconds / 3600;
	 
	    timeInSeconds = timeInSeconds % 3600;
	    fullTime.m_Minutes = timeInSeconds / 60;
	 
	    timeInSeconds = timeInSeconds % 60;
	    fullTime.m_Seconds = timeInSeconds;
	}
}
