
class HitDirectionImagesSplash_Static
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

class HitDirectionImagesSplash extends HitDirectionImagesBase
{
	override protected void PerformRegisterImages()
	{
		HitDirectionImagesSplash_Static.InitArrays();
		
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Splash_0");
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Splash_1");
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Splash_2");
	}
	
	override protected ref array<string> GetTypeLayoutPathArray()
	{
		return HitDirectionImagesSplash_Static.m_ImagePathsLayouts;
	}
	
	override protected ref array<string> GetTypeImagePathArray()
	{
		return HitDirectionImagesSplash_Static.m_ImagePathsImages;
	}
}