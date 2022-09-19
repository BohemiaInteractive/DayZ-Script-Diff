
class HitDirectionImagesArrow_Static
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

class HitDirectionImagesArrow extends HitDirectionImagesBase
{
	override protected void PerformRegisterImages()
	{
		HitDirectionImagesArrow_Static.InitArrays();
		
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Arrow_0");
	}
	
	override protected ref array<string> GetTypeLayoutPathArray()
	{
		return HitDirectionImagesArrow_Static.m_ImagePathsLayouts;
	}
	
	override protected ref array<string> GetTypeImagePathArray()
	{
		return HitDirectionImagesArrow_Static.m_ImagePathsImages;
	}
}