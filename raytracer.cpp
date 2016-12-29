// -----------------------------------------------------------
// raytracer.cpp
// 2004 - Jacco Bikker - jacco@bik5.com - www.bik5.com -   <><
// -----------------------------------------------------------

#include "raytracer.h"
#include "scene.h"
#include "common.h"
#include "windows.h"
#include "winbase.h"
#include<stdio.h>
namespace Raytracer {

Ray::Ray( vector3& a_Origin, vector3& a_Dir ) : 
	m_Origin( a_Origin ), 
	m_Direction( a_Dir )
{
}
//first define Ray  vector
Engine::Engine()
{
	m_Scene = new Scene();
}
//build scene
Engine::~Engine()
{
	delete m_Scene;
}
//delete it
// -----------------------------------------------------------
// Engine::SetTarget
// Sets the render target canvas
// -----------------------------------------------------------
void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
{
	// set pixel buffer address & size
	m_Dest = a_Dest;
	m_Width = a_Width;
	m_Height = a_Height;
}
//???????????????????????????????????????????????????????
// -----------------------------------------------------------
// Engine::Raytrace
// Naive ray tracing: Intersects the ray with every primitive
// in the scene to determine the closest intersection
// -----------------------------------------------------------
Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist, Color& dirLC,bool flag, Color& Specular, Color& B_withoutOBJ,Color& realB,bool bG)
{
	//if(a_Acc.r < dirLC.r)
	//{
	//	return 0;
	//}
	if (a_Depth > TRACEDEPTH) return 0;
	// trace primary ray
	a_Dist = 1000000.0f;
	vector3 pi;
	Primitive* prim = 0;
	int result;
	// find the nearest intersection
	for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
	{
		Primitive* pr = m_Scene->GetPrimitive( s );
		int res;
		if (res = pr->Intersect( a_Ray, a_Dist )) 
		{
			prim = pr;
			result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
			if(pr->getFront())
			{
				bG = false;
			}
		}
	}
	// no hit, terminate ray
	if (!prim) return 0;
	// handle intersection
	if (prim->IsLight())
	{
		// we hit a light, stop tracing
		a_Acc = Color( 1, 1, 1);
	}
	else
	{
		// determine color at point of intersection
		pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist;
		// trace lights

		for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
		{
			Primitive* p = m_Scene->GetPrimitive( l );
			if (p->IsLight()) 
			{
				Primitive* light = p;
				// handle point light source
				float shade = 1.0f;
				/*if (light->GetType() == Primitive::SPHERE)
				{
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					float tdist = LENGTH( L );
					L *= (1.0f / tdist);
					Ray r = Ray( pi + L * EPSILON, L );
					for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
					{
						Primitive* pr = m_Scene->GetPrimitive( s );
						if ((pr != light) && (pr->Intersect( r, tdist )))
						{
							shade = 0;
							break;
						}
					}
				}*/
				if (shade > 0)
				{
					// calculate diffuse shading
					vector3 L = ((Sphere*)light)->GetCentre() - pi;
					NORMALIZE( L );
					vector3 N = prim->GetNormal( pi );
					if (prim->GetMaterial()->GetDiffuse() > 0)
					{
						float dot = DOT( L, N );
						if (dot > 0)
						{
							float diff = dot * prim->GetMaterial()->GetDiffuse() * shade;
							// add diffuse component to ray color
							a_Acc += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
							if(prim->getFront()) {
							dirLC += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
							}
							else if(!(prim->getFront()))
							{
								B_withoutOBJ += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
								if(bG)
								{
									realB += diff * prim->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
								}
							}
									 


						}
					}
					// determine specular component
					if (prim->GetMaterial()->GetSpecular() > 0)
					{
						// point light source: sample once for specular highlight
						vector3 V = a_Ray.GetDirection();
						vector3 R = L - 2.0f * DOT( L, N ) * N;
						float dot = DOT( V, R );
						if (dot > 0)
						{
							float spec = powf( dot, 20 ) * prim->GetMaterial()->GetSpecular() * shade;
							// add specular component to ray color
							a_Acc += spec * light->GetMaterial()->GetColor();
							Specular +=spec * light->GetMaterial()->GetColor();
							if(prim->getFront()) {
								dirLC += spec * light->GetMaterial()->GetColor();
								
							}
							else if(!(prim->getFront()))
							{
								if(bG)
								{
									realB +=  spec * light->GetMaterial()->GetColor();
								}
								B_withoutOBJ += spec * light->GetMaterial()->GetColor();
							}
					
						}
					}
				}
			}
		}
		// calculate reflection
		float refl = prim->GetMaterial()->GetReflection();
		if ((refl > 0.0f) && (a_Depth < TRACEDEPTH))
		{
			vector3 N = prim->GetNormal( pi );
			vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
			Color rcol( 0, 0, 0 );
			Color dcol( 0, 0, 0 );
			Color bcol( 0, 0, 0 );
			Color scol(0,0,0);
			Color rbcol(0,0,0);
			float dist;
			Raytrace( Ray( pi + R * EPSILON, R ), rcol, a_Depth + 1, a_RIndex, dist, dcol,flag,Specular,bcol,rbcol,bG);
			a_Acc += refl * rcol * prim->GetMaterial()->GetColor();
			/*if(prim->getFront()) {
				dirLC+=refl * dcol * prim->GetMaterial()->GetColor();
			}
			else if(!(prim->getFront()))
			{
				B_withoutOBJ +=refl * bcol * prim->GetMaterial()->GetColor();
			}*/
			dirLC +=refl * dcol * prim->GetMaterial()->GetColor();
			B_withoutOBJ +=refl * bcol * prim->GetMaterial()->GetColor();
			Specular +=refl * scol * prim->GetMaterial()->GetColor();
			if(bG)
			{
			realB +=refl * rbcol * prim->GetMaterial()->GetColor();
			}

		}
		

		
		// calculate refraction
		float refr = prim->GetMaterial()->GetRefraction();
		if ((refr > 0) && (a_Depth < TRACEDEPTH))
		{
			float rindex = prim->GetMaterial()->GetRefrIndex();
			float n = a_RIndex / rindex;
			vector3 N = prim->GetNormal( pi ) * (float)result;
			float cosI = -DOT( N, a_Ray.GetDirection() );
			float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
			if (cosT2 > 0.0f)
			{
				vector3 T = (n * a_Ray.GetDirection()) + (n * cosI - sqrtf( cosT2 )) * N;
				Color rcol( 0, 0, 0 );
				Color dcol( 0, 0, 0 );
			    Color bcol( 0, 0, 0 );
				Color scol(0, 0, 0);
				Color rbcol(0,0,0);
				float dist;
				Raytrace( Ray( pi + T * EPSILON, T ), rcol, a_Depth + 1, rindex, dist, dcol,true,Specular,bcol,rbcol,bG);
				// apply Beer's law
				Color absorbance = prim->GetMaterial()->GetColor() * 0.15f * -dist;
				Color transparency = Color( expf( absorbance.r ), expf( absorbance.g ), expf( absorbance.b ) );
				
				a_Acc += rcol * transparency;
				dirLC+= dcol * transparency;
				 //B_withoutOBJ+= bcol * transparency;
				if(bG)
				{
					realB += rbcol *transparency;
				}
				 Specular +=scol*transparency;
				//Specular += rcol * transparency;
				/*if(prim->getFront()) {
					 dirLC+= dcol * transparency;
					
			    }
				else if(!(prim->getFront()))
				{
					 B_withoutOBJ+= bcol * transparency;
				}*/

		     }
	    }
	// return pointer to primitive hit by primary ray
		printf("%f, %f, %f",a_Acc.r,a_Acc.g,a_Acc.b);
		printf("%f, %f, %f",dirLC.r,dirLC.g,dirLC.b);
	return prim;
}
}

// -----------------------------------------------------------
// Engine::InitRender
// Initializes the renderer, by resetting the line / tile
// counters and precalculating some values
// -----------------------------------------------------------
void Engine::InitRender()
{
	// set firts line to draw to
	m_CurrLine = 20;
	// set pixel buffer address of first pixel
	m_PPos = m_CurrLine * m_Width;
	// screen plane in world space coordinates
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation
	m_DX = (m_WX2 - m_WX1) / m_Width;
	m_DY = (m_WY2 - m_WY1) / m_Height;
	m_SY += m_CurrLine * m_DY;
	// allocate space to store pointers to primitives for previous line
	m_LastRow = new Primitive*[m_Width];
	memset( m_LastRow, 0, m_Width * 4 );

	int total = (m_Height - 40) * m_Width;

	ralpha = new float[total];
	galpha = new float[total];
	balpha = new float[total];



}

// -----------------------------------------------------------
// Engine::Render
// Fires rays in the scene one scanline at a time, from left
// to right
// -----------------------------------------------------------
bool Engine::Render()
{

	// render scene
	vector3 o( 0, 0, -5 );
	// initialize timer
	int msecs = GetTickCount();
	// reset last found primitive pointer
	Primitive* lastprim = 0;
	// render remaining lines
	float ra, ga, ba;
	float temp[10000][10];


	for ( int y = m_CurrLine; y < (m_Height - 20); y++ )
	{
		m_SX = m_WX1;
		// render pixels for current line
		for ( int x = 0; x < m_Width; x++ )
		{
			// fire primary rays
			Color acc( 0, 0, 0 );
			Color dirLC(0, 0, 0);
			Color B_withoutOBJ(0,0,0);
			Color Specular(0,0,0);
			Color realB(0,0,0);
			vector3 dir = vector3( m_SX, m_SY, 0 ) - o;
			NORMALIZE( dir );
			Ray r( o, dir );
			float dist;
			Primitive* prim = Raytrace( r, acc, 1, 1.0f, dist, dirLC,false,Specular,B_withoutOBJ,realB,true);
			int red, green, blue;
/*
 			if (0)
			{
				lastprim = prim;
				Color acc( 0, 0, 0 );
				Color dirLC(0, 0, 0);
				for ( int tx = -1; tx < 2; tx++ ) for ( int ty = -1; ty < 2; ty++ )
				{
					vector3 dir = vector3( m_SX + m_DX * tx / 2.0f, m_SY + m_DY * ty / 2.0f, 0 ) - o;
					NORMALIZE( dir );
					Ray r( o, dir );
					float dist;
					Primitive* prim = Raytrace( r, acc, 1, 1.0f, dist, dirLC);
				}
				red = (int)(acc.r * (256 / 9));
				green = (int)(acc.g * (256 / 9));
				blue = (int)(acc.b * (256 / 9));
				ra = dirLC.r / acc.r;
				ga = dirLC.g / acc.g;
				ba = dirLC.b / acc.b;
			}
*/
			red = (int)(acc.r * 256);
			green = (int)(acc.g * 256);
			blue = (int)(acc.b * 256);
			
	/*		if(acc.r>0) ra = dirLC.r / acc.r; else ra = 1;
			if(acc.g>0) ga = dirLC.g / acc.g; else ga = 1;
			if(acc.b>0) ba = dirLC.b / acc.b; else ba = 1;*/

			//if(acc.r>0) ra = dirLC.r / (2*dirLC.r-acc.r); else ra = 1;
			//if(acc.g>0) ga = dirLC.g / (2*dirLC.g-acc.g); else ga = 1;
			//if(acc.b>0) ba = dirLC.b / (2*dirLC.b-acc.b); else ba = 1;


		//	B_withoutOBJ += acc - dirLC;
		/*	if(acc.r>0) ra = (acc.r - B_withoutOBJ.r)/ (Specular.r - B_withoutOBJ.r); else ra = 1;
			if(acc.g>0) ga = (acc.g - B_withoutOBJ.g)/ (Specular.g- B_withoutOBJ.g); else ga = 1;
			if(acc.b>0) ba = (acc.b - B_withoutOBJ.b)/ (Specular.b - B_withoutOBJ.b); else ba = 1;
*/

			/*if(acc.r!=0)ra =B_withoutOBJ.r/acc.r; else ra =0;
			if(acc.b!=0)ba = dirLC.b/acc.b; else ba =0;
			if(acc.g!=0)ga = dirLC.g/acc.g; else ga =0;*/


			//if(acc.r>0)
			//{
			//	if((Specular.r - B_withoutOBJ.r)!=0)
			//	{
			//		 ra = (acc.r - B_withoutOBJ.r)/ (Specular.r - B_withoutOBJ.r);
			//		 if(ra>1) ra=1;
			//	}
			//	else
			//	{
			//		ra = 1;
			//	}
			//}
			//else
			//{
			//	ra = 0;
			//}
			//if(acc.g>0)
			//{
			//	if((Specular.g - B_withoutOBJ.g)!=0)
			//	{
			//		 ga = (acc.g - B_withoutOBJ.g)/ (Specular.g - B_withoutOBJ.g);
			//		 if(ga>1) ga=1;
			//	}
			//	else
			//	{
			//		ga = 1;
			//	}
			//}
			//else
			//{
			//	ga = 0;
			//}
			//if(acc.b>0)
			//{
			//	if((Specular.b - B_withoutOBJ.b)!=0)
			//	{
			//		 ba = (acc.b - B_withoutOBJ.b)/ (Specular.b - B_withoutOBJ.b);
			//		 if(ba >1) ba=1;
			//	}
			//	else
			//	{
			//		ba = 1;
			//	}
			//}
			//else
			//{
			//	ba = 0;
			//}
			/////////////////

			if( acc.r > 0 )
			{
				if((dirLC.r - B_withoutOBJ.r)!=0)
				{
					ra = (acc.r - B_withoutOBJ.r) / (dirLC.r - B_withoutOBJ.r);
					if(ra < 0)
					{
						ra = -ra;
					}
					//if(ra >1) ra = 1;
				}
				else
				{
					ra = 0.5;
				}
			}
			else
			{
				ra = 0;
			}


				if( acc.g > 0 )
			{
				if((dirLC.g - B_withoutOBJ.g)!=0)
				{
					ga = (acc.g - B_withoutOBJ.g) / (dirLC.g - B_withoutOBJ.g);
					if(ga < 0) ga = -ga;
					//if(ga > 1) ga = 1;
				}
				else
				{
					ga = 0.5;
				}
			}
			else
			{
				ga = 0;
			}



					if( acc.b > 0 )
			{
				if((dirLC.b - B_withoutOBJ.b)!=0)
				{
					ba = (acc.b - B_withoutOBJ.b) / (dirLC.b - B_withoutOBJ.b);
					if(ba <0 ) ba = -ba;
					//if(ba > 1) ba = 1;
				}
				else
				{
					ba = 0.5;
				}
			}
			else
			{
				ba = 0;
			}


            
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
			m_SX += m_DX;
			
			int index = (y - 20) * m_Width + x;
			ralpha[index] = acc.r;
			galpha[index] = B_withoutOBJ.r;
			balpha[index] =dirLC.r;	
		}
		m_SY += m_DY;
		// see if we've been working to long already
		if ((GetTickCount() - msecs) > 100) 
		{
			// return control to windows so the screen gets updated
			m_CurrLine = y + 1;
			return false;
		}
	}

	FILE * rfp = fopen("newalphar.txt", "w");
	FILE * gfp = fopen("newalphag.txt", "w");
	FILE * bfp = fopen("newalphab.txt", "w");

	if(!(rfp && gfp && bfp)) return false;

	for(int i = 0; i < m_Height - 40; i++)
	{
		for(int j = 0; j < m_Width; j++)
		{
			int ind = i * m_Width + j;
			fprintf(rfp,"%.9f\t", ralpha[ind]);
			fprintf(gfp,"%.9f\t", galpha[ind]);
			fprintf(bfp,"%.9f\t", balpha[ind]);
		}
		if(rfp) fprintf(rfp,"\n");
		if(gfp) fprintf(gfp,"\n");
		if(bfp) fprintf(bfp,"\n");
	}
	// all done 
	fclose(rfp);
	fclose(gfp);
	fclose(bfp);
	delete [] ralpha;
	delete [] galpha;
	delete [] balpha;
	return true;
}

}; // namespace Raytracer