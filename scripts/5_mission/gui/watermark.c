class Watermark
{
	protected Widget m_Root;
	protected Widget m_WatermarkPanel;
	protected TextWidget m_WatermarkLabel;
	protected TextWidget m_WatermarkBuild;

	void Watermark(Widget parent)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud_watermark.layout", parent);
		
		m_WatermarkPanel = m_Root.FindAnyWidget("WatermarkPanel");
		m_WatermarkLabel = TextWidget.Cast(m_Root.FindAnyWidget("WatermarkLabel"));
		m_WatermarkBuild = TextWidget.Cast(m_Root.FindAnyWidget("WatermarkBuild"));

		m_WatermarkPanel.Show(true);
		m_WatermarkLabel.SetText("Testing Build");
		string buildVersion;
		GetGame().GetVersion(buildVersion);
		m_WatermarkBuild.SetText(buildVersion);
	}
}