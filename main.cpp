//=============================================================================
// Sample Application: Lighting (Per Fragment Phong)
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "glm.h"
#include <FreeImage.h> //*** Para Textura: Incluir librer?a

//---------------------------------------------------------------------------

class myWindow : public cwc::glutWindow
{
protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   cwc::glShader* shader1; //Para Textura: variable para abrir los shader de textura
   GLuint ProgramObject;
   clock_t time0,time1;
   float timer010;  // timer counting 0->1->0
   bool bUp;        // flag if counting up or down.
   GLMmodel* objmodel_ptr[3];
   GLMmodel* objmodel_ptr1; //*** Para Textura: variable para objeto texturizado
   GLuint texid; //*** Para Textura: variable que almacena el identificador de textura
   int cantMallas = 3;  // Cantidad de mallas que vamos a incluir en nuestro mundo
   std::string objetosMundo[3] = {"palm_island_OBJ.obj", "personaje.obj", "DibujoTribu.obj"};  // Nombre de las mallas que vamos a usar
   bool moverDerecha, moverIzquierda, moverAtras, moverFrente;  // Variables que dicen cuando se tiene que mover el objeto interactivo
   float posicionX, posicionZ;  // El objeto solo se movera en X y Z, en Y no porque sino estaria volando.
   float cantMovimiento = 0.25;  

public:
	myWindow(){}

	void interactuarMalla(){  // Funcion que se encarga de mover un objeto dentro del mundo. Este sera nuestro objeto interactivo.  
		if (moverDerecha) {
			posicionX += cantMovimiento; // Mueve el objeto a la derecha
		}
		else if (moverIzquierda){
			posicionX -= cantMovimiento; // Mueve el objeto a la izquierda
		}
		else if (moverAtras){
			posicionZ -= cantMovimiento;  // Mueve el objeto hacia atras
		}
		else if (moverFrente){
			posicionZ += cantMovimiento;  // Mueve el hacia el frente
		}
	}

	//*** Para Textura: aqui adiciono un m?todo que abre la textura en JPG
	void initialize_textures(void)
	{
		int w, h;
		GLubyte* data = 0;
		//data = glmReadPPM("soccer_ball_diffuse.ppm", &w, &h);
		//std::cout << "Read soccer_ball_diffuse.ppm, width = " << w << ", height = " << h << std::endl;

		//dib1 = loadImage("soccer_ball_diffuse.jpg"); //FreeImage

		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Loading JPG file
		FIBITMAP* bitmap = FreeImage_Load(
			FreeImage_GetFileType("./Mallas/texturaBoat.jpg", 0),
			"./Mallas/texturaBoat.jpg");  //*** Para Textura: esta es la ruta en donde se encuentra la textura

		FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
			0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

		FreeImage_Unload(pImage);
		//
		glEnable(GL_TEXTURE_2D);
	}


	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		interactuarMalla(); // Mover objeto interactivo

		if (shader) shader->begin();
		glTranslatef(0, 0, -6.5);

		// Se agrega la isla... (Objeto artificial)
		glPushMatrix();
		glScalef(4, 4, 4);
		glmDraw(objmodel_ptr[0], GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();

		// Se agrega el personaje... (Objeto natural e interactivo)
		glPushMatrix();
		glTranslatef(-1.0f, 1.0f, -2.0f);
		glScalef(0.5, 0.5, 0.5);
		glmDraw(objmodel_ptr[1], GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();

		// Se agrega el objeto creado por nosotros... (Objeto creado por la tribu)
		glPushMatrix();
		glTranslatef(-0.5f, 1.0f, -2.0f);
		glScalef(0.3, 0.3, 0.3);
		glmDraw(objmodel_ptr[2], GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();

		if (shader) shader->end();
		
		// Para nuestro objeto texturizado... (Objeto con textura)
		if (shader) shader->begin();
		glPushMatrix();
		glTranslatef(1.0f, 1.0f, -2.0f);
		glRotatef(45, 0.5, 1.0f, 0.1f);
		glScalef(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texid);
		glmDraw(objmodel_ptr1, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
		glPopMatrix();
		if (shader) shader->end();

      glutSwapBuffers();
      glPopMatrix();

      UpdateTimer();

		Repaint();
	}

	virtual void OnIdle() {}

	// When OnInit is called, a render context (in this case GLUT-Window) 
	// is already available!
	virtual void OnInit()
	{
		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);

		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		if (shader==0) 
         std::cout << "Error Loading, compiling or linking shader\n";
      else
      {
         ProgramObject = shader->GetProgramObject();
      }

	 //*** Para Textura: abre los shaders para texturas
		shader1 = SM.loadfromFile("vertexshaderT.txt", "fragmentshaderT.txt"); // load (and compile, link) from file
		if (shader1 == 0)
			std::cout << "Error Loading, compiling or linking shader\n";
		else
		{
			ProgramObject = shader1->GetProgramObject();
		}

      time0 = clock();
      timer010 = 0.0f;
      bUp = true;

	  //Abrir mallas

	  for (int j = 0; j < cantMallas; j++) {
		  objmodel_ptr[j] = NULL;
		  if (!objmodel_ptr[j]) {
			  std::string ubicacionMalla = "./Mallas/" + objetosMundo[j];  // Carga la ubicacion del .obj para no hacerlo uno por uno
			  char ubicacionMallaArr[100];
			  std::strcpy(ubicacionMallaArr, ubicacionMalla.c_str());
			  printf("%s", ubicacionMallaArr);
			  objmodel_ptr[j] = glmReadOBJ(ubicacionMallaArr);  
			  if (!objmodel_ptr[j]) {
				  exit(0);
			  }
			  glmUnitize(objmodel_ptr[j]);
			  glmFacetNormals(objmodel_ptr[j]);
			  glmVertexNormals(objmodel_ptr[j],90.0);
		  }
	  }

	  //*** Para Textura: abrir malla de objeto a texturizar
		objmodel_ptr1 = NULL;

	  if (!objmodel_ptr1)
	  {
		  objmodel_ptr1 = glmReadOBJ("./Mallas/boat1obj.obj");
		  if (!objmodel_ptr1)
			  exit(0);

		  glmUnitize(objmodel_ptr1);
		  glmFacetNormals(objmodel_ptr1);
		  glmVertexNormals(objmodel_ptr1, 90.0);
	  }
 
	  //*** Para Textura: abrir archivo de textura
	  initialize_textures();
      DemoLight();

	}

	virtual void OnResize(int w, int h)
   {
      if(h == 0) h = 1;
	   float ratio = 1.0f * (float)w / (float)h;

      glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	
	   glViewport(0, 0, w, h);

      gluPerspective(45,ratio,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(0.0f, 8.0f, -15.0f,
		   0.0, -1.0, .0,
		   0.0f, 1.0f, 0.0f);
   }
	virtual void OnClose(void){}
	virtual void OnMouseDown(int button, int x, int y) {}    
	virtual void OnMouseUp(int button, int x, int y) {}
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}

	virtual void OnKeyDown(int nKey, char cAscii)
	{       
		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		} 
		switch (cAscii)
		{
		case 'w':
			moverFrente = false;
			break;
		case 's':
			moverAtras = false;
			break;
		case 'a':
			moverIzquierda = false;
			break;
		case 'd':
			moverDerecha = false;
			break;
		}
	};

	virtual void OnKeyUp(int nKey, char cAscii)
	{
		if (cAscii == 'q')      // q: Shader
			shader->enable();
		else if (cAscii == 'e') // e: Fixed Function
			shader->disable();
	};

   void UpdateTimer()
   {
      time1 = clock();
      float delta = static_cast<float>(static_cast<double>(time1-time0)/static_cast<double>(CLOCKS_PER_SEC));
      delta = delta / 4;
      if (delta > 0.00005f)
      {
         time0 = clock();
         if (bUp)
         {
            timer010 += delta;
            if (timer010>=1.0f) { timer010 = 1.0f; bUp = false;}
         }
         else
         {
            timer010 -= delta;
            if (timer010<=0.0f) { timer010 = 0.0f; bUp = true;}
         }
      }
   }

   void DemoLight(void)
   {
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
   }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() {std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------

