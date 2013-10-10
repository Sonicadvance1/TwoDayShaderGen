#ifndef ASTEMITTER_H_
#define ASTEMITTER_H_

#include <vector>
#include <string>
#include <string.h>

typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;

namespace ASTEmitter
{
	enum Opcode 
	{
		// Binary operations
		Add,
		// Output
		Col0Out,
		// Const values
		CFloat, CVec2, CVec3, CVec4,
		// Tmp variable
		CVec4Tmp,
		// Uniform Consts
		CVec4Uniform,
		MainStart, MainEnd,
	};
	struct Inst
	{
		const Opcode _op;
		const u32 _extra;
		const bool _used;
		Inst(Opcode op, u32 extra, bool used) : _op(op), _extra(extra), _used(used) {} 
	};
	class value {
		public:
		enum DataType
		{
			T_FLOAT,
			T_INT,
			T_UINT,
		};
		protected:
		DataType _type;
		u32 _numvalues;
		union {
			s32 _svalues[4];
			u32 _uvalues[4];
			float _fvalues[4];
		};
		public:
		
		u32 getNumValues() { return _numvalues; }
		DataType getDataType() { return _type; }
		u32 getDataUInt(int x) { return _uvalues[x]; }
		s32 getDataSInt(int x) { return _svalues[x]; }
		float getDataFloat(int x) { return _fvalues[x]; }
		float* getRawFloat() { return _fvalues; }	
		value& operator+(const value &rhs)
		{
			switch (getDataType())
			{
				case T_FLOAT:
					_fvalues[0] += rhs._fvalues[0];
					_fvalues[1] += rhs._fvalues[1];
					_fvalues[2] += rhs._fvalues[2];
					_fvalues[3] += rhs._fvalues[3];
				break;
				case T_UINT:
					_uvalues[0] += rhs._uvalues[0];
					_uvalues[1] += rhs._uvalues[1];
					_uvalues[2] += rhs._uvalues[2];
					_uvalues[3] += rhs._uvalues[3];
				break;
				case T_INT:
					_svalues[0] += rhs._svalues[0];
					_svalues[1] += rhs._svalues[1];
					_svalues[2] += rhs._svalues[2];
					_svalues[3] += rhs._svalues[3];
				break;
			}
			return *this;
		}
		float* operator=(const value &rhs)
		{
			if (this != &rhs)
			{
				printf("Error in asignment\n");
			}
			else
				return _fvalues;
		}
	};
	class Vec4: public value {
		public:
			Vec4(float x, float y, float z, float w)
			{
				_type = T_FLOAT;
				_numvalues = 4;
				_fvalues[0] = x;
				_fvalues[1] = y;
				_fvalues[2] = z;
				_fvalues[3] = w;
			}
		};
	unsigned inline getOpcode(Inst I)
	{
		return I._op;
	}
	unsigned inline getOp1(Inst I)
	{
		return (I._extra >> 8) & 0xFF;
	}
	unsigned inline getOp2(Inst I)
	{
		return I._extra >> 16;
	}
	unsigned inline getExtra(Inst I)
	{
		return I._extra & 0xFF;
	}

	unsigned inline isImm(Inst i) {
		return i._op >= CFloat && i._op <= CVec4;
	}
	typedef u32 InstLoc;
	class ASTBuilder
	{
	private:
		InstLoc EmitZeroOp(Opcode op, u32 extra);
		InstLoc EmitUOp(Opcode op, InstLoc Op1, u32 extra);
		InstLoc EmitBiOp(Opcode op, InstLoc Op1, InstLoc Op2, u32 extra);
		InstLoc FoldZeroOp(Opcode op, u32 extra);
		InstLoc FoldUOp(Opcode op, InstLoc Op1, u32 extra);
		InstLoc FoldBiOp(Opcode op, InstLoc Op1, InstLoc Op2, u32 extra);
		InstLoc FoldAdd(InstLoc Op1, InstLoc Op2);
	public:
		ASTBuilder() : _tmpNumber(0), _readPointer(0) {}
		void StartForwardPass() { _readPointer = 0; }
		InstLoc ReadForward() { return _readPointer++; } 
		unsigned int getNumInsts() { return (unsigned int)_instList.size(); }
		Inst getInst(InstLoc I) { return _instList[I]; }
		value getImmValue(InstLoc I) { return _constList[_instList[I]._extra]; }
		std::string getUniformName(InstLoc I) { return _uniformList[_instList[I]._extra]; }

		InstLoc EmitMain() {
			return FoldZeroOp(MainStart, 0);
		}
		InstLoc EmitMainEnd() {
			return FoldZeroOp(MainEnd, 0);
		}
		InstLoc EmitVec4Const(float x, float y, float z, float w);
		InstLoc EmitVec4Const(float *xyzw) { return EmitVec4Const(xyzw[0], xyzw[1], xyzw[2], xyzw[3]); }
		InstLoc EmitVec4Uniform(std::string name);
		InstLoc EmitAdd(InstLoc Op1, InstLoc Op2){
			return FoldBiOp(Add, Op1, Op2, 0);
		}
		InstLoc EmitCol0(InstLoc value) {
			return FoldUOp(Col0Out, value, 0);
		}
	private:
		std::vector<Inst> _instList;
		std::vector<value> _constList;
		std::vector<std::string> _uniformList;
		unsigned _tmpNumber;
		InstLoc _readPointer;
	};
};

#endif
