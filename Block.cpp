﻿#include "Block.h"
vec3<float> texCoord[8]={
  vec3<float>(0,0,0),
  vec3<float>(1,0,0),
  vec3<float>(1,1,0),
  vec3<float>(0,1,0),  
  vec3<float>(0,0,1),
  vec3<float>(1,0,1),
  vec3<float>(1,1,1),
  vec3<float>(0,1,1),
};
///< これはたぶん、内側にするために必要なんだ。
static vec3<float> INSIDE[8]={
  vec3<float>(0,0,0),
  vec3<float>(-1.0f,0,0),
  vec3<float>(-1.0f,-1.0f,0),
  vec3<float>(0,-1.0f,0),	
  vec3<float>(0,0,-1.0f),
  vec3<float>(-1.0f,0,-1.0f),
  vec3<float>(-1.0f,-1.0f,-1.0f),
  vec3<float>(0,-1.0f,-1.0f),
};
unsigned int Block::LEVEL_NUM=0;//実態
Block Block::ROOT_BLOCK;
unsigned int  Block::ORIGINAL_VOXEL_NUM;//2のべき乗である必要がある。
unsigned int  Block::BOXEL_PER_BLOCK;

/*!
@brief デフォルトコンストラクタはルートのブロック
*/
Block::Block()
  :m_x(0),m_y(0),m_z(0),m_level(Block::LEVEL_NUM),m_block_num(1),m_size(1.0){
    m_cube.setFromCorner(vec3<float>(0.0f,0.0f,0.0f),1.0f);
}
Block::Block( int _x, int _y, int _z, int _level,  float _blockLength)
  :m_x(_x),m_y(_y),m_z(_z),m_level(_level),m_size(_blockLength)
{
  m_block_num=(int)(Block::ROOT_BLOCK.m_size/m_size);
  m_cube.setFromCorner(vec3<float>(m_x,m_y,m_z)*m_size,m_size);
}
const Block Block::getChildren(int _x,int _y,int _z)const{
  
  return Block(m_x*2+_x,m_y*2+_y,m_z*2+_z,m_level-1,m_size*0.5f);
}
//Block& Block::operator=(const Block& _in){//デバッグのためい必要
//	m_x=_in.m_x;
//	m_y=_in.m_y;
//	m_z=_in.m_z;
//	m_cube=_in.m_cube;
//	m_level=_in.m_level;
//	m_block_num=_in.m_block_num;
//	return Block(_
//}
void Block::Init(int _original_voxel_num,int _voxel_per_block,float _root_length){
  Block::LEVEL_NUM=0;
  Block::ORIGINAL_VOXEL_NUM=_original_voxel_num;
  Block::BOXEL_PER_BLOCK=_voxel_per_block;
  int voxel=Block::ORIGINAL_VOXEL_NUM;
  while(voxel!=Block::BOXEL_PER_BLOCK){
    voxel=voxel>>1;
    Block::LEVEL_NUM++;
  }
  ROOT_BLOCK.set(0,0,0,Block::LEVEL_NUM,_root_length);
}
void Block::set(const int _x,const int _y,const int _z,const int _level,const  float _blockLength){
  m_x=_x;
  m_y=_y;
  m_z=_z;
  m_level=_level;
  m_size=_blockLength;
  m_block_num=(int)(Block::ROOT_BLOCK.m_size/m_size);

}
void Block::info(const char* _message)const{
  if(m_level<0){assert(!"illegal");}
  printf("%s:",_message);
  printf("Level[%d](%d,%d,%d)\n",m_level,m_x,m_y,m_z);
  m_cube.print("cube");
}
/*!
@brief frustum<T>の汎用性を保持したかったのでBlockのほうに判定を作った。
*/
int Block::IsInFrustum(const frustum<float>& _frustum){
  //もしビューボリュームにblockが入っていれば
  aabox<float> abox(m_cube.corner[0],m_size,m_size,m_size);//ブロックの情報をAxis Aligned Boxで表現している。AABoxのx,y,zはブロックの辺の長さ。
  return _frustum.boxInFrustum(abox);
}
Block::~Block(void){}

void Block::renderBlockLines(){

  glColor3f(1.0,1.0,1.0);
  m_cube.DrawWireCube();
}

void Block::renderBlockQUADS(float times){
  glColor4f(m_x*times,m_y*times,m_z*times,0.5);
  m_cube.DrawQuads();
}
/*!
@return false:これ以上詳細にする必要なし　true:もっと詳細にしてよし
*/bool Block::IsBestResolution(const mat4<float>& _modelMatrix,const mat4<float>& _projmatrix,const vec2<int>& _winsize)const
{//最適レベルの値を返す
	if( m_level==0){//オリジナル解像度なので無理。
		return false;
	}
	int far_id=m_cube.FarthestIndex(_modelMatrix);

	//int=0 粗くする 1=stay same 2=詳細にする
	cube<float> fartherest_voxel;
	float one_voxel_size=m_size/(float)BOXEL_PER_BLOCK*0.5f;//*0.5fにしたのは、仮にもう一段階したとき、１ピクセルより小さくなるかどうかってやり方にしたいから
	vec3<float> far_corner=m_cube.corner[far_id]+vec3<float>(vec3<float>(m_x,m_y,m_z)*one_voxel_size)*INSIDE[far_id];
	fartherest_voxel.setFromCorner(far_corner,one_voxel_size);

	vec2<float> projected_size=fartherest_voxel.projectedsize(_modelMatrix,_projmatrix,_winsize);
	if((projected_size.x<1.0f)|| (projected_size.y<1.0f)){
		return false;//これ以上詳細にしなくてもいい	　			
	}	
	return true;//これ以上詳細んいする必要がある


}
