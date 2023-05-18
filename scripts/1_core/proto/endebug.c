/**
 * \defgroup Debug Debug utilities
 * @{
 */

/**
\brief Prints current call stack (stack trace)
	\return \p void
	@code
		DumpStack();

	@endcode

	\verbatim
	Output:
		-- Stack trace --
	   SaveFile() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler.c : 51
	   SaveConfigToFile() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 114
	   SaveParameterArray() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 133
	   SetParameterArray() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 231
	   PresetAdd() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile\ModuleLocalProfileUI.h : 46
	   OnKeyPress() Scripts/mission/missionGameplay.c : 215
	   OnKeyPress() Scripts/DayZGame.c : 334
	   -----------------
	\endverbatim
*/
proto void DumpStack();

/**
\brief Prints current call stack (stack trace) to given output
	\return \p void
	@code
    string tmp;
		DumpStackString(tmp);
    Print(tmp);

	@endcode

	\verbatim
	Output:
	  SaveFile() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler.c : 51
	  SaveConfigToFile() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 114
	  SaveParameterArray() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 133
	  SetParameterArray() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile.c : 231
	  PresetAdd() Scripts\Entities\Modules\ModuleBase\ModuleFileHandler\ModuleLocalProfile\ModuleLocalProfileUI.h : 46
	  OnKeyPress() Scripts/mission/missionGameplay.c : 215
	  OnKeyPress() Scripts/DayZGame.c : 334
	\endverbatim
*/
proto void DumpStackString(out string stack);

//! Triggers breakpoint in C++ in run time(when app is running in debug enviroment)
proto void DebugBreak(bool condition = true, void param1 = NULL, void param2 = NULL, void param3 = NULL, void param4 = NULL, void param5 = NULL, void param6 = NULL, void param7 = NULL, void param8 = NULL, void param9 = NULL);

//! Triggers breakpoint in C++ in compile time(when app is running in debug enviroment)
void CompileBreak();

//! Prints content of variable to console/log. Should be used for critical messages so it will appear in debug log
proto void DPrint(string var);

enum ErrorExSeverity
{
	INFO,
	WARNING,
	ERROR,
}

/**
\brief Error message, prefixed by method name, above 'INFO' will show a messagebox
	\note Format: [%className%::%methodName%] :: [%severity%] :: %errString%
	\param \p string Error message to use
	@code
	class ErrorExTest
	{
		void ThrowWarning()
		{
			// [ErrorExTest::ThrowWarning] :: [WARNING] :: This is a warning.
			ErrorEx("This is a warning.", ErrorExSeverity.WARNING);		
		}
	}
	@endcode
*/
proto void ErrorEx(string err, ErrorExSeverity severity = ErrorExSeverity.ERROR);
proto void ErrorExString(string err, out string str, ErrorExSeverity severity = ErrorExSeverity.ERROR);

//! Messagebox with error message
proto native void Error2(string title, string err);

//! Messagebox with error message
void Error(string err)
{
	Error2("", err);
}

//!Prints content of variable to console/log
proto void Print(void var);

//!Prints content of variable to RPT file (performance warning - each write means fflush! use with care)
proto void PrintToRPT(void var);
	
/**
\brief Prints formated text to console/log
	@code
		string c = "Peter";
		PrintFormat("Hello %1, how are you?", c); // prints "Hello 'Peter', how are you?"
	@endcode
*/	
proto void PrintFormat(string fmt, void param1 = NULL, void param2 = NULL, void param3 = NULL, void param4 = NULL, void param5 = NULL, void param6 = NULL, void param7 = NULL, void param8 = NULL, void param9 = NULL);

	//------------------------------------------
/**
 * \defgroup DebugShape Debug Shape API definition
 * @{
 */
enum ShapeType
{
	BBOX,			//<	Just box
	LINE,			//<	One or more lines
	SPHERE,		//<	Sphere represented by triangle mesh
	CYLINDER,	//<	Cylinder represented by triangle mesh
	DIAMOND,	//< Eight faced pyramid. Defined by bound-box, where middle part is  equal to horizontal extents of box and top/bottom apogees lies on top/bottom side of box.
	PYRAMID	//< Four sided pyramid. Defined by bound-box, where base is equal to bottom side of box.
};

enum ShapeFlags
{
	NOZBUFFER,	//< Do not compare z-buffer when render
	NOZWRITE,		//< Do not update z-buffer when render
	WIREFRAME,	//< Render just wire-frame outline. No solid faces
	TRANSP,			//< Is translucent
	DOUBLESIDE,	//< Double-sided (do not cull back-faces)
	ONCE,				//< Rendered just once and then it's automatically destroyed. Do not keep pointer to these!!
	NOOUTLINE,	//< Render just solid faces. No wire-frame outline.
	BACKFACE,		//< Render just back faces
	NOCULL,			//< Do not cull shapes by view frustum
	VISIBLE,		//< Make it visible. Set by default
	ADDITIVE		//< Additive blending (works with ShapeFlags.TRANSP)
};

enum CollisionFlags
{
	FIRSTCONTACT,	//<In many cases only collided=true/false is enough
	NEARESTCONTACT	//<We want only one, the nearest contact
	ONLYSTATIC		//<Only static objects
	ONLYDYNAMIC		//<Only dynamic objects
	ONLYWATER		//<Only water components (legacy support for "walk on geometry")
	ALLOBJECTS		//<Valid when CF_FIRST_CONTACT, we get first contact for each object
}
		
/*!
Instance of created debug visualizer
*/
class Shape
{
	//!don't call destructor directly. Use Destroy() instead
	proto private void ~Shape();

	proto native void GetMatrix(out vector mat[4]);
	proto native void SetMatrix(vector mat[4]);
	proto native void SetDirection(vector direction);
	proto native void SetPosition(vector position);
	proto native void SetColor(int color);
	proto native void SetFlags(ShapeFlags flags);
	proto native void Destroy();

	proto static native Shape Create(ShapeType type, int color, ShapeFlags flags, vector p1, vector p2);
	proto static native Shape CreateLines(int color, ShapeFlags flags, vector p[], int num);
	proto static native Shape CreateTris(int color, ShapeFlags flags, vector p[], int num);
	proto static native Shape CreateSphere(int color, ShapeFlags flags, vector origin, float radius);
	proto static native Shape CreateFrustum(float horizontalAngle, float verticalAngle, float length, int color, ShapeFlags flags);
	proto static native Shape CreateCylinder(int color, ShapeFlags flags, vector origin, float radius, float length);

	static Shape CreateArrow(vector from, vector to, float size, int color, ShapeFlags flags)
	{
		vector dir = to - from;
		dir.Normalize();
		vector dir1 = dir * size;
		size = size * 0.5;
	
		vector dir2 = dir.Perpend() * size;
	
		vector pts[5];
		pts[0] = from;
		pts[1] = to;
		pts[2] = to - dir1 - dir2;
		pts[3] = to - dir1 + dir2;
		pts[4] = to;
	
		return CreateLines(color, flags, pts, 5);
	}

	static Shape CreateBridgeArrow(vector from, vector to, float size, int color, ShapeFlags flags)
	{
		vector dir = to - from;
		dir.Normalize();
	
		vector dir1 = Vector(0, 0, -size);
		size = size * 0.5;
	
		vector dir2 = dir.Perpend() * size;
	
		vector pts[7];
		pts[0] = from;
		pts[1] = from + "0 0 1";
		pts[2] = to + "0 0 1";
		pts[3] = to;
		pts[4] = to - dir1 - dir2;
		pts[5] = to - dir1 + dir2;
		pts[6] = to;
	
		return CreateLines(color, flags, pts, 7);
	}

	static void CreateMatrix(vector mat[4])
	{
		vector org = mat[3];
		int flags = ShapeFlags.NOZWRITE|ShapeFlags.DOUBLESIDE|ShapeFlags.TRANSP|ShapeFlags.ONCE;
		Create(ShapeType.LINE, 0xffff0000, flags, org, mat[0] * 0.5 + org);
		Create(ShapeType.LINE, 0xff00ff00, flags, org, mat[1] * 0.5 + org);
		Create(ShapeType.LINE, 0xff0000ff, flags, org, mat[2] * 0.5 + org);
	}
};

//@}


//------------------------------------------
/**
 * \defgroup DiagMenu Diag menu API definition
 * @{
 */
class DiagMenu
{
	//! Checks if DiagMenu is initialized
	static proto bool IsInitialized();
	
	//! To be used before registering scripted diags
	static proto void InitScriptDiags();
	//! To be used when scripted diags should not be present
	static proto void ClearScriptDiags();
	
	/**
	\brief Register a new menu
		\param id \p int The unique ID of the menu in the range [0,512]
		\param name \p string The name of the menu
		\param parent \p int The index of the parent of the menu
	*/
	static proto void RegisterMenu(int id, string name, int parent);
	
	/**
	\brief Register a new item
		\param id \p int The unique ID of the item in the range [0,512]
		\param shortcut \p string The keyboard shortcut of the item
		\param name \p string The name of the item
		\param parent \p int The index of the parent of the item
		\param values \p string The values of the item separated by commas, internally this will be an int starting at 0 for the first item
		\param callback \p func Callback to call when the value is changed (OPTIONAL) (Also read BindCallback)
	*/
	static proto void RegisterItem(int id, string shortcut, string name, int parent, string values, func callback = null);
	
	/**
	\brief Register a new bool item
	\note This is just a RegisterItem with value="true,false" or when reversed value="false,true"
		\param id \p int The unique ID of the item in the range [0,512]
		\param shortcut \p string The keyboard shortcut of the item
		\param name \p string The name of the item
		\param parent \p string The index of the parent of the item
		\param values \p string The values of the item, separated by commas
		\param reverse \p bool Whether to reverse the bool (OPTIONAL)
		\param callback \p func Callback to call when the value is changed (OPTIONAL) (Also read BindCallback)
	*/
	static proto void RegisterBool(int id, string shortcut, string name, int parent, bool reverse = false, func callback = null);

	/**
	\brief Register a new range item
		\param id \p int The unique ID of the item in the range [0,512]
		\param shortcut \p string The keyboard shortcut of the item
		\param name \p string The name of the item
		\param parent \p int The index of the parent of the item
		\param values \p string Range specification in format "min,max,startValue,step"
		\param callback \p func Callback to call when the value is changed (OPTIONAL) (Also read BindCallback)
	*/
	static proto void RegisterRange(int id, string shortcut, string name, int parent, string valuenames, func callback = null);
	
	//! Unregister the item at given id
	static proto void Unregister(int id);
	
	//! Check if the item at given id has been registered
	static proto bool IsRegistered(int id);
	
	/**
	*\brief Bind a callback to the given id
	*	\note Only one callback can be registered, so when attempting to registering multiple, only the last one will be present
	*	\note The callbacks are required to have one of following signatures
	*		- All Register... support:
	*			o static void Callback();
	*		- RegisterItem & RegisterBool:
	*			o static void Callback(int value);
	*			o static void Callback(int value, int id);
	*			o static void Callback(bool value);
	*			o static void Callback(bool value, int id);
	*		- RegisterRange:
	*			o static void Callback(float value);
	*			o static void Callback(float value, int id);
	*			o static void Callback(int value);
	*			o static void Callback(int value, int id);
	*	\note Keep in mind that bool and int are interchangeable, so 'bool value' is possible for RegisterRange too
	*/
	static proto bool BindCallback(int id, func callback);
	//! Unbind the callback from the given id
	static proto void UnbindCallback(int id);
	
	//! Get value as bool from the given script id
	static proto bool GetBool(int id, bool reverse = false);
	//! Get value as int from the given script id
	static proto int GetValue(int id);
	//! Set value at the given script id
	static proto void SetValue(int id, int value);
	
	//! Get range value at the given script id 
	static proto float GetRangeValue(int id);
	//! Set range value at the given script id
	static proto void SetRangeValue(int id, float value);
	
	//! Get value at the given engine id
	static proto int GetEngineValue(int id);
	//! Set value at the given engine id
	static proto void SetEngineValue(int id, int value);
	
	//! Get range value at the given engine id
	static proto float GetEngineRangeValue(int id);
	//! Set range value at the given engine id
	static proto void SetEngineRangeValue(int id, float value);
	
	//! Check if a menu with supplied name already exists
	static proto bool MenuExists(string name);	
};

//@}

//@}
