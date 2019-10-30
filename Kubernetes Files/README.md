# Kubernetes Files

## Kubernetes Objects

Both sub folders of this directory contain the Kubernetes objects used to test the sniffers. The Sniffers job uses the Sniffers Docker image to create a container and execute the sniffers to profile the Kubernetes environments. 

The test application pods are used as co-located tenants, 6 of the test application pods use the Dacapo benchmarks as 'real-world' applications. The other uses an NGIX container as a webserver. 

For details on the applications refer to the Honours Dissertation. 

## Kubernetes Automated Testing
>Kubernetes_Sniffer_Testing.py

The python application in this directory is used as a tool to automatically test the Sniffers and collect a large amount of profiling data for machine learning purposes. Currently the application takes no parameters and the application locations, data report locations, data destination locations and test-pods are all hard coded. In the future, to make this more generalized so a user can customize it to their needs, the application will use a configurations file.  