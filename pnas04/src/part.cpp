//
//  part.cpp
//  PNAS04Team
//
//  Created by Jack Kang on 12-8-1.
//
//

#include "part.h"


namespace ustc {
Part::Part(const part_Type& _pType,const std::string& _id,const std::string& _name){
	pType = _pType;
	id = _id;
	name = _name;
}
    
part_Type Part::getPtype(){
	return pType;
}

std::string Part::getId(){
	return id;
}

std::string Part::getName(){
	return name;
}
    
    
}// namespace ustc