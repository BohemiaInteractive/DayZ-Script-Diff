class bool
{
	string ToString()
	{
		if (value) return "true";
		else return "false";
	}
};

enum EBool
{
	NO = 0,
	YES = 1	
}

class int
{
	protected const int ZERO_PAD_SIZE = 8;
	protected static string m_ZeroPad[ZERO_PAD_SIZE] = {"", "0", "00", "000", "0000", "00000", "000000", "0000000"};
	
	const int MAX = 2147483647;
	const int MIN = -2147483648;
	
	proto string ToString();
	
	/**
	\brief Converts ASCII code to string
		\param ascii ASCII code for convert to \p string.
		\return \p string - Converted \p int.
		@code
			int ascii_code = 77;
			string str = ascii_code.AsciiToString();
			Print(str);

			>> str = 'M'
		@endcode
	*/
	proto string AsciiToString();
	
	/**
	\brief Integer to string with fixed length, padded with zeroes
		\param num \p int integer to convert
		\param len \p int fixed length
		\return \p vector Converted s as vector
		@code
			int num = 123;
			string s = num.ToStringLen(5);
			Print(s);

			>> s = '00123'
		@endcode
	*/
	string ToStringLen(int len)
	{
		string str = value.ToString();

		int l = len - str.Length();

		if (l > 0 && l < ZERO_PAD_SIZE )
			return m_ZeroPad[l] + str;

		return str;
	}
	
	/**
	\brief Check whether integer falls into an inclusive range
		\param min \p int low end of range
		\param max \p int high end of range
		\return \p bool True if this value is withing the set range
		@code
			int num = 123;
			bool test = num.InRange( 100, 150 );
			Print(test);

			>> s = true
		@endcode
	*/
	bool InRange( int min, int max, bool inclusive_min = true, bool inclusive_max = true )
	{
		if( ( !inclusive_min && value <= min ) || value < min )
			return false;
		
		if( ( !inclusive_max && value >= max ) || value > max )
			return false;
		
		return true;
	}
};

class float
{
	const float MIN = FLT_MIN;
	const float MAX = FLT_MAX;
	const float LOWEST = -FLT_MAX;
	
	proto string ToString();
};

class vector
{
	static const vector Up = "0 1 0";
	static const vector Aside = "1 0 0";
	static const vector Forward = "0 0 1";
	static const vector Zero = "0 0 0";
	
	/**
	\brief Vector to  string
		\param beautify If true verbose vector in more human readable form "<1, 1, 1>" instead of simple form "1 1 1"
		\return \p string Converted vector as parsed string
		@code
			vector v = "1 0 1";
			Print( v.ToString() );
			Print( v.ToString(false) );

			>> '<1, 0, 1>'
			>> '1 0 1'
		@endcode
	*/
	proto string ToString(bool beautify = true);
	
	/**
	\brief Normalizes vector. Returns length
		\return \p float Length of origin vector
		@code
			vector vec = "1 0 1";
			float length = vec.Normalize();
			Print( vec );
			Print( length );

			>> vec = <0.707107,0,0.707107>
			>> length = 1.41421
		@endcode
	*/
	proto float Normalize();

	//! return normalized vector (keeps orginal vector untouched)
	proto vector Normalized();
	
	/**
	\brief Returns length of vector (magnitude)
		\return \p float Length of vector
		@code
			vector vec = "1 0 1";
			float length = vec.Length();
			Print( length );

			>> length = 1.41421
		@endcode
	*/
	proto native float Length();

	/**
	\brief Returns squared length (magnitudeSqr)
		\return \p float Length of vector
		@code
			vector vec = "1 1 0";
			float length = vec.LengthSq();
			Print( length );

			>> length = 2
		@endcode
	*/
	proto native float LengthSq();
	
	/**
	\brief Returns the distance between tips of two 3D vectors.
		\param v1 \p vector 3D Vector 1
		\param v2 \p vector 3D Vector 2
		\return \p float Distance
		@code
			float dist = vector.Distance( "1 2 3", "4 5 6" );
			Print( dist );

			>> dist = 5.19615
		@endcode
	*/
	proto static native float Distance(vector v1, vector v2);
	
	/**
	\brief Returns the squere distance between tips of two 3D vectors.
		\param v1 \p vector 3D Vector 1
		\param v2 \p vector 3D Vector 2
		\return \p float Squere distance
		@code
			float dist = vector.DistanceSq( "0 0 0", "0 5 0" );
			Print( dist );

			>> dist = 25
		@endcode
	*/
	proto static native float DistanceSq(vector v1, vector v2);

	/**
	\brief Returns perpendicular vector. Perpendicular vector is computed as cross product between input vector and up vector (0, 1, 0).
	    \return \p vector perpendicular vector
		@code
			vector vec = "1 0 0";
			Print( vec.Perpend() );

			>> <0,0,1>
		@endcode
	*/
	vector Perpend()
	{
		return value * vector.Up;
	}
	
	/**
	\brief Returns direction vector from point p1 to point p2
	    \param p1 \p vector point from
		\param p2 \p vector point to
		\return \p vector direction vector
	*/
	static vector Direction(vector p1, vector p2)
	{
		vector dir_vec;
		
		dir_vec[0] = p2[0] - p1[0];
		dir_vec[1] = p2[1] - p1[1];
		dir_vec[2] = p2[2] - p1[2];
		
		return dir_vec;
	}
	
	/**
	\brief Returns randomly generated unit vector
	    \return \p randomly generated unit vector
		@code
			vector vec = vector.RandomDir();
			Print(vec);
			Print(vec.Length());

			>> <-0.179424,0.966825,0.181816>
			>> 1
		@endcode
	*/
	static vector RandomDir()
	{
		return Vector(Math.RandomFloatInclusive(-1,1),Math.RandomFloatInclusive(-1,1),Math.RandomFloatInclusive(-1,1)).Normalized();
	}

	/**
	\brief Returns randomly generated XZ unit vector with the Y(up) axis set to 0
	    \return \p randomly generated XZ unit vector
		@code
			vector vec = vector.RandomDir();
			Print(vec);
			Print(vec.Length());

			>> <0.631697,0,0.775216>
			>> 1
		@endcode
	*/
	static vector RandomDir2D()
	{
		return Vector(Math.RandomFloatInclusive(-1,1),0,Math.RandomFloatInclusive(-1,1)).Normalized();
	}
	
	/**
	\brief Returns Dot product of vector v1 and vector v2
	    \param v1 \p vector input vector
		\param v2 \p vector input vector
		\return \p vector direction vector
	*/
	static float Dot(vector v1, vector v2)
	{
		return ((v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]));
	}

	/**
	\brief Returns relative angles between -180 and 180, not 0 to 360
		\return \p float Relative angles
		@code
			vector angles = "-45 190 160";
			Print( angles.GetRelAngles() );
	
			>> <-45,-170,160>
		@endcode
	*/
	vector GetRelAngles()
	{
		for(int i = 0; i < 3; i++) {
			if(value[i] > 180)
				value[i] = value[i] - 360;
			if(value[i] < -180)
				value[i] = value[i] + 360;
		}
		return value;
	}
	
	/**
	\brief Returns yaw of vector
		\param vec \p vector Vector to convert yaw
		\return \p float Yaw of vector
		@code
			vector v1 = "0 1 0";
			vector v2 = "0.7 0.7 0";
			Print( v1.ToYaw() );
			Print( v2.ToYaw() );

			>> 90
			>> 45
		@endcode
	*/
	proto float VectorToYaw();

	/**
	\brief Returns vector of yaw
		\param yaw \p float Value of yaw
		\return \p vector Yaw converted in vector
		@code
			Print( vector.Yaw2Vector(90) );
			Print( vector.Yaw2Vector(45) );

			>> <0,1,0>
			>> <0.707107,0.707107,0>
		@endcode
	*/
	proto native static vector YawToVector(float yaw);

	/**
	\brief Converts vector to spherical coordinates with radius = 1
		\return \p vector spherical coordinates (yaw, pitch, roll in degrees)
		@code
			vector v1 = "1 0 1";
			vector v2 = "1 1 1";
			Print( v1.VectorToAngles() );
			Print( v2.VectorToAngles() );

			>> <45,-0,0>
			>> <45,35.2644,0>
		@endcode
	*/
	proto vector VectorToAngles();

	/**
	\brief Converts spherical coordinates (yaw, pitch, roll in degrees) to unit length vector
		\return \p normalized direction vector
		@code
			vector v1 = "45 0 0";
			vector v2 = "15 60 0";
			Print( v1.AnglesToVector() );
			Print( v2.AnglesToVector() );

			>> <0.707107,0,0.707107>
			>> <0.12941,0.866025,0.482963>
		@endcode
	*/
	proto vector AnglesToVector();

	/**
	\brief Creates rotation matrix from angles
		\param ang \p vector which contains angles
	    \param[out] mat \p vector created rotation matrix
		@code
	        vector mat[3];
			vector ang = "70 15 45";
	    	ang.RotationMatrixFromAngles( mat );
	    	Print( mat );

			>> <0.330366,0.0885213,-0.939693>,<0.458809,0.854988,0.241845>,<0.824835,-0.511037,0.241845>
		@endcode
	*/
	proto void RotationMatrixFromAngles(out vector mat[3]);
	
	/**
	\brief Transforms position
		\param mat \p vector[4] transformation matrix
		\param vec \p vector position to transform
		\return \p vector transformed position
		@code
			vector mat[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 1", "3 1 2 1" }; // translation matrix
			vector pos = "1 1 1";
			Print( pos.Multiply4(mat) );

			>> <4,2,3>
		@endcode
	*/
	proto vector Multiply4(vector mat[4]);

	/**
	\brief Transforms vector
		\param mat \p vector[3] transformation matrix
		\param vec \p vector vector to transform
		\return \p vector transformed vector
		@code
			vector mat[3] = { "2 0 0", "0 3 0", "0 0 1" }; // scale matrix
			vector vec = "1 1 1";
			Print( vec.Multiply3(mat) );

			>> <2,3,1>
		@endcode
	*/
	proto vector Multiply3(vector mat[3]);

	/**
	\brief Invert-transforms position
		\param mat \p vector[4] transformation matrix
		\param vec \p vector position to transform
		\return \p vector transformed position
		@code
			vector mat[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 1", "3 1 2 1" }; // translation matrix
			vector pos = "1 1 1";
			Print( pos.InvMultiply4(mat) );

			>> <-2,0,-1>
		@endcode
	*/
	proto vector InvMultiply4(vector mat[4]);

	/**
	\brief Invert-transforms vector
		\param mat \p vector[3] transformation matrix
		\param vec \p vector vector to transform
		\return \p vector transformed vector
		@code
			vector mat[3] = { "1.5 2.5 0", "0.1 1.3 0", "0 0 1" }; // rotation matrix
			vector vec = "1 1 1";
			Print( vec.InvMultiply3(mat) );

			>> <4,1.4,1>
		@endcode
	*/
	proto vector InvMultiply3(vector mat[3]);
	
	/**
	\brief Lerp between two vectors
		@code
			vector v1 = Vector(0,0,0);
			vector v2 = Vector(5,6,1);
			Print( vector.Lerp(v1, v2, 0.5) );
		@endcode
	*/
	proto static native vector Lerp(vector v1, vector v2, float t);
	
	/**
	\brief Rotate a vector around 0,0,0 by an angle in degrees
		\param vec \p vector to rotate
		\param axis \p axis to rotate around
		\param cosAngle \p angle in degrees
		\return \p vector transformed vector
	*/
	
	static vector RotateAroundZeroDeg(vector vec, vector axis, float angle)
	{
		return (vec * Math.Cos(angle * Math.DEG2RAD)) + ((axis * vec) * Math.Sin(angle * Math.DEG2RAD)) + (axis * vector.Dot(axis, vec)) * (1 - Math.Cos(angle * Math.DEG2RAD));
	}
	
	/**
	\brief Rotate a vector around 0,0,0 by an angle in radians
		\param vec \p vector to rotate
		\param axis \p axis to rotate around
		\param cosAngle \p angle in radians
		\return \p vector transformed vector
	*/
	
	static vector RotateAroundZeroRad(vector vec, vector axis, float angle)
	{
		return (vec * Math.Cos(angle)) + ((axis * vec) * Math.Sin(angle)) + (axis * vector.Dot(axis, vec)) * (1 - Math.Cos(angle));
	}
	
	/**
	\brief Rotate a vector around 0,0,0
		\param pos \p vector to rotate
		\param axis \p axis to rotate around
		\param cosAngle \p cos of angle
		\param sinAngle \p sin of angle
		\return \p vector transformed vector
	*/
	
	static vector RotateAroundZero(vector pos, vector axis, float cosAngle, float sinAngle)
	{
		return (pos * cosAngle) + ((axis * pos) * sinAngle) + (axis * vector.Dot(axis, pos)) * (1 - cosAngle);
	}
	
	/**
	\brief Rotate a vector around point
		\param point \p point to rotate around
		\param pos \p vector to rotate
		\param axis \p axis to rotate around
		\param cosAngle \p cos of angle
		\param sinAngle \p sin of angle
		\return \p vector transformed vector
	*/
	static vector RotateAroundPoint(vector point, vector pos, vector axis, float cosAngle, float sinAngle)
	{
		vector offsetPos = pos - point;
		return RotateAroundZero(offsetPos, axis, cosAngle, sinAngle) + point;
	}
	
	/**
	\brief Covert static array of floats into a vector
	    \param arr \p vector in array format
		\return \p vector resulting vector
	*/
	static vector ArrayToVec(float arr[])
	{
		return Vector(arr[0], arr[1], arr[2]);
	}
	
	
};

class typename
{
	/**
	\brief Dynamic variant to 'new' keyword. It creates new instance of class
		\returns \p volatile instance of class
		@code
			???
		@endcode
	*/
	proto volatile Class Spawn();
	
	/**
	\brief Get the name of the module the typename belongs to
		\returns \p string Name of parent module (1_Core)
	*/
	proto owned string GetModule();
	
	//!Returns type name of variable as string
	proto native owned string ToString();
	
	/**
	\brief Returns true when type is the same as 'baseType', or inherited one.
		\param baseType typename
		\returns \p bool true when type is the same as 'baseType', or inherited one.
		@code
			???
		@endcode
	*/
	proto native bool IsInherited(typename baseType);
	
	proto native int GetVariableCount();
	proto native owned string GetVariableName(int vIdx);
	proto native typename GetVariableType(int vIdx);
	proto bool GetVariableValue(Class var, int vIdx, out void val);
	
	/**
	\brief Return string name of enum value
	@code
		DialogPriority prio = DialogPriority.WARNING;
		Print( typename.EnumToString(DialogPriority, prio) );
	@endcode
	*/
	static string EnumToString(typename e, int enumValue)
	{
		int cnt = e.GetVariableCount();
		int val;

		for (int i = 0; i < cnt; i++)
		{
			if (e.GetVariableType(i) == int && e.GetVariableValue(null, i, val) && val == enumValue)
			{
				return e.GetVariableName(i);
			}
		}
				
		return "unknown";
	}
	
	/**
	\brief Return enum value from string name
	@code
		Print( typename.StringToEnum(DialogPriority, "WARNING") );
	@endcode
	*/
	static int StringToEnum(typename e, string enumName)
	{
	    int count = e.GetVariableCount();
	    int value;
	   
	    for (int i = 0; i < count; i++)
	    {
	        if (e.GetVariableType(i) == int && e.GetVariableValue(null, i, value) && e.GetVariableName(i) == enumName)
	        {
	            return value;
	        }
	    }
		
	    return -1;
	}
};

class EnumTools
{
	private void EnumTools();
	private void ~EnumTools();
	
	/**
	\brief Return string name of enum value
	@code
		DialogPriority prio = DialogPriority.WARNING;
		Print( EnumTools.EnumToString(DialogPriority, prio) );
	@endcode
	*/
	static string EnumToString(typename e, int enumValue)
	{
		return typename.EnumToString(e, enumValue);
	}
	
	/**
	\brief Return enum value from string name
	@code
		Print( EnumTools.StringToEnum(DialogPriority, "WARNING") );
	@endcode
	*/
	static int StringToEnum(typename e, string enumName)
	{
		return typename.StringToEnum(e, enumName);
	}
	
	/**
	\brief Return amount of values in enum
	@code
		Print( EnumTools.GetEnumSize(DialogPriority) );
	@endcode
	*/
	static int GetEnumSize(typename e)
	{
		return e.GetVariableCount();
	}
	
	/**
	\brief Return the nth value in the enum
	@code
		Print( EnumTools.GetEnumValue(DialogPriority, 1) );
	@endcode
	*/
	static int GetEnumValue(typename e, int idx)
	{
		int value;
		e.GetVariableValue(null, idx, value);		
		return value;
	}
	
	/**
	\brief Return amount of values in enum
	@code
		Print( EnumTools.GetLastEnumValue(DialogPriority) );
	@endcode
	*/
	static int GetLastEnumValue(typename e)
	{
		int lastValue;
		e.GetVariableValue(null, e.GetVariableCount() - 1, lastValue);		
		return lastValue;
	}
}