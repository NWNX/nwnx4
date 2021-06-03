if (!(Test-Path -Path vcpkg)) {
    git clone https://github.com/Microsoft/vcpkg
    ./vcpkg/bootstrap-vcpkg.bat
}

# Install dependencies.
./vcpkg/vcpkg.exe install --triplet=x86-windows-static-md

echo "All NWNx4 dependencies are installed!";
