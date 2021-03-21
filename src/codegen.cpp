#include "codegen.hpp"

#include <cstdlib>
#include <iostream>

#include <llvm/IR/Module.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

namespace cannon {

void codegen(program p) {
    llvm::LLVMContext context;
    llvm::Module module("Cannon Bootstrap Compiler", context);

    llvm::InitializeNativeTarget();

    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    module.setTargetTriple(targetTriple);

    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if(!target) {
        std::cerr << "Failed to start codegen: " << error << std::endl;
        abort();
    }
    llvm::TargetOptions options;
    llvm::Optional<llvm::Reloc::Model> rm = llvm::Optional<llvm::Reloc::Model>();
    llvm::TargetMachine *targetMachine = target->createTargetMachine(targetTriple, "generic", "", options, rm);

    module.setDataLayout(targetMachine->createDataLayout());
}

}
