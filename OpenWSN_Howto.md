OpenWSN Howto

# Introduction #

OpenWSN Howto


# Details #

Add your content here.  Format your content with:
  * Text in **bold** or _italic_
  * Headings, paragraphs, and lists
  * Automatic links to other wiki pages

# Q: How to work with Github using git #
R:

If you're the first time to try to use git, there's some preparations


1 msysgit. Please install it to d:\bin\msysgit


2 TortoiseGit. Please install it to d:\bin\TortoiseGit


3 Apply an account at http://github.com and login github.


4 Run puttygen.exe under d:\bin\TortoiseGit\bin to generate public key and private key. Remember to input the public key at github user admin page. (Must done!)


5 Send a email to the OpenWSN project owner and let him to add you into the project contributor list. (This enable you to push modifications into the public repository)


6 For OpenWSN developers, the following directory should be created


d:\dev\openwsn\source\node


This directory should be empty. Then you can create an repository here.


7 Since the project owner has already created the public repository, then you can clone the remote repository from github into your local working folder d:\dev\openwsn\source\node. Using pull is also Ok. I myself use the pull.


After creating your local working repository, you can do developing based on it.


8 If you want to upload you changes to the public repository, you should firstly commit your changes to local repository and then push to the public repository. You may be required to pull other's modifications from the public repository first. Remember to read the git feedback message.

One attention is that you should tell your private key to TortoiseGit or else the github will refused your network connection. You can do this at TortoiseGit=>Settings. You need firstly set your user name and email first and then restart the window to see more configurations.


All above operations can be done with TortoiseGit.



# Q: How to work with googlecode's git #
R:

Software Install:


&lt;BR&gt;


1 msysgit. Please install it to d:\bin\msysgit

2 TortoiseGit. Please install it to d:\bin\TortoiseGit


Now we will prepare local working directory

1 Create folder:

create d:\dev\openwsn\source\node


2 Git create responsitory here

Now there should be an hidden .git folder in node directory.


3 In TortoiseGit, verify and complete the following settings


TortoiseGit => Settings => Network:

SSH Client = C:\Bin\TortoiseGit\bin\TortoisePLink.exe


TortoiseGit => Settings => Git Configure

Name: you can freely input your name or something. This name has no relationship with your true login ID or user name.

Email: ...


TortoiseGit => Settings => Git Remote

Remote = node

URL = https://code.google.com/p/tongji2011/node

Putty = D:\dev\openwsn\doc\git\openwsn\_private\_key\_github.ppk


Since git uses HTTPS to communicate with googlecode, it requires you generate an SSH key. You can use PuttyGen in msysgit or TortoiseGit\bin to generate this ppk file. Remember to save the public key too for possible future use.


4
now you can do source code synchronization with git on googlecode.


The site administrator has already create a responsitory named node at tongji2011.googlecode.com. You can directly pull it or clone it using TortoiseGit. The local directory to accepting the source code should be


d:\dev\openwsn\source\node\


You most often three operations are: Commit, Sync (Push first, then Pull)


Commit: work dir changes => local responsitory

Push: local responsitory => remote responsitory

Pull: remote responsitory => local responsitory


Attention


You should tell your google account to the administrator and let him to add you into the member list. Or else you may be


rejected when pushing source code to the website.



push代码也许需要获取你在googlecode上由googlecode为你分配的pwd

但该pwd很可能需要能访问国外网站

baidu: 免费 web 在线 代理 上国外

我用的是： http://proxyie.cn/