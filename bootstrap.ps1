if (!(Test-Path -Path vcpkg)) {
    git clone https://github.com/Microsoft/vcpkg
    ./vcpkg/bootstrap-vcpkg.bat
}

# Install dependencies.
./vcpkg/vcpkg.exe install --triplet=x86-windows-static-md

# Install nwn-erf
Invoke-WebRequest -Uri 'https://github.com/CromFr/nwn-lib-d/releases/latest/download/nwn-lib-d-tools-windows-x86_64.zip' -OutFile 'vcpkg_installed\nwn-lib-d-tools-windows-x86_64.zip'
Expand-Archive -LiteralPath 'vcpkg_installed\nwn-lib-d-tools-windows-x86_64.zip' -DestinationPath 'vcpkg_installed\'

echo "All NWNx4 dependencies are installed!";
