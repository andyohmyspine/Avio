pushd ..\..\
cmake . -Brelease_build -DAVIO_ENABLE_GPU_VALIDATION=OFF
cmake --build release_build --config MinSizeRel --target sandbox
popd