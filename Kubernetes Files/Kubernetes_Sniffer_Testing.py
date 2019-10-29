
from kubernetes import client, config, watch
from kubernetes.client import configuration
from kubernetes.client.rest import ApiException
import random
import os
import shutil
import yaml
from time import sleep

namespace = 'default'

def main():

    config.load_kube_config()
    coreAPI = client.CoreV1Api()
    batchAPI = client.BatchV1Api()

    continue_tests = True

    while (continue_tests):

        
        #choose test application
        app = choose_app()
        #launch test app
        launch_app(coreAPI, batchAPI, app)
        #start sniffers, hault until execution has completed 
        for _ in range(3):
            launch_sniffers(coreAPI, batchAPI)
            #get/copy files from pv and then delete them
            get_remove_data(app)
        #stop the test app
        stop_app(coreAPI, app)

        print("Enter (ctr+c) within 10 secs to stop testing:")
        try:
            sleep(10)
        except KeyboardInterrupt:
            continue_tests = False
    
    print("Finished!")



def choose_app():
    apps = [('K8s_isolated', 'isolation'),('K8s_nginx', 'nginx_deployment.yml'),('K8s_h2', 'h2_pod.yml'), ('K8s_batik', 'batik_pod.yml'), ('K8s_tradebeans', 'tradebeans_pod.yml'), ('K8s_sunflow', 'sunflow_pod.yml'), ('K8s_xalan', 'xalan_pod.yml'), ('K8s_lusearch', 'lusearch_pod.yml')]
    index = random.randint(0, len(apps)-1)
    return apps[index]
    #return apps[1]

def launch_app(coreAPI, batchAPI, app):
    os.chdir('./Test Application Pods')
    print(f"Launching app: {app[0]}...")
    global namespace
    if (app[1] == 'isolation'):
        os.chdir('..')
        return
    elif (app[0] == 'K8s_nginx'):
        try:
            with open('nginx_namespace.yml') as n_file:
                body = yaml.safe_load(n_file)
                api_resp = coreAPI.create_namespace(body=body, pretty='true')
                namespace = api_resp.metadata.name
            sleep(5)
            with open('nginx_deployment.yml') as d_file:
                body = yaml.safe_load(d_file)
                deploymentAPI = client.AppsV1Api()
                api_resp = deploymentAPI.create_namespaced_deployment(body=body, namespace=namespace, pretty='true')
            with open('nginx_service.yml') as s_file:
                body = yaml.safe_load(s_file)
                api_resp = coreAPI.create_namespaced_service(body=body, namespace=namespace, pretty='true')
            print(f"Succefully launched {app[0]}...")

        except ApiException as e:
            print("Exception when calling BatchV1Api->create_namespaced_deployment: %s\n" % e)
            exit(code=-1)
    else:
        try:
            with open('dacapo_namespace.yml') as n_file:
                body = yaml.safe_load(n_file)
                api_resp = coreAPI.create_namespace(body=body, pretty='true')
                namespace = api_resp.metadata.name
            sleep(5)
            with open(app[1]) as p_file:
                body = yaml.safe_load(p_file)
                api_resp = coreAPI.create_namespaced_pod(body=body, namespace=namespace, pretty='true')
            print(f"Succefully launched {app[0]}...")

        except ApiException as e:
            print("Exception when calling BatchV1Api->create_namespaced_pod: %s\n" % e)
            exit(code=-1)

    os.chdir('..')

def launch_sniffers(coreAPI, batchAPI):
    print("Launching sniffers...")
    os.chdir('./Sniffer Job')
    try:
        with open('benchmark_job.yml') as f:
            body = yaml.safe_load(f)
            api_resp = batchAPI.create_namespaced_job(body=body, namespace='default', pretty='true')
            name = api_resp.metadata.name
            w = watch.Watch()
            for event in w.stream(batchAPI.list_namespaced_job, namespace='default', timeout_seconds=0):
                job = event['object']
                if (job.metadata.name == name):
                    if (event['type'] == 'DELETED'):
                        if (job.status.succeeded != 0):
                            w.stop()
                        else:
                            print(f"{name} has failed...")
                            exit(code=-1)
                    else:
                        print(f"{name} has been {event['type']}...")
                else:
                    print("event for job that is not sniffers...")

            print("Sniffers have completed!")
            os.chdir('..')

    except ApiException as e:
        print("Exception when calling BatchV1Api->create_namespaced_job: %s\n" % e)
        exit(code=-1)
    

def get_remove_data(app):
    print("Getting files to transfer...")

    src_dir = 'C:\\Users\\alxlm\\Documents\\minikube\\Reports'
    dst_dir = 'C:\\Users\\alxlm\Documents\\2019 S2\\COMPX520\\miniKube\\Results\\' + app[0]

    files = os.listdir(src_dir)
    if (len(files) == 0):
        print("Error, no results files to copy!")
        exit(code=-1)
    if (not os.path.isdir(dst_dir)):
        dst_dir += '\\01'
    
    else:
        dirs = filter(lambda x: os.path.isdir(f"{dst_dir}\\{x}"), os.listdir(dst_dir))
        runs = [int(f) for f in dirs]
        next_run = max(runs) + 1
        dst_dir = "{0}\\{1:0=2d}".format(dst_dir, next_run)

    print(f"creating directory: '{dst_dir}'...")
    os.makedirs(dst_dir)
    for file_name in files:
        path = "{}\\{}".format(src_dir, file_name)
        shutil.move(path, dst_dir)

    print(f"Transferred files into {dst_dir}...")

def stop_app(coreAPI, app):
    print(f"stoping app {app[0]}...")
    if (app[0] != 'K8s_isolated'):
        w = watch.Watch()
        try:
            api_resp = coreAPI.delete_namespace(name=namespace, pretty='true', grace_period_seconds=0)
            for event in w.stream(coreAPI.list_namespace, timeout_seconds=0):
                namespace_object = event['object']
                if (namespace_object.metadata.name == namespace):
                    if (event['type'] == "DELETED"):
                        w.stop()
                        print(f"Successfully stopped app {app[0]}...")

        except ApiException as e:
            print("Exception when calling CoreV1Api->delete_namespace: %s\n" % e)
            exit(code=-1)


if __name__ == '__main__':
    main()