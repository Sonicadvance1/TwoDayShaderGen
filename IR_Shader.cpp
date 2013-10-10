#include <sstream>
#include "IR_Shader.h"

using namespace ASTEmitter;
std::string getValueName(ASTBuilder *build, InstLoc I) {
	std::string tmp;
	char out[128];
	if (!isImm(build->getInst(I)))
	{
		switch(getOpcode(build->getInst(I)))
		{
			case CVec4Uniform:
				return build->getUniformName(I);	
			break;
			case CVec4Tmp:
				sprintf(out, "tmp%d", getExtra(build->getInst(I))); 
				return std::string(out); 
			break;
			case Add:
			{
				InstLoc Op1 = getOp1(build->getInst(I));
				return getValueName(build, Op1);	
				break;
			}
			default:
				return "(Bad !Imm)";
			break;
		}
	}
	value Imm = build->getImmValue(I);
	switch (Imm.getDataType())
	{
		case value::DataType::T_FLOAT:
			tmp = "vec%d(";
			for(int a = 0; a < Imm.getNumValues(); ++a)
				tmp += (a + 1 == Imm.getNumValues()) ? "%f": "%f, ";
			tmp += ")";
			sprintf(out, tmp.c_str(), Imm.getNumValues(), 
				Imm.getDataFloat(0),
				Imm.getDataFloat(1),
				Imm.getDataFloat(2),
				Imm.getDataFloat(3));
			break;
		case value::DataType::T_INT:
			tmp = "ivec%d(";
			for(int a = 0; a < Imm.getNumValues(); ++a)
				tmp += (a + 1 == Imm.getNumValues()) ? "%f": "%f, ";
			tmp += ")";
			sprintf(out, tmp.c_str(), Imm.getNumValues(), 
				Imm.getDataSInt(0),
				Imm.getDataSInt(1),
				Imm.getDataSInt(2),
				Imm.getDataSInt(3));
		break;
		case value::DataType::T_UINT:
			tmp = "uvec%d(";
			for(int a = 0; a < Imm.getNumValues(); ++a)
				tmp += (a + 1 == Imm.getNumValues()) ? "%f": "%f, ";
			tmp += ")";
			sprintf(out, tmp.c_str(), Imm.getNumValues(), 
				Imm.getDataUInt(0),
				Imm.getDataUInt(1),
				Imm.getDataUInt(2),
				Imm.getDataUInt(3));
		break;
	}
	return std::string(out);
}
std::string WriteShader(ASTBuilder *build)
{
	std::ostringstream outputShader;

	build->StartForwardPass();
	for (unsigned int i = 0; i < build->getNumInsts(); ++i)
	{
		Inst I = build->getInst(i);	
		switch(getOpcode(I))
		{
			case Add:
			{
				InstLoc Op1 = getOp1(I);
				InstLoc Op2 = getOp2(I);
				outputShader << getValueName(build, Op1) << " += " << getValueName(build, Op2) << ";\n";
				break;
			}
			case Col0Out:
			{
				Inst Op1 = build->getInst(getOp1(I));
				outputShader << "ocol0 = " << getValueName(build, getOp1(I)) << ";\n";
			}
			break;
			case MainStart:
				outputShader << "void main() {\n";
			break;
			case MainEnd:
				outputShader << "}\n";
			break;
			case CVec4Uniform:
				outputShader << "uniform vec4 " << build->getUniformName(i) << ";\n";
			break;
			case CVec4Tmp:
				outputShader << "uniform vec4 tmp" << getExtra(I) << " = vec4(0.0, 0.0, 0.0, 0.0);\n";
			break;
			case CVec4: break;
			default:
				printf("Unknown AST instruction; aborting!\n");
				exit(1);
			break;
		}
	}
	return outputShader.str();
}
