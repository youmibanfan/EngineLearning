#pragma once


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

// ===========================================================
	template<typename DSTTYPE, typename SRCTYPE>
	DSTTYPE TypeTransform(SRCTYPE src_type);

}