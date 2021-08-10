
#include <vector>
#include <iostream>
#include <string>
#include <utility>

// DONE: Separate clients from Domain
// #include "Checker.h"

#include "Domain.h"

//#include <g3log/g3log.hpp>

#ifndef leanInferenceWildcard
#define leanInferenceWildcard "_"
#endif

using namespace std;
using namespace domain;


DomainObject::DomainObject(std::initializer_list<DomainObject*> args) {
    for(auto dom : args){
        operands_.push_back(dom);
    }
    operand_count = this->operands_.size();
}
DomainObject* DomainObject::getOperand(int i) { return this->operands_.at(i); };
void DomainObject::setOperands(std::vector<DomainObject*> operands) { this->operands_ = operands; };

DomainContainer::DomainContainer(std::initializer_list<DomainObject*> operands) : DomainObject(operands) {
    this->inner_ = nullptr;
    this->error_ = nullptr;
    this->as_ = AnnotationState::Unannotated;
};

DomainContainer::DomainContainer(std::vector<DomainObject*> operands) : DomainObject(operands) {
    this->inner_ = nullptr;
    this->error_ = nullptr;
    this->as_ = AnnotationState::Unannotated;
};
DomainContainer::DomainContainer(std::initializer_list<DomainContainer*> operands) {
    for(auto dom : operands){
        operands_.push_back((DomainObject*)dom);
    }
    operand_count = this->operands_.size();
    this->inner_ = nullptr;
    this->error_ = nullptr;
    this->as_ = AnnotationState::Unannotated;
};

DomainContainer::DomainContainer(std::vector<DomainContainer*> operands) {
    for(auto dom : operands){
        operands_.push_back((DomainObject*)dom);
    }
    operand_count = this->operands_.size();
    this->inner_ = nullptr;
    this->error_ = nullptr;
    this->as_ = AnnotationState::Unannotated;
};



void DomainContainer::setValue(DomainObject* dom_){

    //dom_->setOperands(this->inner_->getOperands());
    
    /*
    WARNING - THIS IS NOT CORRECT CODE. YOU ALSO NEED TO UNMAP/ERASE FROM THE "OBJECT_VEC". DO THIS SOON! 7/12
    */
    //these are getting cached and potentially reused now, don't delete
    //delete this->inner_;

    this->inner_ = dom_;
};

bool DomainContainer::hasValue() const{
    return this->inner_ != nullptr && (dynamic_cast<ErrorObject*>(this->inner_) == nullptr);
};

std::string DomainContainer::toString() const{
    if(this->hasValue()){
        if(auto dc = dynamic_cast<domain::ErrorObject*>(inner_)){
            if(dc->hasValue()){
                return this->inner_->toString();
            }
            else
                return "Error Detected";
        }
        else 
            return this->inner_->toString();
    }
    else{
        return "No Interpretation provided";
    }
};

DomainContainer* Domain::mkDefaultDomainContainer(){
    return new domain::DomainContainer();
};

DomainContainer* Domain::mkDefaultDomainContainer(std::initializer_list<DomainContainer*> operands){
    return new domain::DomainContainer(operands);
};

DomainContainer* Domain::mkDefaultDomainContainer(std::vector<DomainContainer*> operands){
    return new domain::DomainContainer(operands);
};

/*

    StandardTimeCoordinateSpace* mkStandardTimeCoordinateSpace(string name);
    DerivedTimeCoordinateSpace* mkDerivedTimeCoordinateSpace(string name, TimeCoordinateSpace* parent, string* originData, string** basisData);

    Duration* mkDuration(string name, TimeCoordinateSpace* parent, string* value);
    Time* mkTime(string name, TimeCoordinateSpace* parent, string* value);

*/

StandardTimeCoordinateSpace* Domain::mkStandardTimeCoordinateSpace(std::string name){
    StandardTimeCoordinateSpace* sp = new StandardTimeCoordinateSpace(name);
    this->spaces.push_back(static_cast<TimeCoordinateSpace*>(sp));
    this->timeSpaces.push_back(sp);
    return sp;
};

DerivedTimeCoordinateSpace* Domain::mkDerivedTimeCoordinateSpace(std::string name, TimeCoordinateSpace* parent, string* originData, string** basisData){
    DerivedTimeCoordinateSpace* sp = new DerivedTimeCoordinateSpace(name, parent, originData, basisData);
    this->spaces.push_back(static_cast<TimeCoordinateSpace*>(sp));
    this->timeSpaces.push_back(sp);
    return sp;
};
StandardGeom1DCoordinateSpace* Domain::mkStandardGeom1DCoordinateSpace(std::string name){
    StandardGeom1DCoordinateSpace* sp = new StandardGeom1DCoordinateSpace(name);
    this->spaces.push_back(static_cast<Geom1DCoordinateSpace*>(sp));
    this->geom1dSpaces.push_back(sp);
    return sp;
};

DerivedGeom1DCoordinateSpace* Domain::mkDerivedGeom1DCoordinateSpace(std::string name, Geom1DCoordinateSpace* parent, string* originData, string** basisData){
    DerivedGeom1DCoordinateSpace* sp = new DerivedGeom1DCoordinateSpace(name, parent, originData, basisData);
    this->spaces.push_back(static_cast<Geom1DCoordinateSpace*>(sp));
    this->geom1dSpaces.push_back(sp);
    return sp;
};

StandardGeom3DCoordinateSpace* Domain::mkStandardGeom3DCoordinateSpace(std::string name){
    StandardGeom3DCoordinateSpace* sp = new StandardGeom3DCoordinateSpace(name);
    this->spaces.push_back(static_cast<Geom3DCoordinateSpace*>(sp));
    this->geom3dSpaces.push_back(sp);
    return sp;
};

DerivedGeom3DCoordinateSpace* Domain::mkDerivedGeom3DCoordinateSpace(std::string name, Geom3DCoordinateSpace* parent, string* originData, string** basisData){
    DerivedGeom3DCoordinateSpace* sp = new DerivedGeom3DCoordinateSpace(name, parent, originData, basisData);
    this->spaces.push_back(static_cast<Geom3DCoordinateSpace*>(sp));
    this->geom3dSpaces.push_back(sp);
    return sp;
};


Scalar* Domain::mkScalar(std::string name, string* value){
    auto scalar = new Scalar(name, value);
    return scalar;
}

Duration* Domain::mkDuration(std::string name, TimeCoordinateSpace* parent, string* value){
    auto dur = new Duration(name, parent, value);
    return dur;
}

Time* Domain::mkTime(std::string name, TimeCoordinateSpace* parent, string* value){
    auto time = new Time(name, parent, value);
    return time;
}

TimeTransform* Domain::mkTimeTransform(std::string name, TimeCoordinateSpace* domain_, TimeCoordinateSpace* codomain_){
    auto ttransform = new TimeTransform(name, domain_, codomain_);
    return ttransform;
}

Displacement1D* Domain::mkDisplacement1D(std::string name, Geom1DCoordinateSpace* parent, string* value){
    auto disp = new Displacement1D(name, parent, value);
    return disp;
}

Position1D* Domain::mkPosition1D(std::string name, Geom1DCoordinateSpace* parent, string* value){
    auto pos = new Position1D(name, parent, value);
    return pos;
}

Geom1DTransform* Domain::mkGeom1DTransform(std::string name, Geom1DCoordinateSpace* domain_, Geom1DCoordinateSpace* codomain_){
    auto g1transform = new Geom1DTransform(name, domain_, codomain_);
    return g1transform;
}

Displacement3D* Domain::mkDisplacement3D(std::string name, Geom3DCoordinateSpace* parent, string* value){
    auto disp = new Displacement3D(name, parent, value);
    return disp;
}

Position3D* Domain::mkPosition3D(std::string name, Geom3DCoordinateSpace* parent, string* value){
    auto pos = new Position3D(name, parent, value);
    return pos;
}

Orientation3D* Domain::mkOrientation3D(std::string name, Geom3DCoordinateSpace* parent, string* value){
    auto ort = new Orientation3D(name, parent, value);
    return ort;
}
Rotation3D* Domain::mkRotation3D(std::string name, Geom3DCoordinateSpace* parent, string* value){
    auto rot = new Rotation3D(name, parent, value);
    return rot;
}
Pose3D* Domain::mkPose3D(std::string name, Geom3DCoordinateSpace* parent, Orientation3D* ort, Position3D* rot){
    auto pose = new Pose3D(name, parent, ort, rot);
    return pose;
}


/*
Orientation3D* mkOrientation3D(string name, Geom3DCoordinateSpace* parent, string* value);
Rotation3D* mkRotation3D(string name, Geom3DCoordinateSpace* parent, string* value);
Pose3D* mkPose3D(string name, Geom3DCoordinateSpace* parent, Orientation3D* orientation_, Position3D position_);
*/

TimeStampedPose3D* Domain::mkTimeStampedPose3D(std::string name, Time* time, Pose3D* pose){
    return new TimeStampedPose3D(name, time, pose);
}

TimeStampedGeom3DTransform* Domain::mkTimeStampedGeom3DTransform(std::string name, Time* time, Geom3DTransform* transform){
    return new TimeStampedGeom3DTransform(name, time ,transform);
};


Geom3DTransform* Domain::mkGeom3DTransform(std::string name, Geom3DCoordinateSpace* domain_, Geom3DCoordinateSpace* codomain_){
    auto g1transform = new Geom3DTransform(name, domain_, codomain_);
    return g1transform;
}

Pose3DSeries* Domain::mkPose3DSeries(std::string name, TimeCoordinateSpace* timespace, Geom3DCoordinateSpace* space){
    auto pose = new Pose3DSeries(name, timespace, space);
    this->timeSeries.push_back(pose);
    return pose;
}

Geom3DTransformSeries* Domain::mkGeom3DTransformSeries(std::string name, 
    TimeCoordinateSpace* timespace, Geom3DCoordinateSpace* domain, Geom3DCoordinateSpace* codomain){
    auto gt = new Geom3DTransformSeries(name, timespace, domain, codomain);
    this->timeSeries.push_back(gt);
    return gt;
}

/*


    Pose3DSeries* mkPose3DSeries(string name, Geom3DCoordinateSpace* space);
    Geom3DTransformSeries* mkGeom3DTransformSeries(string name, Geom3DCoordinateSpace* domain_, Geom3DCoordinateSpace* codomain_);

*/