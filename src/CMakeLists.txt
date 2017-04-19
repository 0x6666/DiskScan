cmake_minimum_required (VERSION 2.8)

include("x/check.cmake")

project(DiskScan)

include("x/x.cmake")

x_sub_dir(disktool)
x_sub_dir(strtool)
x_sub_dir(3rdparty)
x_sub_dir(shell)
x_sub_dir(support)

x_finish_project()
