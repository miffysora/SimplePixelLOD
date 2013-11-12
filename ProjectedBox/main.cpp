#pragma once
#include <windows.h>
#include <GL/glew.h> 
#include <GL/glfw.h>
#include <GL/freeglut.h>
#include <sstream>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
using namespace std;
#include <miffy/math/vec2.h>
#include <miffy/math/vec3.h>
#include <miffy/math/vec4.h>
#include<miffy/math/matrix.h>
#include <miffy/math/quaternion.h>
#include <miffy/math/cube.h>
#pragma comment(lib,"GLFW.lib")
#pragma comment(lib,"glew32.lib")
const static double M_PI = 4.0*atan(1.0);
using namespace miffy;
class OpenGL{ 
public:
 mat4<float> m_modelview;
  mat4<float> m_rotation_matrix;
  mat4<float> m_proj_matrix;
  mat4<float> m_inv_modelview;
  mat4<float> m_inv_next_modelview;
  mat4<float> m_next_modelview;
  vec2<float> m_translate;
  double m_zoom;
  const   vec3<double> m_world_eye_pos;//ワールド座標での視点座標
  const float m_near;
  const float m_far;
  const float m_fovy;
  float m_aspect_ratio;
 
public:
  vec2<int> m_win_size;
  vec2<int> m_last_pushed;
  quat<float> m_current_quaternion;
  quat<float> m_target_quaternion;
  OpenGL(void)
		:m_world_eye_pos(vec3<double>(0.0, 0.0, 9.0))
		,m_near(0.1f)
		,m_far(100.0f)
		,m_fovy(50.0f)
		,m_zoom(m_world_eye_pos.z)
	{
		
	}
	void display(){
		//描画
		glClearColor(1.0,1.0,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();

		glLoadIdentity();
		gluLookAt(m_world_eye_pos.x, m_world_eye_pos.y, m_zoom,0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		glTranslated(m_translate.x,m_translate.y,0.0);/* 平行移動(奥行き方向) 　　*/
		m_target_quaternion.toMat4(const_cast<float*>(m_rotation_matrix.m));
		glMultMatrixf(m_rotation_matrix.m);//クォータニオンによる回転
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
		glLineWidth(3.0);
		glColor4f(0.0,0.0,0.0,1.0);
		cube<float> mycube;
		mycube.DrawWireCube();
		glColor4ub(116,183,196,255);
		glGetFloatv(GL_MODELVIEW_MATRIX,m_modelview.m);
		glLineWidth(4.0);
		mycube.drawProjectedBoundingBox(m_modelview,m_proj_matrix,m_win_size);
		//glutWireCube(1.0);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		glPopMatrix();

	}//for display

	void RotateFromScreen(int _mx,int _my){
		float dx=(float)(_mx-m_last_pushed.x)/(float)m_win_size.x;
		float dy=(float)(_my-m_last_pushed.y)/(float)m_win_size.y;
		//過去との回転の合成をどうやっていいかわからない。やっぱクオータニオンが必要
		vec3<float> rotate_axis=vec3<float>(dy,dx,0.0);
		float axislength=rotate_axis.length();
		if(axislength!=0.0){
			float radian=(float)fmod(axislength*(float)M_PI,360.0);//画面いっぱいで調度一周になるようにする。
			rotate_axis.normalize();//軸の長さを1にする。
			quat<float> difq(cos(radian),rotate_axis.x*sin(radian),rotate_axis.y*sin(radian),0.0);
			m_target_quaternion=difq*m_current_quaternion;

		}
	}
	
	void reshape(int _width, int _height){
		m_win_size.set(_width,_height);
		m_translate.x = 0.0;
		m_translate.y = 0.0;

		glViewport(0, 0, (GLsizei) _width, (GLsizei) _height);

		m_aspect_ratio = (float)_width/(float)_height;

		//ブロック前後クリッピング初期化
		glMatrixMode(GL_PROJECTION);  /* 投影変換の設定 */
		glLoadIdentity(); 
		gluPerspective(m_fovy, m_aspect_ratio, m_near, m_far);//original
		glGetFloatv(GL_PROJECTION_MATRIX, m_proj_matrix.m);    
		glMatrixMode(GL_MODELVIEW);

	}
	void zoom(int _direction){
		m_zoom=(double)_direction*0.4+m_world_eye_pos.z;
		

	}

};
int main(int argc, char *argv[]){
	glutInit(&argc,argv);
	glfwInit();
	glfwOpenWindow( 0,0, 0,0,0,0,0,0, GLFW_WINDOW );
	glfwSetWindowTitle("Miffy OpenGL");
	glewInit();
	enum STATE{STATIC,ROTATING,SHIFT,ZOOM};
	OpenGL opengl;
	while (glfwGetWindowParam(GLFW_OPENED))//ウィンドウが開いている場合
	{
		int winx,winy;
		glfwGetWindowSize(&winx,&winy);
		opengl.reshape(winx,winy);
		int mx,my;
		glfwGetMousePos(&mx,&my);
		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)){
			opengl.RotateFromScreen(mx,my);
		}else{//回転やめ
			opengl.m_last_pushed.set(mx,my);
			opengl.m_current_quaternion=opengl.m_target_quaternion;
		}
		opengl.zoom(glfwGetMouseWheel());
		opengl.display();
		glfwSwapBuffers();
	}
	return 0;
}
