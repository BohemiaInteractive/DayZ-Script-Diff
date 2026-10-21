#ifdef WORKBENCH

typedef int[] ScriptEditor;
sealed class ScriptEditor: WBModuleDef
{
	proto external bool GetCurrentFile(out string filename);
	proto external int GetCurrentLine();
	proto external int GetLinesCount();
	//! Gets line text (if line is -1, current line is used).
	proto external bool GetLineText(out string text, int line = -1);
	//! Sets line text (if line is -1, current line is used).
	proto external void SetLineText(string text, int line = -1);
	//! Insert line before line (if line is -1, current line is used).
	proto external void InsertLine(string text, int line = -1);
	//! Removes line (if line is -1, current line is used).
	proto external void RemoveLine(int line = -1);
}

#endif