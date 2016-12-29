// -----------------------------------------------------------
// scene.cpp
// 2004 - Jacco Bikker - jacco@bik5.com - www.bik5.com -   <><
// -----------------------------------------------------------

#include "common.h"
#include "string.h"
#include "scene.h"
#include "raytracer.h"
#include "iostream"

namespace Raytracer {

// -----------------------------------------------------------
// Primitive class implementation
// -----------------------------------------------------------

void Primitive::SetName( char* a_Name )
{
	delete m_Name; 
	m_Name = new char[strlen( a_Name ) + 1]; 
	strcpy( m_Name, a_Name ); 
}

// -----------------------------------------------------------
// Material class implementation
// -----------------------------------------------------------

Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ), m_Diff( 0.2f ), m_Spec( 0.8f ), m_RIndex( 1.5f )
{
}

// -----------------------------------------------------------
// Sphere primitive methods
// -----------------------------------------------------------

int Sphere::Intersect( Ray& a_Ray, float& a_Dist )
{
	vector3 v = a_Ray.GetOrigin() - m_Centre;
	float b = -DOT( v, a_Ray.GetDirection() );
	float det = (b * b) - DOT( v, v ) + m_SqRadius;
	int retval = MISS;
	if (det > 0)
	{
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0) 
			{
				if (i2 < a_Dist) 
				{
					a_Dist = i2;
					retval = INPRIM;
				}
			}
			else
			{
				if (i1 < a_Dist)
				{
					a_Dist = i1;
					retval = HIT;
				}
			}
		}
	}
	return retval;
}

// -----------------------------------------------------------
// Plane primitive class implementation
// -----------------------------------------------------------

int PlanePrim::Intersect( Ray& a_Ray, float& a_Dist )
{
	float d = DOT( m_Plane.N, a_Ray.GetDirection() );
	if (d != 0)
	{
		float dist = -(DOT( m_Plane.N, a_Ray.GetOrigin() ) + m_Plane.D) / d;
		if (dist > 0)
		{
			if (dist < a_Dist) 
			{
				a_Dist = dist;
				return HIT;
			}
		}
	}
	return MISS;
}

vector3 PlanePrim::GetNormal( vector3& a_Pos )
{
	return m_Plane.N;
}

// -----------------------------------------------------------
// Scene class implementation
// -----------------------------------------------------------

Scene::~Scene()
{
	delete m_Primitive;
}

void Scene::InitScene()
{
	m_Primitive = new Primitive*[500];
	// ground plane
	m_Primitive[0] = new PlanePrim( vector3( 0, 1, 0 ), 4.4f );
	m_Primitive[0]->SetName( "plane" );
	m_Primitive[0]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[0]->GetMaterial()->SetRefraction( 0.0f );
	m_Primitive[0]->GetMaterial()->SetDiffuse( 1.0f );
	m_Primitive[0]->GetMaterial()->SetColor( Color( 0.4f, 0.3f, 0.3f ) );
	// big sphere
	m_Primitive[1] = new Sphere( vector3( 2, 0.8f, 3 ), 2.5f );
	m_Primitive[1]->SetName( "big sphere" );
	m_Primitive[1]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[1]->GetMaterial()->SetRefraction( 0.8f );
	m_Primitive[1]->GetMaterial()->SetRefrIndex( 1.5f );
	m_Primitive[1]->setFront();

	m_Primitive[1]->GetMaterial()->SetColor( Color( 0.7f, 0.7f, 1.0f ) );
	// small sphere
	m_Primitive[2] = new Sphere( vector3( -5.5f, -0.5, 7 ), 2 );
	m_Primitive[2]->SetName( "small sphere" );
	m_Primitive[2]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[2]->GetMaterial()->SetRefraction( 0.8f );
	m_Primitive[2]->GetMaterial()->SetRefrIndex( 1.5f );
	m_Primitive[2]->GetMaterial()->SetDiffuse( 0.2f );
	m_Primitive[2]->setFront();

	m_Primitive[2]->GetMaterial()->SetColor( Color( 0.7f, 0.7f, 1.0f ) );
	// light source 1
	m_Primitive[3] = new Sphere( vector3( 0, 5, 5 ), 0.1f );
	m_Primitive[3]->Light( true );
	m_Primitive[3]->GetMaterial()->SetColor( Color( 0.4f, 0.4f, 0.4f ) );
	// light source 2
	m_Primitive[4] = new Sphere( vector3( -3, 5, 1 ), 0.1f );
	m_Primitive[4]->Light( true );
	m_Primitive[4]->GetMaterial()->SetColor( Color( 0.6f, 0.6f, 0.8f ) );
	// extra sphere
	m_Primitive[5] = new Sphere( vector3( -1.5f, -3.8f, 1 ), 1.5f );
	m_Primitive[5]->SetName( "extra sphere" );
	m_Primitive[5]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[5]->setFront();

	m_Primitive[5]->GetMaterial()->SetRefraction( 0.8f );
	m_Primitive[5]->GetMaterial()->SetColor( Color( 1.0f, 0.4f, 0.4f ) );
	// back plane
	m_Primitive[6] = new PlanePrim( vector3( 0.4f, 0, -1 ), 12 );
	m_Primitive[6]->SetName( "back plane" );
	m_Primitive[6]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[6]->GetMaterial()->SetRefraction( 0.0f );
	m_Primitive[6]->inFront();
	m_Primitive[6]->GetMaterial()->SetSpecular( 0 );
	m_Primitive[6]->GetMaterial()->SetDiffuse( 0.6f );
	m_Primitive[6]->GetMaterial()->SetColor( Color( 0.5f, 0.3f, 0.5f ) );
	// ceiling plane
	m_Primitive[7] = new PlanePrim( vector3( 0, -1, 0 ), 7.4f );
	m_Primitive[7]->SetName( "back plane" );
	m_Primitive[7]->GetMaterial()->SetReflection( 0.0f );
	m_Primitive[7]->GetMaterial()->SetRefraction( 0.0f );
	m_Primitive[7]->GetMaterial()->SetSpecular( 0 );
	m_Primitive[7]->inFront();
	m_Primitive[7]->GetMaterial()->SetDiffuse( 0.5f );
	m_Primitive[7]->GetMaterial()->SetColor( Color( 0.4f, 0.7f, 0.7f ) );
	// grid
	int prim = 8;
	//for ( int x = 0; x < 8; x++ ) for ( int y = 0; y < 7; y++ )
	//{
	//	m_Primitive[prim] = new Sphere( vector3( -4.5f + x * 1.5f, -4.3f + y * 1.5f, 10 ), 0.3f );
	//	m_Primitive[prim]->SetName( "grid sphere" );
	//	m_Primitive[prim]->GetMaterial()->SetReflection( 0 );
	//	m_Primitive[prim]->GetMaterial()->SetRefraction( 0 );
	//	m_Primitive[prim]->GetMaterial()->SetSpecular( 0.6f );
	//	m_Primitive[prim]->GetMaterial()->SetDiffuse( 0.6f );
	//	m_Primitive[prim]->GetMaterial()->SetColor( Color( 0.3f, 1.0f, 0.4f ) );
	//	m_Primitive[prim]->setFront();

	//	prim++;
	//}
	// set number of primitives
	m_Primitives = prim;

}

}; // namespace Raytracer
