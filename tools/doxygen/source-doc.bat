@echo off

pushd "%~dp0"

call "../windows/get-source-version.bat" "../../source/base/version.h"
call doxygen.bat "source-doc" "source-doc\pdf\povray-v%POV_RAY_FULL_VERSION%-sourcedoc.pdf"

popd
