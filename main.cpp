#include "Lod.h"
int main(int argc, char *argv[]){
	glutInit(&argc,argv);
	glfwInit();
	Lod lod;
	lod.m_window = glfwCreateWindow(720, 576,"Miffy OpenGL", NULL, NULL);
    
	glewInit();
	glfwMakeContextCurrent(lod.m_window);
	enum STATE{STATIC,ROTATING,SHIFT,ZOOM};
	
	while (!glfwWindowShouldClose(lod.m_window))//ウィンドウが開いている場合
    {
		int winx;int winy;
		glfwGetWindowSize(lod.m_window,&winx,&winy);
		lod.reshape();
		lod.mouse();
		//lod.zoom(glfwGetMouseWheel());
		lod.display();
		glfwSwapBuffers(lod.m_window);
		glfwPollEvents();
	 }
	 glfwDestroyWindow(lod.m_window);
	 glfwTerminate();
	return 0;
}
