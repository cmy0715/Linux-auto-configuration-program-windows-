#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
using namespace std;
// 执行系统命令的函数
void runCommand(const string& command) {
    cout << "执行命令: " << command << endl;
    int result = system(command.c_str());
    if (result != 0) {
        cerr << "命令执行失败: " << command << endl;
        exit(EXIT_FAILURE);
    }
}
// 下载文件
void downloadFile(const string& url, const string& outputPath) {
    cout << "下载文件: " << url << " 到 " << outputPath << endl;
    HRESULT result = URLDownloadToFile(NULL, url.c_str(), outputPath.c_str(), 0, NULL);
    if (result != S_OK) {
        cerr << "下载文件失败: " << url << endl;
        exit(EXIT_FAILURE);
    }
    cout << "下载完成: " << outputPath << endl;
}
// 检查 VirtualBox 是否已安装
bool isVirtualBoxInstalled() {
    string command = "where VBoxManage";
    return system(command.c_str()) == 0;
}
// 安装 VirtualBox
void installVirtualBox() {
    string virtualBoxUrl = "https://download.virtualbox.org/virtualbox/7.0.4/VirtualBox-7.0.4-154605-Win.exe"; // 更新版本链接
    string virtualBoxInstaller = "C:\\temp\\VirtualBoxInstaller.exe";
    // 下载 VirtualBox 安装程序
    downloadFile(virtualBoxUrl, virtualBoxInstaller);
    // 执行安装
    cout << "安装 VirtualBox..." << endl;
    string installCommand = virtualBoxInstaller + " --silent";
    runCommand(installCommand);
    cout << "VirtualBox 安装完成。" << endl;
}
// 自动下载 Linux 发行版 ISO
void downloadLinuxISO(const string& isoUrl, const string& isoPath) {
    cout << "正在下载 Linux 发行版 ISO 文件..." << endl;
    downloadFile(isoUrl, isoPath);
}
// 创建虚拟机并启动
void createAndStartVM(const string& vmName, const string& isoPath, const string& vdiPath, int memorySize, int vramSize, int cpuCount) {
    // 创建虚拟机
    runCommand("VBoxManage createvm --name " + vmName + " --register");
    // 设置虚拟机内存和 CPU
    runCommand("VBoxManage modifyvm " + vmName + " --memory " + to_string(memorySize) + " --vram " + to_string(vramSize));
    runCommand("VBoxManage modifyvm " + vmName + " --cpus " + to_string(cpuCount));
    // 创建虚拟硬盘
    runCommand("VBoxManage createmedium --filename " + vdiPath + " --size 20000"); // 创建 20GB 的虚拟硬盘
    // 添加虚拟硬盘到虚拟机
    runCommand("VBoxManage storagectl " + vmName + " --name \"SATA Controller\" --add sata --controller IntelAhci");
    runCommand("VBoxManage storageattach " + vmName + " --storagectl \"SATA Controller\" --port 0 --device 0 --type hdd --medium " + vdiPath);
    // 添加 ISO 文件作为虚拟光驱
    runCommand("VBoxManage storagectl " + vmName + " --name \"IDE Controller\" --add ide");
    runCommand("VBoxManage storageattach " + vmName + " --storagectl \"IDE Controller\" --port 1 --device 0 --type dvddrive --medium " + isoPath);
    // 配置虚拟机启动顺序为从光盘启动
    runCommand("VBoxManage modifyvm " + vmName + " --boot1 dvd --boot2 disk --boot3 none --boot4 none");
    // 启动虚拟机
    cout << "启动虚拟机以安装 Linux..." << endl;
    runCommand("VBoxManage startvm " + vmName);
    cout << "虚拟机创建成功并已启动，请手动完成 Linux 安装。" << endl;
}

int main() {
    string vmName = "Linux_VM";
    string isoUrl = "https://releases.ubuntu.com/20.04/ubuntu-20.04.4-desktop-amd64.iso"; // Ubuntu 20.04 ISO 下载链接
    string isoPath = "C:\\temp\\ubuntu.iso";
    string vdiPath = "C:\\temp\\linux.vdi";
    int memorySize = 2048; // 虚拟机内存大小，单位为 MB
    int vramSize = 128; // 虚拟显存大小，单位为 MB
    int cpuCount = 2; // 虚拟机 CPU 数量
    // 创建临时目录
    runCommand("mkdir C:\\temp");
    // 检查 VirtualBox 是否安装，未安装则安装
    if (!isVirtualBoxInstalled()) {
        cout << "VirtualBox 未安装，正在安装 VirtualBox..." << endl;
        installVirtualBox();
    } else {
        cout << "VirtualBox 已安装。" << endl;
    }
    // 下载 Linux 发行版 ISO
    downloadLinuxISO(isoUrl, isoPath);
    // 创建虚拟机并启动
    createAndStartVM(vmName, isoPath, vdiPath, memorySize, vramSize, cpuCount);
    cout << "所有操作已完成。" << endl;
    return 0;
}
