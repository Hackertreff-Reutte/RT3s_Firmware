import os

Import("env", "projenv")


def set_uploader(source, target, env):

    if(os.name == 'nt'):
        #windows
        env.Replace(
        UPLOADER=".\\scripts\\radio_tool\\radio_tool.exe",
        UPLOADCMD="$UPLOADER -d 0 $UPLOADERFLAGS .\\.pio\\build\\RT3s\\firmware_wrapped.bin"
        )
    else:
        #unix
        env.Replace(
        UPLOADER="./scripts/radio_tool/radio_tool",
        UPLOADCMD="$UPLOADER -d 0 $UPLOADERFLAGS ./.pio/build/RT3s/firmware_wrapped.bin"
        )

env.AddPreAction("upload", set_uploader)