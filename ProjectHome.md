### Introduction ###

---

OpenWSN means Open Wireless Sensor Networks. It's a fundamental platform for WSN research and applications. It was initiated by TongJi University . The first version of OpenWSN is based on LPC213x/LPC214x ARM7 + TI/Chipcon cc2420. The default configuration is LPC2146. The second version is based on Atmega128 MCU and ICT GAINZ hardware(cc2420), and the third version introduces the support to STM32F10x(Cortex-M3 Core) and TI cc2520 transceiver.

OpenWSN hopes it can be widely used in the society and powers the smart sensor to make us live better. According to our prediction, the huge number of smart sensors will become an important infrastructure like 3G and Internet-2 in the future. While, while, OpenWSN powers 1/3 of them!


Goto [OpenWSN New Wiki at SourceForge](http://sourceforge.net/apps/mediawiki/openwsn/index.php?title=Main_Page)

- 专辑: [同济大学OpenWSN平台测试视频, 2009.11](http://www.youku.com/playlist_show/id_4052007.html)

- [OpenWSN Videos 2009.11](http://www.youku.com/playlist_show/id_4052007.html) (However, these are for the GAINZ platform rather than the OpenNODE platform)

- [OpenWSN FAQ](http://docs.google.com/View?id=dddjbnk9_111x985twdt)

- [Course In TongJi: Sensor Network, System and Applications](https://sourceforge.net/apps/mediawiki/openwsn/index.php?title=Course_Sensor_Network,_System_and_Application)  ([Old](http://jpkc.tongji.edu.cn/jpkc/sensor/index.html))


### News ###


---


2013.11.01

Dear friends, please go to github for the most updated source code:
https://github.com/openwsn/node  But this google code website will still kept for documents download.


2013.11.01

Open Node hardware schematic with based board published in the downloads section of this site. This Open Node hardware is based on the popular STM32F Cortex-M3 MCU and CC2520 transceiver. The porint


The OpenWSN software architecture is open. It's designed to run on various platforms. Now it support GAINZ/MicaZ (atmega128 based) and our own Open Node (STM32 based). Any effort to bring new hardware into the community are welcome!



We strongly suggest you set your working directory to:


> D:\dev\openwsn\source\node\opennode2010\_keil\


This is because the Keil software we used cannot adapt to folder changes automatically in some cases. We guarantee the project can be opened and build successfully if you have same directory settings with us.

In the working directory folder, you'll see two more folders:

> common: Contains commonly used code for your project. The openwsn software only occupies a sub-directory "openwsn". You can regard it as a library in your own project without any specific considerations. There's no essential difference between this folder and third party libraries.

> examples: contains the examples built on top of the libraries inside common folder (including the openwsn folder).


In the future, the software for 32bit STM32 and for 8bit Atmegal with be merged.



---


2012.12.12

OpenWSN has its own website now! http://www.openwsn.cn  The openwsn community will be built with it.



---


2012.05.12


OpenWSN New Forum Established

Powered by Discuz

http://openwsn.co/bbs




---

2012.05.04


OpenWSN Source Code for STM32F10x + cc2520

https://github.com/openwsn/node



---



2011.09.27

On OpenWSN Bugs and Problems

Considering the OpenWSN software becomes larger and larger (more than 80000 lines now), we cannot test all the modules on all supported platforms for each release. If you found some bugs or problems, please submit your comments through the "google code issues" page in openwsn.googlecode.com. The OpenWSN community maintains the software and the partners will give the commercial support.


2011.09.27

On CWSN2011

Thanks for all the attention about the OpenWSN project. Since it consumes us some time to change the version management system from svn to git, we had to delay the next version to Oct. 2011. Besides the GAINZ hardware platform support, it will further support OpenNode(STM32F103(cortex) + cc2520), and you may even buy the new hardware platform from different companies.


Furthermore, the openwsn software for GAINZ and OpenNode are in different folders now. It will be merged into one unified system in the future.



---


2011.07.15

OpenWSN will be ported to git!

Thanks code.google.com for the support of git version control from today!


2011.04.27

OpenWSN Ranks Top 1 in the 2010 Open Source Competition in China

OpenWSN wons the only ''Golden Award'' in the ''2010 Open Source Competition in China (2010第四届中国开源软件竞赛)'', which is organized by cosoft (中国软件行业协会共创软件分会（共创软件联盟）). The team members include Jiang Ridong, Xu Fuzhen, Lu Weihui, He Yaxi, Yan Shixing, Li Pengfei, Sun Qiang, Shi Miaojing, and Li Xin. However, there's also a lot of more who contributed to this project before. I'm sorry for I cannot listing them all. The Silver Awards include ZenTao Project Management System, Resource Management for Cloud Computing and RT-Thread RTOS for Dragon Microprocessor. The Bronze Awards include an middleware project and other four.

I do appreciate all the friends who solicitude the OpenWSN project in the past. As an open source project still ongoing, we promise to continue push it forward, and make it a robust system which can support the future Internet-Of-Things world and satisfy highly embedded   applications in critical industry fields.


Link: Award list at http://www.cosoft.com, http://www.cosoft.org.cn/site/?action-viewnews-itemid-1051



---


2010.10 On Hardware Platforms


Currently, OpenWSN runs on GAINZ platform only. GAINZ is based on cc2420 and atmega128L which is quite similar to Crossbow's MicaZ node.
Please contact [Shen Lian Tech](http://www.shenlian-tech.com) (Manager Jiang, Peng) for GAINZ hardware.



---


2011.03.18

Bug fixed and prepare for the next milestone release. Also improved the network architecture. Fully discard the old TiOpenFrame and replace it with TiFrame(in rtl\_frame) and TiIEEE802Frame154Descriptor(in rtl\_ieee802frame154). Upgraded a lot of network components including aloha(in svc\_nio\_aloha), csma(in svc\_nio\_csma), flood(in svc\_nio\_flood), etc.  Also new example and testings.



---


2010.01.10

Next version will be released in May 2010. The main improvements will includes a new network protocol architecture, containing new timesync component and mesh routing component, maybe wireless update if possible ...


---


2010.10.14

openwsn 2.1.0 released!

this is an bug fix version of 2.1.0.  correct errors in csma communication component and also upgrade related testing programs.



---


2010.10.10

openwsn 2.0.0 released!

[click here to download](http://code.google.com/p/openwsn/downloads/list)

this version improved a lot of core features, in particular the low power event and task scheduling architecture (osx kernel), and some protocols. since we upgraded the low level MAC protocols, the flooding coomponent failed to run on the new MAC component now, but we will fix this soon in the next release before the end of this year.


- upgrading developing environment to the newest WinAVR and AVR Studio;

- rewrite the task scheduling component. now using heap data structure for priority queuing based scheduling;

- using interface design and implementation to split transceiver components and mac components;

- add interface definition for byte, block and frame devices;

- upgrade the MAC components, including simple aloha(tested), aloha(tested), csma(in testing) and maca;

- reorganize the folder structures. it's intend to support multiple developing platform; and target platforms;

- add some new examples such as low power testing;

- upgrade cc2420 testings and other testing/examples;

- and a lot of other minor improvements.

openwsn 2.0.0 is an important release due to the great improvements of osx kernel and files reorganization. but it maybe not very stable due to some high level components may cannot keep consistent with the low level upgradings. but this will be solved in the next release.

The next release will contain more bug fix, protocol improvements and extensions. and will released in Dec. 2010. the time synchronization feature will be provided in the next release too.



---


2010.09.21


Upgrading the developing environment to the newest software

- [AVR Studio 4.18 and SP3](http://www.atmel.com/dyn/products/tools_card.asp?tool_id=2725) for ATmega128L from ATMEL.com

- [WinAVR 20100110](http://sourceforge.net/projects/winavr/) Toolchain


You should: 1) install AVR Studio D:\binx\embedded\avrstudio (not mandatory); 2) install WinAVR to d:\binx\embedded\winavr (mandatory).


Attention in the past, we use [Portable WinAVR 20090313](http://www.chip45.com/info/Portable-WinAVR.html). now we changed back to standard WinAVR to help the beginners to avoid directory settings. Since we haven't changed the path settings in all the example project, we still keep a copy of [Portable WinAVR 20090313](http://www.chip45.com/info/Portable-WinAVR.html) in "d:\portable\WinAVR-20090313" is by default our former settings. You only need uninstall the AVR studio and reinstall the new version.

This upgrading may cause some projects failed to open by AVR studio, you can simply re-created the project file. If you encounter such problems, please report to us through google code's issue report. We'll correct them in the next release.



2010.08.12


Event and Task hybrid architecture of OpenWSN was finished and in testing...
Priority task scheduling of OpenWSN was finished and in testing
All these will be released in the next version.



---


2010.07.30

OpenWSN version 2.0 will be released in Oct this year. Major improvements will be includes: 1) systematic support low power scheduling kernel; 2) more protocol stacks; 3) timesync; 4) more examples. Currently, it's used in the teaching practice of <Wireless Sensor Network System and Applications> in [TongJi University](http://www.tongji.edu.cn).


[Historical News...](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDhyM3hucTNkYg&hl=en)


[专辑: 同济大学OpenWSN平台测试视频, 2009.11](http://www.youku.com/playlist_show/id_4052007.html)

[OpenWSN Videos 2009.11](http://www.youku.com/playlist_show/id_4052007.html)


### Introduction on Wireless Sensor Networks ###

---


[Wireless Sensor Networks On WikiPedia](http://en.wikipedia.org/wiki/Wireless_sensor_network)

### OpenWSN HandBook ###

---


| CH 1 | [OpenWSN Introduction](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDZkeGQ0bm5kcQ&hl=en|) |
|:-----|:--------------------------------------------------------------------------------------------------------|
| CH 2 | [OpenWSN Experiment FAQ](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDdmamM0czdkdA&hl=en|) |
| CH 3 | [OpenWSN Examples](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDJmaHh0OW1ncw&hl=en|) |
| CH 4 | [OpenWSN Porting To New Platforms](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDlnNnptNXpmdw&hl=en|) |
| CH 5 | [OpenWSN Developing Interface on Computer Side](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfOTJnY3I0MzdjMg&hl=en|) |
| CH 6 | [OpenWSN License](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDNkZDh3N2RjcA&hl=en|) |
| CH 7 | [OpenWSN Contributors & Acknowledgements](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDVkY2h6dGJjMg&hl=en|) |
| CH 8 | [OpenWSN Blueprint](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDFmNTlncW1jdA&hl=en|) |
| CH 9 | [OpenWSN On Internet](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDRna3JwczM0OA&hl=en|) |
| Appendix | [Source Code Review](http://code.google.com/p/openwsn/source/browse/trunk/node/)    |

Reference

- [OpenWSN传感器网络操作系统设计与分析](http://docs.google.com/fileview?id=0B4S8PUsdf99xYWQ1MWQ0ZDgtZWZkNC00Y2Y1LTllZmQtMDZmZDQ5MTRiZjhi&hl=en) (11 Pages) [download](http://openwsn.googlecode.com/files/openwsn_design.pdf)

- [An Brief Introduction of OpenWSN Sensor OS/Kernel](http://docs.google.com/fileview?id=0B4S8PUsdf99xNDc4MTQ2MGEtYWNmMC00YTQ0LWIwZWUtNGFhZWZhMjBmNmRj&hl=en) (slice), CWSN 2009, Suzhou [download](http://openwsn.googlecode.com/files/openwsn_kernel%28cwsn2009%29.pdf)

More reference are listed in document [OpenWSN License](http://docs.google.com/Doc?docid=0AYS8PUsdf99xZGRkamJuazlfNDNkZDh3N2RjcA&hl=en|).


### Donation ###

---


[Support This Project](http://sourceforge.net/donate/index.php?group_id=187215): [![](http://images.sourceforge.net/images/project-support.jpg)](http://sourceforge.net/donate/index.php?group_id=187215)



### OpenWSN On Internet ###

---


| Project Homepage |  http://openwsn.googlecode.com |
|:-----------------|:-------------------------------|
| OpenWSN On GoogleCode |  http://openwsn.googlecode.com/ or http://code.google.com/p/openwsn/ |
| Blog | http://openwsn.blogspot.com or http://www.inblogs.net/openwsn if you cannot visit the first link |
| Forum | [OpenWSN on GoogleGroups(you may need to replace http with https manually)](https://groups.google.com/group/openwsn) |
| Source Code | [download](http://code.google.com/p/openwsn/downloads/list) |

### New Members Wanted ###

---


OpenWSN Project is still in evolving. It's open to everyone. We warmly welcome any new members with diverse backgrounds to attend it, from research and algorithm design, hardware driver developing, application developer and even the multi language document translation. The core members will maintain the low level kernel modules and the major version control branches to support all kinds of high level developing and low level platform porting.


### About the Designer and Organizor ###

---


The designer and organizer of OpenWSN project is Dr. Wei Zhang (张伟). He's with Dept. of Control Science and Engineering, TongJi University, which is a hundred year university in China since 1907. His interests include the fundamental techniques in wireless sensor networks, complex networks behaviors, and intelligent data analysis. Currently, he focus on sensor network platforms & applications. You can contact him through openwsn#gmail$com or through the his personal web site [http://www.tongji.edu.cn/~weizhang](http://www.tongji.edu.cn/~weizhang)