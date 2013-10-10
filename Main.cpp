#include <vector>
#include "ASTEmitter.h"
#include "IR_Shader.h"

using namespace ASTEmitter;
int main()
{
	ASTBuilder ibuild;
	InstLoc UniConst = ibuild.EmitVec4Uniform("uv");
	ibuild.EmitMain();
	InstLoc vecConst = ibuild.EmitVec4Const(1.0, 0.5, 0.2, 0.1);
	InstLoc vecConst2 = ibuild.EmitVec4Const(2.0, 0.5, 0.2, 0.1);
	InstLoc newConst = ibuild.EmitAdd(vecConst, vecConst2);
	InstLoc newVal = ibuild.EmitAdd(UniConst, newConst);
	ibuild.EmitCol0(newVal);
	ibuild.EmitMainEnd();

	std::string shader = WriteShader(&ibuild);
	printf("Shader is:\n%s\n", shader.c_str());
}
