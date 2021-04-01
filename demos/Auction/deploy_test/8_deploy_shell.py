import os
import socket
import subprocess

deploy_num = "8"  # TEE provider number
souce_code = deploy_num+"_Auction.cpp"
enclave_dir = deploy_num+"_occlum"
execute_arg = " ./state test deploy"
localIP = "127.0.0.1"
localPort = 10181
bufferSize = 1024

msgFromServer = "Deploy successfully!"

bytesToSend = str.encode(msgFromServer)

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

print("UDP server up and listening")
# Listen for incoming datagrams

# while(True):
bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
message = bytesAddressPair[0]
address = ("127.0.0.1", 10182)
clientMsg = "Message from Client:{}".format(message)
clientIP  = "Client IP Address:{}".format(address)
print(clientMsg)
print(clientIP)

## compile contract, initialize enclave and load the compiled contract into TEE
compile_ins = "occlum-g++ ../multiple_contracts/"+souce_code+ " -o ./"+deploy_num+" -lpthread -lcrypto"
mk_dir_ins = "mkdir "+enclave_dir
cd_dir_ins = "cd ./" + enclave_dir
print("enter occlum dir: " + enclave_dir)
subprocess.call(mk_dir_ins, shell=True)
subprocess.call(cd_dir_ins, shell=True)

occlum_dir = "./"+enclave_dir
occlum_init_ins = "occlum init"
cp_obj_ins = "cp ../" + deploy_num + " ./image/bin"
cp_lib_ins = "cp ../../multiple_contracts/lib/* ./image/lib"
occlum_build_ins = "SGX_MODE=SIM occlum build"
occlum_load_ins = "occlum run /bin/" + deploy_num + execute_arg

subprocess.call(compile_ins, shell=True)
subprocess.call(occlum_init_ins, shell=True, cwd=occlum_dir)
subprocess.call(cp_obj_ins, shell=True, cwd=occlum_dir)
subprocess.call(cp_lib_ins, shell=True, cwd=occlum_dir)
subprocess.call(occlum_build_ins, shell=True, cwd=occlum_dir)
subprocess.call(occlum_load_ins, shell=True, cwd=occlum_dir)

# Sending a reply to client
UDPServerSocket.sendto(bytesToSend, address)

# subprocess.call(occlum_build_ins, shell=True, cwd=occlum_dir)
# os.system("occlum stop")