cmake_minimum_required (VERSION 3.14)
project (KenKen-SAT VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        minisat
        GIT_REPOSITORY https://github.com/master-keying/minisat.git
)
FetchContent_MakeAvailable(minisat)

add_executable(kenken-cli
    main.cpp
    solver.cpp cage.cpp
    # Headers for IDEs
    solver.hpp
)
target_link_libraries(kenken-cli MiniSat::libminisat)
