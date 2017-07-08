#include <stdio.h>
#include <stdlib.h>

// gcc
#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <gimple.h>
#include <gimple-iterator.h>
#include <basic-block.h>
#include <tree-pass.h>
#include <version.h>
#include <intl.h>
#include <context.h>

static const pass_data pass_info =
{
  /* type */
  GIMPLE_PASS,
  /* name */ "afl-inst",
  /* optinfo_flags */ 0,
  /* tv_id */ TV_NONE,
  /* pops required */ 0,
  /* pops provided */ 0,
  /* pops destroyed */ 0,

  /* todo_flags_start */ 0,
  TODO_update_ssa | TODO_verify_all | TODO_cleanup_cfg,
};

class instrumentation_pass : public gimple_opt_pass
{
public:
  instrumentation_pass(gcc::context* ctx) : gimple_opt_pass(pass_info, ctx)
  {
  }

  int counter = 0;

  virtual unsigned int execute(function* fun)
  {
    basic_block bb;

    FOR_EACH_BB_FN(bb, fun)
    {
      tree cur_loc = build_int_cst(uint32_type_node, counter++);

      tree fntype = build_function_type_list(void_type_node, uint32_type_node, NULL_TREE);

      tree fndecl = build_fn_decl("__visit_item", fntype);
      TREE_STATIC(fndecl) = 1; // no function body
      TREE_PUBLIC(fndecl) = 1;
      DECL_EXTERNAL(fndecl) = 1;
      DECL_ARTIFICIAL(fndecl) = 1;

      auto g = gimple_build_call(fndecl, 1, cur_loc);
      gimple_seq seq = NULL;
      gimple_seq_add_stmt(&seq, g);

      auto bentry = gsi_start_bb(bb);
      gsi_insert_seq_before(&bentry, seq, GSI_SAME_STMT);
    }

    return 0;
  }
};

int plugin_init(plugin_name_args* plugin_info, plugin_gcc_version* version)
{
  register_pass_info pass_info;

  pass_info.pass = new instrumentation_pass(g);
  pass_info.reference_pass_name = "cfg";
  pass_info.ref_pass_instance_number = 1;
  pass_info.pos_op = PASS_POS_INSERT_AFTER;

  if(!plugin_default_version_check(version, &gcc_version))
  {
    printf("Incompatible version\n");
    exit(123);
  }

  static struct plugin_info g_info =
  {
    .version = "20170708",
    .help = "Agrafuzz gcc plugin\n",
  };

  register_callback(plugin_info->base_name, PLUGIN_INFO, NULL, &g_info);
  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

  return 0;
}

int plugin_is_GPL_compatible = 1;

