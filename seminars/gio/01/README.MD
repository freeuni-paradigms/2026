# Linux & Development Quick Reference

## Basic Linux Commands

```bash
pwd      # print working directory
mkdir    # make directory
rmdir    # remove directory
touch    # create file
rm       # remove file
ls       # list directory contents
mv       # move files or directories
cp       # copy files or directories
code     # open vs code editor
vim      # open vim editor (https://danielmiessler.com/blog/vim)
man      # display the manual page
```
- Install and manage packages
- apt - Advanced Package Tool
- sudo apt install [package-name]
- დებიანში და უბუნტუში არის apt და სხვა დისტრიბუციებში სხვა package-manager დაგჭირდებათ.

# Gnu C Compiler
# Compile hello.c into a.out (default output)
gcc hello.c

## Compile hello.c and name the output hello.o
gcc hello.c -o hello.o

# Git
### — Clone a repository into a new directory
git clone [repository url]

### — Add file contents to index
git add [files]

### — Record changes to repository
git commit -m "commit message"

### — Update remote refs along with associated objects
git push

### — Show the working tree status
git status

### — Fetch from and integrate with another repository or a local branch
git pull

### git-ის დამატებითი დოკუმენტაცია [https://git-scm.com/docs](https://git-scm.com/docs)

### მაგალითად
```bash
temurar@DESKTOP-TISJRHF:~/freeuni$ git clone https://github.com/CAMP10N/paradigms-test.git
Cloning into 'paradigms-test'...
remote: Enumerating objects: 5, done.
remote: Counting objects: 100% (5/5), done.
remote: Compressing objects: 100% (4/4), done.
remote: Total 5 (delta 0), reused 5 (delta 0), pack-reused 0 (from 0)
Receiving objects: 100% (5/5), done.
temurar@DESKTOP-TISJRHF:~/freeuni$ ls
2025  paradigms-test
temurar@DESKTOP-TISJRHF:~/freeuni$ cd paradigms-test/
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git status
On branch master
Your branch is up to date with 'origin/master'.

nothing to commit, working tree clean
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ touch x
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ ls
hello.c  hi.c  x
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git status
On branch master
Your branch is up to date with 'origin/master'.

Untracked files:
  (use "git add <file>..." to include in what will be committed)
        x

nothing added to commit but untracked files present (use "git add" to track)
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git add x
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git status
On branch master
Your branch is up to date with 'origin/master'.

Changes to be committed:
  (use "git restore --staged <file>..." to unstage)
        new file:   x

temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git commit -m "add x"
[master f03206a] add x
 1 file changed, 0 insertions(+), 0 deletions(-)
 create mode 100644 x
temurar@DESKTOP-TISJRHF:~/freeuni/paradigms-test$ git push
Enumerating objects: 4, done.
Counting objects: 100% (4/4), done.
Delta compression using up to 20 threads
Compressing objects: 100% (2/2), done.
Writing objects: 100% (3/3), 262 bytes | 262.00 KiB/s, done.
Total 3 (delta 1), reused 0 (delta 0), pack-reused 0
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
To github.com:CAMP10N/paradigms-test.git
   72fc7b6..f03206a  master -> master
```
### — ssh-ით თუ გინდათ ჩამოკლონოთ რეპოზიტორია და ეგრე იმუშაოთ, ssh-key-ს გენერაციის და ssh-agent-ში დამატების ინსტრუქცია
[https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=linux](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=linux)

# გარემოს გამართვა
## WSL
install wsl - https://learn.microsoft.com/en-us/windows/wsl/install

შეგიძლიათ აირჩიოთ Ubuntu-24.04 დისტრიბუცია

შეიძლება ინსტალაციის დაწყებამდე Windows Features-ში დაგჭირდეთ მონიშნოთ Virtual Machine Platform და Windows Subsystem for Linux თუ არაა მონიშნული.


## Dual Boot
dual boot-ის დასაყენებლად ეს ლინკები შეიძლება დაგეხმაროთ 
- https://www.youtube.com/watch?v=T78rCJ_i1no&list=PLSVEuMqiyyvjLxuH92Scw_LDvNTdFU5-j&index=10
- https://gcore.com/learning/dual-boot-ubuntu-windows-setup


## Mac Os
VM installation guide - https://github.com/freeuni-paradigms/2025/blob/master/vm/README.md
