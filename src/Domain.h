#ifndef BRIDGE_H
#define BRIDGE_H

#include <cstddef>  
#include "clang/AST/AST.h"
#include <vector>
#include <string>

#include "Coords.h"

using namespace std;

namespace domain {
	
/*
Named space. Later on this will become a full-fledged Euclidean space object.
*/
class Space {
public:
	Space() : name_("") {};
	Space(string name) : name_(name) {};
	string getName() const;
	string toString() const { return getName(); } 
    friend ostream & operator << (ostream &out, const Space &s)
	{ 
    out << s.getName(); 
    return out; 
	} 
private:
	string name_;
};


/*
The next set of definitions provides a basis for representing code 
expressions lifted to domain expressions.
*/

class VecIdent {
public:
	VecIdent(Space& space, const coords::VecIdent* vardecl) : space_(&space), vardecl_(vardecl) {}
	Space* getSpace() const { return space_; }
	const coords::VecIdent* getVarDeclWrapper() const { return vardecl_; }
	string toString() const { return getName(); }
	string getName() const;
private:
	Space* space_;
	const coords::VecIdent* vardecl_;
};

// TODO - Change name of this class? DomainExpr?
class VecExpr {
public:
    VecExpr(const Space& s, const coords::VecExpr* ast) : space_(s), ast_(ast) {}
	const coords::VecExpr* getVecExpr() const { return ast_; }
    const Space& getSpace() const;
	virtual string toString() const {
		if (ast_ != NULL) {
			//cerr << "Domain::VecExpr::toString: coords::VecVecExpr pointer is " << std::hex << ast_ << "\n";
			return "(" + ast_->toString() + " : " + space_.getName() + ")";
		}
		else {
			return "domain.VecExpr:toString() NULL ast_\n";	
		}
	}
	const coords::VecExpr* getCoords() {
		return ast_;
	}
protected:
    const Space& space_;
	const coords::VecExpr* ast_;	// TODO: Call it coords_
};


// VecExpr?? It's a Ctor
class VecLitExpr : public VecExpr {
public:
    VecLitExpr(Space& s, const coords::VecLitExpr* ast) : VecExpr(s, ast) { }
    void addFloatLit(float num);
	virtual string toString() const {
		return "(" + ast_->toString() + " : " + getSpace().toString() + ")";
	}
private:
};


/*
BIG TODO : Have Domain objects connect back to ast ***containers***, as in VecLitExpr here.
*/
// Note: No printing of space, as it's inferred
class VecVarExpr : public VecExpr {
public:
    VecVarExpr(Space& s, const coords::VecExpr* ast) : domain::VecExpr(s, ast) {}
	virtual string toString() const {
		return "(" + ast_->toString() + " )";
	}
private:
};


class VecVecAddExpr : public VecExpr {
public:
   VecVecAddExpr(
        Space& s, const coords::VecExpr* ast, VecExpr *mem, VecExpr *arg) : 
			VecExpr(s, ast), arg_(arg), mem_(mem) {	
	}

	const VecExpr& getMemberVecExpr();
	const VecExpr& getArgVecExpr();

	virtual string toString() const {
		return "(add " + mem_->toString() + " " + arg_->toString() + ")";
	}

	// get the default space for this VecAddVecExpr using the space of the arg_left_
	//const Space& getVecVecAddExprDefaultSpace();
private:
    VecExpr* arg_;
    VecExpr* mem_;
};

/*
Domain representation of binding of identifier to expression.
Takes clang::VarDecl establishing binding (in a wrapper) and 
the *domain* VecIdent and Expression objects being bound.
*/
class Binding {
public:
	Binding(const coords::Binding* ast_wrapper, const domain::VecIdent* identifier, const domain::VecExpr* expr):
			ast_wrapper_(ast_wrapper), identifier_(identifier), expr_(expr) {}
	const coords::Binding* getVarDecl() const {return ast_wrapper_; } 
	const domain::VecExpr* getVecExpr() const { return expr_; }
	const domain::VecIdent* getVecIdent() { return identifier_; }
	string toString() const {
		return "def " + identifier_->toString() + " := " + expr_->toString();
	}
private:
	const coords::Binding* ast_wrapper_;
	const VecIdent* identifier_;
	const VecExpr* expr_;
};

/*
Domain representation of binding of identifier to expression.
*/
enum VecCtorType {VEC_EXPR, VEC_LIT, VEC_VAR, VEC_NONE } ; 

class Vector  {
public:
	Vector(const Space& s, const coords::AddConstruct* coords, domain::VecExpr* expr) :
		space_(&s), coords_(coords), expr_(expr), tag_(VEC_NONE) { 
	}
	bool isExpr() { return (tag_ == VEC_EXPR); } 
	bool isLit() { return (tag_ == VEC_LIT); } 
	const Space* getSpace() {return space_; }
	//
	// TODO: Normalize coords out of this class
	//
	const coords::Vector* getCoords() const {return coords_; /* const coords::AddConstruct* */} 
	const domain::VecExpr* getVecExpr() const { return expr_; /* domain::VecExpr* */ }

	/*
	Domain.h:160:50: error: cannot convert 'const coords::AddConstruct* const' to 'const coords::Vector*' in return
  const coords::Vector* getCoords() const {return coords_; }
    */

	string toString() const {
		return expr_->toString();
	}
private:
	const Space* space_; // INFER
	const coords::AddConstruct* coords_; //TODO: WRONG
	const domain::VecExpr* expr_; // child
	VecCtorType tag_;
};

/*
A Domain is a lifted version of selected code represented as a collection 
of C++ objects. It should be isomorphic to the domain, and domain models 
(e.g., in Lean) should be producible using a Domain as an input.
*/

// Definition for Domain class 

class Domain {
public:
	Space& addSpace(const string& name);
	//VecLitExpr& addLitExpr(Space& s, const coords::coords::Lit* ast);		/* BIG TODO: Fix others */
	VecIdent* addVecIdent(Space& s, const coords::VecIdent* ast);
	VecExpr& addVecVarExpr(const coords::VecVarExpr* ast);
	// should be addVecLit*Ctor*, with contained lit data 
	VecExpr* addVecLitExpr(Space& s, const coords::VecLitExpr* e);
	VecExpr* addVecVecAddExpr(Space& s, coords::VecVecAddExpr* e, domain::VecExpr* left_, domain::VecExpr* right_);
	// coords for container, domain object for child, lit | expr
	// if lit, child is -- empty? -- else coords and domain VecExpr
	Vector* addVector(coords::Vector* v, domain::VecExpr *vec);
	Binding& addBinding(const coords::Binding* vardecl, const VecIdent* identifier, const VecExpr* expression);
	void dump() {
		cerr << "Domain expressions:\n";
		dumpExpressions(); // print contents on cerr
		cerr << "Domain VecIdents\n";
		dumpVecIdents(); // print contents on cerr
		cerr << "Domain Bindings\n";
		dumpBindings(); // print contents on cerr
	}
	void dumpExpressions(); // print contents on cerr
	void dumpVecIdents(); // print contents on cerr
	void dumpBindings(); // print contents on cerr

	bool isConsistent();
	vector<Space>& getAllSpaces();
private:
	vector<Space> spaces;
	vector<VecIdent> identifiers;
	vector<VecExpr*> expressions;
	vector<Binding> bindings;
	vector<Vector*> vectors;
};

} // end namespace

#endif