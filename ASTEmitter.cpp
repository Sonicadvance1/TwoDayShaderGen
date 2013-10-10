#include "ASTEmitter.h"
namespace ASTEmitter
{
InstLoc ASTBuilder::EmitVec4Const(float x, float y, float z, float w)
{
	InstLoc curIndex = _instList.size();
	_instList.push_back(Inst(CVec4, (unsigned int)_constList.size(), false));
	_constList.push_back(Vec4(x, y, z, w));
	return curIndex;
}
InstLoc ASTBuilder::EmitVec4Uniform(std::string name)
{
	InstLoc curIndex = _instList.size();
	_instList.push_back(Inst(CVec4Uniform, (unsigned int)_uniformList.size(), false));
	_uniformList.push_back(name);
	return curIndex;
}
InstLoc ASTBuilder::EmitZeroOp(Opcode op, u32 extra)
{
	InstLoc curIndex = _instList.size();
	_instList.push_back(Inst(op, extra, false));
	return curIndex;
}
InstLoc ASTBuilder::EmitUOp(Opcode op, InstLoc Op1, u32 extra)
{
	InstLoc curIndex = _instList.size();
	_instList.push_back(Inst(op, (Op1 << 8) | extra, false));
	return curIndex;
}
InstLoc ASTBuilder::EmitBiOp(Opcode op, InstLoc Op1, InstLoc Op2, u32 extra)
{
	InstLoc curIndex = _instList.size();
	_instList.push_back(Inst(op, (Op1 << 8) | (Op2 << 16) | extra, false));
	return curIndex;
}

InstLoc ASTBuilder::FoldZeroOp(Opcode op, u32 extra)
{
	return EmitZeroOp(op, extra);
}
InstLoc ASTBuilder::FoldUOp(Opcode op, InstLoc Op1, u32 extra)
{
	return EmitUOp(op, Op1, extra);
}
InstLoc ASTBuilder::FoldBiOp(Opcode op, InstLoc Op1, InstLoc Op2, u32 extra)
{
	switch(op)
	{
		case Add: return FoldAdd(Op1, Op2);
		default:
			printf("Don't support this Folding\n");
			exit(1);
	}
}
InstLoc ASTBuilder::FoldAdd(InstLoc Op1, InstLoc Op2)
{
	if (isImm(getInst(Op1)) && isImm(getInst(Op2)))
		return EmitVec4Const((getImmValue(Op1) + getImmValue(Op2)).getRawFloat());  
	if (getOpcode(getInst(Op1)) == CVec4Uniform && isImm(getInst(Op2)))
	{
		// Can't write to Uniform, spawn a tmp
		InstLoc tmp = FoldZeroOp(CVec4Tmp, _tmpNumber++);
		Op1 = FoldBiOp(Add, tmp, Op1, 0);
		return EmitBiOp(Add, tmp, Op2, 0);
	}
	return EmitBiOp(Add, Op1, Op2, 0);
}

};

