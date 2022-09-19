// #include "Scripts\Classes\Gui\ChatLine.c"

/*!
	channel type, possible values
	CCSystem(1)
	CCAdmin(2)
	CCDirect(4)
	CCMegaphone(8)
	CCTransmitter(16)
	CCPublicAddressSystem(32)
	CCBattlEye(64)
*/

class Chat
{
	const int LINE_COUNT = 12;

	protected Widget					m_RootWidget;
	protected int						m_LineHeight;
	protected int						m_LastLine;
	protected ref array<ref ChatLine>	m_Lines;

	void Chat()
	{
		m_Lines = new array<ref ChatLine>;
	}

	void ~Chat()
	{
		Destroy();
	}
	
	void Init(Widget root_widget)
	{
		Destroy();

		m_RootWidget = root_widget;

		if (m_RootWidget)
		{
			float w;
			float h;
			m_RootWidget.GetSize(w,h);
			m_LineHeight = h / LINE_COUNT;
			m_LastLine = 0;

			for (int i = 0; i < LINE_COUNT; i++)
			{
				ChatLine line = new ChatLine(m_RootWidget);
				m_Lines.Insert(line);
			}
		}
	}

	void Destroy()
	{
		m_Lines.Clear();
	}
	
	void Clear()
	{
		for (int i = 0; i < LINE_COUNT; i++)
		{
			m_Lines.Get(i).Clear();
		}
	}
	
	void Add(ChatMessageEventParams params)
	{
		int max_lenght = ChatMaxUserLength;
		int name_lenght = params.param2.Length();
		int text_lenght = params.param3.Length();
		int total_lenght = text_lenght + name_lenght;
		int channel =  params.param1;

		if( channel & CCSystem || channel & CCBattlEye) //TODO separate battleye bellow
 		{
			if( g_Game.GetProfileOption( EDayZProfilesOptions.GAME_MESSAGES ) )
				return;
			
			max_lenght = ChatMaxSystemLength; // system messages can be longer
 		}
		//TODO add battleye filter to options
		/*else if( channel & CCBattlEye ) 
		{
			if( g_Game.GetProfileOption( EDayZProfilesOptions.BATTLEYE_MESSAGES ) )
				return;
		}*/
		else if( channel & CCAdmin )
		{
			if( g_Game.GetProfileOption( EDayZProfilesOptions.ADMIN_MESSAGES ) )
				return;
		}
		else if( channel & CCDirect || channel & CCMegaphone || channel & CCTransmitter || channel & CCPublicAddressSystem ) 
		{
			if( g_Game.GetProfileOption( EDayZProfilesOptions.PLAYER_MESSAGES ) )
				return;
		}
		else if( channel != 0 ) // 0 should be local messages to self
		{
			Print("Chat: Unknown channel " + channel);
			return;
		}
		
		if (total_lenght > max_lenght)
		{
			int pos = 0;
			int lenght = Math.Clamp(max_lenght - name_lenght, 0, text_lenght);
			ref ChatMessageEventParams tmp = new ChatMessageEventParams(params.param1, params.param2, "", params.param4);
			
			while (pos < text_lenght)
			{
				tmp.param3 = params.param3.Substring(pos, lenght);
				AddInternal(tmp);
				
				tmp.param2 = "";
				pos += lenght;
				lenght = Math.Clamp(text_lenght - pos, 0, max_lenght);			
			}
		}
		else
		{
			AddInternal(params);
		}
	}
	
	void AddInternal(ChatMessageEventParams params)
	{
		m_LastLine = (m_LastLine + 1) % m_Lines.Count();

		ChatLine line = m_Lines.Get(m_LastLine);
		line.Set(params);

		for (int i = 0; i < m_Lines.Count(); i++)
		{
			line = m_Lines.Get((m_LastLine + 1 + i) % LINE_COUNT);
			line.m_RootWidget.SetPos(0, i * m_LineHeight);
			
			float x = 0;
			float y = 0;
			
			line.m_RootWidget.GetPos(x, y);
		}
	}
}
