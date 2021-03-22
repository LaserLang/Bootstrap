#include "codegen.hpp"

#include <cstdlib>
#include <iostream>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

namespace cannon {

llvm::Value* codegen_function_body(const std::vector<std::unique_ptr<statement>> &statements) {
    return nullptr;
}

void codegen(program p) {
    llvm::LLVMContext context;
    llvm::Module module("Cannon Bootstrap Compiler", context);

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

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
    module.setTargetTriple(targetTriple);

    std::error_code errorCode;
    llvm::raw_fd_ostream dest("a.o", errorCode);
    if(errorCode) {
        std::cerr << "Failed to open output file: " << errorCode.message() << std::endl;
        abort();
    }

    llvm::legacy::PassManager pass;
    if(targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile)) {
        std::cerr << "Can't emit object file!" << std::endl;
        abort();
    }

    llvm::IRBuilder<> builder(context);
    for(const std::unique_ptr<function> &f_p : p.functions()) {
        // Assume all functions return i32 and take no parameters
        llvm::Function *function = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false), llvm::Function::ExternalLinkage, std::string(f_p->name()), module); // LLVM naming conventions are garbage
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(block);
        builder.CreateRet(codegen_function_body(f_p->statements()));
        llvm::verifyFunction(*function);
    }

    pass.run(module);
    dest.flush();
}

}
