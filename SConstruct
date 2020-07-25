#!python
import os, subprocess

opts = Variables([], ARGUMENTS)
env = DefaultEnvironment()

opts.Add(EnumVariable('target', "Compilation target", 'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'", '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))
opts.Update(env)

# Gets the standard flags CC, CCX, etc.

# platform= makes it in line with Godots scons file, keeping p for backwards compatibility
platform = ARGUMENTS.get("p", "linux")
platform = ARGUMENTS.get("platform", platform)

# This makes sure to keep the session environment variables on windows, 
# that way you can run scons in a vs 2017 prompt and it will find all the required tools
env = Environment()
if platform == "windows":
    env = Environment(ENV = os.environ)

godot_headers_path = "godot-cpp/godot_headers/"
godot_bindings_path = "godot-cpp/"
godot_library = "libgodot-cpp"

# default to debug build, must be same setting as used for cpp_bindings
target = ARGUMENTS.get("target", "debug")


if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

# put stuff that is the same for all first, saves duplication 
if platform == "osx":
    env.Append(CCFLAGS = ['-g','-O3', '-std=c++14', '-arch', 'x86_64'])
    env.Append(LINKFLAGS = ['-arch', 'x86_64', '-framework', 'Cocoa', '-Wl,-undefined,dynamic_lookup'])
elif platform == "linux":
    env.Append(CCFLAGS = ['-g','-O3', '-std=c++14', '-Wno-writable-strings'])
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])
    godot_library += '.linux'
    if target == "debug":
        env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
    else:
        env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])

elif platform == "windows":
    # need to add detection of msvc vs mingw, this is for msvc...
    env.Append(LINKFLAGS = ['/WX'])
    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD'])

def add_sources(sources, dir):
    for f in os.listdir(dir):
        if f.endswith(".cpp"):
            sources.append(dir + "/" + f)


if target == "debug":
    godot_library += '.debug'
else:
    godot_library += '.release'

bits = 64
godot_library += '.' + str(bits)

env.Append(CPPPATH=[godot_headers_path, godot_bindings_path + '/include/', godot_bindings_path + '/include/core/', godot_bindings_path + 'include/gen/'])
env.Append(LIBPATH=[ godot_bindings_path + '/bin/' ])
env.Append(LIBS=[godot_library, 'util'])


sources = []
add_sources(sources, "src")

library = env.SharedLibrary(target='demo/addons/terminal/bin/libterminal', source=sources)
Default(library)

