// -----------------------------------------------------------
class AutoHeightSpacer : ScriptedWidgetEventHandler
{
	reference bool AlignChilds;
	reference int MinHeight;
	reference int MaxHeight;
	reference int Gap;
	reference float Top;
	protected Widget m_root;

	void Update()
	{
		float x = 0;
		float y = 0;
		float width = 0;
		float height = 0;
		float heightOld = 0;
		float top = Top;
		float rowRight;
		float rowHeight;
		float rowWidth;
		Widget child = m_root.GetChildren();

		//PrintString(m_root.GetName() + ": AutoHeightSpacer::Update()");

		if ( !AlignChilds )	top = -100000;

		if (child != NULL)
		{
		// first row init
		m_root.GetScreenSize(rowWidth, height);
		rowHeight = 0;
		rowRight = rowWidth;

		while (child)
		{
			if (child.IsVisible() == false || child.GetName() == "SelectedContainer" || child.GetName() == "Icon")
			{
				// skip invisible widgets
				child = child.GetSibling();
				continue;
			}

			child.GetScreenSize(width, height);

			if (AlignChilds)
			{
				child.SetFlags(WidgetFlags.EXACTPOS, false);

				// no space left in this row, move to next one
				if (rowRight < width)
				{
					top += rowHeight;
					if ( rowHeight > 0 ) top += Gap;
					rowRight = rowWidth;
					rowHeight = 0;
				}

				// increase row height if necessary
				if (height > rowHeight) rowHeight = height;

				child.SetPos(rowWidth - rowRight, top, false);
				rowRight -= width + Gap;
			}
			else
			{
				child.GetScreenPos(x, y);
				y += height;
				if (top < y) top = y;
			}

			child = child.GetSibling();
		}

		// add last row height;
		top += rowHeight;

		if (AlignChilds)
		{
			height = top;
		}
		else
		{
			m_root.GetScreenPos(x, y);
			height = top - y;
		}
		}

		m_root.GetSize(width, heightOld);

		if (MaxHeight > 0 && height > MaxHeight)
		{
			height = MaxHeight;
		}

		if (MinHeight > height)
		{
			height = MinHeight;
		}

		if (Math.AbsInt(heightOld - height) > 1)
		{
			m_root.SetSize(width, height);
		}
		else if (AlignChilds)
		{
			m_root.Update();
		}

		return;
	}

	void OnWidgetScriptInit(Widget w)
	{
		m_root = w;
		m_root.SetHandler(this);
		m_root.SetFlags(WidgetFlags.VEXACTPOS);
		Update();
	}

	override bool OnChildRemove( Widget  w, Widget  child) {
		if (w == m_root)
		{
			Update();
		}

		return false;
	}
};
