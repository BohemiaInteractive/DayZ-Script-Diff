
class HitDirectionImagesBase
{
	int 						m_ImageIndex;
	
	void HitDirectionImagesBase()
	{
		if ( (!GetTypeLayoutPathArray() && !GetTypeImagePathArray()) )
		{
			PerformRegisterImages();
		}
		
		RandomizeImageIdx();
	}
	
	protected void PerformRegisterImages()
	{
		/*
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Splash_1");
		RegisterImage("gui/layouts/gameplay/HitDirectionIndication.layout","Single_indicator_Splash_2");
		...
		*/
	}
	
	protected void RegisterImage(string layout_path, string image_name)
	{
		GetTypeLayoutPathArray().Insert(layout_path);
		GetTypeImagePathArray().Insert(image_name);
	}
	
	protected void RandomizeImageIdx()
	{
		m_ImageIndex = Math.RandomIntInclusive(0,GetTypeImagePathArray().Count() - 1);
	}
	
	protected array<string> GetTypeLayoutPathArray(){}
	protected array<string> GetTypeImagePathArray(){}
	
	//! Layout should be sufficient for generic manipulation of the hit image
	void GetCurrentImageData(out Widget layout, out Widget image)
	{
		layout = GetGame().GetWorkspace().CreateWidgets(GetTypeLayoutPathArray()[m_ImageIndex]);
		image = layout.FindAnyWidget(GetTypeImagePathArray()[m_ImageIndex]);
	}
}