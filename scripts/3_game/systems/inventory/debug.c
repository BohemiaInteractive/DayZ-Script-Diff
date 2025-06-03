void syncDebugPrint (string s)
{
#ifdef LOG_TO_RPT
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#endif
	
#ifdef LOG_TO_SCRIPT
	Print(string.Format("%1", s));
#endif
}

void actionDebugPrint (string s)
{
#ifdef LOG_TO_RPT
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#endif
	
#ifdef LOG_TO_SCRIPT
	Print(string.Format("%1", s));
#endif
}

void inventoryDebugPrint (string s)
{
#ifdef LOG_TO_RPT
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#endif
	
#ifdef LOG_TO_SCRIPT
	Print(string.Format("%1", s));
#endif
}