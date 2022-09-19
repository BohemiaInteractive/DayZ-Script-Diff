///@{ states

enum HandStateID
{
	UNKNOWN				=  0,
	Empty				=  1,
	Equipped			=  2,
}

class HandStateEmpty : HandStableState
{
	override void OnEntry(HandEventBase e)
	 { 
		if(e)
		{
			switch (e.m_EventID)
			{
				case HandEventID.MOVETO: 
					if (HumanInventory.HasInventoryReservation(e.GetSrcEntity(), e.GetDst()))
						HumanInventory.ClearInventoryReservation(e.GetSrcEntity(), e.GetDst());
					break;
				
				default: {};
			}
		}
		super.OnEntry(e); 
	}
	override void OnExit(HandEventBase e) { super.OnExit(e); }
	override int GetCurrentStateID() { return HandStateID.Empty; }
};

class HandStateEquipped : HandStableState
{
	override void OnEntry(HandEventBase e)
	{
		if(e)
		{
			switch (e.m_EventID)
			{
				case HandEventID.MOVETO: 
					if (HumanInventory.HasInventoryReservation(e.GetSrcEntity(), e.GetDst()))
						HumanInventory.ClearInventoryReservation(e.GetSrcEntity(), e.GetDst());
					break;
				
				default: {};
			} 
		}
		super.OnEntry(e); 
	}
	override void OnExit(HandEventBase e)
	{
		/*switch (e.m_EventID)
		{
			case HandEventID.MOVETO: 
				if (HumanInventory.HasInventoryReservation(e.GetSrcEntity(), e.GetDst()))
					HumanInventory.ClearInventoryReservation(e.GetSrcEntity(), e.GetDst());
				break;
				
			default: {};
		}*/
		super.OnExit(e);
	}
	override int GetCurrentStateID() { return HandStateID.Equipped; }
};

///@} states

