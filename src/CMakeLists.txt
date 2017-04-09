cmake_minimum_required (VERSION 3.8)

include("x/check.cmake")

project(DiskScan)

include("x/x.cmake")

x_sub_dir(disktool)
x_sub_dir(strtool)
x_sub_dir(diskscan)

x_finish_project()
