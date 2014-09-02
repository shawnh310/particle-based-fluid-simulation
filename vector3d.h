#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <cmath>

template <class T>
class Vector3D
{
public:
	T x, y, z;
	
public:
    Vector3D(T x_, T y_, T z_) :
        x(x_),
        y(y_),
        z(z_)
    {
    }
    
    Vector3D() :
        x(0),
        y(0),
        z(0)
    {
    }
    
    const Vector3D& operator = (const Vector3D<T> &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        
        return *this;
    }
    
    Vector3D operator + (const Vector3D<T> &v) const
    {
        return Vector3D(x+v.x, y+v.y, z+v.z);       
    }
    
    Vector3D& operator += (const Vector3D &v)
    {
		x += v.x;
        y += v.y;
        z += v.z;
        
		return *this;
	}
	
    Vector3D operator - (const Vector3D &v) const
    {
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}
    
	Vector3D& operator -= (const Vector3D &v)
    {
		x -= v.x; y -= v.y; z -= v.z;
        
		return *this;
	}
	
    bool operator == (const Vector3D &v) const
    {
		return (x == v.x) && (y == v.y) && (z == v.z);
	}
	
    Vector3D operator * (T f) const
    {
		return Vector3D(f*x, f*y, f*z);
	}
	
	Vector3D& operator *= (T f)
    {
		x *= f;
        y *= f;
        z *= f;
        
		return *this;
	}
    
	Vector3D operator / (T f) const
    {
		T inv = 1.f / f;
        
		return Vector3D(x * inv, y * inv, z * inv);
	}
	
	Vector3D& operator /= (T f)
    {
		T inv = 1 / f;
		x *= inv;
        y *= inv;
        z *= inv;
        
		return *this;
	}
    
    T Length() const
    {
        return sqrtf(Length2());
    }
    
    T Length2() const
    {
        return x*x + y*y + z*z;
    }
    
    T Dot(const Vector3D<T> &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    
    Vector3D Cross(const Vector3D<T> &v) const
    {
        return Vector3D((y * v.z) - (z * v.y),
                        (z * v.x) - (x * v.z),
                        (x * v.y) - (y * v.x));
    }
    
    bool Normalize()
    {
        T len = Length();
        if (len == 0)
            return false;
        
        T inv = 1 / len;
        x *= inv;
        y *= inv;
        z *= inv;
        
        return true;
    }

    T Distance(const Vector3D<T>& v) const
    {
        return (*this - v).Length();
    }
    
    T Distance2(const Vector3D<T>& v) const
    {
        return (*this - v).Length2();
    }
    
    void Clear()
    {
        x = 0;
        y = 0;
        z = 0;
    }
};

typedef Vector3D<double> Vector3Dd;
typedef Vector3D<float>  Vector3Df;
typedef Vector3D<int>    Vector3Di;

#endif 
