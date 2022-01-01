# With the current project setup, all .dll dynamic libraries need to be placed into the same
# folder as the project's .exe file. This script copies those files from libs/dynamic_libs
# Run this script from OpenGL/Projects/SpiderGame

import os
import shutil

dynamic_lib_src = "../../lib/dynamic_libs/"
# Location VSCode will build the final project .exe files
exe_dir = "x64/"
build_types = ["Debug", "Release"]

print("Copying dynamic libs...")
# Make the directories if they don't exist
for type in build_types:
    src_dir = dynamic_lib_src + type
    dest_dir = exe_dir + type
    os.makedirs(dest_dir, exist_ok=True)
    for dll_name in os.listdir(src_dir):
        print(f"- {dll_name}")
        shutil.copy(src_dir + "/" + dll_name, dest_dir)
print("Done!")
