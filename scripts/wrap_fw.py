import os

Import("env", "projenv")

def after_build(source, target, env):

    #this will create a wrapped firmware
    if(os.name == 'nt'):
        env.Execute(".\\scripts\\radio_tool\\radio_tool.exe --wrap -o .\\.pio\\\\build\\RT3s\\firmware_wrapped.bin -r UV3X0 -s 0x0800C000:.\\.pio\\build\\RT3s\\firmware.bin")
    else:
        env.Execute("radio_tool --wrap -o ./.pio/build/RT3s/firmware_wrapped.bin -r UV3X0 -s 0x0800C000:./.pio/build/RT3s/firmware.bin")

env.AddPostAction("buildprog", after_build)