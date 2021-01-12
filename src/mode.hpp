//
// Created by chorm on 2020-08-05.
//

#ifndef CANNON_MODE_HPP
#define CANNON_MODE_HPP

namespace cannon {

enum class compiler_mode {
    CompileAndLink,  // Produce final binary, according to LinkType
    GenerateObjects, // Produce object files for linking, may include
                     // joined-module form.
    CompileOnly,     // Compile and typecheck to .lsm (IR format)
    TypeCheck,    // Check syntax and type validity, but does not generate any
                  // artifacts
    Dependencies, // Produces a dependency graph for the compilation, but does
                  // not
    ModuleServer, // Act as a module server, and marshall compilation of cannon
                  // dependency graphs, by a combination of cannon runs using
                  // -c/--compile-only, and --check.
};

enum class link_type {
    Exec,
    JoinedModule, // .tbd
    SharedModule, // .so/.dll, with a tbd section
    StaticLib,    // .a/.lib, usable with C/not-cannon
    SharedLib, // .so/.dll, usable with C/not-cannon, no cannon module information
    Partial,   // .tbd or .tbd2, but incomplete
};

} // namespace cannon

#endif // CANNON_MODE_HPP
