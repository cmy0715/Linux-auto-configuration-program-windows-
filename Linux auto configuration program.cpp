#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

using namespace std;

// Function to run a system command
void runCommand(const string& command) {
    cout << "Executing command: " << command << endl;
    int result = system(command.c_str());
    if (result != 0) {
        cerr << "Command failed: " << command << endl;
        exit(EXIT_FAILURE);
    }
}

// Function to download a file
void downloadFile(const string& url, const string& outputPath) {
    cout << "Downloading file: " << url << " to " << outputPath << endl;
    HRESULT result = URLDownloadToFile(NULL, url.c_str(), outputPath.c_str(), 0, NULL);
    if (result != S_OK) {
        cerr << "File download failed: " << url << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Download completed: " << outputPath << endl;
}

// Function to check if VirtualBox is installed
bool isVirtualBoxInstalled() {
    string command = "where VBoxManage";
    return system(command.c_str()) == 0;
}

// Function to install VirtualBox
void installVirtualBox() {
    string virtualBoxUrl = "https://download.virtualbox.org/virtualbox/7.0.4/VirtualBox-7.0.4-154605-Win.exe"; // Update to the correct version link
    string virtualBoxInstaller = "C:\\temp\\VirtualBoxInstaller.exe";

    // Download VirtualBox installer
    downloadFile(virtualBoxUrl, virtualBoxInstaller);

    // Execute the installer
    cout << "Installing VirtualBox..." << endl;
    string installCommand = virtualBoxInstaller + " --silent";
    runCommand(installCommand);

    cout << "VirtualBox installation completed." << endl;
}

// Function to automatically download Linux ISO
void downloadLinuxISO(const string& isoUrl, const string& isoPath) {
    cout << "Downloading Linux distribution ISO file..." << endl;
    downloadFile(isoUrl, isoPath);
}

// Function to create and start the VM
void createAndStartVM(const string& vmName, const string& isoPath, const string& vdiPath, int memorySize, int vramSize, int cpuCount) {
    // Create the VM
    runCommand("VBoxManage createvm --name " + vmName + " --register");

    // Configure VM memory and CPU
    runCommand("VBoxManage modifyvm " + vmName + " --memory " + to_string(memorySize) + " --vram " + to_string(vramSize));
    runCommand("VBoxManage modifyvm " + vmName + " --cpus " + to_string(cpuCount));

    // Create virtual hard disk
    runCommand("VBoxManage createmedium --filename " + vdiPath + " --size 20000"); // Create a 20GB virtual hard disk

    // Attach the virtual hard disk to the VM
    runCommand("VBoxManage storagectl " + vmName + " --name \"SATA Controller\" --add sata --controller IntelAhci");
    runCommand("VBoxManage storageattach " + vmName + " --storagectl \"SATA Controller\" --port 0 --device 0 --type hdd --medium " + vdiPath);

    // Attach the ISO file as a virtual optical drive
    runCommand("VBoxManage storagectl " + vmName + " --name \"IDE Controller\" --add ide");
    runCommand("VBoxManage storageattach " + vmName + " --storagectl \"IDE Controller\" --port 1 --device 0 --type dvddrive --medium " + isoPath);

    // Set VM boot order to boot from DVD first
    runCommand("VBoxManage modifyvm " + vmName + " --boot1 dvd --boot2 disk --boot3 none --boot4 none");

    // Start the VM
    cout << "Starting the VM to install Linux..." << endl;
    runCommand("VBoxManage startvm " + vmName);

    cout << "VM successfully created and started. Please complete the Linux installation manually." << endl;
}

int main() {
    string vmName = "Linux_VM";
    string isoUrl = "https://releases.ubuntu.com/20.04/ubuntu-20.04.4-desktop-amd64.iso"; // Ubuntu 20.04 ISO download link
    string isoPath = "C:\\temp\\ubuntu.iso";
    string vdiPath = "C:\\temp\\linux.vdi";
    int memorySize = 2048; // VM memory size in MB
    int vramSize = 128; // VM VRAM size in MB
    int cpuCount = 2; // VM CPU count

    // Create a temp directory
    runCommand("mkdir C:\\temp");

    // Check if VirtualBox is installed, if not, install it
    if (!isVirtualBoxInstalled()) {
        cout << "VirtualBox is not installed, installing VirtualBox..." << endl;
        installVirtualBox();
    } else {
        cout << "VirtualBox is already installed." << endl;
    }

    // Download Linux distribution ISO
    downloadLinuxISO(isoUrl, isoPath);

    // Create and start the VM
    createAndStartVM(vmName, isoPath, vdiPath, memorySize, vramSize, cpuCount);

    cout << "All operations completed successfully." << endl;
    return 0;
}
