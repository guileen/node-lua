srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'
 
def set_options(opt):
  opt.tool_options('compiler_cxx')
  opt.tool_options("compiler_cc")
 
def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("compiler_cc")
  conf.check_tool("node_addon")
  conf.check(header_name='lua.h')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-Wall"]
  obj.ldflags = ["-llua"]
  obj.target = 'node_lua'
  obj.source = 'src/node_lua.cc'
