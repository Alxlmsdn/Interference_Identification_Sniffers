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
6. Create a labelled

## Sniffers