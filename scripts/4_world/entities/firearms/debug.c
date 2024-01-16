void wpnPrint (string s)
{
#ifdef WPN_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see at least important info
#else
	//Print("" + s); // comment/uncomment to hide/see at least important info
#endif
}
void wpnDebugPrint (string s)
{
#ifdef WPN_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void wpnDebugSpam (string s)
{
#ifdef WPN_DEBUG_SPAM
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void wpnDebugSpamALot (string s)
{
#ifdef WPN_DEBUG_SPAM_FREQ
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}

// @NOTE: to see output from FSM transitions, go to:
//			4_World/Entities/HFSMBase.c
// @see fsmDebugPrint @see fsmDebugSpam

