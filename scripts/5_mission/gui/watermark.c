class Watermark
{
	protected Widget m_Root;
	protected ImageWidget m_WatermarkImage;
	protected TextWidget m_WatermarkLabel;
	protected TextWidget m_WatermarkBuild;

	void Watermark(Widget parent = null)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud_watermark.layout", parent);
		
		m_WatermarkImage = ImageWidget.Cast(m_Root.FindAnyWidget("WatermarkLogo"));
		m_WatermarkLabel = TextWidget.Cast(m_Root.FindAnyWidget("WatermarkLabel"));
		m_WatermarkBuild = TextWidget.Cast(m_Root.FindAnyWidget("WatermarkBuild"));
		m_WatermarkImage.LoadImageFile(0, "set:dayz_gui image:DayZLogo");
		m_WatermarkLabel.SetText("EXPERIMENTAL BUILD");

		string buildVersion;
		GetGame().GetVersion(buildVersion);
		m_WatermarkBuild.SetText(buildVersion);
	}
}