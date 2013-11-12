#include "Lod.h"

Lod::Lod(void)
	:m_world_eye_pos(vec3<double>(0.0, 0.0, 9.0))
	,m_near(0.1f)
	,m_far(100.0f)
	,m_fovy(30.0f)
	
{
	m_zoom=m_world_eye_pos.z;
	Block::Init(1024,64,1.0);

}
void Lod::display(){
	//描画
	glClearColor(0.3,0.3,0.3,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	
	gluLookAt(m_world_eye_pos.x, m_world_eye_pos.y, m_zoom,0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslated(m_translate.x,m_translate.y,0.0);//平行移動(奥行き方向) 
	m_target_quaternion.toMat4(const_cast<float*>(m_rotation_matrix.m));
	glMultMatrixf(m_rotation_matrix.m);//クォータニオンによる回転
	glPushMatrix();
	glTranslatef(-0.5f,-0.5f,0.0f);//0-1座標から-0.5-0.5座標にする　見た目上、真ん中にするため
	glGetFloatv(GL_MODELVIEW_MATRIX,m_modelview.m);
	//文字を書く 
	glColor3d(1.0,1.0,1.0);
	//Block::ROOT_BLOCK.m_cube.DrawVertexIdBitmap();
	m_block_list.clear();//毎フレームクリアしないとだめ
	OctreeTraversal(Block::ROOT_BLOCK,&m_block_list);
	renderGeometry(m_block_list);//ボリュームデータとかを描画。
	glPopMatrix();
	glPopMatrix();
	


}//for display
/*!
@param _dst このブロックリストに最適解像度のブロックを詰める。
トラバースしながら描くでもいいけど。
*/
void Lod::OctreeTraversal(const Block& _parent_block, list<Block>* _dst){
	if(_parent_block.IsBestResolution(m_modelview,m_proj_matrix,m_win_size)){
		for(int z=0;z<2;z++){
			for(int y=0;y<2;y++){
				for(int x=0;x<2;x++){

					OctreeTraversal(_parent_block.getChildren(x,y,z),_dst);
				}
			}
		}
	}else{
		_dst->push_back(_parent_block);
	}

}
void Lod::RotateFromScreen(int _mx,int _my){
	float dx=(float)(_mx-m_last_pushed.x)/(float)m_win_size.x;
	float dy=(float)(_my-m_last_pushed.y)/(float)m_win_size.y;
	vec3<float> rotate_axis=vec3<float>(dy,dx,0.0);
	float axislength=rotate_axis.length();
	if(axislength!=0.0){
		float radian=(float)fmod(axislength*(float)M_PI,360.0);//画面いっぱいで調度一周になるようにする。
		rotate_axis.normalize();//軸の長さを1にする。
		quat<float> difq(cos(radian),rotate_axis.x*sin(radian),rotate_axis.y*sin(radian),0.0);
		m_target_quaternion=difq*m_current_quaternion;

	}
}
void Lod::renderGeometry(list<Block>& _blocklist){
	//描画
	list<Block>::iterator it=_blocklist.begin();
	while(it!=_blocklist.end()){
		it->renderBlockLines();
		it++;
	}
}
void Lod::reshape(){
	glfwGetFramebufferSize(m_window, &m_win_size.x, &m_win_size.y);
	m_translate.x = 0.0;
	m_translate.y = 0.0;


	glViewport(0, 0, (GLsizei) m_win_size.x, (GLsizei) m_win_size.y);
	

	m_aspect_ratio = (float)m_win_size.x/(float)m_win_size.y;

	//ブロック前後クリッピング初期化
	glMatrixMode(GL_PROJECTION);  /* 投影変換の設定 */
	glLoadIdentity(); 
	gluPerspective(m_fovy, m_aspect_ratio, m_near, m_far);
	glGetFloatv(GL_PROJECTION_MATRIX, m_proj_matrix.m);	
	m_render_frsutum.setFromPerspective(m_fovy,m_aspect_ratio,m_near,m_far,m_proj_matrix);	
	glMatrixMode(GL_MODELVIEW);


}
void Lod::zoom(int _direction){
	m_zoom=(double)_direction*0.4f+m_world_eye_pos.z;

}

Lod::~Lod(void)
{
}
void Lod::mouse(){
	static vec2<int> tra_lastpush;
	double  mx,my;
	glfwGetCursorPos(m_window,&mx,&my);
	if(glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_LEFT)){
		RotateFromScreen(mx,my);
	}else{//回転やめ
		m_last_pushed.set(mx,my);
		m_current_quaternion=m_target_quaternion;
	}
	if(glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_RIGHT)){//平行移動
		m_translate.x+=(float)(mx-tra_lastpush.x)/(float)m_win_size.x;
		m_translate.y+=(float)(my-tra_lastpush.y)*-1.0f/(float)m_win_size.y;
		tra_lastpush.set(mx,my);
	}else{
		tra_lastpush.set(mx,my);
	}
}