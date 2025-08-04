# SimpleCPP

SimpleCPP is a C++ library to simplify C++, it aims to ensure safety whilst keeping maximum performance and being intuitive to use. It is an alternative to the C++ standard library and will hopefully have better features in the future.

# Goals
1. Provide a simple and intuitive API with extensive testing
1. Maximize safety, discouraging unsafety such as pointers
1. Flexibility, safe but allows unsafe behavior for optimizations
1. High performance with safety
1. Minimal dependencies on external and standard libraries

# Features
1. `simplecpp::Pointer` - An alternative to `std::shared_ptr`. 
	1. Thread safe
	1. Supports a custom deleter and allocator as a template parameter
	1. Supports comparisons with manual pointers
	1. Copy constructor from existing pointer array of any length