@echo on
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d "%~dp0"
if exist build-test rd /s /q build-test
md build-test
cd build-test
md cmake
cd cmake
:: -DINPUT_mimetype_database_compression=zstd -DQT_DISABLE_DEPRECATED_UP_TO=0x070000 -DINPUT_intelcet=yes -DINPUT_spectre=yes -DINPUT_ehcont=yes
set __common_flags=-Wno-dev -DCMAKE_MESSAGE_LOG_LEVEL=STATUS -DQT_BUILD_TESTS=OFF -DQT_BUILD_EXAMPLES=OFF -DFEATURE_relocatable=ON -DFEATURE_system_zlib=OFF
cmake %__common_flags% -DCMAKE_INSTALL_PREFIX="%~dp0build-test\shared" -DCMAKE_CONFIGURATION_TYPES=Release;Debug -DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE=ON -DBUILD_SHARED_LIBS=ON -G"Ninja Multi-Config" "%~dp0"
cmake --build . --parallel
ninja install
cd ..
rd /s /q cmake
md cmake
cd cmake
cmake %__common_flags% -DCMAKE_INSTALL_PREFIX="%~dp0build-test\static" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE=OFF -DBUILD_SHARED_LIBS=OFF -GNinja "%~dp0"
cmake --build . --parallel
cmake --install .
cd ..
rd /s /q cmake
endlocal
cd /d "%~dp0"
pause
exit /b 0
