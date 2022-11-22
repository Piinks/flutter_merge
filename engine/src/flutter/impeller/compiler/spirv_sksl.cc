// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/compiler/spirv_sksl.h"

using namespace spv;
using namespace SPIRV_CROSS_NAMESPACE;

namespace impeller {
namespace compiler {

// This replaces the SPIRV_CROSS_THROW which aborts and drops the
// error message in non-debug modes.
void report_and_exit(const std::string& msg) {
  fprintf(stderr, "There was a compiler error: %s\n", msg.c_str());
  fflush(stderr);
  exit(1);
}

#define FLUTTER_CROSS_THROW(x) report_and_exit(x)

std::string CompilerSkSL::compile() {
  ir.fixup_reserved_names();

  if (get_execution_model() != ExecutionModelFragment) {
    FLUTTER_CROSS_THROW("Only fragment shaders are supported.'");
    return "";
  }

  options.es = false;
  options.version = 100;
  options.vulkan_semantics = false;
  options.enable_420pack_extension = false;
  options.flatten_multidimensional_arrays = true;

  backend.allow_precision_qualifiers = false;
  backend.basic_int16_type = "short";
  backend.basic_int_type = "int";
  backend.basic_uint16_type = "ushort";
  backend.basic_uint_type = "uint";
  backend.double_literal_suffix = false;
  backend.float_literal_suffix = false;
  backend.long_long_literal_suffix = false;
  backend.needs_row_major_load_workaround = true;
  backend.nonuniform_qualifier = "";
  backend.support_precise_qualifier = false;
  backend.uint32_t_literal_suffix = false;
  backend.use_array_constructor = true;
  backend.workgroup_size_is_hidden = true;

  fixup_user_functions();

  fixup_anonymous_struct_names();
  fixup_type_alias();
  reorder_type_alias();
  build_function_control_flow_graphs_and_analyze();
  fixup_image_load_store_access();
  update_active_builtins();
  analyze_image_and_sampler_usage();
  analyze_interlocked_resource_usage();

  uint32_t pass_count = 0;
  do {
    reset(pass_count);

    // Move constructor for this type is broken on GCC 4.9 ...
    buffer.reset();

    emit_header();
    emit_resources();

    emit_function(get<SPIRFunction>(ir.default_entry_point), Bitset());

    pass_count++;
  } while (is_forcing_recompilation());

  statement("half4 main(float2 iFragCoord)");
  begin_scope();
  statement("  flutter_FragCoord = float4(iFragCoord, 0, 0);");
  statement("  FLT_main();");
  statement("  return " + output_name_ + ";");
  end_scope();

  return buffer.str();
}

void CompilerSkSL::fixup_user_functions() {
  const std::string prefix = "FLT_flutter_local_";
  ir.for_each_typed_id<SPIRFunction>([&](uint32_t, const SPIRFunction& func) {
    const auto& original_name = get_name(func.self);
    // Just in case. Don't add the prefix a second time.
    if (original_name.rfind(prefix, 0) == 0) {
      return;
    }
    std::string new_name = prefix + original_name;
    set_name(func.self, new_name);
  });

  ir.for_each_typed_id<SPIRFunctionPrototype>(
      [&](uint32_t, const SPIRFunctionPrototype& func) {
        const auto& original_name = get_name(func.self);
        // Just in case. Don't add the prefix a second time.
        if (original_name.rfind(prefix, 0) == 0) {
          return;
        }
        std::string new_name = prefix + original_name;
        set_name(func.self, new_name);
      });
}

void CompilerSkSL::emit_header() {
  statement("// This SkSL shader is autogenerated by spirv-cross.");
  statement("");
  statement("float4 flutter_FragCoord;");
  statement("");
}

void CompilerSkSL::emit_uniform(const SPIRVariable& var) {
  auto& type = get<SPIRType>(var.basetype);
  if (type.basetype == SPIRType::UInt && is_legacy()) {
    FLUTTER_CROSS_THROW("SkSL does not support unsigned integers: '" +
                        get_name(var.self) + "'");
  }

  add_resource_name(var.self);
  statement(variable_decl(var), ";");

  // The Flutter FragmentProgram implementation passes additional uniforms along
  // with shader uniforms that encode the shader width and height.
  if (type.basetype == SPIRType::SampledImage) {
    std::string name = to_name(var.self);
    statement("uniform half2 " + name + "_size;");
  }
}

bool CompilerSkSL::emit_constant_resources() {
  bool emitted = false;

  for (auto& id : ir.ids) {
    if (id.get_type() == TypeConstant) {
      auto& c = id.get<SPIRConstant>();
      bool needs_declaration = c.specialization || c.is_used_as_lut;
      if (needs_declaration) {
        if (!options.vulkan_semantics && c.specialization) {
          c.specialization_constant_macro_name = constant_value_macro_name(
              get_decoration(c.self, DecorationSpecId));
        }
        emit_constant(c);
        emitted = true;
      }
    } else if (id.get_type() == TypeConstantOp) {
      emit_specialization_constant_op(id.get<SPIRConstantOp>());
      emitted = true;
    }
  }

  return emitted;
}

bool CompilerSkSL::emit_struct_resources() {
  bool emitted = false;

  // Output all basic struct types which are not Block or BufferBlock as these
  // are declared inplace when such variables are instantiated.
  for (auto& id : ir.ids) {
    if (id.get_type() == TypeType) {
      auto& type = id.get<SPIRType>();
      if (type.basetype == SPIRType::Struct && type.array.empty() &&
          !type.pointer &&
          (!ir.meta[type.self].decoration.decoration_flags.get(
               DecorationBlock) &&
           !ir.meta[type.self].decoration.decoration_flags.get(
               DecorationBufferBlock))) {
        emit_struct(type);
        emitted = true;
      }
    }
  }

  return emitted;
}

void CompilerSkSL::detect_unsupported_resources() {
  for (auto& id : ir.ids) {
    if (id.get_type() == TypeVariable) {
      auto& var = id.get<SPIRVariable>();
      auto& type = get<SPIRType>(var.basetype);

      // UBOs and SSBOs are not supported.
      if (var.storage != StorageClassFunction && type.pointer &&
          type.storage == StorageClassUniform && !is_hidden_variable(var) &&
          (ir.meta[type.self].decoration.decoration_flags.get(
               DecorationBlock) ||
           ir.meta[type.self].decoration.decoration_flags.get(
               DecorationBufferBlock))) {
        FLUTTER_CROSS_THROW("SkSL does not support UBOs or SSBOs: '" +
                            get_name(var.self) + "'");
      }

      // Push constant blocks are not supported.
      if (!is_hidden_variable(var) && var.storage != StorageClassFunction &&
          type.pointer && type.storage == StorageClassPushConstant) {
        FLUTTER_CROSS_THROW("SkSL does not support push constant blocks: '" +
                            get_name(var.self) + "'");
      }

      // User specified inputs are not supported.
      if (!is_hidden_variable(var) && var.storage != StorageClassFunction &&
          type.pointer && type.storage == StorageClassInput) {
        FLUTTER_CROSS_THROW("SkSL does not support inputs: '" +
                            get_name(var.self) + "'");
      }
    }
  }
}

bool CompilerSkSL::emit_uniform_resources() {
  bool emitted = false;

  // Output Uniform Constants (values, samplers, images, etc).
  std::vector<ID> regular_uniforms;
  std::vector<ID> shader_uniforms;
  for (auto& id : ir.ids) {
    if (id.get_type() == TypeVariable) {
      auto& var = id.get<SPIRVariable>();
      auto& type = get<SPIRType>(var.basetype);
      if (var.storage != StorageClassFunction && !is_hidden_variable(var) &&
          type.pointer &&
          (type.storage == StorageClassUniformConstant ||
           type.storage == StorageClassAtomicCounter)) {
        // Separate out the uniforms that will be of SkSL 'shader' type since
        // we need to make sure they are emitted only after the other uniforms.
        if (type.basetype == SPIRType::SampledImage) {
          shader_uniforms.push_back(var.self);
        } else {
          regular_uniforms.push_back(var.self);
        }
        emitted = true;
      }
    }
  }

  // Sort uniforms by location.
  auto compare_locations = [this](ID id1, ID id2) {
    auto& flags1 = get_decoration_bitset(id1);
    auto& flags2 = get_decoration_bitset(id2);
    // Put the uniforms with no location after the ones that have a location.
    if (!flags1.get(DecorationLocation)) {
      return false;
    }
    if (!flags2.get(DecorationLocation)) {
      return true;
    }
    // Sort in increasing order of location.
    return get_decoration(id1, DecorationLocation) <
           get_decoration(id2, DecorationLocation);
  };
  std::sort(regular_uniforms.begin(), regular_uniforms.end(),
            compare_locations);
  std::sort(shader_uniforms.begin(), shader_uniforms.end(), compare_locations);

  for (const auto& id : regular_uniforms) {
    auto& var = get<SPIRVariable>(id);
    emit_uniform(var);
  }

  for (const auto& id : shader_uniforms) {
    auto& var = get<SPIRVariable>(id);
    emit_uniform(var);
  }

  return emitted;
}

bool CompilerSkSL::emit_output_resources() {
  bool emitted = false;

  // Output 'out' variables. These are restricted to the cases handled by
  // SkSL in 'emit_interface_block'.
  for (auto& id : ir.ids) {
    if (id.get_type() == TypeVariable) {
      auto& var = id.get<SPIRVariable>();
      auto& type = get<SPIRType>(var.basetype);
      if (var.storage != StorageClassFunction && !is_hidden_variable(var) &&
          type.pointer &&
          (var.storage == StorageClassInput ||
           var.storage == StorageClassOutput) &&
          interface_variable_exists_in_entry_point(var.self)) {
        emit_interface_block(var);
        emitted = true;
      }
    }
  }

  return emitted;
}

bool CompilerSkSL::emit_global_variable_resources() {
  bool emitted = false;

  for (auto global : global_variables) {
    auto& var = get<SPIRVariable>(global);
    if (is_hidden_variable(var, true)) {
      continue;
    }
    if (var.storage != StorageClassOutput) {
      if (!variable_is_lut(var)) {
        add_resource_name(var.self);
        std::string initializer;
        if (options.force_zero_initialized_variables &&
            var.storage == StorageClassPrivate && !var.initializer &&
            !var.static_expression &&
            type_can_zero_initialize(get_variable_data_type(var))) {
          initializer = join(" = ", to_zero_initialized_expression(
                                        get_variable_data_type_id(var)));
        }
        statement(variable_decl(var), initializer, ";");
        emitted = true;
      }
    } else if (var.initializer &&
               maybe_get<SPIRConstant>(var.initializer) != nullptr) {
      emit_output_variable_initializer(var);
    }
  }

  return emitted;
}

void CompilerSkSL::emit_resources() {
  detect_unsupported_resources();

  if (emit_constant_resources()) {
    statement("");
  }

  if (emit_struct_resources()) {
    statement("");
  }

  if (emit_uniform_resources()) {
    statement("");
  }

  if (emit_output_resources()) {
    statement("");
  }

  if (emit_global_variable_resources()) {
    statement("");
  }
}

void CompilerSkSL::emit_interface_block(const SPIRVariable& var) {
  auto& type = get<SPIRType>(var.basetype);
  bool block =
      ir.meta[type.self].decoration.decoration_flags.get(DecorationBlock);
  if (block) {
    FLUTTER_CROSS_THROW("Interface blocks are not supported: '" +
                        to_name(var.self) + "'");
  }

  // The output is emitted as a global variable, which is returned from the
  // wrapper around the 'main' function. Only one output variable is allowed.
  add_resource_name(var.self);
  statement(variable_decl(type, to_name(var.self), var.self), ";");
  if (output_name_.empty()) {
    output_name_ = to_name(var.self);
  } else if (to_name(var.self) != output_name_) {
    FLUTTER_CROSS_THROW("Only one output variable is supported: '" +
                        to_name(var.self) + "'");
  }
}

void CompilerSkSL::emit_function_prototype(SPIRFunction& func,
                                           const Bitset& return_flags) {
  // If this is not the entrypoint, then no special processsing for SkSL is
  // required.
  if (func.self != ir.default_entry_point) {
    CompilerGLSL::emit_function_prototype(func, return_flags);
    return;
  }

  auto& type = get<SPIRType>(func.return_type);
  if (type.basetype != SPIRType::Void) {
    FLUTTER_CROSS_THROW(
        "Return type of the entrypoint function must be 'void'");
  }

  if (func.arguments.size() != 0) {
    FLUTTER_CROSS_THROW(
        "The entry point function should not acept any parameters.");
  }

  processing_entry_point = true;

  // If this is the entrypoint of a fragment shader, then GLSL requires the
  // prototype to be "void main()", and so it is safe to rewrite as
  // "void FLT_main()".
  statement("void FLT_main()");
}

std::string CompilerSkSL::image_type_glsl(const SPIRType& type, uint32_t id) {
  if (type.basetype != SPIRType::SampledImage || type.image.dim != Dim2D) {
    FLUTTER_CROSS_THROW("Only sampler2D uniform image types are supported.");
    return "???";
  }
  return "shader";
}

std::string CompilerSkSL::builtin_to_glsl(BuiltIn builtin,
                                          StorageClass storage) {
  std::string gl_builtin = CompilerGLSL::builtin_to_glsl(builtin, storage);
  switch (builtin) {
    case BuiltInFragCoord:
      return "flutter_FragCoord";
    default:
      FLUTTER_CROSS_THROW("Builtin '" + gl_builtin + "' is not supported.");
      break;
  }

  return "???";
}

std::string CompilerSkSL::to_texture_op(
    const Instruction& i,
    bool sparse,
    bool* forward,
    SmallVector<uint32_t>& inherited_expressions) {
  auto op = static_cast<Op>(i.op);
  if (op != OpImageSampleImplicitLod) {
    FLUTTER_CROSS_THROW("Only simple shader sampling is supported.");
    return "???";
  }
  return CompilerGLSL::to_texture_op(i, sparse, forward, inherited_expressions);
}

std::string CompilerSkSL::to_function_name(
    const CompilerGLSL::TextureFunctionNameArguments& args) {
  std::string name = to_expression(args.base.img);
  return name + ".eval";
}

std::string CompilerSkSL::to_function_args(const TextureFunctionArguments& args,
                                           bool* p_forward) {
  std::string name = to_expression(args.base.img);

  std::string glsl_args = CompilerGLSL::to_function_args(args, p_forward);
  // SkSL only supports coordinates. All other arguments to texture are
  // unsupported and will generate invalid SkSL.
  if (args.grad_x || args.grad_y || args.lod || args.coffset || args.offset ||
      args.sample || args.min_lod || args.sparse_texel || args.bias ||
      args.component) {
    FLUTTER_CROSS_THROW(
        "Only sampler and position arguments are supported in texture() "
        "calls.");
  }

  // GLSL puts the shader as the first argument, but in SkSL the shader is
  // implicitly passed as the reciever of the 'eval' method. Therefore, the
  // shader is removed from the GLSL argument list.
  std::string no_shader;
  auto npos = glsl_args.find(", ");  // The first ','.
  if (npos != std::string::npos) {
    no_shader = glsl_args.substr(npos + 1);  // The string after the first ','.
  }

  if (no_shader.empty()) {
    FLUTTER_CROSS_THROW("Unexpected shader sampling arguments: '(" + glsl_args +
                        ")'");
    return "()";
  }

  return name + "_size * " + no_shader;
}

}  // namespace compiler
}  // namespace impeller
