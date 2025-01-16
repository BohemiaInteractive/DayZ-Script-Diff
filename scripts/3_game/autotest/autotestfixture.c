class AutoTestFixture
{
	private static const string REPORT_FILE_NAME_TEMPLATE = "$mission:Autotest_%1.xml";
	private static const string DATETIME_FORMAT = "%1%2%3T%4%5%6Z";

	private static string m_WorldName;

	static bool SaveXMLReport(string data, out string errorMessage)
	{		
		int year, month, day, hour, minute, second;
		GetYearMonthDayUTC(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
		string datetimeUTC	= string.Format(DATETIME_FORMAT, year, month.ToStringLen(2), day.ToStringLen(2), hour.ToStringLen(2), minute.ToStringLen(2), second.ToStringLen(2));
		string filename		= string.Format(REPORT_FILE_NAME_TEMPLATE, datetimeUTC);

		FileHandle handle = OpenFile(filename, FileMode.WRITE);
		if (handle == 0)
		{
			errorMessage = string.Format("Cannot open file \"%1\" for writing", filename);
			return false;
		}

		FPrint(handle, data);
		CloseFile(handle);

		return true;
	}
	
	static EntityAI SpawnEntityAI(string typeName, int flags = ECE_PLACE_ON_SURFACE)
	{
		EntityAI entity = EntityAI.Cast(g_Game.CreateObjectEx(typeName, g_Game.GetPlayer().GetPosition(), ECE_PLACE_ON_SURFACE));
		return entity;
	}
	
	static void SetWorldName()
	{
		string worldName = "empty";
		GetGame().GetWorldName(worldName);
		worldName.ToLower();

		m_WorldName = worldName;
	}
	
	static string GetWorldName()
	{
		return m_WorldName;
	}
	
	static void LogRPT(string message)
	{
		PrintToRPT(string.Format("[Autotest] %1", message));
	}
}
