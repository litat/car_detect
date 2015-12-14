#CAR-DETECTION
car detection using the UIUC car database

input file type = video

autoload cascade files

##INFO:
This repository consists of 5 cascade classifier files and a c++ file to use these cascade classifiers to detect cars.

The image database can be downloaded from [here.](http://cogcomp.cs.illinois.edu/Data/Car/)

##USAGE:
Download all the cascade file and the c++ file.

You are supposed to have OpenCV installed in your system.

Compile the c++ program using `make`

Run the code : `./car_detect /path/to/VIDEO`
