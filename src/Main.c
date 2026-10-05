#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Cube.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mathlib.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mesh.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_World3D.h"
#include "/home/codeleaded/System/Static/Library/MarchingCubes.h"
#include "/home/codeleaded/System/Static/Library/PerlinNoise.h"

#define FIELDX	100
#define FIELDY	100
#define FIELDZ	100

Camera cam;
World3D world;
int Mode = 0;
int Menu = 0;
float Speed = 4.0f;

float pos = 0.0f;
float height = 1.0f;

void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window);
	}
	
	Menu = m;
}
void Setup(AlxWindow* w){
	Menu_Set(1);

	cam = Camera_Make(
		(Vec3D){ 0.0f,5.0f,-25.0f,1.0f },
		(Vec3D){ 0.0f,0.0f,0.0f,1.0f },
		90.0f
	);

	world = World3D_Make(
		Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }),
		Matrix_MakePerspektive(cam.p,cam.up,cam.a),
		Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f)
	);
	world.normal = WORLD3D_NORMAL_CAP;

	Vector_Clear(&world.trisIn);
	MarchingCubes_Render2D(PerlinNoise_2D_Get,&world.trisIn,cam.p.x,cam.p.z,FIELDX,FIELDZ,height,WHITE);
	//MarchingCubes_Render3D(PerlinNoise_3D_Get,&world.trisIn,cam.p.x,cam.p.y,cam.p.z,FIELDX,FIELDY,FIELDZ);
}
void Update(AlxWindow* w){
	if(Menu==1){
		Camera_Focus(&cam,GetMouseBefore(),GetMouse(),GetScreenRect().d);
		Camera_Update(&cam);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	
	if(Stroke(ALX_KEY_ESC).PRESSED)
		Menu_Set(!Menu);

	if(Stroke(ALX_KEY_Z).PRESSED)
		Mode = Mode < 2 ? Mode+1 : 0;

	const int prepos_x = (int)floorf(cam.p.x * 0.05f); 
	const int prepos_y = (int)floorf(cam.p.y * 0.05f); 
	const int prepos_z = (int)floorf(cam.p.z * 0.05f); 

	if(Stroke(ALX_KEY_W).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.ld,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_S).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.ld,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_A).DOWN)
		cam.p = Vec3D_Add(cam.p,Vec3D_Mul(cam.sd,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_D).DOWN)
		cam.p = Vec3D_Sub(cam.p,Vec3D_Mul(cam.sd,Speed * w->ElapsedTime));
	if(Stroke(ALX_KEY_R).DOWN)
		cam.p.y += Speed * w->ElapsedTime;
	if(Stroke(ALX_KEY_F).DOWN)
		cam.p.y -= Speed * w->ElapsedTime;

	if(Stroke(ALX_KEY_LEFT).DOWN)
		height *= 1.01f;
	else if(Stroke(ALX_KEY_RIGHT).DOWN)
		height *= 0.99f;

	if(Stroke(ALX_KEY_UP).DOWN)
		PerlinNoise_Persistance_Set(PerlinNoise_Persistance_Get() * 1.01);
	else if(Stroke(ALX_KEY_DOWN).DOWN)
		PerlinNoise_Persistance_Set(PerlinNoise_Persistance_Get() * 0.99);

	World3D_Set_Model(&world,Matrix_MakeWorld((Vec3D){ 0.0f,0.0f,0.0f,1.0f },(Vec3D){ 0.0f,0.0f,0.0f,1.0f }));
	World3D_Set_View(&world,Matrix_MakePerspektive(cam.p,cam.up,cam.a));
	World3D_Set_Proj(&world,Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f));
	
	const int newpos_x = (int)floorf(cam.p.x * 0.05f); 
	const int newpos_y = (int)floorf(cam.p.y * 0.05f); 
	const int newpos_z = (int)floorf(cam.p.z * 0.05f);

	Vector_Clear(&world.trisIn);
	MarchingCubes_Render2D(PerlinNoise_2D_Get,&world.trisIn,cam.p.x,cam.p.z,FIELDX,FIELDZ,height,WHITE);

	//if(prepos_x != newpos_x || prepos_y != newpos_y || prepos_z != newpos_z){
	//	Vector_Clear(&world.trisIn);
	//	MarchingCubes_Render2D(PerlinNoise_2D_Get,&world.trisIn,cam.p.x,cam.p.z,FIELDX,FIELDZ,height,WHITE);
	//	MarchingCubes_Render3D(PerlinNoise_3D_Get,&world.trisIn,cam.p.x,cam.p.y,cam.p.z,FIELDX,FIELDY,FIELDZ,WHITE);
	//}

	Clear(LIGHT_BLUE);
	World3D_Update(&world,cam.p,(Vec2){ GetWidth(),GetHeight() });

	for(int i = 0;i<world.trisOut.size;i++){
		Tri3D* t = (Tri3D*)Vector_Get(&world.trisOut,i);
		const Pixel c = Pixel_Mulf(t->c.c,t->c.l);

		if(Mode==0)
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
		if(Mode==1)
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c,1.0f);
		if(Mode==2){
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),WHITE,1.0f);
		}
	}

	CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0,0,RED,"X: %f, Y: %f, Z: %f",cam.p.x,cam.p.y,cam.p.z);
	CStr_RenderAlxFontf(WINDOW_STD_ARGS,GetAlxFont(),0,GetAlxFont()->CharSizeY + 1,RED,"SizeIn: %d, SizeBuff: %d, SizeOut: %d",world.trisIn.size,world.trisBuff.size,world.trisOut.size);
}
void Delete(AlxWindow* w){
	World3D_Free(&world);
	AlxWindow_Mouse_SetVisible(&window);
}

int main(){
	if(Create("Marching Cubes",2500,1440,1,1,Setup,Update,Delete))
        Start();
    return 0;
}