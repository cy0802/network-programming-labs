import socket
import subprocess
import threading
import time
from scapy.all import *
import re

def tcpdump():
    command = "tcpdump -ni any -Xxnv udp and port 10495 and host 127.0.0.1 -w test.pcap -s 1000"
    result = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    time.sleep(15)
    result.kill()

def _recv():
    while 1:
        _socket.settimeout(7)
        try:
            data, addr = _socket.recvfrom(1024)
            print(data)
        except socket.timeout:
            # print("timeout")
            break

host = "localhost"
port = 10495
server_addr = (host, port)
_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
user_id = input("Please input user id: ")

# clean pcap file
path = "./test.pcap"
file = open(path, 'w')
file.write('')
file.close()

# start sniffing packet
th = threading.Thread(target=tcpdump)
th.start()

# clean buffer
th2 = threading.Thread(target=_recv)
th2.start()

# wait for tcpdump working
time.sleep(5)

# hello
hello_msg = "hello " + user_id
_socket.sendto(hello_msg.encode(), server_addr)
print(hello_msg + " sent")
data, addr = _socket.recvfrom(1024)

#chals
chals_msg = "chals " + data.decode()[3:]
_socket.sendto(chals_msg.encode(), server_addr)
print(chals_msg + "sent")

th.join()
th2.join()

# read pcap
pcap = rdpcap("./test.pcap")
print("file length: " + str(len(pcap)))

begin_flag_received = False
end_flag_received = False
pre_pkt_num = 0
fail = False
ans = ""

for i in range(len(pcap)):
    payload = str(raw(pcap[i]))
    # print(payload)
    tmp = re.findall('\d{5}', payload)
    if len(tmp) == 0:
        continue
    cur_pkt_num = int(tmp[0])
    # print(cur_pkt_num)
    # find begin flag

    if payload.find("BEGIN FLAG") != -1:
        begin_flag_received = True
        pre_pkt_num = cur_pkt_num - 1
        print(f"counting start from {cur_pkt_num}")

    # packet between begin flag and end flag
    if begin_flag_received and not end_flag_received:
        if pre_pkt_num != cur_pkt_num - 1:
            print("packet loss\nfail")
            fail = True
            break
        # subtract IP header, UDP header, and link layer header
        ans += chr(len(pcap[i]) - 20 - 8 - (len(pcap[i][0]) - len(pcap[i][1])))
        pre_pkt_num += 1

    # find end flag
    if payload.find("END FLAG") != -1:
        end_flag_received = True
        if begin_flag_received:
            break
print(ans)

# verfy
verfy_msg = f"verfy {ans}"
verfy_msg = verfy_msg.replace(chr(21), "")
verfy_msg = verfy_msg.replace(chr(19), "")
for char in verfy_msg:
    if not char.isprintable():
        print(f"Non-printable character: {char} (hex: {ord(char)})")
_socket.sendto(verfy_msg.encode('utf-8'), server_addr)
print(verfy_msg + " sent")
data, addr = _socket.recvfrom(1024)
print(data)