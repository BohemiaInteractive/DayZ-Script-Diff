void syncDebugPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

void actionDebugPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

void inventoryDebugPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}