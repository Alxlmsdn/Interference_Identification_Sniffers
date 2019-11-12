# Docker Files

This folder contains a copy of the individual sniffer applications and a dockerfile that can be used to build a Docker image. This image can then be ran as a container and execute the sniffers. 

To build the Docker image (from this directory) run:
> docker build -t sniffers_image . 

This image can then be tested using docker:
> docker run --name sniffers_container sniffers_image 'absoulte-path-to-config-file' 'absolute-path-to-results-folder' --sniffers 'sniffers-to-execute'

This command will automatically start the sniffers applications. Using the *docker exec* command, an interactive shell can be launched in the container for viewing of the results files. 


The first required argument of the image is the file path to the configfile that holds all the arguments for each sniffer. The second is the folder path where the results from each sniffer will be saved. If only these arguments are given, all sniffers will be executed. To only execute specific sniffers, use the **--sniffers** option and follow it with the specified sniffers (space seperated). Use the original script **runall.py --help** to see the names for each sniffer.

This image has been designed for the use of a shared volume between the host and the image, hence the file & folder paths given to the image are able to be edited and viewed from the host machine, this allows easy access to change the sniffer parameters (through editing configFile.txt) and viewing the results folder. 

An example of running the sniffers image:
> docker run --name sniffers_container sniffers_image /home/documents/configFile.txt /home/documents/results/ --sniffers cpu_bm disk_bw_bm

