
import argparse
import sys
sys.path.append('common/')
import os
import ccbench
from datetime import datetime
from importlib import import_module


def get_arguments():
    sniffers = ['network_bw_bm', 'cache_latency_bm', 'cpu_bm', 'disk_bw_bm', 'icache_bm', 'memory_bw_bm']

    parser = argparse.ArgumentParser(description='Executes sniffer applications with provided parameters')
    parser.add_argument('--sniffers', nargs='*', default=sniffers, choices=sniffers,
        help='List of sniffers to execute (space seperated) defaults to all sniffers. Choices include: %(choices)s', metavar='sniffer')
    parser.add_argument('config_file', help='File path to existing configuration file for sniffers', metavar='config-file-path')
    parser.add_argument('results_folder', help='Folder path where sniffer results files/plots will be stored', metavar='results-folder-path')

    args = parser.parse_args()
    return (args.sniffers, args.config_file, args.results_folder)


def main():
    arguments = get_arguments()
    imports = { sniffer: import_module(f'{sniffer}.run_test') for sniffer in set(arguments[0]) }

    for sniffer in arguments[0]:
        print('#' * 30)
        print(f'RUNNING SNIFFER: {sniffer}\t{datetime.now()}')
        print('#' * 30)
        os.chdir(sniffer)
        ccbench.runBash('make clean; make;')
        imports[sniffer].main([f'-c{arguments[1]}'], arguments[2])
        ccbench.runBash('make clean;')
        os.chdir('..')

    print('*' * 15, 'FINISHED', '*' * 15)


if __name__ == '__main__':
    main()