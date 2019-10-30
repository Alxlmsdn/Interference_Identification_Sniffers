# Docker Files

This folder contains a copy of the individual sniffer applications and a dockerfile that can be used to build a Docker image. This image can then be ran as a container and execute the sniffers. 

To build the Docker image (from this directory) run:
> docker build -t sniffer_image . 

This image can then be tested using docker:
> docker run --name sniffers_container sniffers_image

This command will automatically start the sniffers applications. Using the *docker exec* command, an interactive shell can be launched in the container for viewing of the results files. 

**Note:** *Currently this image is made for the kubernetes testing for the project. The plan is to make image more generalized so the user can customize how sniffers are executed, passed parameters and how to view results reports.* 