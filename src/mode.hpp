//
// Created by chorm on 2020-08-05.
//

#ifndef LASERC_MODE_HPP
#define LASERC_MODE_HPP

namespace laserc
{

enum class compiler_mode
{
    CompileAndLink,  // Produce final binary, according to LinkType
    GenerateObjects, // Produce object files for linking, may include joined-module form.
    CompileOnly,     // Compile and typecheck to .lsm (IR format)
    TypeCheck,       // Check syntax and type validity, but does not generate any artifacts
    Dependencies,    // Produces a dependency graph for the compilation, but does not
    ModuleServer,    // Act as a module server, and marshall compilation of laserc dependency graphs,
                     // by a combination of laserc runs using -c/--compile-only, and --check.
};

enum class link_type
{
    Exec,
    JoinedModule, // .lslib
    SharedModule, // .so/.dll, with a lslib section
    StaticLib, // .a/.lib, usable with C/not-laser
    SharedLib, // .so/.dll, usable with C/not-laser, no laser module information
    Partial, // .lslib or .lsm, but incomplete
};

}

#endif // LASERC_MODE_HPP
