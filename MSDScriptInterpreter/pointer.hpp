//
//  macros.hpp
//  MSDScriptInterpreter
//
//  Created by Warner Nielsen on 2/28/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#ifndef pointer_hpp
#define pointer_hpp

#if 0

#define NEW(T) new T
#define PTR(T) T*
#define CAST(T) dynamic_cast<T*>

#else

#define NEW(T) std::make_shared<T>
#define PTR(T) std::shared_ptr<T>
#define CAST(T) std::dynamic_pointer_cast<T>

#endif

#endif /* pointer_hpp */
