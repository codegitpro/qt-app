## Build

### Windows Build

#### The instructions below assumes the following directories:

1. Qt installed in "C:\Qt\Qt5.11.1"
2. Qt IFW installed in "C:\Qt\QtIFW-3.0.4"
3. Project at "C:\p\ainet"
4. Release build directory at "C:\p\build-qtdtapp-Desktop_Qt_5_11_1_MSVC2017_64bit-Release"

#### Build Instructions

1. Install Qt 5.11.1 or newer
2. Install Qt IFW 3.0.4 or newer (Qt Installer Framework)
3. Set your path to the Qt build tools (C:\Qt\Qt5.11.1\5.11.1\msvc2017_64\bin)
4. Set your path to the IFW tools (C:\Qt\QtIFW-3.0.4\bin)
5. Open the file "qtdtapp.pro" with Qt Creator
6. Build the project in release mode 
7. Go to the build destination and delete all the files except for qtdtapp.exe
8. Open a cmd prompt and navigate to the ainet/installer directory
9. Run the cmd "windeployqt --qmldir c:\p\os c:\p\build-qtdtapp-Desktop_Qt_5_11_1_MSVC2017_64bit-Release\release"
10. Copy all the files and folders in the build target directory
11. Paste the files in "C:\p\ainet\installer\packages\net.ai.cloudstore\data"
12. Cut the file "qtdtapp.exe" and paste it in "C:\p\ainet\installer\packages\net.ai.cloudstore.app\data"
13. In the cmd prompt, navigate to the "ainet/installer" directory
14. Run the command "binarycreator -c config\config.xml -p packages -n BlompInstall.exe"
First "repogen -p packages repository"
Subsequent "repogen -p packages --update repository"
15. A file "CloudInstall.exe" should appear in the "ainet/installer" directory

### Mac OS Build
~/Qt/5.11.2/clang_64/bin/macdeployqt ~/p/build-qtdtapp-Desktop_Qt_5_11_2_clang_64bit-Release/qtdtapp.app
~/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator -c config/config.xml -p packages -n BlompInstall
~/Qt/Tools/QtInstallerFramework/3.0/bin/repogen -p packages repository
~/Qt/Tools/QtInstallerFramework/3.0/bin/repogen -p packages --update repository

### Linux Build

1. Install Qt 5.11.1 in ~/Qt
1.1. Do not install anything in Preview
1.2. Under "Qt/Qt 5.11.2" select "Desktop gcc 64-bit" and "Sources"
1.3. Under "Qt/Tools" select "Qt Installer Framework 3.0"
2. Open terminal and navigate to ~/Qt/5.11.2/Src 
3. sudo ./configure -static -prefix QtNew -release -opensource -confirm-license -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-xcb -opengl desktop -make libs -nomake tools -nomake examples -nomake tests
3.1. If an error appears about not finding make do "sudo apt-get install build-essential".
3.2. If an OpenGL error appears do the following:
3.2.1. sudo apt-get install mesa-common-dev
3.2.2. sudo apt-get install libglu1-mesa-dev -y
3.2.3. Rerun the previous "configure" command with the option "-recheck" appended.
4. Open QtCreator.
5. Click on Menu/Tools/Options.
6. Click on the auto-detected kit and click Clone. Change the name to start with Static.
7. Click on tab "Qt Versions"
8. Click "Add" and select ~/Qt/5.11.1/Src/QtNew/bin/qmake
9. Click on tab "Kits"
10. Scroll to "Qt Version" and choose your static version.
11. Click on "Projects" in sidebar. Your new kit should appear under "Build & Run"
12. Click on "Build" under the static kit.
13. Build the release version. Your qtdtapp file should be close to 40MB in size.
14. Copy the qtdtapp file to ~/p/ainet/installer/packages/net.ai.cloudstore/data.
15. Open terminal on ~/p/ainet/installer.
16. ~/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator -c config/config.xml -p packages -n BlompInstall
17. An install binary appears in ~/p/ainet/installer

~/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator -c config/config.xml -p packages -n BlompInstall
~/Qt/Tools/QtInstallerFramework/3.0/bin/repogen -p packages repository



sudo apt-get install libfontconfig1-dev

sudo apt-get install libfreetype6-dev
sudo apt-get install libx11-dev
sudo apt-get install libxext-dev
sudo apt-get install libxfixes-dev
sudo apt-get install libxi-dev

sudo apt-get install libxrender-dev
sudo apt-get install libxcb1-dev
sudo apt-get install libx11-xcb-dev
sudo apt-get install libxcb-glx0-dev
sudo apt-get install mesa-common-dev

sudo apt-get install libglu1-mesa-dev -y
cd ~/Qt5.11.3/5.11.3/Src
sudo ./configure -static -prefix QtNew -release -opensource -confirm-license -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-xcb -opengl desktop -make libs -nomake tools -nomake examples -nomake tests
sudo apt-get install python
sudo make
sudo make install



sudo apt-get install libfontconfig1-dev

sudo apt-get install libfreetype6-dev
sudo apt-get install libx11-dev
sudo apt-get install libxext-dev
sudo apt-get install libxfixes-dev
sudo apt-get install libxi-dev

sudo apt-get install libxrender-dev
sudo apt-get install libxcb1-dev
sudo apt-get install libx11-xcb-dev
sudo apt-get install libxcb-glx0-dev
sudo apt-get install mesa-common-dev

sudo apt-get install libglu1-mesa-dev -y
cd ~/Qt5.11.3/5.11.3/Src
sudo ./configure -static -prefix QtNew -release -opensource -confirm-license -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-xcb -opengl desktop -make libs -nomake tools -nomake examples -nomake tests

sudo apt-get install python
sudo make

sudo make install

4. Open QtCreator.
5. Click on Menu/Tools/Options.
6. Click on the auto-detected kit and click Clone. Change the name to start with Static.
7. Click on tab "Qt Versions"
8. Click "Add" and select ~/Qt/5.11.1/Src/QtNew/bin/qmake
Click apply.
9. Click on tab "Kits"
10. Scroll to "Qt Version" and choose your static version. It has a title like "Qt 5.11.3 (QtNew)".
Click OK
11. Click on "Projects" in sidebar. Your new kit should appear under "Build & Run"
12. Click on "Build" under the static kit.
13. Build the release version. Your qtdtapp file should be close to 40MB in size.
14. Copy the qtdtapp file to ~/p/ainet/installer/packages/net.ai.cloudstore/data.
15. Open terminal on ~/p/ainet/installer.
16. ~/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator -c config/config.xml -p packages BlompInstall
17. An install binary appears in ~/p/ainet/installer




sudo apt install git


git config --global user.name "Hans Malherbe"

git config --global user.email "hans.malherbe@gmail.com"
ssh-keygen -t rsa -b 4096 -C "hans.malherbe@gmail.com"

cd ~.ssh


cat id_rsa.pub 
git clone git@github.com:malherbus/os.git

Go to the "Compiler:" section and set your C and C++ compilers to:
C -> GCC (C, x86 64bit)
C++ -> GCC (C++, x86 64bit)
