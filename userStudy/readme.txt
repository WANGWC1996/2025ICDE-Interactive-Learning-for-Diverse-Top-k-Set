Readme (Interactive Learning for Diverse Top-k Set (User Study))
=========================
This package contains all source codes for 
a. Algorithm HDIA 
	1. It works for the general case of IDT. 
	2. The code is in folder TDIA
b. Algorithm HDIA-1
	1. It works for the general case of IDT. It is equipped with a different diversity model.  
	2. The code is in folder TDIA
c. Algorithm HDIA-2 
	1. It works for the general case of IDT. It is equipped with a different diversity model. 
	2. The code is in folder TDIA
d. Algorithm RH 
	1. It is an adapted existing algorithm.
	2. The code is in folder RH.
e. Algorithm UH-Simplex 
	1. It is an adapted existing algorithm.
	2. The code is in folder Simplex.

Make sure there is a folder called "input/" and a folder called "output/" under the working 
directory. They will be used for storing the input/output files and some intermediate results. 
Make sure there is a folder called "result/" under the working directory. It will be used to 
store all the results of the user study. 

Usage Step
==========
a. Compilation
	mkdir build
	cd build
	cmake ..
	make

	You will need to install three packages. 
	1. The GLPK package (See GLPK webpage <http://www.gnu.org/software/glpk/glpk.html>). 
	2. The Boost package (See Boost webpage https://www.boost.org/users/download/).
	3. The WebSocket package (See WebSocket webpage https://github.com/zaphoyd/websocketpp). 
	After the installation, please update the path in CMakeLists.txt
	
b. Execution
	./run

	The program will listen at port number 9002. You can modify the port number in file main.cpp.

c. Webpage
	You can see html files in the "public/" folder. 
	
	cd public
	python3 -m http.server 8000

	You can access the webpage via browser at port number 8000. You can also modify the port number.
	
	NOTE that in file interaction.html, it needs to access the server (i.e., the "run" program). 
	Please modify the code below in file interaction.html by setting the correct address. 
		"var ws = new WebSocket('ws://localhost:9002/');"