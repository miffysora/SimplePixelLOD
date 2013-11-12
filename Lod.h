#pragma once
#include <windows.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
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
//視錐台カリング関係
#include <miffy/math/cull/frustum.h>
#include <miffy/math/vec2.h>
#include <miffy/math/vec3.h>
#include <miffy/math/vec4.h>
#include <miffy/math/matrix.h>
#include <miffy/math/quaternion.h>
#include "Block.h"
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32.lib")
//#pragma comment(lib, "opengl32.lib")
using namespace miffy;
static const double M_PI = 6*asin( 0.5 );
class Lod
{
public:
  Lod(void);
  ~Lod(void);
  void display();
  void RotateFromScreen(int _mx,int _my);
  void reshape();
  void renderGeometry(list<Block>& _blocklist);
  void mouse();
  void zoom(int _direction);
private:
  void OctreeTraversal(const Block& _parent_block, list<Block>* _dst);
  
private:
  
  list<Block> m_block_list;
  mat4<float> m_modelview;
  mat4<float> m_rotation_matrix;
  mat4<float> m_proj_matrix;
  mat4<float> m_inv_modelview;
  mat4<float> m_inv_next_modelview;
  mat4<float> m_next_modelview;
  vec2<float> m_translate;
  double m_zoom;
  frustum<float> m_render_frsutum;
  const vec3<double> m_world_eye_pos;//ワールド座標での視点座標
  const float m_near;
  const float m_far;
  const float m_fovy;
  float m_aspect_ratio;
  Block m_root_block;
  
  
public:
  vec2<int> m_win_size;
  vec2<int> m_last_pushed;
  quat<float> m_current_quaternion;
  quat<float> m_target_quaternion;
  GLFWwindow* m_window;
};
