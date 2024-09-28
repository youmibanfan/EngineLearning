#pragma once

#if defined (MATCH_INNER_VISIBLE)
    #define INNER_VISIBLE public
    #define INNER_PROTECT public
#else
    #define INNER_VISIBLE private
    #define INNER_PROTECT protected
#endif

namespace Core
{
#define no_copy_construction(Class) \
	Class(const Class &) = delete;


#define no_move_construction(Class) \
	Class(Class &&) = delete;

#define default_construction(Class) \
public: \
	Class() = default;

#define no_copy_move_construction(Class) \
	no_copy_construction(Class) \
	no_move_construction(Class)

#define default_no_copy_move_construction(Class) \
	no_copy_construction(Class) \
	no_move_construction(Class) \
	default_construction(Class)

}