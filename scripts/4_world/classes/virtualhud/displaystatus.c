enum DSLevels
{
	NORMAL = 0,//no bit, default
	WARNING = 1,//first bit
	CRITICAL = 2,//second bit
	BLINKING = 3,//first + second bit
	EXTRA = 4,//third bit
}

enum DSLevelsTemp
{
	NORMAL = 0,//no bit, default
	WARNING_MINUS = 1,//
	CRITICAL_MINUS = 2,//
	BLINKING_MINUS = 3,//
	WARNING_PLUS = 4,//
	CRITICAL_PLUS = 5,//
	BLINKING_PLUS = 6,//
}

class VirtualHud
{
	const int NUMBER_OF_MASKS = 2;//how many INT numbers we need to accommodate all elements
	ref array<int> m_LastSentArray;
	//ref map<int, ref DisplayElement> m_Elements;
	const int NUMBER_OF_ELEMENTS = eDisplayElements.COUNT;
	ref DisplayElementBase m_Elements[NUMBER_OF_ELEMENTS];
	Mission mission;
	Hud m_Hud;
	int m_LastTick;
	PlayerBase m_Player;
	string m_System 		= "VirtualHud";
	
	ref array<ref Param> rpcParams;
	
	void VirtualHud(PlayerBase player)
	{
		m_Player 		= player;
		m_LastTick		= 0;

		// !!!!!! don't add new stuff unless you really really REALLY know what you're doing !!!!!!
		RegisterElement(new BadgeStuffed(m_Player));// size 2
		RegisterElement(new BadgeWet(m_Player));// size 1
		RegisterElement(new BadgeSick(m_Player));// size 1
		RegisterElement(new BadgePills(m_Player));// size 1
		RegisterElement(new BadgePoisoned(m_Player));// size 1
		RegisterElement(new BadgeFracture(m_Player));// size 1
		RegisterElement(new TendencyHealth(m_Player));// size 6
		RegisterElement(new TendencyBlood(m_Player));// size 6
		RegisterElement(new TendencyTemperature(m_Player));// size 6
		// sum 25/32
		RegisterElement(new TendencyHunger(m_Player));// size 6
		RegisterElement(new TendencyThirst(m_Player));// size 6
		RegisterElement(new TendencyBacteria(m_Player));// size 6
		RegisterElement(new BadgeHeartbeat(m_Player));// size 2(client only)
		// sum 20/32

		RegisterElement(new ElementStance(m_Player));// size 0(client only)
		RegisterElement(new BadgeBleeding(m_Player));// size 0(client only)
		

		mission = GetGame().GetMission();
		if ( mission )
		{
			m_Hud = mission.GetHud();
		}
		//UpdateStatus();
	}
	
	
	void OnScheduledTick()
	{
		if( GetGame().IsServer() )
		{
			if(GetGame().GetTime() > (m_LastTick + VIRTUAL_HUD_UPDATE_INTERVAL))
			{
				SendRPC();
				m_LastTick = GetGame().GetTime();
			}
		}
		if( !GetGame().IsDedicatedServer() )
		{
			ImmediateUpdate();
			//DisplayPresence();
		}
	}

	void RegisterElement(DisplayElementBase element)
	{
		int id = element.GetType();
		m_Elements[id] = element;
		//Log("adding element:"+id.ToString());
	}
	
	DisplayElementBase GetElement(eDisplayElements element_id)
	{
		if( element_id < 0 || element_id >= NUMBER_OF_ELEMENTS )
		{
			return null;
		}
		return m_Elements[element_id];
	}

	void SerializeElements(ref array<int> mask_array)
	//this will serialize all elements and 'compresses' them into integer(s) through bit shifting, these integers are placed into an array
	{
		int offset = 0;
		int mask = 0;
		
		for(int i = 0; i < NUMBER_OF_ELEMENTS; i++)
		{
			if(  GetElement(i) && !GetElement(i).IsClientOnly() )
			{
				if( (GetElement(i).GetNumberOfBits() + offset) > BIT_INT_SIZE )
				{
					mask_array.Insert(mask);
					offset = 0;
					mask = 0;
				}
				mask = mask | (GetElement(i).GetValue() << offset);
				offset = offset + GetElement(i).GetNumberOfBits();
			}
		}
		mask_array.Insert(mask);
	}

	void DeserializeElements(ref array<int> mask_array)//extracts elements from mask
	{
		int maskArrayIndex = 0;
		int offset = 0;
		int mask = 0;
		
		for(int i = 0; i < NUMBER_OF_ELEMENTS;i++)
		{
			if( GetElement(i) && !GetElement(i).IsClientOnly() )
			{
				//Log("entity> " + ToString(GetElement(i)) );
				if(offset + GetElement(i).GetNumberOfBits() > BIT_INT_SIZE)
				{
					maskArrayIndex++;
					offset = 0;
				}
				mask = mask_array.Get(maskArrayIndex);
				int value = BitToDec( mask, offset, GetElement(i).GetCompareMask() );
				offset = offset + GetElement(i).GetNumberOfBits();
				GetElement(i).SetValue( value );
			}
		}
	}

	int BitToDec(int mask, int index, int compareMask)
	{
		int value = mask & (compareMask << index);
		value = value >> index;
		return value;
	}


	void PrintElements()
	{
		for(int i = 0; i < NUMBER_OF_ELEMENTS; i++)
		{
			PrintString(i.ToString() +": "+ GetElement(i).m_Value.ToString() );
		}
	}

	void SendRPC()
	{
		array<int> mask_array = new array<int>;
		SerializeElements(mask_array);
		if( !m_LastSentArray || !AreArraysSame(m_LastSentArray, mask_array)  )
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(mask_array);
			rpc.Send(m_Player, ERPCs.RPC_SYNC_DISPLAY_STATUS, false, m_Player.GetIdentity());
			m_LastSentArray = mask_array;
		}
	}

	bool AreArraysSame( notnull array<int> array_a, notnull array<int> array_b )
	{
		if( array_a.Count() != array_b.Count() ) return false;
		for(int i = 0; i  <array_a.Count(); i++)
		{
			if(	array_a.Get(i) != array_b.Get(i) )
			{
				return false;
			}
		}
		return true;
	}

	void ImmediateUpdate()
	{
		for(int i = 0; i < NUMBER_OF_ELEMENTS;i++)
		{
			DisplayElementBase element = GetElement(i);
			if( element && element.IsClientOnly() && element.IsValueChanged() ) element.UpdateHUD();
		}
	}
	/*
	void DisplayPresence()
	{
		if ( m_Hud )
		{
			m_Hud.DisplayPresence();
		}
	}
*/
	void UpdateStatus()
	{
		//Log("UpdateStatus called for entity: "+ToString(m_Player));
		for(int i = 0; i < NUMBER_OF_ELEMENTS; i++)
		{
			DisplayElementBase element = GetElement(i);
			if(  element && !element.IsClientOnly() && element.IsValueChanged() ) 
			{
				element.UpdateHUD();
			}
		}
	}

	void OnRPC(ParamsReadContext ctx)//on Client
	{
		//Log("OnRPC called");
		array<int> mask_array = new array<int>;
		ctx.Read(mask_array); 
		DeserializeElements(mask_array);
		UpdateStatus();
	}

	void Debug()
	{
		Log("debug");
		PluginPlayerStatus 	m_ModulePlayerStatus = PluginPlayerStatus.Cast(GetPlugin(PluginPlayerStatus));
		m_ModulePlayerStatus.DisplayTendency(NTFKEY_HUNGRY, 2);
	}
}