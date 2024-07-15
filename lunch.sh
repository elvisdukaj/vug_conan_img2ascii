adb=${ANDROID_SDK}/platform-tools/adb
device="emulator-5554"
deploy_folder="/data/local/tmp"
img2ascii="img2ascii"
$adb push $1 ${deploy_folder}
$adb shell chmod +x ${deploy_folder}/img2ascii
$adb shell ${deploy_folder}/img2ascii --help
$adb shell ${deploy_folder}/img2ascii
