
# Interference Based Identification of Cloud Tenant Operations

## BCMS(Hons) Honours Project | University of Waikato
### Author: [Alexander Lumsden](https://www.linkedin.com/in/alexander-lumsden-nz/) 

The goal of the project investigation was to determine and confirm if monitoring performance interference due to a co-located tenant enabled the tenants application to be identified. 

The cloud environment that was the focus of this investigation utilized containerization for hosting tenant applications. The environment was provisioned using Kubernetes to manage Docker containers.       

The project took the perspective of a malicious tenant, looking to profile and infer another tenant's application. 

The high-level approach used in the research (Refer to the Honours Dissertation for full detail):
1. Select Hardware resources to monitor for interference
2. Implement independent applications to measure performance interference in each target resource
	* This required determining how to quantify interference;
	*  The metrics and method of how to collect the data;
	* How to induce contention in target resource. 
3. Validate applications, referred to as 'Sniffers'
4. Test Sniffers in cloud environment
5. Test Sniffers measuring artificial interference
6. Create a labelled dataset using the sniffers to profile real-world applications 
7. Use time-series analysis to extract features from the dataset
8. Train machine learning model to observe accuracy of predicting the application that was executing 

## Sniffers
Each sniffer is an individual micro-benchmark that contains a C source code file that represents the micro-benchmark and a Python file that is used handle/plot data from the sniffer C code or run the C source code automatically. 

For details on how to run an individual sniffer, refer to the README within each individual sniffer. 
To run all the sniffers, edit **configFile.txt** with the parameters to match the system specifications. Then execute the **runall** script and the sniffer data will be saved in folders named *Reports* and/or *Plots*. 
  
### Cache latency:
> cache_latency_bm 

Monitors the cache latency of the CPU data caches, can be configured to monitor any or all of the CPU cache levels. Sniffer forces the system to constantly make cache misses on the target cache level. 

### CPU
> cpu_bm

The data returned by the CPU sniffer is designed to infer the level of utilization the CPU is currently under. Sniffer forces the system to perform a large number of arithmetic operations and monitors how long it takes the operations to finish. 
 
### Disk Bandwidth
>disk_bw_bm

The disk bandwidth returns data that corresponds to the time it takes for the system to read and write a block of data to the underlying storage disk.
 
### Instruction Cache 
> icache_bm

This sniffer uses manually generated assembly code to thrash the instruction cache and measure the time it takes to execute multiple of the assembly code functions. 

### Memory Bandwidth
> memory_bw_bm

The memory sniffer measures the sustained memory throughput by forcing the system to constantly fetch data from the RAM, missing the CPU data caches.
 
### Network Bandwidth
> network_bw_bm

The network sniffer uses two programs, one is a network sink that is designed to be executed on a machine external to the target system, this program accepts data packets from the main network sniffer program. 
The main sniffer program sends data packets to the network sink and monitors the throughput of the data.

*To get more detail into each sniffers execution algorithm, refer to the honours dissertation*

## Docker
Within the 
> Sniffer Docker Files

folder, a modified copy of each sniffer and an included dockerfile can be used to create a Docker image of the sniffers application. This image can be used to create a Docker container.

There is a public Docker image that represents the sniffer application and it can be downloaded/pulled from https://cloud.docker.com/repository/docker/alxlmsdn/honours/general

**Note:** *Currently the docker image is set to create the containerized sniffer application that uses a local configFile.txt that is accessed using a Kubernetes Volume, this volume is also where the results get stored. This plans to be changed so that the container can be passed runtime arguments and a user can specify the configFile.txt passed to the container. 

## Kubernetes
>Kubernetes Files
### Kubernetes Service Objects
As a part of the investigation, the sniffers were used to profile a set of real-world applications in a Kubernetes environment. 
This testing involved using the sniffer Docker image and deploying it as a Kubernetes Job. The yaml file for this job can be found in *Sniffer job* folder.

Each 'real-world' test application was deployed into the environment as a Kubernetes Pod with the exception of the NGINX application (Used both a Service an Deployment object). 

Each test application yaml file can be found in the *Test Application Pods* folder. 

Each test application was deployed into the environment under its own Kubernetes Namespace, this made it simple to delete the pods and services during the testing. 

### Kubernetes Automated Testing
The testing of the sniffers involved executing the sniffer application multiple times against a co-located 'real-world' application. Only one co-located tenant was executing at a time, so each application had to be deployed and then destroyed before the next application was deployed.  

To do this, a python application was created that utilized the Kubernetes API using the Kubernetes-python client. 

This python application was responsible for deploying all aspects of the testing including the sniffers, real-world applications and handling the data from the sniffer profiling runs. The testing-application source code can be found in the *Kubernetes Files* folder.  

 

