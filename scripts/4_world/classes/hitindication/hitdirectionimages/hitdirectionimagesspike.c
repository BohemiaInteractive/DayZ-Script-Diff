
class HitDirectionImagesSpike_Static
{
	static ref array<string> 	m_ImagePathsLayouts;
	static ref array<string> 	m_ImagePathsImages;
	
	static void InitArrays()
	{
		if ( !m_ImagePathsLayouts && !m_ImagePathsImages )
		{
			m_ImagePathsLayouts = new array<string>;
			m_ImagePathsImages = new array<string>;
		}
	}
}

class HitDirectionImagesSpike extends HitDirectionImagesBase
{
	override protected void PerformRegisterImages()
	{
		HitDirectionImagesSpike_Static.InitArrays();
		
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Spike_0");
	}
	
	override protected ref array<string> GetTypeLayoutPathArray()
	{
		return HitDirectionImagesSpike_Static.m_ImagePathsLayouts;
	}
	
	override protected ref array<string> GetTypeImagePathArray()
	{
		return HitDirectionImagesSpike_Static.m_ImagePathsImages;
	}
}