//
// Created by chorm on 2020-08-05.
//

#ifndef CANNON_MODE_HPP
#define CANNON_MODE_HPP

namespace cannon {

enum class compiler_mode {
    CompileAndLink,  // produce final binary, according to the link type
    GenerateObjects, // produce object files for linking, may include joined-module form
    CompileOnly,     // compile and typecheck to IR format
    TypeCheck,       // check syntax and type validity, but does not generate any artifacts
    Dependencies,    // produce a dependency graph for the compilation, but does
                     // not ...
    ModuleServer,    // act as a module server, and marshall compilation of cannon
                     // dependency graphs, by a combination of cannon runs using
                     // -c/--compile-only, and --check
};

enum class link_type {
    Exec,
    JoinedModule, // .tbd
    SharedModule, // .so/.dll, with a tbd section
    StaticLib,    // .a/.lib, usable with C/not-cannon
    SharedLib,    // .so/.dll, usable with C/not-cannon, no cannon module information
    Partial,      // .tbd or .tbd2, but incomplete
};

enum class optimization_level {
    FirstReserved = 240,

    // -Og
    Debug = 250,
    // -Ofast (not necessarily compliant)
    Fast = 252,
    // -Os
    Size = 253,
    // -Oz
    Zize = 254,
    // -Oextra (not necessarily compliant)
    Extra = 255
};

} // namespace cannon

#endif // CANNON_MODE_HPP
