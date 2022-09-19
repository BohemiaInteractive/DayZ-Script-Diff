void wpnPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see at least important info
#else
	//Print("" + s); // comment/uncomment to hide/see at least important info
#endif
}
void wpnDebugPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void wpnDebugSpam (string s)
{
	//Print("" + s); // comment/uncomment to hide/see debug spam
}
void wpnDebugSpamALot (string s)
{
	//Print("" + s); // comment/uncomment to hide/see debug spam
}

// @NOTE: to see output from FSM transitions, go to:
//			4_World/Entities/HFSMBase.c
// @see fsmDebugPrint @see fsmDebugSpam

