class WorldTimeTracker
{
	private const float POLL_INTERVAL = 10.0;

    // Last sampled total minutes from calendar
    protected int m_LastTotalMinutes 	= -1;
    protected float m_PollAccumulated 	= 0.0;
	
	bool TryPollDelta(float timeSlice, out int outDeltaMinutes)
    {
        outDeltaMinutes = 0;

        m_PollAccumulated += timeSlice;
        if (m_PollAccumulated < POLL_INTERVAL)
            return false;

        m_PollAccumulated = 0.0;

        int year, month, day, hour, minute;
        g_Game.GetWorld().GetDate(year, month, day, hour, minute);
        int total = ToTotalMinutes(year, month, day, hour, minute);

        if (m_LastTotalMinutes >= 0)
        {
            int delta = total - m_LastTotalMinutes;
            if (delta < 0)
				delta = 0;

            outDeltaMinutes = delta;
        }

        m_LastTotalMinutes = total;

        return true;
    }

    // Convert calendar to monotonically increasing minutes.
    // Handles varying month lengths and leap years in a simple Gregorian manner
    protected int ToTotalMinutes(int year, int month, int day, int hour, int minute)
    {
        int days = DaysSinceYearZero(year) + DaysBeforeMonthInYear(year, month) + (day - 1);
        return days * 24 * 60 + hour * 60 + minute;
    }

    protected int DaysSinceYearZero(int year)
    {
        // Count days from year 0 to the start of 'year'
        // Gregorian leap year rules: leap if divisible by 4, not by 100 unless divisible by 400
        int y = year; // assuming year >= 0
        int leapDays = y / 4 - y / 100 + y / 400;
        return y * 365 + leapDays;
    }

    protected bool IsLeapYear(int year)
    {
        return ((year % 4) == 0 && (year % 100) != 0) || ((year % 400) == 0);
    }

    protected int DaysBeforeMonthInYear(int year, int month)
    {
        static const int daysInMonthNormal[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        static const int daysInMonthLeap[12]   = {31,29,31,30,31,30,31,31,30,31,30,31};

        int days = 0;
		int i = 0;
        if (IsLeapYear(year))
        {
            for (i = 0; i < month - 1; ++i)
                days += daysInMonthLeap[i];
        }
        else
        {
            for (i = 0; i < month - 1; ++i)
                days += daysInMonthNormal[i];
        }

        return days;
    }
}
