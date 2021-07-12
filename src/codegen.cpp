#include "codegen.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

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

std::string mangle(const function &fn) {
    std::string result = "_C";
    result += std::to_string(fn.name().size());
    result += fn.name();
    result += "v"; // All functions have no parameters; don't ask too many questions.
    return result;
}

llvm::Value* codegen_expr(const expression &expr, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, std::map<std::string, std::pair<llvm::Function*, const std::unique_ptr<function>*>> functions) {
    if(const binary_expression *bin_expr = dynamic_cast<const binary_expression*>(&expr)) {
        llvm::Value *lhs = codegen_expr(bin_expr->lhs(), context, builder, functions);
        llvm::Value *rhs = codegen_expr(bin_expr->rhs(), context, builder, functions);
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
    } else if(const function_call_expression *fn_expr = dynamic_cast<const function_call_expression*>(&expr)) {
        return builder.CreateCall(llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false), codegen_expr(fn_expr->func(), context, builder, functions), std::vector<llvm::Value*>(), std::vector<llvm::OperandBundleDef>());
    } else if(const identifier_expression *id_expr = dynamic_cast<const identifier_expression*>(&expr)) {
        // Fun.
        // So, for now, we're assuming identifiers refer to functions. This'll be dealt with in semantic analysis eventually.
        // Also, semantic analysis will make it so we know which function is being referred to instead of having to assume signature as always
        return functions[std::string("_C") + std::to_string(id_expr->value().size()) + id_expr->value() + "v"].first;
    } else {
        std::cerr << "Heh. Heh." << std::endl;
        return nullptr;
    }
}

llvm::Value* codegen_function_body(const std::vector<std::unique_ptr<statement>> &statements, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, std::map<std::string, std::pair<llvm::Function*, const std::unique_ptr<function>*>> functions) {
    // FIXME: So, for now, I'm assuming there's only one expr. Because there is only one expr.
    return codegen_expr(*(dynamic_cast<expression*>(&(*statements[0]))), context, builder, functions); // Uh, this statement is garbage
}

void codegen(program p, std::string output_file) {
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
    llvm::raw_fd_ostream dest(output_file, errorCode);
    if(errorCode) {
        std::cerr << "Failed to open output file: " << errorCode.message() << std::endl;
        abort();
    }

    llvm::legacy::PassManager pass;
    if(targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile)) {
        std::cerr << "Can't emit object file!" << std::endl;
        abort();
    }

    std::map<std::string, std::pair<llvm::Function*, const std::unique_ptr<function>*>> functions;

    for(const std::unique_ptr<function> &f_p : p.functions()) {
        std::string name;
        llvm::FunctionType *type;
        if(f_p->name() == "main") {
            // All valid `main` functions have the same signature. All invalid `main` functions don't exist, NDR.
            type = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false);
            name = std::string("main");
        } else {
            // FIXME: Currently assuming all functions return i32 and take no parameters.
            type = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false);
            name = mangle(*f_p);
        }
        functions[name] = std::pair<llvm::Function*, const std::unique_ptr<function>*>(llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, module), &f_p);
    }

    llvm::IRBuilder<> builder(context);
    for(auto const &[name, pair] : functions) {
        auto function = pair.first;
        auto &f_p = *pair.second;
        llvm::BasicBlock *block = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(block);
        builder.CreateRet(codegen_function_body(f_p->statements(), context, builder, functions));
        llvm::verifyFunction(*function);
    }

    pass.run(module);
    dest.flush();
}

}
