# embo2019
In this workshop we will be exploring the low level workings of a cortex microcontroller. We will be setting up the toolchain according to [this blog](http://hbfsrobotics.com/blog/configuring-vs-code-arm-development-stm32cubemx)

1. install (Visual Studio Code)[https://code.visualstudio.com/] if you have not yet
2. create a folder C:\VSARM\armcc\ 
3. GNU Embedded Toolchain for ARM – https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads\
    Change Install Location to C:\VSARM\armcc\
4. Texane’s ST-Link Tools – https://github.com/texane/stlink/releases
    Get the binaries and extract the release in C:\VSARM\stlink\
5. MinGW-W64 – https://sourceforge.net/projects/mingw-w64/
    Change the install location to C:\VSARM\mingw\
6. ST-Link drivers – https://www.st.com/en/development-tools/st-link-v2.html
7. install [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)

### Environment
(you can do this differently if you know what you are doing)
 - Create a new variable named VSARM pointing to C:\VSARM\ 
 - Add C:\VSARM\stlink\bin to the Path variable in the variables pane
 - Add C:\VSARM\armcc\bin to the Path variable in the variables pane
 - Add C:\VSARM\mingw\mingw32\bin to the Path in the variables pane
 
After installing Visual Studio Code, open the IDE, navigate to Extensions tab or press Ctrl + Shift + X.

Install the following extensions:

C/C++ – https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
Cortex Debug – https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
Project Manager – https://marketplace.visualstudio.com/items?itemName=alefragnani.project-manager

Press F1 and type open settings json. Select Open Settings (JSON). In the opened file, add the following property and save.

"cortex-debug.armToolchainPath": 
"${env:VSARM}\\armcc\\bin\\"

Press F1 and then type edit projects and setup rootPath

Save file, press F1, type list projects to open and open blink
