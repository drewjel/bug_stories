#include "CodeCoordinate.h"
#include "Domain.h"
#include "Interpretation.h"

#include <iostream>

using namespace std;

void Interpretation::putVectorInterp(const VectorASTNode& n, const Vector& av) {
    interpVector.insert(std::make_pair(n,av));
    cout << "INTERPETATION: Linked code vector to abstract vector!\n";
}

Vector& Interpretation::getVectorInterp(const VectorASTNode& n) {
    return interpVector[n];
}

void Interpretation::putExpressionInterp(const ExprASTNode& n, const Expression& e) {
    interpExpression.emplace(n, e);
}

Expression& Interpretation::getExpressionInterp(const ExprASTNode& n) {
   return interpExpression[n]; 
}