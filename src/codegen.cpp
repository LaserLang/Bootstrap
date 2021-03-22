#include "codegen.hpp"

#include <cstdlib>
#include <iostream>

#include <llvm/IR/Constants.h>
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

llvm::Value* codegen_expr(const expression &expr, llvm::LLVMContext &context, llvm::IRBuilder<> &builder) {
    if(const binary_expression *bin_expr = dynamic_cast<const binary_expression*>(&expr)) {
        llvm::Value *lhs = codegen_expr(bin_expr->lhs(), context, builder);
        llvm::Value *rhs = codegen_expr(bin_expr->rhs(), context, builder);
        switch(bin_expr->op()) {
          case ADD:
            return builder.CreateAdd(lhs, rhs);
          case SUB:
            return builder.CreateSub(lhs, rhs);
          case MUL:
            return builder.CreateMul(lhs, rhs);
          case DIV:
            return builder.CreateUDiv(lhs, rhs);
          default:
            return nullptr;
        }
    } else if(const integer_expression *int_expr = dynamic_cast<const integer_expression*>(&expr)) {
        return llvm::ConstantInt::get(context, llvm::APInt(32, int_expr->value() & 0x00000000FFFFFFFFULL, true));
    } else {
        std::cerr << "Heh. Heh." << std::endl;
        return nullptr;
    }
}

llvm::Value* codegen_function_body(const std::vector<std::unique_ptr<statement>> &statements, llvm::LLVMContext &context, llvm::IRBuilder<> &builder) {
    // FIXME: So, for now, I'm assuming there's only one expr. Because there is only one expr.
    return codegen_expr(*(dynamic_cast<expression*>(&(*statements[0]))), context, builder); // Uh, this statement is garbage
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
    llvm::raw_fd_ostream dest("a.o", errorCode); // FIXME: Pass around output file name
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
        // FIXME: Currently assuming all functions return i32 and take no parameters
        llvm::Function *function = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false), llvm::Function::ExternalLinkage, std::string(f_p->name()), module); // LLVM naming conventions are garbage
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(block);
        builder.CreateRet(codegen_function_body(f_p->statements(), context, builder));
        llvm::verifyFunction(*function);
    }

    pass.run(module);
    dest.flush();
}

}
