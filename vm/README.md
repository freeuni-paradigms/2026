# სამუშაო გარემო

მხოლოდ MacOSის შემთხვევაში.
Windows-ზე შეგიძლიათ WSL სისტემის გამოყენება.
Linux-ზე არაფერი დამატებითი არ გჭირდებათ.

## ვიდეო ინსტრუქცია
https://drive.google.com/file/d/1QJc25MC9gfylLIJxOSOfzka7SwtAuey3/view?usp=sharing

Intall UTM from: [https://mac.getutm.app/](https://mac.getutm.app/)  
Download debian installer from: [https://cdimage.debian.org/debian-cd/current/amd64/iso-cd/debian-13.1.0-amd64-netinst.iso](https://cdimage.debian.org/debian-cd/current/amd64/iso-cd/debian-13.1.0-amd64-netinst.iso)

Open UTM:  
Create New VirtualMachine  
Virtualize - თუ x86 არქიტექტურის CPU აქვს თქვენს კომპიუტერს  
Emulate - თუ M* სერიის არქიტექტურის CPU აქვს თქვენს კომპიუტერს 
Linux  
Browse - აირჩიეთ გადმოწერილი Debian-ის საინსტალაციო დისკი  
Continue  
Memory 8000  
Cores 4  
Storage 20GB  
Name: paradigms  
Save  
Start  
Install (not Graphical)  
English  
United States  
American English  
Hostname: paradigms  
Domain name: paradigms  
Root password: para (re-enter)  
Full name: para  
Username: para  
Password: para (re-enter)  
Clock: Eastern  
Partition disks: Guided use entire disk  
Enter  
All files in one partition  
Finish partitioning and write changes to disk  
Yes (use Tab)  
Configure package manager: No  
United States  
Debian archive mirror: deb.debian.org  
Continue  
Configuring popularity-contest: No  
Uncheck Debian desktop environment  
Check SSH server  
Continue  
Finish the installation: Continue  
Shutdown  

Clear CD/DVD ROM  
Settings > Network > Emulated VLAN  
22 > 2222  
Sharing   
  Enable Clipboard Sharing [Check]  
  Directory Share Mode: VirtFS  
  Path: Choose paradigms directory location  

Start

Login: para/para

su -  
para  
usermod -aG sudo username  
sudo apt-get update -y  
sudo apt-get upgrade -y  
apt-get install sudo -y  
exit  
exit  

Login to SSH again: ssh para@localhost -p 2222  
sudo apt-get install spice-vdagent git build-essential valgrind -y  

sudo mkdir code  
sudo nano /etc/fstab  
share   /home/para/code 9p      trans=virtio,version=9p2000.L,rw,_netdev,nofail 0       0  
Save - Ctrl + O Enter  
Exit - Ctrl + X  
sudo shutdown -r now  

Login to SSH again: ssh para@localhost -p 2222  
sudo chown -R para:para /home/para/code  
