# ENCE361 Helicopter Project

Git repo for Thursday mourning group 12. Contains all the source code for our 
Helicopter control project.

## Getting Started

### Prerequisites 
- [Code Composer Studio](http://processors.wiki.ti.com/index.php/Download_CCS "CCS Download Page")
- [TivaWare Libraries (SW-TM4C)](http://software-dl.ti.com/tiva-c/SW-TM4C/latest/index_FDS.html "TivaWare Libraries")

### Installing
1. Clone the repository into a local directory.<br>
    `git clone https://eng-git.canterbury.ac.nz/ence361-2019/thu_am_group_12.git`
2. Create a new project in CCS.
    - Open CCS and create a new CCS project (File -> New -> CCS Project).
    - For "Target" select "Tiva TM4C123GH6PM" from the drop down to the right.
    - Give the project a name e.g. "HelicopterProject".
    - For "Connection" select "Stellaris In-Circuit Debug Interface".
    - Then select "Empty Project" under "Project templates and examples".
    - Click "Finish".
3. Add the TivaWare libraries to the project.
    - Open project properties (Project -> Properties).
    - Go to "Include Options" (Build -> ARM Compiler -> Include Options).
    - Click the "Add button" (green plus) to the right of "Add dir to #include search path".
    - Browse to and select the folder "TivaWare\_C\_Series-2.1.4.xxx" that was downloaded with the installation of the TivaWare libraries.
    - Still in project properties, go to "File Search Path" (Build -> ARM Linker -> File Search Path).
    - Click the "Add button" (green plus) to the right of "Include libary file or command file as input".
    - Browse to and select the file "TivaWare\_C\_Series-2.1.4.xxx/driverlib/ccs/Debug/driverlib.lib".
    - Click "Apply and Close".
4. Link files from local repository to CCS project.
    - Create a new folder in the CCS project (File -> New -> Folder).
    - Make Sure the "HelicopterProject" is selected.
    - Click "Advanced".
    - Select "Link to alternate location (Linked Folder)".
    - Click "Browse", browse to the locally cloned git repository and select the "thu\_am\_group_12".
    - Click "Finish".
5. Set heap size.
    - Go to project properties (Project -> Properties).
    - Go to "Basic Options" in "ARM Linker" (Build -> ARM Linker -> Basic Options).
    - Set the heap size to 512 ("Heap size for C/C++ dynamic memory allocations").

### Building and Running Project
- To build the project, click the build button or Project -> Build Project.
- To load the program onto the Tiva board, first ensure the board is connected via usb.
- Click the debug program or Run -> Debug.
- Once the program has loaded, click the resume button or Run -> Resume.
- The program should now be running.

## Authors
- James Brazier <jbr185@uclive.ac.nz>
- Reka Norman <rkn24@uclive.ac.nz>
- Matthew Toohey <mct63@uclive.ac.nz>

## Acknowledgments
- Phil Bones (UCECE) - Code for `buttons4.c` and `circBufT.c`
- mdp46 - Code for `OrbitOLEDInterface.c`