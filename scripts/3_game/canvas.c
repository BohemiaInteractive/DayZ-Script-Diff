/*!
Simple debug painting canvas with 0/1 pixel representation.
Usage:
	Canvas c = new Canvas(100,100);
	for(int i = 0; i < 100;i++)
	{
		float val = i/100;
		float y = Easing.EaseInOutExpo(val);
		c.DrawPixel(i,y*100);
	}
	c.SaveToFile("output");
*/

typedef bool PIXEL;
class Canvas
{
	int m_SizeX; int m_SizeY;
	
	ref array<ref array<PIXEL>> m_Pixels = new array<ref array<PIXEL>>;
	
	void Canvas(int size_x, int size_y)
	{
		m_SizeX = size_x;
		m_SizeY = size_y;
		for(int i = 0; i < size_y; i++)
		{
			array<PIXEL> x_line = new array<PIXEL>;
			for(int z = 0; z < size_x;z++)
			{
				x_line.Insert(false);
			}
			m_Pixels.Insert(x_line);
			
		}
	}
	
	void DrawPixel(int x, int y)
	{
		if((x > m_SizeX - 1) || (y > m_SizeY - 1))
			return;
		//Print("x:" +x+",y:"+y);
		m_Pixels.Get(y).InsertAt(true, x);
	}
	
	void PrintOut()
	{
		string line = "";
		int y_lines = m_SizeY - 1;
		for(int i = y_lines; i >= 0; i--)
		{
			line = "";
			for(int z = 0; z < m_SizeX;z++)
			{
				if(m_Pixels.Get(i).Get(z))
				{
					line += "X";
				}
				else
				{
					line += " ";
				}
			}
			Print(line);
		}
	}
	
	void SaveToFile(string filename)
	{
		FileHandle file = OpenFile("$profile:"+filename, FileMode.WRITE);
		
		string line = "";
		int y_lines = m_SizeY - 1;
		for(int i = y_lines; i >= 0; i--)
		{
			line = "";
			for(int z = 0; z < m_SizeX;z++)
			{
				if(m_Pixels.Get(i).Get(z))
				{
					line += "X";
				}
				else
				{
					line += " ";
				}
			}
			FPrintln(file, line);
		}
	}
}