class BannerHandlerBase extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_Parent; //frame
	protected Widget m_BannerBody;
	protected Widget m_BannerWedge;
	protected TextWidget m_BannerText;
	
	void BannerHandlerBase(Widget parent)
	{
		m_Parent = parent;
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/dlc_panels/BannerTemplate.layout", m_Parent);
		m_Root.SetHandler(this);
		
		m_BannerBody = m_Root.FindAnyWidget("BadgeBody");
		m_BannerWedge = m_Root.FindAnyWidget("BadgeWedge");
		m_BannerText = TextWidget.Cast(m_Root.FindAnyWidget("BadgeText"));
		
		m_BannerBody.SetAlpha(1.0);
		m_BannerWedge.SetAlpha(1.0);
	}
	
	void SetBannerColor(int argb)
	{
		m_BannerBody.SetColor(argb);
		m_BannerWedge.SetColor(argb);
	}
	
	void SetBannerText(string text)
	{
		m_BannerText.SetText(text);
	}
	
	override bool OnUpdate(Widget w)
	{
		if (w == m_BannerText)
		{
			float w0,h0,x0,y0;
			m_BannerText.GetScreenSize(w0,h0);
			float bannerBodyWidth = w0 + 20;
			
			m_BannerBody.SetScreenSize(bannerBodyWidth,h0);
			m_BannerBody.GetScreenPos(x0,y0);
			
			m_BannerWedge.GetScreenSize(w0,h0);
			m_BannerWedge.SetScreenPos(Math.Ceil(x0 - w0),y0);
		}
		
		return super.OnUpdate(w);
	}
}
