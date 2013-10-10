all:
	g++ -g Main.cpp IR_Shader.cpp ASTEmitter.cpp -o ASTBuild -O2 -std=c++11
